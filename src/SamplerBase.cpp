#include "SamplerBase.h"

#include "common/ObjectContentHasher.h"

namespace rhi::impl
{
    SamplerBase::SamplerBase(DeviceBase* device, const SamplerDesc& desc) :
        ResourceBase(device, desc.name), mAddressModeU(desc.addressModeU), mAddressModeV(desc.addressModeV),
        mAddressModeW(desc.addressModeW), mMagFilter(desc.magFilter), mMinFilter(desc.minFilter),
        mMipmapFilter(desc.mipmapFilter), mMaxAnisotropy(desc.maxAnisotropy)
    {}

    SamplerBase::~SamplerBase() = default;

    void SamplerBase::DestroyImpl()
    {
        Uncache();
    }

    ResourceType SamplerBase::GetType() const
    {
        return ResourceType::Sampler;
    }

    size_t SamplerBase::ComputeContentHash()
    {
        ObjectContentHasher recorder;
        recorder.Record(
                mAddressModeU, mAddressModeV, mAddressModeW, mMagFilter, mMinFilter, mMipmapFilter, mMaxAnisotropy);
        return recorder.GetContentHash();
    }

    bool SamplerBase::Equal::operator()(const SamplerBase* a, const SamplerBase* b) const
    {
        return a->mAddressModeU == b->mAddressModeU && a->mAddressModeV == b->mAddressModeV &&
                a->mAddressModeW == b->mAddressModeW && a->mMagFilter == b->mMagFilter &&
                a->mMinFilter == b->mMinFilter && a->mMipmapFilter == b->mMipmapFilter &&
                a->mMaxAnisotropy == b->mMaxAnisotropy;
    }
} // namespace rhi::impl
