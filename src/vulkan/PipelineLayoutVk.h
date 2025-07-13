#pragma once

#include "common/PipelineLayoutBase.h"
#include "common/Ref.hpp"

#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
    class Device;

    class PipelineLayout final : public PipelineLayoutBase
    {
    public:
        static Ref<PipelineLayout> Create(Device* device, const PipelineLayoutDesc& desc);
        static Ref<PipelineLayout> Create(Device* device, const PipelineLayoutDesc2& desc);
        VkPipelineLayout GetHandle() const;
        VkShaderStageFlags GetPushConstantVisibility() const;

    private:
        explicit PipelineLayout(Device* device, const PipelineLayoutDesc& desc);
        explicit PipelineLayout(Device* device, const PipelineLayoutDesc2& desc);
        ~PipelineLayout() override;
        bool Initialize();
        void DestroyImpl() override;

        VkPipelineLayout mHandle = VK_NULL_HANDLE;
        VkShaderStageFlags mPushConstantVisibility = 0;
    };
} // namespace rhi::impl::vulkan
