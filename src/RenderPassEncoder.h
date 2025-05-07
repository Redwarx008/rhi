#pragma once

#include "RHIStruct.h"
#include "PassEncoder.h"
#include "SyncScopeUsageTracker.h"
#include "EncodingContext.h"

namespace rhi::impl
{
	class CommandEncoder;
	class RenderPipelineBase;
	class RenderPassEncoder : public PassEncoder
	{
	public:
		static Ref<RenderPassEncoder> Create(CommandEncoder* encoder, EncodingContext& encodingContext, SyncScopeUsageTracker&& usageTracker);

		void APISetPipeline(RenderPipelineBase* pipeline);
		void APISetVertexBuffers(uint32_t firstSlot, uint32_t bufferCount, BufferBase* const* buffers, uint64_t* offsets = nullptr);
		void APISetIndexBuffer(BufferBase* buffer, IndexFormat indexFormat, uint64_t offset = 0, uint64_t size = CWholeSize);
		void APISetScissorRect(uint32_t firstScissor, const Rect* scissors, uint32_t scissorCount);
		void APISetStencilReference(uint32_t reference);
		void APISetBlendConstant(const Color& blendConstants);
		void APISetViewport(uint32_t firstViewport, Viewport const* viewports, uint32_t viewportCount);
		void APISetBindSet(BindSetBase* set, uint32_t setIndex, uint32_t dynamicOffsetCount = 0, const uint32_t* dynamicOffsets = nullptr);
		void APIDraw(uint32_t vertexCount, uint32_t instanceCount  = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
		void APIDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t baseVertex, uint32_t firstInstance);
		void APIDrawIndirect(BufferBase* indirectBuffer, uint64_t indirectOffset);
		void APIDrawIndexedIndirect(BufferBase* indirectBuffer, uint64_t indirectOffset);
		void APIMultiDrawIndirect(BufferBase* indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, BufferBase* drawCountBuffer = nullptr, uint64_t drawCountBufferOffset = 0);
		void APIMultiDrawIndexedIndirect(BufferBase* indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, BufferBase* drawCountBuffer = nullptr, uint64_t drawCountBufferOffset = 0);
		void APIEnd();
	protected:
		explicit RenderPassEncoder(CommandEncoder* encoder, EncodingContext& encodingContext, SyncScopeUsageTracker&& usageTracker);
		~RenderPassEncoder();
		SyncScopeUsageTracker mUsageTracker;
	};
}