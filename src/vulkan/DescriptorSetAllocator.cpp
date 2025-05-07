#include "DescriptorSetAllocator.h"

#include "../common/Constants.h"
#include "../common/Utils.h"
#include "DeviceVk.h"
#include "QueueVk.h"
#include "ErrorsVk.h"
#include "BindSetLayoutVk.h"

namespace rhi::impl::vulkan
{

	// Not a real GPU limit, but used to optimize parts of rhi which expect valid usage of the
	// API. There should never be more bindings than the max per stage, for each stage.
	static constexpr uint32_t cMaxBindingsPerPipelineLayout =
		cNumStages * (cMaxSampledTexturesPerShaderStage + cMaxSamplersPerShaderStage +
			cMaxStorageBuffersPerShaderStage + cMaxStorageTexturesPerShaderStage +
			cMaxUniformBuffersPerShaderStage);

	static constexpr uint32_t cMaxDescriptorsPerPool = 512;

	DescriptorSetAllocator::DescriptorSetAllocator(Device* device, std::unordered_map<VkDescriptorType, uint32_t>&& descriptorCountPerType) :
		mDevice(device)
	{
		// Compute the total number of descriptors for this layout.
		uint32_t totalDescriptorCount = 0;
		mPoolSizes.reserve(descriptorCountPerType.size());

		for (const auto& [type, count] : descriptorCountPerType)
		{
			assert(count > 0);
			totalDescriptorCount += count;
			mPoolSizes.push_back(VkDescriptorPoolSize{ type, count });
		}

		assert(totalDescriptorCount <= cMaxBindingsPerPipelineLayout);
		// Compute the total number of descriptors sets that fits given the max.
		mMaxSets = cMaxDescriptorsPerPool / totalDescriptorCount;
		assert(mMaxSets > 0);

		// Grow the number of desciptors in the pool to fit the computed |mMaxSets|.
		for (auto& poolSize : mPoolSizes) 
		{
			poolSize.descriptorCount *= mMaxSets;
		}
	}

	DescriptorSetAllocator::~DescriptorSetAllocator() 
	{
		for (auto& pool : mDescriptorPools) 
		{
			assert(pool.freeSetIndices.size() == mMaxSets);
			if (pool.vkPool != VK_NULL_HANDLE)
			{
				// todo: dstory whe unused?
				vkDestroyDescriptorPool(mDevice->GetHandle(), pool.vkPool, nullptr);
			}
		}
	}

	Ref<DescriptorSetAllocator> DescriptorSetAllocator::Create(Device* device, std::unordered_map<VkDescriptorType, uint32_t>&& descriptorCountPerType)
	{
		Ref<DescriptorSetAllocator> allocator = AcquireRef(new DescriptorSetAllocator(device, std::move(descriptorCountPerType)));
		return allocator;
	}

	DescriptorSetAllocation DescriptorSetAllocator::Allocate(BindSetLayout* layout) 
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mAvailableDescriptorPoolIndices.empty())
		{
			AllocateDescriptorPool(layout);
		}

		assert(!mAvailableDescriptorPoolIndices.empty());

		const uint32_t poolIndex = mAvailableDescriptorPoolIndices.back();
		DescriptorPool* pool = &mDescriptorPools[poolIndex];

		assert(!pool->freeSetIndices.empty());

		uint32_t setIndex = pool->freeSetIndices.back();
		pool->freeSetIndices.pop_back();

		if (pool->freeSetIndices.empty()) 
		{
			mAvailableDescriptorPoolIndices.pop_back();
		}

		return DescriptorSetAllocation{ pool->sets[setIndex], poolIndex, setIndex };
	}


	void DescriptorSetAllocator::AllocateDescriptorPool(BindSetLayout* layout)
	{
		VkDescriptorPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.maxSets = mMaxSets;
		createInfo.poolSizeCount = mPoolSizes.size();
		createInfo.pPoolSizes = mPoolSizes.data();

		VkDescriptorPool descriptorPool;

		VkResult err = vkCreateDescriptorPool(mDevice->GetHandle(), &createInfo, nullptr, &descriptorPool);
		CHECK_VK_RESULT_RETURN(err, "CreateDescriptorPool");

		std::vector<VkDescriptorSetLayout> layouts(mMaxSets, layout->GetHandle());

		VkDescriptorSetAllocateInfo allocateInfo;
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.pNext = nullptr;
		allocateInfo.descriptorPool = descriptorPool;
		allocateInfo.descriptorSetCount = mMaxSets;
		allocateInfo.pSetLayouts = layouts.data();

		std::vector<VkDescriptorSet> sets(mMaxSets);

		err = vkAllocateDescriptorSets(mDevice->GetHandle(), &allocateInfo, sets.data());
		CHECK_VK_RESULT(err, "AllocateDescriptorSets");

		if (err != VK_SUCCESS)
		{
			// On an error we can destroy the pool immediately because no command references it.
			vkDestroyDescriptorPool(mDevice->GetHandle(), descriptorPool, nullptr);
			return;
		}

		std::vector<uint32_t> freeSetIndices;
		freeSetIndices.reserve(mMaxSets);

		for (uint32_t i = 0; i < mMaxSets; ++i)
		{
			freeSetIndices.push_back(i);
		}

		mAvailableDescriptorPoolIndices.push_back(mDescriptorPools.size());
		mDescriptorPools.emplace_back(
			DescriptorPool{ descriptorPool, std::move(sets), std::move(freeSetIndices) });
	}

	void DescriptorSetAllocator::Deallocate(DescriptorSetAllocation* allocationInfo, bool usedInGraphicsQueue, bool usedInComputeQueue)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		assert(allocationInfo != nullptr);
		assert(allocationInfo->set != VK_NULL_HANDLE);

		// We can't reuse the descriptor set right away because the Vulkan spec says in the
		// documentation for vkCmdBindDescriptorSets that the set may be consumed any time between
		// host execution of the command and the end of the draw/dispatch.

		auto deallocation = new Deallocation();
		deallocation->poolIndex = allocationInfo->poolIndex;
		deallocation->setIndex = allocationInfo->setIndex;
		deallocation->refQueueCount = 0;


		auto deferredDeallocationFunc = [&](QueueType queueType)
			{
				deallocation->refQueueCount += 1;

				Queue* queue = checked_cast<Queue>(mDevice->GetQueue(queueType).Get());

				const uint64_t serial = queue->GetPendingSubmitSerial();

				assert(static_cast<uint32_t>(queueType) <= 1);

				mDeallocationsInQueues[static_cast<uint32_t>(queueType)].pendingDeallocations.Push(serial, deallocation);
				if (mDeallocationsInQueues[static_cast<uint32_t>(queueType)].lastDeallocationSerial != serial)
				{
					mDeallocationsInQueues[static_cast<uint32_t>(queueType)].lastDeallocationSerial = serial;
					queue->EnqueueDeferredDeallocation(this);
				}
			};

		if (usedInGraphicsQueue)
		{
			deferredDeallocationFunc(QueueType::Graphics);
		}
			
		if (usedInComputeQueue)
		{
			deferredDeallocationFunc(QueueType::Compute);
		}

		// we can deallocate it right now
		if (deallocation->refQueueCount == 0)
		{
			auto& freeSetIndices = mDescriptorPools[deallocation->poolIndex].freeSetIndices;
			if (freeSetIndices.empty())
			{
				mAvailableDescriptorPoolIndices.emplace_back(deallocation->poolIndex);
			}
			freeSetIndices.emplace_back(deallocation->setIndex);
		}

		// Clear the content of allocation so that use after frees are more visible.
		*allocationInfo = {};
	}

	void DescriptorSetAllocator::FinishDeallocation(Queue* queue, uint64_t completedSerial)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		for (Deallocation* dealloc : mDeallocationsInQueues[static_cast<uint32_t>(queue->GetType())].pendingDeallocations.IterateUpTo(completedSerial))
		{
			assert(dealloc->poolIndex < mDescriptorPools.size());
			assert(dealloc->refQueueCount > 1);
			dealloc->refQueueCount -= 1;

			if (dealloc->refQueueCount == 0)
			{
				auto& freeSetIndices = mDescriptorPools[dealloc->poolIndex].freeSetIndices;
				if (freeSetIndices.empty())
				{
					mAvailableDescriptorPoolIndices.emplace_back(dealloc->poolIndex);
				}
				freeSetIndices.emplace_back(dealloc->setIndex);
			}

		}
		mDeallocationsInQueues[static_cast<uint32_t>(queue->GetType())].pendingDeallocations.ClearUpTo(completedSerial);
	}
}