#pragma once

#include "RHIStruct.h"
#include "common/RefCounted.h"
#include "common/Ref.hpp"
#include "EncodingContext.h"

namespace rhi::impl
{
	class PipelineBase;
	class PassEncoder : public RefCounted
	{
	public:
		explicit PassEncoder(CommandEncoder* encoder, EncodingContext& encodingContext);
		~PassEncoder();
		void APISetPushConstant(const void* data, uint32_t size);
		void APIBeginDebugLabel(std::string_view label, const Color* color);
		void APIEndDebugLabel();
	protected:
		void RecordSetBindSet(BindSetBase* set, uint32_t setIndex, uint32_t dynamicOffsetCount = 0, const uint32_t* dynamicOffsets = nullptr);
		EncodingContext& mEncodingContext;
		Ref<CommandEncoder> mCommandEncoder;
		bool mIsEnded = false;
		uint64_t mDebugLabelCount = 0;
		PipelineBase*mLastPipeline;
	};
}