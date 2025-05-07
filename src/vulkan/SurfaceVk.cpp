#include "SurfaceVk.h"
#include "InstanceVk.h"
#include "DeviceVk.h"
#include "ErrorsVk.h"
#include "SwapChainVk.h"
#include "../common/Utils.h"


namespace rhi::impl::vulkan
{
	Ref<Surface> Surface::CreateFromWindowsHWND(InstanceBase* instance, void* hwnd, void* hinstance)
	{
		Ref<Surface> surface = AcquireRef(new Surface(instance));
		surface->mHWND = hwnd;
		surface->mHInstance = hinstance;
		if (!surface->Initialize())
		{
			return nullptr;
		}
		return surface;
	}

	Surface::Surface(InstanceBase* instance) :
		SurfaceBase(instance)
	{

	}

	Surface::~Surface()
	{
		mSwapChain = nullptr;

		if (mHandle != VK_NULL_HANDLE)
		{
			Instance* instance = checked_cast<Instance>(mInstance);
			vkDestroySurfaceKHR(instance->GetHandle(), mHandle, nullptr);
			mHandle = VK_NULL_HANDLE;
		}
	}

	bool Surface::Initialize()
	{
		Instance* instance = checked_cast<Instance>(mInstance);
		Device* device = checked_cast<Device>(mDevice.Get());
#if defined(WIN32)
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hinstance = static_cast<HINSTANCE>(mHInstance);
		createInfo.hwnd = static_cast<HWND>(mHWND);
		VkResult err = vkCreateWin32SurfaceKHR(instance->GetHandle(), &createInfo, nullptr, &mHandle);
		CHECK_VK_RESULT_FALSE(err, "CreateWin32Surface");
#elif defined(USE_WAYLAND_WSI)
#else
		ASSERT(!"No specified platform.");
#endif
		return true;
	}

	VkSurfaceKHR Surface::GetHandle() const
	{
		return mHandle;
	}
}