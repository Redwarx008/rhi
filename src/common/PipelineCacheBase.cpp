#include "PipelineCacheBase.h"

#include "RHIStruct.h"

namespace rhi::impl
{
    PipelineCacheBase::PipelineCacheBase(DeviceBase* device, const PipelineCacheDesc& desc)
        : ResourceBase(device, desc.name)
    {}

    PipelineCacheBase::~PipelineCacheBase() = default;

    ResourceType PipelineCacheBase::GetType() const
    {
        return ResourceType::PipelineCache;
    }

    void PipelineCacheBase::APIGetData(void* data, size_t* dataSize) const
    {
        GetData(data, dataSize);
    }
} // namespace rhi::impl
