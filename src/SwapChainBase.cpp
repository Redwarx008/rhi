#include "SwapchainBase.h"
#include "DeviceBase.h"
#include "TextureBase.h"

namespace rhi::impl
{
	SwapChainBase::SwapChainBase(DeviceBase* device, SurfaceBase* surface, const SurfaceConfiguration& config) :
		mDevice(device),
		mSurface(surface),
		mWidth(config.width),
		mHeight(config.height),
		mFormat(config.format),
		mPresentMode(config.presentMode)
	{

	}

	SwapChainBase::~SwapChainBase() {}

	TextureFormat SwapChainBase::GetFormat() const
	{
		return mFormat;
	}
}