#include "ComputePipelineBase.h"

namespace rhi::impl
{
    ComputePipelineBase::ComputePipelineBase(DeviceBase* device, const ComputePipelineDesc& desc)
        : PipelineBase(device, desc)
    {}

    ComputePipelineBase::~ComputePipelineBase() = default;

    ResourceType ComputePipelineBase::GetType() const
    {
        return ResourceType::ComputePipeline;
    }
} // namespace rhi::impl
