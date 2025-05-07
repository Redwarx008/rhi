#pragma once

#include "RHIStruct.h"
#include "common/Ref.hpp"
#include "common/RefCounted.h"

namespace rhi::impl
{
	class SurfaceBase : public RefCounted
	{
	public:
		void APIConfigure(const SurfaceConfiguration& config);
		SurfaceAcquireNextTextureStatus APIAcquireNextTexture();
		TextureBase* APIGetCurrentTexture();
		TextureViewBase* APIGetCurrentTextureView();
		TextureFormat APIGetSwapChainFormat() const;
		void APIPresent();
		void APIUnconfigure();

		InstanceBase* GetInstance() const;
		void* GetHInstance() const;
		void* GetHWND() const;
	protected:
		explicit SurfaceBase(InstanceBase* instance);
		~SurfaceBase();
		Ref<DeviceBase> mDevice;
		InstanceBase* mInstance;
		Ref<SwapChainBase> mSwapChain;

		// WindowsHwnd
		void* mHWND = nullptr;
		void* mHInstance = nullptr;
	};
}