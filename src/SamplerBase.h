#pragma once

#include "ResourceBase.h"
#include "RHIStruct.h"

namespace rhi::impl
{
	class DeviceBase;

	class SamplerBase : public ResourceBase
	{
	public:
		ResourceType GetType() const override;
	protected:
		explicit SamplerBase(DeviceBase* device, const SamplerDesc& desc);
		~SamplerBase();
		FilterMode mMagFilter = FilterMode::Linear;
		FilterMode mMinFilter = FilterMode::Linear;
		FilterMode mMipmapFilter = FilterMode::Linear;
		SamplerAddressMode mAddressModeU = SamplerAddressMode::ClampToEdge;
		SamplerAddressMode mAddressModeV = SamplerAddressMode::ClampToEdge;
		SamplerAddressMode mAddressModeW = SamplerAddressMode::ClampToEdge;

		float mMaxAnisotropy = 1.f;
	};
}