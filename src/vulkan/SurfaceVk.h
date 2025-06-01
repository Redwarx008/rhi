#pragma once

#include <vulkan/vulkan.h>
#include "../SurfaceBase.h"

namespace rhi::impl::vulkan
{
    class Surface final : public SurfaceBase
    {
    public:
        static Ref<Surface> CreateFromWindowsHWND(InstanceBase* instance, void* hwnd, void* hinstance);
        // static Ref<Surface> CreateFromWaylandSurface(InstanceBase* instance, void* display, void* surface);

        VkSurfaceKHR GetHandle() const;

    private:
        explicit Surface(InstanceBase* instance);
        ~Surface() override;
        bool Initialize();

        VkSurfaceKHR mHandle = VK_NULL_HANDLE;
    };
} // namespace rhi::impl::vulkan
