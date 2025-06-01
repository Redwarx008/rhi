#pragma once

#include "../InstanceBase.h"
#include "../common/Ref.hpp"

#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
    class Instance final : public InstanceBase
    {
    public:
        static Ref<Instance> Create(const InstanceDesc& desc);

        void APIEnumerateAdapters(AdapterBase** adapters, uint32_t* adapterCount) override;
        SurfaceBase* APICreateSurface(void* hwnd, void* hinstance) override;
        VkInstance GetHandle() const;

    private:
        Instance() = default;
        ~Instance() override;
        bool Initialize(const InstanceDesc& desc);
        bool RegisterDebugUtils();

        VkInstance mHandle = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT mDebugUtilsMessenger = VK_NULL_HANDLE;
    };
} // namespace rhi::impl::vulkan
