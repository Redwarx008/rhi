#include "SamplerBase.h"

namespace rhi::impl
{
	SamplerBase::SamplerBase(DeviceBase* device, const SamplerDesc& desc) :
		ResourceBase(device, desc.name),
		mAddressModeU(desc.addressModeU),
		mAddressModeV(desc.addressModeV),
		mAddressModeW(desc.addressModeW),
		mMagFilter(desc.magFilter),
		mMinFilter(desc.minFilter),
		mMipmapFilter(desc.mipmapFilter),
		mMaxAnisotropy(desc.maxAnisotropy)
	{

	}

	SamplerBase::~SamplerBase() {}

	ResourceType SamplerBase::GetType() const
	{
		return ResourceType::Sampler;
	}
}