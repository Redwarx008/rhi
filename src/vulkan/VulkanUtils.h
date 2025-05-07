#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include "../RHIStruct.h"
#include "../Subresource.h"

namespace rhi::impl::vulkan
{

	template<typename T>
	struct ObjectTypeTraits;

#define DEFINE_OBJECT_TYPE_TRAIT(handleType, objectType) \
    template<> \
    struct ObjectTypeTraits<handleType> { \
        static constexpr VkObjectType value = objectType; \
    }

	DEFINE_OBJECT_TYPE_TRAIT(VkInstance, VK_OBJECT_TYPE_INSTANCE);
	DEFINE_OBJECT_TYPE_TRAIT(VkPhysicalDevice, VK_OBJECT_TYPE_PHYSICAL_DEVICE);
	DEFINE_OBJECT_TYPE_TRAIT(VkDevice, VK_OBJECT_TYPE_DEVICE);
	DEFINE_OBJECT_TYPE_TRAIT(VkQueue, VK_OBJECT_TYPE_QUEUE);
	DEFINE_OBJECT_TYPE_TRAIT(VkSemaphore, VK_OBJECT_TYPE_SEMAPHORE);
	DEFINE_OBJECT_TYPE_TRAIT(VkCommandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER);
	DEFINE_OBJECT_TYPE_TRAIT(VkFence, VK_OBJECT_TYPE_FENCE);
	DEFINE_OBJECT_TYPE_TRAIT(VkDeviceMemory, VK_OBJECT_TYPE_DEVICE_MEMORY);
	DEFINE_OBJECT_TYPE_TRAIT(VkBuffer, VK_OBJECT_TYPE_BUFFER);
	DEFINE_OBJECT_TYPE_TRAIT(VkImage, VK_OBJECT_TYPE_IMAGE);
	DEFINE_OBJECT_TYPE_TRAIT(VkEvent, VK_OBJECT_TYPE_EVENT);
	DEFINE_OBJECT_TYPE_TRAIT(VkQueryPool, VK_OBJECT_TYPE_QUERY_POOL);
	DEFINE_OBJECT_TYPE_TRAIT(VkBufferView, VK_OBJECT_TYPE_BUFFER_VIEW);
	DEFINE_OBJECT_TYPE_TRAIT(VkImageView, VK_OBJECT_TYPE_IMAGE_VIEW);
	DEFINE_OBJECT_TYPE_TRAIT(VkShaderModule, VK_OBJECT_TYPE_SHADER_MODULE);
	DEFINE_OBJECT_TYPE_TRAIT(VkPipelineCache, VK_OBJECT_TYPE_PIPELINE_CACHE);
	DEFINE_OBJECT_TYPE_TRAIT(VkPipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT);
	DEFINE_OBJECT_TYPE_TRAIT(VkRenderPass, VK_OBJECT_TYPE_RENDER_PASS);
	DEFINE_OBJECT_TYPE_TRAIT(VkPipeline, VK_OBJECT_TYPE_PIPELINE);
	DEFINE_OBJECT_TYPE_TRAIT(VkDescriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT);
	DEFINE_OBJECT_TYPE_TRAIT(VkSampler, VK_OBJECT_TYPE_SAMPLER);
	DEFINE_OBJECT_TYPE_TRAIT(VkDescriptorPool, VK_OBJECT_TYPE_DESCRIPTOR_POOL);
	DEFINE_OBJECT_TYPE_TRAIT(VkDescriptorSet, VK_OBJECT_TYPE_DESCRIPTOR_SET);
	DEFINE_OBJECT_TYPE_TRAIT(VkFramebuffer, VK_OBJECT_TYPE_FRAMEBUFFER);
	DEFINE_OBJECT_TYPE_TRAIT(VkCommandPool, VK_OBJECT_TYPE_COMMAND_POOL);

#undef DEFINE_OBJECT_TYPE_TRAIT

	class Device;
	class Texture;

	void SetDebugNameInternal(Device* device, VkObjectType objectType, uint64_t objectHandle, const char* prefix, std::string_view name);

	template <typename HandleType>
	void SetDebugName(Device* device, HandleType handle, const char* prefix, std::string_view name)
	{
		SetDebugNameInternal(device, ObjectTypeTraits<HandleType>::value, reinterpret_cast<uint64_t>(handle), prefix, name);
	}

	VkCompareOp CompareOpConvert(CompareOp op);

	VkShaderStageFlags ShaderStageFlagsConvert(ShaderStage stages);

	VkShaderStageFlagBits ShaderStageFlagBitsConvert(ShaderStage stage);

	VkImageAspectFlags ImageAspectFlagsConvert(Aspect aspects);

	VkBufferImageCopy ComputeBufferImageCopyRegion(const TextureDataLayout& dataLayout, const Extent3D& copySize, Texture* texture, uint32_t mipLevel, Origin3D origin, Aspect aspect);
}
