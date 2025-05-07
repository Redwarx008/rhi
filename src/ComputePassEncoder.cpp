#include "ComputePassEncoder.h"
#include "ComputePipelineBase.h"
#include "CommandEncoder.h"
#include "Commands.h"
#include "common/Error.h"

namespace rhi::impl
{
	ComputePassEncoder::ComputePassEncoder(CommandEncoder* encoder, EncodingContext& encodingContext) :
		PassEncoder(encoder, encodingContext)
	{

	}

	ComputePassEncoder::~ComputePassEncoder()
	{
		if (!mIsEnded)
		{
			APIEnd();
		}
	}

	Ref<ComputePassEncoder> ComputePassEncoder::Create(CommandEncoder* encoder, EncodingContext& encodingContext)
	{
		Ref<ComputePassEncoder> computePassEncoder = AcquireRef(new ComputePassEncoder(encoder, encodingContext));
		return computePassEncoder;
	}

	void ComputePassEncoder::APISetPipeline(ComputePipelineBase* pipeline)
	{
		ASSERT(pipeline != nullptr);

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		SetComputePipelineCmd* cmd = allocator.Allocate<SetComputePipelineCmd>(Command::SetComputePipeline);
		cmd->pipeline = pipeline;

		mLastPipeline = pipeline;
	}

	void ComputePassEncoder::APIDispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	{
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		DispatchCmd* cmd = allocator.Allocate<DispatchCmd>(Command::Dispatch);
		cmd->x = groupCountX;
		cmd->y = groupCountY;
		cmd->z = groupCountZ;
	}

	void ComputePassEncoder::APIDispatchIndirect(BufferBase* indirectBuffer, uint64_t indirectOffset)
	{
		ASSERT(indirectBuffer != nullptr);

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		DispatchIndirectCmd* cmd = allocator.Allocate<DispatchIndirectCmd>(Command::DispatchIndirect);
		cmd->indirectBuffer = indirectBuffer;
		cmd->indirectOffset = indirectOffset;
	}

	void ComputePassEncoder::APISetBindSet(BindSetBase* set, uint32_t setIndex, uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets)
	{
		ASSERT(set != nullptr);
		RecordSetBindSet(set, setIndex, dynamicOffsetCount, dynamicOffsets);
		mUsageTracker.AddBindSet(set);
	}

	void ComputePassEncoder::APIEnd()
	{
		mIsEnded = true;
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		EndRenderPassCmd* cmd = allocator.Allocate<EndRenderPassCmd>(Command::EndRenderPass);
		mEncodingContext.ExitComputePass(mUsageTracker);
		mCommandEncoder->OnComputePassEnd();
	}
}