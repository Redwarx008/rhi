#pragma once

#include "../SamplerBase.h"
#include "../common/Ref.hpp"

#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
	class Device;

	class Sampler final : public SamplerBase
	{
	public:
		static Ref<Sampler> Create(Device* device, const SamplerDesc& desc);
		VkSampler GetHandle() const;
	private:
		explicit  Sampler(Device* device, const SamplerDesc& desc);
		~Sampler();
		bool Initialize(const SamplerDesc& desc);
		void DestroyImpl() override;

		VkSampler mHandle = VK_NULL_HANDLE;
	};
}