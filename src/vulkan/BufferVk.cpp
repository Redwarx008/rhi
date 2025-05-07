#include "BufferVk.h"

#include "DeviceVk.h"
#include "QueueVk.h"
#include "ErrorsVk.h"
#include "CommandListVk.h"
#include "ResourceToDelete.h"
#include "VulkanUtils.h"
#include "RefCountedHandle.h"
#include "../common/Utils.h"
#include "../common/Error.h"
#include "../common/Constants.h"

#include <algorithm>

namespace rhi::impl::vulkan
{
	constexpr BufferUsage cShaderBufferUsages =
		BufferUsage::Uniform | BufferUsage::Storage | cReadOnlyStorageBuffer;
	constexpr BufferUsage cMappableBufferUsages =
		BufferUsage::MapRead | BufferUsage::MapWrite;
	constexpr BufferUsage cReadOnlyBufferUsages =
		BufferUsage::MapRead | BufferUsage::CopySrc | BufferUsage::Index |
		BufferUsage::Vertex | BufferUsage::Uniform | cReadOnlyStorageBuffer;

	VkBufferUsageFlags BufferUsageConvert(BufferUsage usage)
	{
		VkBufferUsageFlags flags = 0;

		if (HasFlag(usage, BufferUsage::Vertex))
		{
			flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (HasFlag(usage, BufferUsage::Index))
		{
			flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (HasFlag(usage, BufferUsage::Indirect))
		{
			flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		if (HasFlag(usage, BufferUsage::Uniform))
		{
			flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (HasFlag(usage, BufferUsage::Storage))
		{
			flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (HasFlag(usage, BufferUsage::CopySrc))
		{
			flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}
		if (HasFlag(usage, BufferUsage::CopyDst))
		{
			flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		return flags;
	}

	VkAccessFlagBits2 AccessFlagsConvert(BufferUsage usage) {
		VkAccessFlagBits2 flags = 0;

		if (HasFlag(usage, BufferUsage::MapRead)) 
		{
			flags |= VK_ACCESS_2_HOST_READ_BIT;
		}
		if (HasFlag(usage, BufferUsage::MapWrite))
		{
			flags |= VK_ACCESS_2_HOST_WRITE_BIT;
		}
		if (HasFlag(usage, BufferUsage::CopySrc))
		{
			flags |= VK_ACCESS_2_TRANSFER_READ_BIT;
		}
		if (HasFlag(usage, BufferUsage::CopyDst))
		{
			flags |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
		}
		if (HasFlag(usage, BufferUsage::Index))
		{
			flags |= VK_ACCESS_2_INDEX_READ_BIT;
		}
		if (HasFlag(usage, BufferUsage::Vertex))
		{
			flags |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
		}
		if (HasFlag(usage, BufferUsage::Uniform))
		{
			flags |= VK_ACCESS_2_UNIFORM_READ_BIT;
		}
		if (HasFlag(usage, BufferUsage::Storage))
		{
			flags |= VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT;
		}

		if (HasFlag(usage, cReadOnlyStorageBuffer))
		{
			flags |= VK_ACCESS_2_SHADER_READ_BIT;
		}

		if (HasFlag(usage, BufferUsage::QueryResolve))
		{
			flags |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
		}

		return flags;
	}

	VkPipelineStageFlags2 PipelineStageConvert(BufferUsage usage, ShaderStage shaderStage) {
		VkPipelineStageFlags2 flags = 0;

		if (HasFlag(usage, cMappableBufferUsages))
		{
			flags |= VK_PIPELINE_STAGE_2_HOST_BIT;
		}
		if (HasFlag(usage, (BufferUsage::CopySrc | BufferUsage::CopyDst)))
		{
			flags |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		}
		if (HasFlag(usage, (BufferUsage::Index | BufferUsage::Vertex)))
		{
			flags |= VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
		}
		if (HasFlag(usage, cShaderBufferUsages))
		{
			if (HasFlag(shaderStage, ShaderStage::Vertex))
			{
				flags |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
			}
			if (HasFlag(shaderStage, ShaderStage::Fragment))
			{
				flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			}
			if (HasFlag(shaderStage, ShaderStage::Compute))
			{
				flags |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			}
		}
		if (HasFlag(usage, BufferUsage::QueryResolve))
		{
			flags |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		}

		return flags;
	}

	VkSharingMode ShareModeConvert(ShareMode mode)
	{
		if (mode == ShareMode::Exclusive)
		{
			return VK_SHARING_MODE_EXCLUSIVE;
		}
		else
		{
			return VK_SHARING_MODE_CONCURRENT;
		}
	}


	Ref<Buffer> Buffer::Create(DeviceBase* device, const BufferDesc& desc)
	{
		Ref<Buffer> buffer = AcquireRef(new Buffer(device, desc));
		if (!buffer->Initialize())
		{
			return nullptr;
		}
		return buffer;
	}

	Buffer::Buffer(DeviceBase* device, const BufferDesc& desc)
		:BufferBase(device, desc)
	{

	}

	Buffer::~Buffer() {}

	bool Buffer::Initialize()
	{
		BufferBase::Initialize();

		constexpr BufferUsage cMapWriteAllowedUsages = BufferUsage::CopySrc | BufferUsage::MapWrite;
		INVALID_IF(HasFlag(BufferUsage::MapWrite, mUsage) && !IsSubset(mUsage, cMapWriteAllowedUsages),
			"The BufferUsage::MapWrite flag can only compatible with BufferUsage::CopySrc.");
		constexpr BufferUsage cMapReadAllowedUsages = BufferUsage::CopyDst | BufferUsage::MapRead;
		INVALID_IF(HasFlag(BufferUsage::MapRead, mUsage) && !IsSubset(mUsage, cMapReadAllowedUsages),
			"The BufferUsage::MapRead flag can only compatible with BufferUsage::CopyDst.");

		// Vulkan requires the size to be non-zero.
		uint64_t toAllocatedSize = (std::max)(mSize, 4ull);

		ASSERT_MSG(!(toAllocatedSize & (3ull << 62ull)),
			"Buffer size is HUGE and could cause overflows");

		Device* device = checked_cast<Device>(mDevice.Get());

		VkBufferCreateInfo bufferCI{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferCI.size = toAllocatedSize;
		bufferCI.sharingMode = ShareModeConvert(mShareMode);
		bufferCI.usage = BufferUsageConvert(mInternalUsage | BufferUsage::CopyDst);
		if (mShareMode == ShareMode::Concurrent)
		{
			std::vector<uint32_t> queueFamiles;
			queueFamiles.push_back(checked_cast<Queue>(device->GetQueue(QueueType::Graphics))->GetQueueFamilyIndex());
			if (device->GetQueue(QueueType::Compute))
			{
				queueFamiles.push_back(checked_cast<Queue>(device->GetQueue(QueueType::Compute))->GetQueueFamilyIndex());
			}
			if (device->GetQueue(QueueType::Transfer))
			{
				queueFamiles.push_back(checked_cast<Queue>(device->GetQueue(QueueType::Transfer))->GetQueueFamilyIndex());
			}
			bufferCI.queueFamilyIndexCount = queueFamiles.size();
			bufferCI.pQueueFamilyIndices = queueFamiles.data();
		}

		VmaAllocationCreateInfo allocCI{};
		allocCI.usage = VMA_MEMORY_USAGE_AUTO;
		allocCI.priority = 1.0f;
		if ((mUsage & BufferUsage::MapRead) != 0)
		{
			allocCI.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}
		else if ((mUsage & BufferUsage::MapWrite) != 0)
		{
			allocCI.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}
		else
		{
			// No CPU access
			constexpr uint64_t bigSize = 4ull * 1024ull * 1024ull;
			if (toAllocatedSize >= bigSize)
			{
				allocCI.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
			}
		}

		VkResult err = vmaCreateBuffer(device->GetMemoryAllocator(), &bufferCI, &allocCI, &mHandle, &mAllocation, &mAllocationInfo);
		CHECK_VK_RESULT_FALSE(err, "Could not create buffer");

		SetDebugName(device, mHandle, "Buffer", GetName());

		return true;
	}

	void Buffer::DestroyImpl()
	{
		if (mState == State::Destroyed)
		{
			return;
		}

		if (mState == State::PendingMap)
		{
			// todo: update map task state
		}
		mState = State::Unmapped;

		Device* device = checked_cast<Device>(mDevice.Get());

		if (mShareMode == ShareMode::Exclusive)
		{
			checked_cast<Queue>(device->GetQueue(mLastUsedQueue))->GetDeleter()->DeleteWhenUnused({ mHandle, mAllocation });
		}
		else
		{
			// Buffers in concurrent mode may be used by multiple queues and there is no way to tell who was last to use .
			//Ref<RefCountedHandle<BufferAllocation>> bufferAllocation = AcquireRef(new RefCountedHandle<BufferAllocation>(device, { mHandle, mAllocation },
			//	[](Device* device, BufferAllocation handle)
			//	{
			//		vmaDestroyBuffer(device->GetMemoryAllocator(), handle.buffer, handle.allocation);
			//	}
			//));

			for (uint32_t i = 0; i < mUsageTrackInQueues.size(); ++i)
			{
				Queue* queue = checked_cast<Queue>(device->GetQueue(static_cast<QueueType>(i)).Get());
				if (!queue)
				{
					continue;
				}
				//queue->GetDeleter()->DeleteWhenUnused(bufferAllocation);
			}
		}

		mHandle = VK_NULL_HANDLE;
		mAllocation = VK_NULL_HANDLE;

		mState = State::Destroyed;
	}



	uint64_t Buffer::GetAllocatedSize() const
	{
		return mAllocationInfo.size;
	}

	VkBuffer Buffer::GetHandle() const
	{
		return mHandle;
	}

	void Buffer::MarkUsedInPendingCommandList(Queue* queue)
	{
		uint64_t serial = queue->GetPendingSubmitSerial();
		QueueType queueType = queue->GetType();
		assert(serial >= mUsageTrackInQueues[static_cast<uint32_t>(queueType)].lastUsageSerial);
		mUsageTrackInQueues[static_cast<uint32_t>(queueType)].lastUsageSerial = serial;
	}

	void Buffer::TransitionOwnership(Queue* queue, Queue* receivingQueue)
	{
		assert(queue->GetType() == mLastUsedQueue);
		assert(queue->GetType() != receivingQueue->GetType());

		BufferUsage& srcUsage = mUsageTrackInQueues[static_cast<uint32_t>(mLastUsedQueue)].lastWriteUsage;
		ShaderStage& srcStage = mUsageTrackInQueues[static_cast<uint32_t>(mLastUsedQueue)].lastWriteShaderStage;

		VkBufferMemoryBarrier2 barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
		barrier.srcAccessMask = AccessFlagsConvert(srcUsage);
		barrier.srcStageMask = PipelineStageConvert(srcUsage, srcStage);
		barrier.dstAccessMask = 0;
		barrier.dstStageMask = 0;
		barrier.buffer = mHandle;
		barrier.offset = 0;
		barrier.size = VK_WHOLE_SIZE;
		barrier.srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
		barrier.dstQueueFamilyIndex = receivingQueue->GetQueueFamilyIndex();

		queue->GetPendingRecordingContext()->AddBufferBarrier(barrier);

		srcUsage = BufferUsage::None;
		srcStage = ShaderStage::None;
		mUsageTrackInQueues[static_cast<uint32_t>(mLastUsedQueue)].readUsage = BufferUsage::None;
		mUsageTrackInQueues[static_cast<uint32_t>(mLastUsedQueue)].readShaderStages = ShaderStage::None;
	}

	void Buffer::TransitionUsageNow(Queue* queue, BufferUsage usage, ShaderStage stage)
	{
		TrackUsageAndGetResourceBarrier(queue, usage, stage);
		queue->GetPendingRecordingContext()->EmitBarriers();
	}

	void Buffer::TrackUsageAndGetResourceBarrier(Queue* queue, BufferUsage usage, ShaderStage shaderStage)
	{
		QueueType queueType = queue->GetType();
		mLastUsedQueue = queueType;

		// we need a VkBufferMemoryBarrier to transfer queue ownership.
		bool needTransferOwnership = mShareMode != ShareMode::Concurrent && mLastUsedQueue != QueueType::Undefined && mLastUsedQueue != queueType;

		if (shaderStage == ShaderStage::None)
		{
			// If the buffer isn't used in any shader stages, ignore shader usages. Eg. ignore a uniform
			// buffer that isn't actually read in any shader.
			usage &= ~cShaderBufferUsages;
		}

		const bool isMapUsage = HasFlag(usage, cMappableBufferUsages);
		if (!isMapUsage)
		{
			// Request non CPU usage, so assume the buffer will be used in pending commands.
			MarkUsedInPendingCommandList(queue);
		}

		const bool readOnly = IsSubset(usage, cReadOnlyBufferUsages);
		VkAccessFlags2 srcAccess = 0;
		VkPipelineStageFlags2 srcStage = 0;

		BufferUsage& readUsage = mUsageTrackInQueues[static_cast<uint32_t>(queueType)].readUsage;
		ShaderStage& readShaderStages = mUsageTrackInQueues[static_cast<uint32_t>(queueType)].readShaderStages;

		BufferUsage& lastWriteUsage = mUsageTrackInQueues[static_cast<uint32_t>(queueType)].lastWriteUsage;
		ShaderStage& lastWriteShaderStage = mUsageTrackInQueues[static_cast<uint32_t>(queueType)].lastWriteShaderStage;

		if (readOnly)
		{
			if ((shaderStage & ShaderStage::Fragment) != 0 && (readShaderStages & ShaderStage::Vertex) != 0)
			{
				// There is an implicit vertex->fragment dependency, so if the vertex stage has already
				// waited, there is no need for fragment to wait. Add the fragment usage so we know to 
				// wait for it before the next write.
				readShaderStages |= ShaderStage::Fragment;
			}

			if (IsSubset(usage, readUsage) && IsSubset(shaderStage, readShaderStages))
			{
				// This usage and shader stage has already waited for the last write.
				// No need for another barrier.
				return;
			}

			readUsage |= usage;
			readShaderStages |= shaderStage;

			if (lastWriteUsage == BufferUsage::None && !needTransferOwnership)
			{
				// Read dependency with no prior writes. No barrier needed.
				return;
			}
			// Write -> read barrier.
			srcAccess = AccessFlagsConvert(lastWriteUsage);
			srcStage = PipelineStageConvert(lastWriteUsage, lastWriteShaderStage);
		}
		else
		{
			bool skipBarrier = false;

			// vkQueueSubmit does an implicit domain and visibility operation. For HOST_COHERENT
			// memory, we can ignore read (host)->write barriers. However, we can't necessarily
			// skip the barrier if mReadUsage == MapRead, as we could still need a barrier for
			// the last write. Instead, pretend the last host read didn't happen.
			readUsage &= ~BufferUsage::MapRead;

			if ((lastWriteUsage == BufferUsage::None && readUsage == BufferUsage::None && !needTransferOwnership) ||
				IsSubset(usage | lastWriteUsage | readUsage, cMappableBufferUsages))
			{
				// The buffer has never been used before, or the dependency is map->map. We don't need a
				// barrier.
				skipBarrier = true;
			}
			else if (readUsage == BufferUsage::None)
			{
				// No reads since the last write.
				// Write -> write barrier.
				srcAccess = AccessFlagsConvert(lastWriteUsage);
				srcStage = PipelineStageConvert(lastWriteUsage, lastWriteShaderStage);
			}
			else
			{
				// Read -> write barrier.
				srcAccess = AccessFlagsConvert(readUsage);
				srcStage = PipelineStageConvert(readUsage, readShaderStages);
			}

			lastWriteUsage = usage;
			lastWriteShaderStage = shaderStage;

			readUsage = BufferUsage::None;
			readShaderStages = ShaderStage::None;

			if (skipBarrier)
			{
				return;
			}
		}
		if (isMapUsage) {
			// CPU usage, but a pipeline barrier is needed, so mark the buffer as used within the
			// pending commands.
			MarkUsedInPendingCommandList(queue);
		}

		VkBufferMemoryBarrier2 barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
		barrier.srcAccessMask = srcAccess;
		barrier.srcStageMask = srcStage;
		barrier.dstAccessMask = AccessFlagsConvert(usage);
		barrier.dstStageMask = PipelineStageConvert(usage, shaderStage);
		barrier.buffer = mHandle;
		barrier.offset = 0;
		barrier.size = VK_WHOLE_SIZE;
		if (needTransferOwnership)
		{
			barrier.srcQueueFamilyIndex = checked_cast<Queue>(checked_cast<Device>(mDevice)->GetQueue(mLastUsedQueue))->GetQueueFamilyIndex();
			barrier.dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
		}

		queue->GetPendingRecordingContext()->AddBufferBarrier(barrier);
	}

	ResourceType Buffer::GetType() const
	{
		return ResourceType::Buffer;
	}

	void* Buffer::APIGetMappedPointer()
	{
		return mAllocationInfo.pMappedData;
	}

	void Buffer::MapAsyncImpl(QueueBase* queue, MapMode mode)
	{
		CommandRecordContext* recordContext = checked_cast<Queue>(queue)->GetPendingRecordingContext();
		ASSERT(HasOneFlag(mode));
		if (mode == MapMode::Read)
		{
			TransitionUsageNow(checked_cast<Queue>(queue), BufferUsage::MapRead);
		}
		else
		{
			TransitionUsageNow(checked_cast<Queue>(queue), BufferUsage::MapWrite);
		}
	}
}