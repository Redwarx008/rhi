#pragma once

#include "../common/RefCounted.h"
#include "../common/SerialQueue.hpp"
#include "../common/Ref.hpp"
#include "DescriptorSetAllocation.h"

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <mutex>
#include <array>

namespace rhi::impl::vulkan
{
	class Queue;
	class Device;
	class BindSetLayout;

	class DescriptorSetAllocator : public RefCounted
	{
	public:
		static Ref<DescriptorSetAllocator> Create(Device* device, std::unordered_map<VkDescriptorType, uint32_t>&& descriptorCountPerType);
		DescriptorSetAllocation Allocate(BindSetLayout* layout);
		void Deallocate(DescriptorSetAllocation* allocationInfo, bool usedInGraphicsQueue, bool usedInComputeQueue);
		void FinishDeallocation(Queue* queue, uint64_t completedSerial);
	private:
		explicit DescriptorSetAllocator(Device* device, std::unordered_map<VkDescriptorType, uint32_t>&& descriptorCountPerType);
		~DescriptorSetAllocator();

		void AllocateDescriptorPool(BindSetLayout* layout);

		std::vector<VkDescriptorPoolSize> mPoolSizes;
		uint32_t mMaxSets;

		struct DescriptorPool
		{
			VkDescriptorPool vkPool;
			std::vector<VkDescriptorSet> sets;
			std::vector<uint32_t> freeSetIndices;
		};

		std::vector<uint32_t> mAvailableDescriptorPoolIndices;
		std::vector<DescriptorPool> mDescriptorPools;

		struct Deallocation
		{
			uint32_t poolIndex;
			uint32_t setIndex;
			uint32_t refQueueCount;
		};

		struct DeallocationsInQueue
		{
			SerialQueue<uint64_t, Deallocation*> pendingDeallocations;
			uint64_t lastDeallocationSerial = 0;
		};

		std::array<DeallocationsInQueue, 2> mDeallocationsInQueues;

		std::mutex mMutex;
		Device* mDevice;
	};
}