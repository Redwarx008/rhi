#pragma once

#include "CommandRecordContextVk.h"
#include "VkResourceDeleter.h"
#include "../QueueBase.h"
#include "../common/RefCounted.h"
#include "../common/Ref.hpp"
#include "../common/MutexProtected.hpp"
#include "../common/SerialQueue.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>


namespace rhi::impl::vulkan
{
	class Device;
	class DescriptorSetAllocator;

	class Queue final : public QueueBase
	{
	public:
		static Ref<Queue> Create(Device* device, uint32_t family, QueueType type);
		~Queue();
		// internal 
		CommandRecordContext* GetPendingRecordingContext();
		MutexProtected<VkResourceDeleter>& GetDeleter();
		Device* GetDevice() const;
		uint32_t GetQueueFamilyIndex() const;
		VkQueue GetHandle() const;
		VkSemaphore GetTrackingSubmitSemaphore() const;
		void EnqueueDeferredDeallocation(DescriptorSetAllocator* allocator);
		void SubmitPendingCommands();
	private:
		explicit Queue(Device* device, uint32_t family, QueueType type);
		void Initialize();
		void TickImpl(uint64_t completedSerial) override;
		uint64_t SubmitImpl(CommandListBase* const* commands, uint32_t commandListCount, ResourceTransfer const* transfers, uint32_t transferCount) override;
		uint64_t QueryCompletedSerial() override;
		void MarkRecordingContextIsUsed() override;
		void CopyFromStagingToBufferImpl(BufferBase* src, uint64_t srcOffset, BufferBase* dst, uint64_t destOffset, uint64_t size) override;
		void CopyFromStagingToTextureImpl(BufferBase* src, const TextureSlice& dst, const TextureDataLayout& dataLayout) override;
		void WaitForImpl(QueueBase* queue, uint64_t submitSerial) override;
		void RecycleCompletedCommandBuffer(uint64_t completedSerial);
		void SetTrackingSubmitSemaphore();
		CommandPoolAndBuffer GetOrCreateCommandPoolAndBuffer();
		void NextRecordingContext();

		uint32_t mQueueFamilyIndex;

		VkQueue mHandle = VK_NULL_HANDLE;

		VkSemaphore mTrackingSubmitSemaphore = VK_NULL_HANDLE;

		CommandRecordContext mRecordContext;

		SerialQueue<uint64_t, CommandPoolAndBuffer> mCommandBufferInFlight;

		std::vector<CommandPoolAndBuffer> mUnusedCommandBuffer;

		MutexProtected<VkResourceDeleter> mDeleter;

		MutexProtected<SerialQueue<uint64_t, Ref<DescriptorSetAllocator>>> mDescriptorAllocatorsPendingDeallocation;
	};
}

