#pragma once

#include "RHIStruct.h"
#include "common/Ref.hpp"
#include "common/RefCounted.h"
#include "TextureBase.h"

namespace rhi::impl
{
	class SwapChainBase : public RefCounted
	{
	public:
		virtual SurfaceAcquireNextTextureStatus AcquireNextTexture() = 0;
		virtual Ref<TextureViewBase> GetCurrentTextureView() = 0;
		virtual Ref<TextureBase> GetCurrentTexture() = 0;
		virtual void Present() = 0;
		TextureFormat GetFormat() const;
	protected:
		explicit SwapChainBase(DeviceBase* device, SurfaceBase* surface, const SurfaceConfiguration& config);
		~SwapChainBase();
		DeviceBase* mDevice;
		SurfaceBase* mSurface;

		uint32_t mHeight;
		uint32_t mWidth;
		TextureFormat mFormat;
		PresentMode mPresentMode;
	};
}