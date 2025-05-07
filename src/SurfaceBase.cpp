#include "SurfaceBase.h"
#include "DeviceBase.h"
#include "SwapchainBase.h"
#include "common/Error.h"

namespace rhi::impl
{
	SurfaceBase::SurfaceBase(InstanceBase* instance) :
		mInstance(instance)
	{

	}

	SurfaceBase::~SurfaceBase()
	{

	}

	void SurfaceBase::APIConfigure(const SurfaceConfiguration& config)
	{
		ASSERT(config.device != nullptr);
		mDevice = config.device;
		mSwapChain = mDevice->CreateSwapChain(this, mSwapChain.Get(), config);
	}

	void SurfaceBase::APIUnconfigure()
	{
		mSwapChain = nullptr;
	}

	SurfaceAcquireNextTextureStatus SurfaceBase::APIAcquireNextTexture()
	{
		ASSERT(mSwapChain != nullptr);
		return mSwapChain->AcquireNextTexture();
	}

	TextureBase* SurfaceBase::APIGetCurrentTexture()
	{
		ASSERT(mSwapChain != nullptr);
		return mSwapChain->GetCurrentTexture().Detach();
	}

	TextureViewBase* SurfaceBase::APIGetCurrentTextureView()
	{
		ASSERT(mSwapChain != nullptr);
		return mSwapChain->GetCurrentTextureView().Detach();
	}

	TextureFormat SurfaceBase::APIGetSwapChainFormat() const
	{
		TextureFormat format = TextureFormat::Undefined;
		if (mSwapChain)
		{
			format = mSwapChain->GetFormat();
		}
		return format;
	}

	void SurfaceBase::APIPresent()
	{
		ASSERT(mSwapChain != nullptr);
		mSwapChain->Present();
	}

	InstanceBase* SurfaceBase::GetInstance() const
	{
		return mInstance;
	}

	void* SurfaceBase::GetHInstance() const
	{
		return mHInstance;
	}

	void* SurfaceBase::GetHWND() const
	{
		return mHWND;
	}
}