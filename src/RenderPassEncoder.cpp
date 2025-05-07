#include "RenderPassEncoder.h"
#include "CommandEncoder.h"
#include "RenderPipelinebase.h"
#include "BufferBase.h"
#include "common/Error.h"


namespace rhi::impl
{
	RenderPassEncoder::RenderPassEncoder(CommandEncoder* encoder, EncodingContext& encodingContext, SyncScopeUsageTracker&& usageTracker) :
		PassEncoder(encoder, encodingContext),
		mUsageTracker(std::move(usageTracker))
	{

	}

	Ref<RenderPassEncoder> RenderPassEncoder::Create(CommandEncoder* encoder, EncodingContext& encodingContext, SyncScopeUsageTracker&& usageTracker)
	{
		Ref<RenderPassEncoder> renderPassEncoder = AcquireRef(new RenderPassEncoder(encoder, encodingContext, std::move(usageTracker)));
		return renderPassEncoder;
	}



	RenderPassEncoder::~RenderPassEncoder()
	{
		if (!mIsEnded)
		{
			APIEnd();
		}
	}

	void RenderPassEncoder::APISetPipeline(RenderPipelineBase* pipeline)
	{
		ASSERT(pipeline != nullptr);

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		SetRenderPipelineCmd* cmd = allocator.Allocate<SetRenderPipelineCmd>(Command::SetRenderPipeline);
		cmd->pipeline = pipeline;

		mLastPipeline = pipeline;
	}

	void RenderPassEncoder::APISetVertexBuffers(uint32_t firstSlot, uint32_t bufferCount, BufferBase* const * buffers, uint64_t* offsets)
	{
		ASSERT(bufferCount <= cMaxVertexBuffers);
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		SetVertexBufferCmd* cmd = allocator.Allocate<SetVertexBufferCmd>(Command::SetVertexBuffer);
		for (uint32_t i = 0; i < bufferCount; ++i)
		{
			ASSERT(buffers[i] != nullptr);
			ASSERT(HasFlag(buffers[i]->APIGetUsage(), BufferUsage::Vertex));
			VertexBuffer& vertexBuffer = cmd->buffers[i];
			vertexBuffer.buffer = buffers[i];
			vertexBuffer.offset = offsets == nullptr ? 0ull : offsets[i];

			mUsageTracker.BufferUsedAs(buffers[i], BufferUsage::Vertex);
		}
		cmd->firstSlot = firstSlot;
		cmd->bufferCount = bufferCount;
	}

	void RenderPassEncoder::APISetIndexBuffer(BufferBase* buffer, IndexFormat indexFormat, uint64_t offset, uint64_t size)
	{
		ASSERT(buffer != nullptr);
		ASSERT(HasFlag(buffer->APIGetUsage(), BufferUsage::Index));

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		SetIndexBufferCmd* cmd = allocator.Allocate<SetIndexBufferCmd>(Command::SetIndexBuffer);
		cmd->buffer = buffer;
		cmd->format = indexFormat;
		cmd->offset = offset;

		mUsageTracker.BufferUsedAs(buffer, BufferUsage::Index);
	}

	void RenderPassEncoder::APISetScissorRect(uint32_t firstScissor, const Rect* scissors, uint32_t scissorCount)
	{
		ASSERT(firstScissor + scissorCount <= cMaxColorAttachments);
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		SetScissorRectsCmd* cmd = allocator.Allocate<SetScissorRectsCmd>(Command::SetScissorRects);
		for (uint32_t i = 0; i < scissorCount; ++i)
		{
			cmd->scissors[i] = scissors[i];
		}
		cmd->firstScissor = firstScissor;
		cmd->scissorCount = scissorCount;
	}

	void RenderPassEncoder::APISetStencilReference(uint32_t reference)
	{
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		SetStencilReferenceCmd* cmd = allocator.Allocate<SetStencilReferenceCmd>(Command::SetStencilReference);
		cmd->reference = reference;
	}

	void RenderPassEncoder::APISetBlendConstant(const Color& blendConstants)
	{
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		SetBlendConstantCmd* cmd = allocator.Allocate<SetBlendConstantCmd>(Command::SetBlendConstant);
		cmd->color = blendConstants;
	}

	void RenderPassEncoder::APISetViewport(uint32_t firstViewport, Viewport const* viewports, uint32_t viewportCount)
	{
		ASSERT(viewportCount < cMaxViewports);
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		SetViewportCmd* cmd = allocator.Allocate<SetViewportCmd>(Command::SetViewport);
		for (uint32_t i = 0; i < viewportCount; ++i)
		{
			cmd->viewports[i] = viewports[i];
		}
		cmd->viewportCount = viewportCount;
		cmd->firstViewport = firstViewport;
	}


	void RenderPassEncoder::APISetBindSet(BindSetBase* set, uint32_t setIndex, uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets)
	{
		ASSERT(set != nullptr);
		RecordSetBindSet(set, setIndex, dynamicOffsetCount, dynamicOffsets);
		mUsageTracker.AddBindSet(set);
	}

	void RenderPassEncoder::APIDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		DrawCmd* draw = allocator.Allocate<DrawCmd>(Command::Draw);
		draw->vertexCount = vertexCount;
		draw->instanceCount = instanceCount;
		draw->firstVertex = firstVertex;
		draw->firstInstance = firstInstance;
	}

	void RenderPassEncoder::APIDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t baseVertex, uint32_t firstInstance)
	{
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		DrawIndexedCmd* draw = allocator.Allocate<DrawIndexedCmd>(Command::DrawIndexed);
		draw->indexCount = indexCount;
		draw->instanceCount = instanceCount;
		draw->firstIndex = firstIndex;
		draw->baseVertex = baseVertex;
		draw->firstInstance = firstInstance;
	}

	void RenderPassEncoder::APIDrawIndirect(BufferBase* indirectBuffer, uint64_t indirectOffset)
	{
		ASSERT(indirectBuffer != nullptr);
		ASSERT(HasFlag(indirectBuffer->APIGetUsage(), BufferUsage::Indirect));
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		DrawIndirectCmd* cmd = allocator.Allocate<DrawIndirectCmd>(Command::DrawIndirect);
		cmd->indirectBuffer = indirectBuffer;
		cmd->indirectOffset = indirectOffset;
		mUsageTracker.BufferUsedAs(indirectBuffer, BufferUsage::Indirect);
	}


	void RenderPassEncoder::APIDrawIndexedIndirect(BufferBase* indirectBuffer, uint64_t indirectOffset)
	{
		ASSERT(indirectBuffer != nullptr);
		ASSERT(HasFlag(indirectBuffer->APIGetUsage(), BufferUsage::Indirect));
		INVALID_IF(indirectOffset % 4 != 0, "Indirect offset (%u) is not a multiple of 4.", indirectOffset);
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		DrawIndexedIndirectCmd* cmd = allocator.Allocate<DrawIndexedIndirectCmd>(Command::DrawIndexedIndirect);
		cmd->indirectBuffer = indirectBuffer;
		cmd->indirectOffset = indirectOffset;
		mUsageTracker.BufferUsedAs(indirectBuffer, BufferUsage::Indirect);
	}

	void RenderPassEncoder::APIMultiDrawIndirect(BufferBase* indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, BufferBase* drawCountBuffer, uint64_t drawCountBufferOffset)
	{
		ASSERT(indirectBuffer != nullptr);
		ASSERT(HasFlag(indirectBuffer->APIGetUsage(), BufferUsage::Indirect));
		INVALID_IF(drawCountBuffer && !HasFlag(drawCountBuffer->APIGetUsage(), BufferUsage::Indirect), "drawCountBuffer must has Indirect usage.");


		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		MultiDrawIndirectCmd* cmd = allocator.Allocate<MultiDrawIndirectCmd>(Command::MultiDrawIndirect);
		cmd->indirectBuffer = indirectBuffer;
		cmd->indirectOffset = indirectOffset;
		cmd->maxDrawCount = maxDrawCount;
		cmd->drawCountBuffer = drawCountBuffer;
		cmd->drawCountOffset = drawCountBufferOffset;
		mUsageTracker.BufferUsedAs(indirectBuffer, BufferUsage::Indirect);
		if (drawCountBuffer)
		{
			mUsageTracker.BufferUsedAs(drawCountBuffer, BufferUsage::Indirect);
		}
	}

	void RenderPassEncoder::APIMultiDrawIndexedIndirect(BufferBase* indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, BufferBase* drawCountBuffer, uint64_t drawCountBufferOffset)
	{
		ASSERT(indirectBuffer != nullptr);
		ASSERT(HasFlag(indirectBuffer->APIGetUsage(), BufferUsage::Indirect));
		INVALID_IF(drawCountBuffer && !HasFlag(drawCountBuffer->APIGetUsage(), BufferUsage::Indirect), "drawCountBuffer must has Indirect usage.");


		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		MultiDrawIndexedIndirectCmd* cmd = allocator.Allocate<MultiDrawIndexedIndirectCmd>(Command::MultiDrawIndirect);
		cmd->indirectBuffer = indirectBuffer;
		cmd->indirectOffset = indirectOffset;
		cmd->maxDrawCount = maxDrawCount;
		cmd->drawCountBuffer = drawCountBuffer;
		cmd->drawCountOffset = drawCountBufferOffset;
		mUsageTracker.BufferUsedAs(indirectBuffer, BufferUsage::Indirect);
		if (drawCountBuffer)
		{
			mUsageTracker.BufferUsedAs(drawCountBuffer, BufferUsage::Indirect);
		}
	}

	void RenderPassEncoder::APIEnd()
	{
		mIsEnded = true;
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		EndRenderPassCmd* cmd = allocator.Allocate<EndRenderPassCmd>(Command::EndRenderPass);
		mEncodingContext.ExitRenderPass(mUsageTracker);
		mCommandEncoder->OnRenderPassEnd();
	}

}