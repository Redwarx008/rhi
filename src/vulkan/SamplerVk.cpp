#include "SamplerVk.h"

#include "DeviceVk.h"
#include "VulkanUtils.h"
#include "ErrorsVk.h"
#include "../common/Utils.h"

namespace rhi::impl::vulkan
{
	VkFilter SamplerFilterConvert(FilterMode filter)
	{
		switch (filter)
		{
		case rhi::impl::FilterMode::Linear:
			return VK_FILTER_LINEAR;
		case rhi::impl::FilterMode::Nearest:
			return VK_FILTER_NEAREST;
		default:
			break;
		}
		ASSERT(!"Unreachable");
	}

	VkSamplerMipmapMode SamplerMipmapModeConvert(FilterMode filter)
	{
		switch (filter)
		{
		case rhi::impl::FilterMode::Linear:
			return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		case rhi::impl::FilterMode::Nearest:
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		default:
			break;
		}
		ASSERT(!"Unreachable");
	}

	VkSamplerAddressMode SamplerAddressModeConvert(SamplerAddressMode mode)
	{
		switch (mode)
		{
		case rhi::impl::SamplerAddressMode::ClampToEdge:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case rhi::impl::SamplerAddressMode::Repeat:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case rhi::impl::SamplerAddressMode::ClampToBorder:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case rhi::impl::SamplerAddressMode::MirroredRepeat:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case rhi::impl::SamplerAddressMode::MirrorClampToEdge:
			return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		default:
			break;
		}
		ASSERT(!"Unreachable");
	}


	Sampler::Sampler(Device* device, const SamplerDesc& desc) :
		SamplerBase(device, desc)
	{

	}

	Ref<Sampler> Sampler::Create(Device* device, const SamplerDesc& desc)
	{
		Ref<Sampler> sampler = AcquireRef(new Sampler(device, desc));
		if (!sampler->Initialize(desc))
		{
			return nullptr;
		}
		return std::move(sampler);
	}

	bool Sampler::Initialize(const SamplerDesc& desc)
	{
		SamplerBase::Initialize();

		VkSamplerCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.magFilter = SamplerFilterConvert(mMagFilter);
		createInfo.minFilter = SamplerFilterConvert(mMinFilter);
		createInfo.mipmapMode = SamplerMipmapModeConvert(mMipmapFilter);
		createInfo.addressModeU = SamplerAddressModeConvert(mAddressModeU);
		createInfo.addressModeV = SamplerAddressModeConvert(mAddressModeV);
		createInfo.addressModeW = SamplerAddressModeConvert(mAddressModeW);
		createInfo.mipLodBias = 0.0f;
		createInfo.minLod = desc.minLod;
		createInfo.maxLod = desc.maxLod;
		createInfo.compareOp = CompareOpConvert(desc.compareOp);
		createInfo.compareEnable = desc.compareOp != CompareOp::Never ? true : false;
		createInfo.unnormalizedCoordinates = VK_FALSE;

		Device* device = checked_cast<Device>(mDevice.Get());
		if (device->GetVkDeviceInfo().features.samplerAnisotropy == true && mMaxAnisotropy > 1)
		{
			createInfo.anisotropyEnable = true;
			createInfo.maxAnisotropy = (std::min)(mMaxAnisotropy, device->GetVkDeviceInfo().properties.limits.maxSamplerAnisotropy);
		}
		else
		{
			createInfo.anisotropyEnable = false;
		}

		VkResult err = vkCreateSampler(device->GetHandle(), &createInfo, nullptr, &mHandle);
		CHECK_VK_RESULT_FALSE(err, "CreateSampler");

		SetDebugName(device, mHandle, "Sampler", GetName());
	}

	void Sampler::DestroyImpl()
	{
		// Usually there is no situation to destroy the sampler during use, so destroy it directly.
		if (mHandle != VK_NULL_HANDLE)
		{
			Device* device = checked_cast<Device>(mDevice.Get());
			vkDestroySampler(device->GetHandle(), mHandle, nullptr);
			mHandle = VK_NULL_HANDLE;
		}
	}

	Sampler::~Sampler() {}

	VkSampler Sampler::GetHandle() const
	{
		return mHandle;
	}
}