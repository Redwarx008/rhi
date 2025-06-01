#pragma once

#include <vulkan/vulkan.h>
#include "../RenderPipelinebase.h"

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
        ~RenderPipeline() override;
        bool Initialize(PipelineCacheBase* cache);
        void DestroyImpl() override;

        VkPipeline mHandle = VK_NULL_HANDLE;
    };
} // namespace rhi::impl::vulkan
