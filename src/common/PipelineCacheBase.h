#pragma once

#include "RHIStruct.h"
#include "ResourceBase.h"

namespace rhi::impl
{
    class PipelineCacheBase : public ResourceBase
    {
    public:
        void APIGetData(void* data, size_t* dataSize) const;
        virtual void GetData(void* data, size_t* dataSize) const = 0;

        ResourceType GetType() const;
    protected:
        explicit PipelineCacheBase(DeviceBase* device, const PipelineCacheDesc& desc);
        ~PipelineCacheBase() override;
    };
} // namespace rhi::impl
