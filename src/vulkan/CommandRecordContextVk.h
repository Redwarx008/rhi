#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <unordered_set>

namespace rhi::impl::vulkan
{
	class Buffer;

	struct CommandPoolAndBuffer
	{
		VkCommandBuffer bufferHandle = VK_NULL_HANDLE;
		VkCommandPool poolHandle = VK_NULL_HANDLE;
	};

	struct CommandRecordContext
	{
	public:
		CommandPoolAndBuffer commandBufferAndPool;

		bool needsSubmit = false;

		std::vector<VkSemaphoreSubmitInfo> waitSemaphoreSubmitInfos;
		std::vector<VkSemaphoreSubmitInfo> signalSemaphoreSubmitInfos;

		void AddBufferBarrier(VkBufferMemoryBarrier2 barrier);
		void AddTextureBarrier(VkImageMemoryBarrier2 barrier);
		void EmitBarriers();
		void Reset();
	private:
		std::vector<VkImageMemoryBarrier2> mImageMemoryBarriers;
		std::vector<VkBufferMemoryBarrier2> mBufferMemoryBarriers;
	};
}