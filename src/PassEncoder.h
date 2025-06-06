#pragma once

#include "EncodingContext.h"
#include "RHIStruct.h"
#include "common/Ref.hpp"
#include "common/RefCounted.h"

namespace rhi::impl
{
    class PassEncoder : public RefCounted
    {
    public:
        explicit PassEncoder(CommandEncoder* encoder, EncodingContext& encodingContext);
        ~PassEncoder();
        void APISetPushConstant(ShaderStage stage, const void* data, uint32_t size, uint32_t offset);
        void APIBeginDebugLabel(std::string_view label, const Color* color);
        void APIEndDebugLabel();

    protected:
        void RecordSetBindSet(BindSetBase* set,
                              uint32_t setIndex,
                              uint32_t dynamicOffsetCount = 0,
                              const uint32_t* dynamicOffsets = nullptr);
        EncodingContext& mEncodingContext;
        Ref<CommandEncoder> mCommandEncoder;
        bool mIsEnded = false;
        uint64_t mDebugLabelCount = 0;
        PipelineBase* mLastPipeline = nullptr;
    };
} // namespace rhi::impl
