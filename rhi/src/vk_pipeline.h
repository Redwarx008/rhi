#pragma once

#include <vulkan/vulkan.h>

#include "rhi/rhi.h"

namespace rhi
{
	struct ContextVk;

	class GraphicsPipelineVk final : public IGraphicsPipeline
	{
	public:
		explicit GraphicsPipelineVk(const ContextVk& context)
			:m_Context(context){}
		~GraphicsPipelineVk();
		const GraphicsPipelineDesc& getDesc() const override  { return desc; }
		bool getPipelineCacheData(void* pData, size_t* pDataSize) const override;
		Object getNativeObject(NativeObjectType type) const override;

		GraphicsPipelineDesc desc;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		VkPipelineCache pipelineCache;
	private:
		const ContextVk& m_Context;
	};

	class ComputePipelineVk final : public IComputePipeline
	{
	public:
		explicit ComputePipelineVk(const ContextVk& contex)
			:m_Context(contex) {}
		~ComputePipelineVk();
		bool getPipelineCacheData(void* pData, size_t* pDataSize) const override;
		Object getNativeObject(NativeObjectType type) const override;

		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		VkPipelineCache pipelineCache;
	private:
		const ContextVk& m_Context;
	};

	VkCullModeFlags convertCullMode(CullMode mode);
	VkPolygonMode convertPolygonMode(PolygonMode mode);
	VkBlendFactor convertBlendFactor(BlendFactor factor);
	VkBlendOp convertBlendOp(BlendOp op);
	VkColorComponentFlags convertColorMask(ColorMask mask);
	VkCompareOp convertCompareOp(CompareOp op);
	VkStencilOpState convertStencilOpState(StencilOpState stencilOpState);
	VkPrimitiveTopology convertPrimitiveTopology(PrimitiveType type);
	VkViewport convertViewport(const Viewport& viewport);
}