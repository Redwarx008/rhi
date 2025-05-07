#pragma once

#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
	struct DescriptorSetAllocation
	{
		VkDescriptorSet set;
		uint32_t poolIndex;
		uint32_t setIndex;
	};
}