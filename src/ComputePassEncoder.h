#pragma once

#include "RHIStruct.h"
#include "PassEncoder.h"
#include "SyncScopeUsageTracker.h"

namespace rhi::impl
{
	class ComputePassEncoder : public PassEncoder
	{
	public:
		static Ref<ComputePassEncoder> Create(CommandEncoder* encoder, EncodingContext& encodingContext);

		void APISetPipeline(ComputePipelineBase* pipeline);
		void APIDispatch(uint32_t groupCountX, uint32_t groupCountY = 1, uint32_t groupCountZ = 1);
		void APIDispatchIndirect(BufferBase* indirectBuffer, uint64_t indirectOffset);
		void APISetBindSet(BindSetBase* set, uint32_t setIndex, uint32_t dynamicOffsetCount = 0, const uint32_t* dynamicOffsets = nullptr);
		void APIEnd();
	protected:
		explicit ComputePassEncoder(CommandEncoder* encoder, EncodingContext& encodingContext);
		~ComputePassEncoder();
		SyncScopeUsageTracker mUsageTracker;
	};
}