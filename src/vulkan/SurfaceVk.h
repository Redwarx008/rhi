#pragma once

#include "../SurfaceBase.h"
#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
	class Surface final : public SurfaceBase
	{
	public:
		static Ref<Surface> CreateFromWindowsHWND(InstanceBase* instance, void* hwnd, void* hinstance);
		//static Ref<Surface> CreateFromWaylandSurface(InstanceBase* instance, void* display, void* surface);

		VkSurfaceKHR GetHandle() const;
	private:
		explicit Surface(InstanceBase* instance);
		~Surface();
		bool Initialize();

		VkSurfaceKHR mHandle = VK_NULL_HANDLE;
	};
}