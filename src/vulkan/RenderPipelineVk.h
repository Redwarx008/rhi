#pragma once

#include "../RenderPipelinebase.h"
#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
	class Device;
	class RenderPipeline final : public RenderPipelineBase
	{
	public:
		static Ref<RenderPipeline> Create(Device* device, const RenderPipelineDesc& desc);
		VkPipeline GetHandle() const;
	private:
		explicit RenderPipeline(Device* device, const RenderPipelineDesc& desc);
		~RenderPipeline();
		bool Initialize();
		void DestroyImpl() override;

		VkPipeline mHandle = VK_NULL_HANDLE;
	};
}