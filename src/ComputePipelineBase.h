#pragma once

#include "PipelineBase.h"
#include "RHIStruct.h"

namespace rhi::impl
{
    class ComputePipelineBase : public PipelineBase
    {
    public:
        explicit ComputePipelineBase(DeviceBase* device, const ComputePipelineDesc& desc);
        ~ComputePipelineBase() override;
        ResourceType GetType() const override;
    };
} // namespace rhi::impl
