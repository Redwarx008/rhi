#include "QueueVk.h"
#include "ErrorsVk.h"
#include "CommandListVk.h"
#include "DeviceVk.h"
#include "ErrorsVk.h"
#include "BufferVk.h"
#include "TextureVk.h"
#include "DescriptorSetAllocator.h"
#include "../common/Utils.h"
#include "../Subresource.h"
#include "VulkanUtils.h"

namespace rhi::impl::vulkan
{

	Ref<Queue> Queue::Create(Device* device, uint32_t family, QueueType type)
	{
		Ref<Queue> queue = AcquireRef(new Queue(device, family, type));
		queue->Initialize();
		return queue;
	}

	Queue::Queue(Device* device, uint32_t family, QueueType type) :
		QueueBase(device, type),
		mQueueFamilyIndex(family),
		mDeleter(this)
	{
		vkGetDeviceQueue(device->GetHandle(), mQueueFamilyIndex, 0, &mHandle);
	}

	Queue::~Queue() 
	{
		Device* device = checked_cast<Device>(mDevice);

		TickImpl(UINT64_MAX);

		ASSERT(mCommandBufferInFlight.Empty());

		for (CommandPoolAndBuffer& poolAndBuffer : mUnusedCommandBuffer)
		{
			vkDestroyCommandPool(device->GetHandle(), poolAndBuffer.poolHandle, nullptr);
		}
		mUnusedCommandBuffer.clear();

		mUploadAllocator = nullptr;

		vkDestroySemaphore(device->GetHandle(), mTrackingSubmitSemaphore, nullptr);
	}

	void Queue::Initialize()
	{
		SetTrackingSubmitSemaphore();
		NextRecordingContext();
	}

	void Queue::SetTrackingSubmitSemaphore()
	{
		Device* device = checked_cast<Device>(mDevice);

		VkSemaphoreTypeCreateInfo semaphoreTypeCI{};
		semaphoreTypeCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO_KHR;
		semaphoreTypeCI.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE_KHR;

		VkSemaphoreCreateInfo semaphoreCI{};
		semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCI.pNext = &semaphoreTypeCI;

		VkResult err = vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &mTrackingSubmitSemaphore);
		CHECK_VK_RESULT(err, "SetTrackingSubmitSemaphore");
	}

	CommandRecordContext* Queue::GetPendingRecordingContext()
	{
		ASSERT(mRecordContext.commandBufferAndPool.bufferHandle != VK_NULL_HANDLE);
		mRecordContext.needsSubmit = true;
		return &mRecordContext;
	}

	uint64_t Queue::QueryCompletedSerial()
	{
		Device* device = checked_cast<Device>(mDevice);
		uint64_t completedSerial = 0;
		VkResult err = vkGetSemaphoreCounterValue(device->GetHandle(), mTrackingSubmitSemaphore, &completedSerial);
		CHECK_VK_RESULT(err, "QueryCompletedSerial");
		return completedSerial;
	}

	CommandPoolAndBuffer Queue::GetOrCreateCommandPoolAndBuffer()
	{
		Device* device = checked_cast<Device>(mDevice);
		CommandPoolAndBuffer poolAndBuffer;
		if (!mUnusedCommandBuffer.empty())
		{
			poolAndBuffer = mUnusedCommandBuffer.back();
			mUnusedCommandBuffer.pop_back();
			VkResult err = vkResetCommandPool(device->GetHandle(), poolAndBuffer.poolHandle, 0);
			CHECK_VK_RESULT(err, "vkResetCommandPool");
		}
		else
		{
			VkCommandPoolCreateInfo createInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			createInfo.queueFamilyIndex = mQueueFamilyIndex;

			VkResult err = vkCreateCommandPool(device->GetHandle(), &createInfo, nullptr, &poolAndBuffer.poolHandle);
			CHECK_VK_RESULT(err, "vkCreateCommandPool");

			VkCommandBufferAllocateInfo allocateInfo;
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = poolAndBuffer.poolHandle;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = 1;

			err = vkAllocateCommandBuffers(device->GetHandle(), &allocateInfo, &poolAndBuffer.bufferHandle);
			CHECK_VK_RESULT(err, "vkAllocateCommandBuffers");
		}

		return poolAndBuffer;
	}

	void Queue::NextRecordingContext()
	{
		ASSERT(mRecordContext.needsSubmit != true);
		ASSERT(mRecordContext.commandBufferAndPool.bufferHandle == VK_NULL_HANDLE &&
			mRecordContext.commandBufferAndPool.poolHandle == VK_NULL_HANDLE);

		CommandPoolAndBuffer poolAndBuffer = GetOrCreateCommandPoolAndBuffer();

		VkCommandBufferBeginInfo beginInfo;
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr;
		VkResult err = vkBeginCommandBuffer(poolAndBuffer.bufferHandle, &beginInfo);
		CHECK_VK_RESULT_RETURN(err, "vkBeginCommandBuffer");

		mRecordContext.commandBufferAndPool = poolAndBuffer;
	}

	void Queue::SubmitPendingCommands()
	{
		if (!mRecordContext.needsSubmit)
		{
			return;
		}

		VkCommandBuffer commandBuffer = mRecordContext.commandBufferAndPool.bufferHandle;

		VkResult err = vkEndCommandBuffer(commandBuffer);
		CHECK_VK_RESULT_RETURN(err, "vkEndCommandBuffer");

		VkSemaphoreSubmitInfo& trackingSemaphore = mRecordContext.signalSemaphoreSubmitInfos.emplace_back();
		trackingSemaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		trackingSemaphore.pNext = nullptr;
		trackingSemaphore.semaphore = mTrackingSubmitSemaphore;
		trackingSemaphore.value = GetPendingSubmitSerial();
		trackingSemaphore.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

		VkCommandBufferSubmitInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		commandBufferInfo.commandBuffer = commandBuffer;

		VkSubmitInfo2 submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = mRecordContext.signalSemaphoreSubmitInfos.size();
		submitInfo.pSignalSemaphoreInfos = mRecordContext.signalSemaphoreSubmitInfos.data();
		submitInfo.waitSemaphoreInfoCount = mRecordContext.waitSemaphoreSubmitInfos.size();
		submitInfo.pWaitSemaphoreInfos = mRecordContext.waitSemaphoreSubmitInfos.data();

		err = vkQueueSubmit2(mHandle, 1, &submitInfo, nullptr);
		CHECK_VK_RESULT_RETURN(err, "vkQueueSubmit2");

		mLastSubmittedSerial.fetch_add(1u, std::memory_order_release);

		mCommandBufferInFlight.Push(GetLastSubmittedSerial(), mRecordContext.commandBufferAndPool);

		mRecordContext.Reset();
		NextRecordingContext();
	}

	uint64_t Queue::SubmitImpl(CommandListBase* const* commands, uint32_t commandListCount, ResourceTransfer const* transfers, uint32_t transferCount)
	{
		for (uint32_t i = 0; i < commandListCount; ++i)
		{
			checked_cast<CommandList>(commands[i])->RecordCommands(this);
		}

		for (uint32_t i = 0; i < transferCount; ++i)
		{
			const ResourceTransfer& resourceTransfer = transfers[i];
			Queue* receivingQueue = checked_cast<Queue>(resourceTransfer.receivingQueue);

			for (uint32_t bufferIndex = 0; bufferIndex < resourceTransfer.bufferCount; ++bufferIndex)
			{
				Buffer* buffer = checked_cast<Buffer>(resourceTransfer.buffers[i]);
				buffer->TransitionOwnership(this, receivingQueue);
			}

			for (uint32_t textureIndex = 0; textureIndex < resourceTransfer.textureSubresourceCount; ++textureIndex)
			{
				const TextureSubresources& subresources = resourceTransfer.textureSubresources[i];
				Texture* texture = checked_cast<Texture>(subresources.texture);
				SubresourceRange range{};
				range.aspects = AspectConvert(texture->APIGetFormat(), subresources.range.aspect);
				range.baseArrayLayer = subresources.range.baseArrayLayer;
				range.layerCount = subresources.range.arrayLayerCount;
				range.baseMipLevel = subresources.range.baseMipLevel;
				range.levelCount = subresources.range.mipLevelCount;

				texture->TransitionOwnership(this, range, receivingQueue);
			}

			GetPendingRecordingContext()->EmitBarriers();
		}
		SubmitPendingCommands();
		return GetLastSubmittedSerial();
	}

	void Queue::TickImpl(uint64_t completedSerial)
	{
		mDeleter->Tick(completedSerial);

		mDescriptorAllocatorsPendingDeallocation.Use([&](auto pending)
			{
				for (Ref<DescriptorSetAllocator>& allocator : pending->IterateUpTo(completedSerial))
				{
					allocator->FinishDeallocation(this, completedSerial);
				}
				pending->ClearUpTo(completedSerial);
			});

		RecycleCompletedCommandBuffer(completedSerial);
	}

	void Queue::RecycleCompletedCommandBuffer(uint64_t completedSerial)
	{
		for (auto& commands : mCommandBufferInFlight.IterateUpTo(completedSerial))
		{
			mUnusedCommandBuffer.push_back(commands);
		}
		mCommandBufferInFlight.ClearUpTo(completedSerial);
	}

	void Queue::EnqueueDeferredDeallocation(DescriptorSetAllocator* allocator)
	{
		mDescriptorAllocatorsPendingDeallocation->Push(GetPendingSubmitSerial(), allocator);
	}

	void Queue::MarkRecordingContextIsUsed()
	{
		GetPendingRecordingContext()->needsSubmit = true;
	}

	uint32_t Queue::GetQueueFamilyIndex() const
	{
		return mQueueFamilyIndex;
	}

	VkQueue Queue::GetHandle() const
	{
		return mHandle;
	}

	VkSemaphore Queue::GetTrackingSubmitSemaphore() const
	{
		return mTrackingSubmitSemaphore;
	}

	Device* Queue::GetDevice() const
	{
		return checked_cast<Device>(mDevice);
	}

	MutexProtected<VkResourceDeleter>& Queue::GetDeleter()
	{
		return mDeleter;
	}

	void Queue::CopyFromStagingToBufferImpl(BufferBase* src, uint64_t srcOffset, BufferBase* dst, uint64_t destOffset, uint64_t size)
	{
		VkCommandBuffer commanBuffer = GetPendingRecordingContext()->commandBufferAndPool.bufferHandle;

		// There is no need of a barrier to make host writes available and visible to the copy
		// operation for HOST_COHERENT memory. The Vulkan spec for vkQueueSubmit describes that it
		// does an implicit availability, visibility and domain operation.

		Buffer* dstBuffer = checked_cast<Buffer>(dst);
		dstBuffer->TransitionUsageNow(this, BufferUsage::CopyDst);

		VkBufferCopy copy;
		copy.srcOffset = srcOffset;
		copy.dstOffset = destOffset;
		copy.size = size;

		vkCmdCopyBuffer(commanBuffer, checked_cast<Buffer>(src)->GetHandle(), dstBuffer->GetHandle(), 1, &copy);
	}

	void Queue::CopyFromStagingToTextureImpl(BufferBase* src, const TextureSlice& dst, const TextureDataLayout& dataLayout)
	{
		VkCommandBuffer commanBuffer = GetPendingRecordingContext()->commandBufferAndPool.bufferHandle;

		Buffer* buffer = checked_cast<Buffer>(src);

		Texture* texture = checked_cast<Texture>(dst.texture);

		Aspect aspect = AspectConvert(texture->APIGetFormat(), dst.aspect);

		VkBufferImageCopy region = ComputeBufferImageCopyRegion(dataLayout, dst.size, 
			texture, dst.mipLevel, dst.origin, aspect);

		SubresourceRange range = { aspect, dst.origin.z, dst.size.depthOrArrayLayers, dst.mipLevel, 1 };

		texture->TransitionUsageNow(this, TextureUsage::CopyDst, range);

		vkCmdCopyBufferToImage(commanBuffer, buffer->GetHandle(), texture->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}

	void Queue::WaitForImpl(QueueBase* queue, uint64_t submitSerial)
	{
		Queue* waitQueue = checked_cast<Queue>(queue);
		ASSERT(waitQueue->GetPendingSubmitSerial() > submitSerial);
		CommandRecordContext* recordContext = GetPendingRecordingContext();
		ASSERT(recordContext->commandBufferAndPool.bufferHandle != VK_NULL_HANDLE);
		VkSemaphoreSubmitInfo& semaphoreInfo = recordContext->waitSemaphoreSubmitInfos.emplace_back();
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		semaphoreInfo.pNext = nullptr;
		semaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		semaphoreInfo.semaphore = waitQueue->GetTrackingSubmitSemaphore();
		semaphoreInfo.value = submitSerial;
	}
}