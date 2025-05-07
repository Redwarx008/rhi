#include "RenderPipelineVk.h"

#include "DeviceVk.h"
#include "TextureVk.h"
#include "ShaderModuleVk.h"
#include "PipelineLayoutVk.h"
#include "ErrorsVk.h"
#include "VulkanUtils.h"
#include "../common/EnumFlagIterator.hpp"
#include <vector>

namespace rhi::impl::vulkan
{
	RenderPipeline::RenderPipeline(Device* device, const RenderPipelineDesc& desc) :
		RenderPipelineBase(device, desc)
	{

	}

	RenderPipeline::~RenderPipeline() {}

	VkPrimitiveTopology PrimitiveTopologyConvert(PrimitiveType type)
	{
		switch (type)
		{
		case rhi::impl::PrimitiveType::PointList:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case rhi::impl::PrimitiveType::LineList:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case rhi::impl::PrimitiveType::LineStrip:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case rhi::impl::PrimitiveType::TriangleList:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case rhi::impl::PrimitiveType::TriangleStrip:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case rhi::impl::PrimitiveType::TriangleFan:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		case rhi::impl::PrimitiveType::PatchList:
			return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		default:
			ASSERT(!"Unreachable");
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		}
	}

	VkCullModeFlagBits CullModeConvert(CullMode mode)
	{
		switch (mode)
		{
		case rhi::impl::CullMode::None:
			return VK_CULL_MODE_NONE;
		case rhi::impl::CullMode::Front:
			return VK_CULL_MODE_FRONT_BIT;
		case rhi::impl::CullMode::Back:
			return VK_CULL_MODE_BACK_BIT;
		default:
			ASSERT(!"Unreachable");
			break;
		}
	}

	VkFrontFace FrontFaceConvert(FrontFace face)
	{
		switch (face)
		{
		case rhi::impl::FrontFace::FrontCounterClockwise:
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		case rhi::impl::FrontFace::FrontClockwise:
			return VK_FRONT_FACE_CLOCKWISE;
		default:
			ASSERT(!"Unreachable");
			break;
		}
	}

	VkPolygonMode PolygonModeConvert(FillMode mode)
	{
		switch (mode)
		{
		case rhi::impl::FillMode::Fill:
			return VK_POLYGON_MODE_FILL;
		case rhi::impl::FillMode::Line:
			return VK_POLYGON_MODE_LINE;
		case rhi::impl::FillMode::Point:
			return VK_POLYGON_MODE_POINT;
		default:
			ASSERT(!"Unreachable");
			break;
		}
	}

	VkBlendFactor BlendFactorConvert(BlendFactor factor)
	{
		static_assert(uint32_t(BlendFactor::One) == uint32_t(VK_BLEND_FACTOR_ONE));
		static_assert(uint32_t(BlendFactor::Zero) == uint32_t(VK_BLEND_FACTOR_ZERO));
		static_assert(uint32_t(BlendFactor::SrcColor) == uint32_t(VK_BLEND_FACTOR_SRC_COLOR));
		static_assert(uint32_t(BlendFactor::OneMinusSrcColor) == uint32_t(VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR));
		static_assert(uint32_t(BlendFactor::DstColor) == uint32_t(VK_BLEND_FACTOR_DST_COLOR));
		static_assert(uint32_t(BlendFactor::OneMinusDstColor) == uint32_t(VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR));
		static_assert(uint32_t(BlendFactor::SrcAlpha) == uint32_t(VK_BLEND_FACTOR_SRC_ALPHA));
		static_assert(uint32_t(BlendFactor::OneMinusSrcAlpha) == uint32_t(VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA));
		static_assert(uint32_t(BlendFactor::DstAlpha) == uint32_t(VK_BLEND_FACTOR_DST_ALPHA));
		static_assert(uint32_t(BlendFactor::OneMinusDstAlpha) == uint32_t(VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA));
		static_assert(uint32_t(BlendFactor::ConstantColor) == uint32_t(VK_BLEND_FACTOR_CONSTANT_COLOR));
		static_assert(uint32_t(BlendFactor::OneMinusConstantColor) == uint32_t(VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR));
		static_assert(uint32_t(BlendFactor::ConstantAlpha) == uint32_t(VK_BLEND_FACTOR_CONSTANT_ALPHA));
		static_assert(uint32_t(BlendFactor::OneMinusConstantAlpha) == uint32_t(VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA));
		static_assert(uint32_t(BlendFactor::SrcAlphaSaturate) == uint32_t(VK_BLEND_FACTOR_SRC_ALPHA_SATURATE));
		static_assert(uint32_t(BlendFactor::Src1Color) == uint32_t(VK_BLEND_FACTOR_SRC1_COLOR));
		static_assert(uint32_t(BlendFactor::OneMinusSrc1Color) == uint32_t(VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR));
		static_assert(uint32_t(BlendFactor::Src1Alpha) == uint32_t(VK_BLEND_FACTOR_SRC1_ALPHA));
		static_assert(uint32_t(BlendFactor::OneMinusSrc1Alpha) == uint32_t(VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA));

		return static_cast<VkBlendFactor>(uint32_t(factor));
	}

	VkBlendOp BlendOpConvert(BlendOp op)
	{
		static_assert(uint32_t(BlendOp::Add) == uint32_t(VK_BLEND_OP_ADD));
		static_assert(uint32_t(BlendOp::Subrtact) == uint32_t(VK_BLEND_OP_SUBTRACT));
		static_assert(uint32_t(BlendOp::ReverseSubtract) == uint32_t(VK_BLEND_OP_REVERSE_SUBTRACT));
		static_assert(uint32_t(BlendOp::Min) == uint32_t(VK_BLEND_OP_MIN));
		static_assert(uint32_t(BlendOp::Max) == uint32_t(VK_BLEND_OP_MAX));

		return static_cast<VkBlendOp>(uint32_t(op));
	}

	VkColorComponentFlags ColorMaskConvert(ColorMask mask)
	{
		static_assert(uint32_t(ColorMask::Red) == uint32_t(VK_COLOR_COMPONENT_R_BIT));
		static_assert(uint32_t(ColorMask::Green) == uint32_t(VK_COLOR_COMPONENT_G_BIT));
		static_assert(uint32_t(ColorMask::Blue) == uint32_t(VK_COLOR_COMPONENT_B_BIT));
		static_assert(uint32_t(ColorMask::Alpha) == uint32_t(VK_COLOR_COMPONENT_A_BIT));
		static_assert(uint32_t(ColorMask::All) == uint32_t(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT));

		return static_cast<VkColorComponentFlags>(uint32_t(mask));
	}

	VkStencilOp StencilOpConvert(StencilOp op)
	{
		static_assert(uint32_t(StencilOp::Keep) == uint32_t(VK_STENCIL_OP_KEEP));
		static_assert(uint32_t(StencilOp::Zero) == uint32_t(VK_STENCIL_OP_ZERO));
		static_assert(uint32_t(StencilOp::Replace) == uint32_t(VK_STENCIL_OP_REPLACE));
		static_assert(uint32_t(StencilOp::IncrementAndClamp) == uint32_t(VK_STENCIL_OP_INCREMENT_AND_CLAMP));
		static_assert(uint32_t(StencilOp::DecrementAndClamp) == uint32_t(VK_STENCIL_OP_DECREMENT_AND_CLAMP));
		static_assert(uint32_t(StencilOp::Invert) == uint32_t(VK_STENCIL_OP_INVERT));
		static_assert(uint32_t(StencilOp::IncrementAndWrap) == uint32_t(VK_STENCIL_OP_INCREMENT_AND_WRAP));
		static_assert(uint32_t(StencilOp::DecrementAndWrap) == uint32_t(VK_STENCIL_OP_DECREMENT_AND_WRAP));

		return static_cast<VkStencilOp>(uint32_t(op));
	}

	VkStencilOpState StencilOpStateConvert(StencilOpState stencilOpState)
	{
		VkStencilOpState res{};
		res.failOp = StencilOpConvert(stencilOpState.failOp);
		res.passOp = StencilOpConvert(stencilOpState.passOp);
		res.depthFailOp = StencilOpConvert(stencilOpState.depthFailOp);
		res.compareOp = CompareOpConvert(stencilOpState.compareOp);
		res.compareMask = stencilOpState.compareMak;
		res.writeMask = stencilOpState.writeMask;
		res.reference = stencilOpState.referenceValue;
		return res;
	}

	VkFormat VertexFormatConvert(VertexFormat format)
	{
		switch (format)
		{
		case rhi::impl::VertexFormat::Uint8:
			return VK_FORMAT_R8_UINT;
		case rhi::impl::VertexFormat::Uint8x2:
			return VK_FORMAT_R8G8_UINT;
		case rhi::impl::VertexFormat::Uint8x4:
			return VK_FORMAT_R8G8B8A8_UINT;
		case rhi::impl::VertexFormat::Sint8:
			return VK_FORMAT_R8_SINT;
		case rhi::impl::VertexFormat::Sint8x2:
			return VK_FORMAT_R8G8_SINT;
		case rhi::impl::VertexFormat::Sint8x4:
			return VK_FORMAT_R8G8B8A8_SINT;
		case rhi::impl::VertexFormat::Unorm8:
			return VK_FORMAT_R8_UNORM;
		case rhi::impl::VertexFormat::Unorm8x2:
			return VK_FORMAT_R8G8_UNORM;
		case rhi::impl::VertexFormat::Unorm8x4:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case rhi::impl::VertexFormat::Snorm8:
			return VK_FORMAT_R8_SNORM;
		case rhi::impl::VertexFormat::Snorm8x2:
			return VK_FORMAT_R8G8_SNORM;
		case rhi::impl::VertexFormat::Snorm8x4:
			return VK_FORMAT_R8G8B8A8_SNORM;
		case rhi::impl::VertexFormat::Uint16:
			return VK_FORMAT_R16_UINT;
		case rhi::impl::VertexFormat::Uint16x2:
			return VK_FORMAT_R16G16_UINT;
		case rhi::impl::VertexFormat::Uint16x4:
			return VK_FORMAT_R16G16B16A16_UINT;
		case rhi::impl::VertexFormat::Sint16:
			return VK_FORMAT_R16_SINT;
		case rhi::impl::VertexFormat::Sint16x2:
			return VK_FORMAT_R16G16_SINT;
		case rhi::impl::VertexFormat::Sint16x4:
			return VK_FORMAT_R16G16B16A16_SINT;
		case rhi::impl::VertexFormat::Unorm16:
			return VK_FORMAT_R16_UNORM;
		case rhi::impl::VertexFormat::Unorm16x2:
			return VK_FORMAT_R16G16_UNORM;
		case rhi::impl::VertexFormat::Unorm16x4:
			return VK_FORMAT_R16G16B16A16_UNORM;
		case rhi::impl::VertexFormat::Snorm16:
			return VK_FORMAT_R16_SNORM;
		case rhi::impl::VertexFormat::Snorm16x2:
			return VK_FORMAT_R16G16_SNORM;
		case rhi::impl::VertexFormat::Snorm16x4:
			return VK_FORMAT_R16G16B16A16_SNORM;
		case rhi::impl::VertexFormat::Float16:
			return VK_FORMAT_R16_SFLOAT;
		case rhi::impl::VertexFormat::Float16x2:
			return VK_FORMAT_R16G16_SFLOAT;
		case rhi::impl::VertexFormat::Float16x4:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case rhi::impl::VertexFormat::Float32:
			return VK_FORMAT_R32_SFLOAT;
		case rhi::impl::VertexFormat::Float32x2:
			return VK_FORMAT_R32G32_SFLOAT;
		case rhi::impl::VertexFormat::Float32x3:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case rhi::impl::VertexFormat::Float32x4:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case rhi::impl::VertexFormat::Uint32:
			return VK_FORMAT_R32_UINT;
		case rhi::impl::VertexFormat::Uint32x2:
			return VK_FORMAT_R32G32_UINT;
		case rhi::impl::VertexFormat::Uint32x3:
			return VK_FORMAT_R32G32B32_UINT;
		case rhi::impl::VertexFormat::Uint32x4:
			return VK_FORMAT_R32G32B32A32_UINT;
		case rhi::impl::VertexFormat::Sint32:
			return VK_FORMAT_R32_SINT;
		case rhi::impl::VertexFormat::Sint32x2:
			return VK_FORMAT_R32G32_SINT;
		case rhi::impl::VertexFormat::Sint32x3:
			return VK_FORMAT_R32G32B32_SINT;
		case rhi::impl::VertexFormat::Sint32x4:
			return VK_FORMAT_R32G32B32A32_SINT;
		case rhi::impl::VertexFormat::Unorm10_10_10_2:
			return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
		default:
			ASSERT(!"Unreachable");
		}
	}

	bool RenderPipeline::Initialize()
	{
		// shader stage
		ASSERT(HasShaderStage(ShaderStage::Vertex));

		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
		std::vector<VkSpecializationInfo> specInfos;
		std::vector<VkSpecializationMapEntry> specMapEntries;
		std::vector<uint32_t> specDatas;

		// Sufficient space must be allocated in advance to prevent the vector from changing to a new address.
		uint32_t shaderStageCount = 0;
		uint32_t shaderWithSpecCount = 0;
		uint32_t specConstantCount = 0;
		for (ShaderStage stage : IterateEnumFlags(GetShaderStageMask()))
		{
			++shaderStageCount;
			const ShaderStageState& state = GetShaderStageState(stage);
			if (!state.constants.empty())
			{
				++shaderWithSpecCount;
				specConstantCount += state.constants.size();
			}
		}

		shaderStageCreateInfos.reserve(shaderStageCount);
		specInfos.reserve(shaderWithSpecCount);
		specMapEntries.reserve(specConstantCount);
		specDatas.reserve(specConstantCount);

		for (ShaderStage stage : IterateEnumFlags(GetShaderStageMask()))
		{
			const ShaderStageState& state = GetShaderStageState(stage);

			VkPipelineShaderStageCreateInfo& shaderStageCI = shaderStageCreateInfos.emplace_back();
			shaderStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCI.stage = ShaderStageFlagBitsConvert(stage);
			shaderStageCI.module = checked_cast<ShaderModule>(state.shaderModule.Get())->GetHandle();
			shaderStageCI.pName = state.shaderModule->GetEntry().data();

			if (!state.constants.empty())
			{
				// The vectors are pre-allocated, so it's safe to use .data() before writing the data.
				shaderStageCI.pSpecializationInfo = specInfos.data() + specInfos.size();

				VkSpecializationInfo& specInfo = specInfos.emplace_back();
				specInfo.pData = specDatas.data() + specDatas.size();
				specInfo.mapEntryCount = state.constants.size();
				specInfo.pMapEntries = specMapEntries.data() + specMapEntries.size();
				specInfo.dataSize = state.constants.size() * sizeof(uint32_t);

				uint32_t dataOffset = 0;
				for (auto& constant : state.constants)
				{
					VkSpecializationMapEntry& specMapEntry = specMapEntries.emplace_back();
					specMapEntry.constantID = constant.constantID;
					specMapEntry.offset = dataOffset;
					specMapEntry.size = sizeof(uint32_t);
					specDatas.push_back(constant.value.u);
					dataOffset += specMapEntry.size;
				}
			}
		}
		// vertex input 
		std::vector<VkVertexInputBindingDescription> vertexInputBindings;
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;

		constexpr int maxVertexInputBindings = 32;
		int bufferSlotUsed[maxVertexInputBindings]{};
		for (uint32_t i = 0; i < mVertexInputAttributes.size(); ++i)
		{
			auto& attribute = mVertexInputAttributes[i];

			uint32_t bufferSlot = attribute.bindingBufferSlot;
			if (bufferSlotUsed[bufferSlot] == 0)
			{
				auto& vertexInputBinding = vertexInputBindings.emplace_back();
				vertexInputBinding.binding = attribute.bindingBufferSlot;
				vertexInputBinding.stride = attribute.elementStride;
				vertexInputBinding.inputRate = attribute.rate == VertexInputRate::Vertex ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;

				bufferSlotUsed[bufferSlot]++;
			}

			auto& vertexInputAttribute = vertexInputAttributes.emplace_back();
			vertexInputAttribute.binding = attribute.bindingBufferSlot;
			vertexInputAttribute.location = attribute.location;
			vertexInputAttribute.format = VertexFormatConvert(attribute.format);
			vertexInputAttribute.offset = attribute.offsetInElement;
		}

		VkPipelineVertexInputStateCreateInfo vertexInputStateCI{};
		vertexInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCI.vertexBindingDescriptionCount = vertexInputBindings.size();
		vertexInputStateCI.pVertexBindingDescriptions = vertexInputBindings.data();
		vertexInputStateCI.vertexAttributeDescriptionCount = vertexInputAttributes.size();
		vertexInputStateCI.pVertexAttributeDescriptions = vertexInputAttributes.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI{};
		inputAssemblyStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCI.topology = PrimitiveTopologyConvert(mRasterState.primitiveType);
		// Rasterization state
		VkPipelineRasterizationStateCreateInfo rasterizationStateCI{};
		rasterizationStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCI.cullMode = CullModeConvert(mRasterState.cullMode);
		rasterizationStateCI.polygonMode = PolygonModeConvert(mRasterState.fillMode);
		rasterizationStateCI.depthClampEnable = mRasterState.depthClampEnable;
		rasterizationStateCI.depthBiasEnable = mDepthStencilState.depthBias != 0 || mDepthStencilState.depthBiasSlopeScale != 0;
		rasterizationStateCI.depthBiasConstantFactor = mDepthStencilState.depthBias;
		rasterizationStateCI.depthBiasSlopeFactor = mDepthStencilState.depthBiasSlopeScale;
		rasterizationStateCI.lineWidth = mRasterState.lineWidth;
		rasterizationStateCI.frontFace = FrontFaceConvert(mRasterState.frontFace);
		// blend state
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(mColorAttachmentFormats.size());
		for (uint32_t i = 0; i < colorBlendAttachmentStates.size(); ++i)
		{
			VkPipelineColorBlendAttachmentState& blendState = colorBlendAttachmentStates[i];
			blendState.blendEnable = mBlendState.colorAttachmentBlendStates[i].blendEnable;
			blendState.srcColorBlendFactor = BlendFactorConvert(mBlendState.colorAttachmentBlendStates[i].srcColorBlend);
			blendState.dstColorBlendFactor = BlendFactorConvert(mBlendState.colorAttachmentBlendStates[i].destColorBlend);
			blendState.colorBlendOp = BlendOpConvert(mBlendState.colorAttachmentBlendStates[i].colorBlendOp);
			blendState.srcAlphaBlendFactor = BlendFactorConvert(mBlendState.colorAttachmentBlendStates[i].srcAlphaBlend);
			blendState.dstAlphaBlendFactor = BlendFactorConvert(mBlendState.colorAttachmentBlendStates[i].destAlphaBlend);
			blendState.alphaBlendOp = BlendOpConvert(mBlendState.colorAttachmentBlendStates[i].alphaBlendOp);
			blendState.colorWriteMask = ColorMaskConvert(mBlendState.colorAttachmentBlendStates[i].colorWriteMask);

		}
		VkPipelineColorBlendStateCreateInfo colorBlendStateCI{};
		colorBlendStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCI.attachmentCount = colorBlendAttachmentStates.size();
		colorBlendStateCI.pAttachments = colorBlendAttachmentStates.data();
		// Depth and stencil state
		VkPipelineDepthStencilStateCreateInfo depthStencilStateCI{};
		depthStencilStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCI.depthTestEnable = mDepthStencilState.depthTestEnable;
		depthStencilStateCI.depthWriteEnable = mDepthStencilState.depthWriteEnable;
		depthStencilStateCI.depthCompareOp = CompareOpConvert(mDepthStencilState.depthCompareOp);
		depthStencilStateCI.stencilTestEnable = mDepthStencilState.stencilTestEnable;
		depthStencilStateCI.front = StencilOpStateConvert(mDepthStencilState.frontFaceStencil);
		depthStencilStateCI.back = StencilOpStateConvert(mDepthStencilState.backFaceStencil);
		// multi sample state
		VkPipelineMultisampleStateCreateInfo multisampleStateCI{};
		multisampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCI.alphaToCoverageEnable = mBlendState.alphaToCoverageEnable;
		multisampleStateCI.rasterizationSamples = SampleCountConvert(mSampleState.count);
		ASSERT(multisampleStateCI.rasterizationSamples <= 32);
		VkSampleMask sampleMask = mSampleState.mask;
		multisampleStateCI.pSampleMask = &sampleMask;
		multisampleStateCI.alphaToOneEnable = false;
		VkDynamicState dynamicStates[] = 
		{
			VK_DYNAMIC_STATE_VIEWPORT,     VK_DYNAMIC_STATE_SCISSOR,
			/*VK_DYNAMIC_STATE_LINE_WIDTH, */  VK_DYNAMIC_STATE_BLEND_CONSTANTS,
			VK_DYNAMIC_STATE_DEPTH_BOUNDS, VK_DYNAMIC_STATE_STENCIL_REFERENCE,
		};
		VkPipelineDynamicStateCreateInfo dynamicStateCI{};
		dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCI.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
		dynamicStateCI.pDynamicStates = dynamicStates;

		// Viewport state sets the number of viewports and scissor used in this pipeline
		// Note: This is actually overridden by the dynamic states 
		VkPipelineViewportStateCreateInfo viewportStateCI{};
		viewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCI.viewportCount = mViewportCount;
		viewportStateCI.scissorCount = mViewportCount;

		// Attachment information for dynamic rendering
		ASSERT(mColorAttachmentFormats.size() <= cMaxColorAttachments);

		VkFormat colorAttachmentFormats[cMaxColorAttachments];
		for (uint32_t i = 0; i < mColorAttachmentFormats.size(); ++i)
		{
			colorAttachmentFormats[i] = ToVkFormat(mColorAttachmentFormats[i]);
		}
		VkPipelineRenderingCreateInfo pipelineRenderingCI{};
		pipelineRenderingCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineRenderingCI.colorAttachmentCount = mColorAttachmentFormats.size();
		pipelineRenderingCI.pColorAttachmentFormats = colorAttachmentFormats;
		pipelineRenderingCI.depthAttachmentFormat = ToVkFormat(mDepthStencilFormat);
		pipelineRenderingCI.stencilAttachmentFormat = pipelineRenderingCI.depthAttachmentFormat;

		VkGraphicsPipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.layout = checked_cast<PipelineLayout>(mPipelineLayout.Get())->GetHandle();
		createInfo.stageCount = shaderStageCount;
		createInfo.pStages = shaderStageCreateInfos.data();
		createInfo.pVertexInputState = &vertexInputStateCI;
		createInfo.pInputAssemblyState = &inputAssemblyStateCI;
		createInfo.pViewportState = &viewportStateCI;
		createInfo.pRasterizationState = &rasterizationStateCI;
		createInfo.pColorBlendState = &colorBlendStateCI;
		createInfo.pMultisampleState = &multisampleStateCI;
		createInfo.pDepthStencilState = &depthStencilStateCI;
		createInfo.pDynamicState = &dynamicStateCI;
		createInfo.pNext = &pipelineRenderingCI;

		Device* device = checked_cast<Device>(mDevice.Get());
		// todo: apply pipeline cache 
		VkResult err = vkCreateGraphicsPipelines(device->GetHandle(), nullptr, 1, &createInfo, nullptr, &mHandle);
		CHECK_VK_RESULT_FALSE(err, "CreateGraphicsPipelines");

		SetDebugName(device, mHandle, "RenderPipeline", GetName());

		return true;
	}

	void RenderPipeline::DestroyImpl()
	{
		Device* device = checked_cast<Device>(mDevice.Get()); 

		if (mHandle != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(device->GetHandle(), mHandle, nullptr);
			mHandle = VK_NULL_HANDLE;
		}
	}

	VkPipeline RenderPipeline::GetHandle() const
	{
		return mHandle;
	}

	Ref<RenderPipeline> RenderPipeline::Create(Device* device, const RenderPipelineDesc& desc)
	{
		Ref<RenderPipeline> pipeline = AcquireRef(new RenderPipeline(device, desc));
		if (!pipeline->Initialize())
		{
			return nullptr;
		}
		return pipeline;
	}
}