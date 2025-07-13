#pragma once

#include "common/ShaderModuleBase.h"
#include "common/Ref.hpp"

#include <vector>
#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
    class Device;

    class ShaderModule : public ShaderModuleBase
    {
    public:
        static Ref<ShaderModule> Create(Device* device, const ShaderModuleDesc& desc);
        VkShaderModule GetHandle() const;

    private:
        explicit ShaderModule(Device* device, const ShaderModuleDesc& desc);
        ~ShaderModule() override;
        bool Initialize(const ShaderModuleDesc& desc);
        void DestroyImpl() override;

        VkShaderModule mHandle = VK_NULL_HANDLE;
    };
} // namespace rhi::impl::vulkan
