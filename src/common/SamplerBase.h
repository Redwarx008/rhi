#pragma once

#include "RHIStruct.h"
#include "ResourceBase.h"
#include "common/Cached.hpp"

namespace rhi::impl
{
    class SamplerBase : public ResourceBase, public Cached<SamplerBase>
    {
    public:
        explicit SamplerBase(DeviceBase* device, const SamplerDesc& desc);
        ~SamplerBase() override;
        ResourceType GetType() const override;
        size_t ComputeContentHash() override;

        struct Equal
        {
            bool operator()(const SamplerBase* a, const SamplerBase* b) const;
        };

    protected:
        void DestroyImpl() override;
        FilterMode mMagFilter = FilterMode::Linear;
        FilterMode mMinFilter = FilterMode::Linear;
        FilterMode mMipmapFilter = FilterMode::Linear;
        SamplerAddressMode mAddressModeU = SamplerAddressMode::ClampToEdge;
        SamplerAddressMode mAddressModeV = SamplerAddressMode::ClampToEdge;
        SamplerAddressMode mAddressModeW = SamplerAddressMode::ClampToEdge;

        float mMaxAnisotropy = 1.f;
    };
} // namespace rhi::impl
