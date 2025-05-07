#pragma once

#include "../PipelineLayoutBase.h"
#include "../common/Ref.hpp"

#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
	class Device;

	class PipelineLayout final : public PipelineLayoutBase
	{
	public:
		static Ref<PipelineLayout> Create(Device* device, const PipelineLayoutDesc& desc);
		VkPipelineLayout GetHandle() const;
		VkShaderStageFlags GetPushConstantVisibility() const;
	private:
		explicit PipelineLayout(Device* device, const PipelineLayoutDesc& desc);
		~PipelineLayout();
		bool Initialize(const PipelineLayoutDesc& desc);
		void DestroyImpl() override;

		VkPipelineLayout mHandle = VK_NULL_HANDLE;
		VkShaderStageFlags mPushConstantVisibility = 0;
	};
}