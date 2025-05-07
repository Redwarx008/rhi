#pragma once

#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
	struct VulkanExtFunctions
	{
		PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
		PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
		PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
	};
}