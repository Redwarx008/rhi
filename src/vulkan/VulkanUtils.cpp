#include "VulkanUtils.h"

#include "DeviceVk.h"
#include "TextureVk.h"
#include "../common/Error.h"
#include <sstream>

namespace rhi::impl::vulkan
{
	void SetDebugNameInternal(Device* device, VkObjectType objectType, uint64_t objectHandle, const char* prefix, std::string_view name)
	{
		if (!objectHandle || !device)
		{
			return;
		}

		if (!device->IsDebugLayerEnabled())
		{
			return;
		}

		VkDebugUtilsObjectNameInfoEXT objectNameInfo{};
		objectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		objectNameInfo.pNext = nullptr;
		objectNameInfo.objectType = objectType;
		objectNameInfo.objectHandle = objectHandle;

		std::ostringstream objectNameStream;
		objectNameStream << prefix;
		if (!name.empty())
		{
			objectNameStream << "_" << name;
		}
		std::string objectName = objectNameStream.str();
		objectNameInfo.pObjectName = objectName.c_str();
		device->Fn.vkSetDebugUtilsObjectNameEXT(device->GetHandle(), &objectNameInfo);
	}

	VkCompareOp CompareOpConvert(CompareOp op)
	{
		switch (op)
		{
		case CompareOp::Never:
			return VK_COMPARE_OP_NEVER;
		case CompareOp::Less:
			return VK_COMPARE_OP_LESS;
		case CompareOp::Equal:
			return VK_COMPARE_OP_EQUAL;
		case CompareOp::LessOrEqual:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case CompareOp::Greater:
			return VK_COMPARE_OP_GREATER;
		case CompareOp::NotEqual:
			return VK_COMPARE_OP_NOT_EQUAL;
		case CompareOp::GreaterOrEqual:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case CompareOp::Always:
			return VK_COMPARE_OP_ALWAYS;
		default:
			break;
		}
		ASSERT(!"Unreachable");
	}

	VkShaderStageFlags ShaderStageFlagsConvert(ShaderStage stages)
	{
		VkShaderStageFlags flags = 0;

		if ((stages & ShaderStage::Vertex) != 0)
		{
			flags |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if ((stages & ShaderStage::Fragment) != 0)
		{
			flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if ((stages & ShaderStage::Compute) != 0)
		{
			flags |= VK_SHADER_STAGE_COMPUTE_BIT;
		}

		if ((stages & ShaderStage::TessellationControl) != 0)
		{
			flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		}

		if ((stages & ShaderStage::TessellationEvaluation) != 0)
		{
			flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		}

		if ((stages & ShaderStage::Geometry) != 0)
		{
			flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		}

		if ((stages & ShaderStage::Task) != 0)
		{
			flags |= VK_SHADER_STAGE_TASK_BIT_EXT;
		}

		if ((stages & ShaderStage::Mesh) != 0)
		{
			flags |= VK_SHADER_STAGE_MESH_BIT_EXT;
		}

		return flags;
	}

	VkShaderStageFlagBits ShaderStageFlagBitsConvert(ShaderStage stage)
	{
		switch (stage)
		{

		case ShaderStage::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderStage::TessellationControl:
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case ShaderStage::TessellationEvaluation:
			return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case ShaderStage::Geometry:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case ShaderStage::Fragment:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case ShaderStage::Task:
			return VK_SHADER_STAGE_TASK_BIT_EXT;
		case ShaderStage::Mesh:
			return VK_SHADER_STAGE_MESH_BIT_EXT;
		case ShaderStage::Compute:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		case ShaderStage::AllGraphics:
			return VK_SHADER_STAGE_ALL_GRAPHICS;
		case ShaderStage::All:
			return VK_SHADER_STAGE_ALL;
		case ShaderStage::None:
		default:
			ASSERT(!"Unreachable");
			break;
		}
	}

	VkImageAspectFlags ImageAspectFlagsConvert(Aspect aspects)
	{
		VkImageAspectFlags flags = 0;
		for (Aspect aspect : IterateEnumFlags(aspects))
		{
			switch (aspect)
			{

			case Aspect::Color:
				flags |= VK_IMAGE_ASPECT_COLOR_BIT;
				break;
			case Aspect::Depth:
				flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
				break;
			case Aspect::Stencil:
				flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
				break;
			case Aspect::Plane0:
				flags |= VK_IMAGE_ASPECT_PLANE_0_BIT;
				break;
			case Aspect::Plane1:
				flags |= VK_IMAGE_ASPECT_PLANE_1_BIT;
				break;
			case Aspect::Plane2:
				flags |= VK_IMAGE_ASPECT_PLANE_2_BIT;
				break;
			case Aspect::None:
			default:
				assert(!"UNREACHABLE");
			}
		}

		return flags;
	}

	VkBufferImageCopy ComputeBufferImageCopyRegion(const TextureDataLayout& dataLayout, const Extent3D& copySize, Texture* texture, uint32_t mipLevel, Origin3D origin, Aspect aspect)
	{
		VkBufferImageCopy region{};
		region.bufferOffset = dataLayout.offset;
		// In Vulkan the row length is in texels
		ASSERT(dataLayout.bytesPerRow % GetFormatInfo(texture->APIGetFormat()).bytesPerBlock == 0);
		region.bufferRowLength = dataLayout.bytesPerRow / GetFormatInfo(texture->APIGetFormat()).bytesPerBlock * GetFormatInfo(texture->APIGetFormat()).blockSize;
		region.bufferImageHeight = dataLayout.rowsPerImage * GetFormatInfo(texture->APIGetFormat()).blockSize;
		region.imageSubresource.aspectMask = ImageAspectFlagsConvert(aspect);
		region.imageSubresource.mipLevel = mipLevel;
		region.imageOffset.x = origin.x;
		region.imageOffset.y = origin.y;
		region.imageOffset.z = origin.z;
		region.imageExtent.width = copySize.width;
		region.imageExtent.height = copySize.height;
		region.imageExtent.depth = copySize.depthOrArrayLayers;
		region.imageSubresource.baseArrayLayer = origin.z;
		region.imageSubresource.layerCount = copySize.depthOrArrayLayers;
		return region;
	}


}