#pragma once

#include <vk_mem_alloc.h>

namespace rhi::impl::vulkan
{
	struct BufferAllocation
	{
		VkBuffer buffer;
		VmaAllocation allocation;
	};

	struct ImageAllocation
	{
		VkImage image;
		VmaAllocation allocation;
	};
}