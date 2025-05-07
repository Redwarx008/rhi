#pragma once

#include "../SwapchainBase.h"

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
	class Device;
	class SwapChainTexture;
	class SwapChain final : public SwapChainBase
	{
	public:
		static Ref<SwapChain> Create(Device* device, SurfaceBase* surface, SwapChainBase* previous, const SurfaceConfiguration& config);
		~SwapChain();
		VkSwapchainKHR GetHandle() const;
		SurfaceAcquireNextTextureStatus AcquireNextTexture() override;
		Ref<TextureViewBase> GetCurrentTextureView() override;
		Ref<TextureBase> GetCurrentTexture() override;
		void Present() override;
		void DestroySwapChain();
	private:
		SwapChain(Device* device, SurfaceBase* surface, const SurfaceConfiguration& config);
		bool Initialize(SwapChainBase* previous);
		bool CreateSwapChainInternal(SwapChainBase* previous);
		SurfaceAcquireNextTextureStatus AcquireNextTextureImpl(bool isReentrant);

		VkSwapchainKHR mHandle = VK_NULL_HANDLE;

		struct SemaphoreAndFence
		{
			VkSemaphore semaphore;
			VkFence fence;
		};

		std::vector<SemaphoreAndFence> mAquireImageSemaphoreAndFences;

		uint32_t mCurrentFrameIndex = 0;

		struct PerTexture
		{
			Ref<SwapChainTexture> texture;
			Ref<TextureViewBase> defualtView;
			VkSemaphore renderingDoneSemaphore;
		};

		std::vector<PerTexture> mTextures;

		uint32_t mImageIndex = UINT32_MAX;
	};
}