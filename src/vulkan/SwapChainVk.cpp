#include "SwapChainVk.h"

#include "SurfaceVk.h"
#include "InstanceVk.h"
#include "DeviceVk.h"
#include "QueueVk.h"
#include "TextureVk.h"
#include "ErrorsVk.h"
#include "../common/Constants.h"

#include <algorithm>
#include <memory>

namespace rhi::impl::vulkan
{
	VkPresentModeKHR ToVulkanPresentMode(PresentMode mode)
	{
		switch (mode)
		{
		case PresentMode::Fifo:
			return VK_PRESENT_MODE_FIFO_KHR;
		case PresentMode::FifoRelaxed:
			return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
		case PresentMode::Immediate:
			return VK_PRESENT_MODE_IMMEDIATE_KHR;
		case PresentMode::Mailbox:
			return VK_PRESENT_MODE_MAILBOX_KHR;
		}
		ASSERT(!"Unreachable");
	}

	PresentMode ToPresentMode(VkPresentModeKHR mode)
	{
		switch (mode)
		{
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
			return PresentMode::Immediate;
		case VK_PRESENT_MODE_MAILBOX_KHR:
			return PresentMode::Mailbox;
		case VK_PRESENT_MODE_FIFO_KHR:
			return PresentMode::Fifo;
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
			return PresentMode::FifoRelaxed;
		}
		ASSERT(!"Unreachable");
	}

	uint32_t MinImageCountForPresentMode(VkPresentModeKHR mode)
	{
		switch (mode) 
		{
		case VK_PRESENT_MODE_FIFO_KHR:
		case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
			return 2;
		case VK_PRESENT_MODE_MAILBOX_KHR:
			return 3;
		default:
			break;
		}
		ASSERT(!"Unreachable");
	}

	Ref<SwapChain> SwapChain::Create(Device* device, SurfaceBase* surface, SwapChainBase* previous, const SurfaceConfiguration& config)
	{
		Ref<SwapChain> swapChain = AcquireRef(new SwapChain(device, surface, config));
		if (!swapChain->Initialize(previous))
		{
			return nullptr;
		}
		return swapChain;
	}

	SwapChain::SwapChain(Device* device, SurfaceBase* surface, const SurfaceConfiguration& config) :
		SwapChainBase(device, surface, config)
	{

	}

	SwapChain::~SwapChain()
	{
		DestroySwapChain();
	}

	bool SwapChain::Initialize(SwapChainBase* previous)
	{
		return CreateSwapChainInternal(previous);
	}

	bool SwapChain::CreateSwapChainInternal(SwapChainBase* previous)
	{
		Surface* surface = checked_cast<Surface>(mSurface);
		Device* device = checked_cast<Device>(mDevice);

		ASSERT(surface->GetHandle());
		// Get list of supported surface formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetVkPhysicalDevice(), surface->GetHandle(), &formatCount, nullptr);
		ASSERT(formatCount > 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetVkPhysicalDevice(), surface->GetHandle(), &formatCount, surfaceFormats.data());

		VkSurfaceFormatKHR selectedFormat = surfaceFormats[0];
		std::vector<VkFormat> preferredImageFormats = 
		{
			ToVkFormat(mFormat),
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_FORMAT_R8G8B8A8_UNORM,
		};

		for (auto& availableFormat : surfaceFormats)
		{
			for (uint32_t i = 0; i < preferredImageFormats.size(); ++i)
			{
				if (preferredImageFormats[i] == availableFormat.format)
				{
					selectedFormat = availableFormat;
					break;
				}
			}
		}

		if (selectedFormat.format != ToVkFormat(mFormat))
		{
			LOG_WARNING("Requested color format is not supported and replaced by %s", GetFormatInfo(ToTextureFormat(selectedFormat.format)).name);
			mFormat = ToTextureFormat(selectedFormat.format);
		}

		// Store the current swap chain handle so we can use it later on to ease up recreation
		VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE;
		if (previous != nullptr)
		{
			oldSwapchain = checked_cast<SwapChain>(previous)->GetHandle();
		}

		// Get physical device surface properties and formats
		VkSurfaceCapabilitiesKHR surfCaps;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetVkPhysicalDevice(), surface->GetHandle(), &surfCaps);

		// Get available present modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetVkPhysicalDevice(), surface->GetHandle(), &presentModeCount, NULL);
		assert(presentModeCount > 0);

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetVkPhysicalDevice(), surface->GetHandle(), &presentModeCount, presentModes.data());

		VkExtent2D swapchainExtent = {};
		// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
		if (surfCaps.currentExtent.width == (uint32_t)-1)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = std::clamp(mWidth, surfCaps.minImageExtent.width, surfCaps.maxImageExtent.width);
			swapchainExtent.height = std::clamp(mHeight, surfCaps.minImageExtent.height, surfCaps.maxImageExtent.height);
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCaps.currentExtent;
		}
		mWidth = swapchainExtent.width;
		mHeight = swapchainExtent.height;
		// Select a present mode for the swapchain

		VkPresentModeKHR presentMode;

		// Choose the present mode. The only guaranteed one is FIFO so it has to be the fallback for
		// all other present modes. IMMEDIATE has tearing which is generally undesirable so it can't
		// be the fallback for MAILBOX. So the fallback order is always IMMEDIATE -> MAILBOX ->
		// FIFO.
		{
			auto HasPresentMode = [](const std::vector<VkPresentModeKHR>& modes, VkPresentModeKHR target) -> bool 
				{
					return std::find(modes.begin(), modes.end(), target) != modes.end();
				};

			VkPresentModeKHR targetMode = ToVulkanPresentMode(mPresentMode);
			const std::array<VkPresentModeKHR, 4> presentModeFallbacks =
			{
				VK_PRESENT_MODE_IMMEDIATE_KHR,
				VK_PRESENT_MODE_MAILBOX_KHR,
				VK_PRESENT_MODE_FIFO_RELAXED_KHR,
				VK_PRESENT_MODE_FIFO_KHR,
			};

			// Go to the target mode.
			size_t modeIndex = 0;
			while (presentModeFallbacks[modeIndex] != targetMode) 
			{
				modeIndex++;
			}

			// Find the first available fallback.
			while (!HasPresentMode(presentModes, presentModeFallbacks[modeIndex]))
			{
				modeIndex++;
			}

			ASSERT(modeIndex < presentModeFallbacks.size());
			presentMode = presentModeFallbacks[modeIndex];

			if (presentMode != targetMode)
			{
				// todo: log a warning.
				mPresentMode = ToPresentMode(presentMode);
			}
		}

		// Determine the number of images
		uint32_t desiredNumberOfSwapchainImages = (std::max)(surfCaps.minImageCount, MinImageCountForPresentMode(presentMode));
		if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
		{
			desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
		}

		// Find the transformation of the surface
		VkSurfaceTransformFlagsKHR preTransform;
		if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			// We prefer a non-rotated transform
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = surfCaps.currentTransform;
		}

		// Find a supported composite alpha format (not all devices support alpha opaque)
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// Simply select the first composite alpha format available
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags =
		{
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (auto& compositeAlphaFlag : compositeAlphaFlags)
		{
			if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) 
			{
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		VkSwapchainCreateInfoKHR swapchainCI = {};
		swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCI.surface = surface->GetHandle();
		swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
		swapchainCI.imageFormat = selectedFormat.format;
		swapchainCI.imageColorSpace = selectedFormat.colorSpace;
		swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
		swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
		swapchainCI.imageArrayLayers = 1;
		swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCI.queueFamilyIndexCount = 0;
		swapchainCI.presentMode = presentMode;
		// Setting oldSwapChain to the saved handle of the previous swapchain aids in resource reuse and makes sure that we can still present already acquired images
		swapchainCI.oldSwapchain = oldSwapchain;
		// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
		swapchainCI.clipped = VK_TRUE;
		swapchainCI.compositeAlpha = compositeAlpha;

		// Enable transfer source on swap chain images if supported
		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		// Enable transfer destination on swap chain images if supported
		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		VkResult err = vkCreateSwapchainKHR(device->GetHandle(), &swapchainCI, nullptr, &mHandle);
		CHECK_VK_RESULT_FALSE(err, "CreateSwapchain");

		// If an existing swap chain is re-created, destroy the old swap chain
		// This also cleans up all the presentable images
		if (oldSwapchain != VK_NULL_HANDLE)
		{
			Queue* queue = checked_cast<Queue>(device->GetQueue(QueueType::Graphics).Get());
			for (auto& perTexture : mTextures)
			{
				queue->GetDeleter()->DeleteWhenUnused(perTexture.renderingDoneSemaphore);	
			}

			for (auto& [semaphore, fence] : mAquireImageSemaphoreAndFences)
			{
				queue->GetDeleter()->DeleteWhenUnused(semaphore);
				queue->GetDeleter()->DeleteWhenUnused(fence);
			}

			vkDestroySwapchainKHR(device->GetHandle(), oldSwapchain, nullptr);
			oldSwapchain = VK_NULL_HANDLE;
		}

		uint32_t imageCount;
		vkGetSwapchainImagesKHR(device->GetHandle(), mHandle, &imageCount, NULL);
		std::vector<VkImage> images{ imageCount };
		err = vkGetSwapchainImagesKHR(device->GetHandle(), mHandle, &imageCount, images.data());
		CHECK_VK_RESULT_FALSE(err, "GetSwapchainImages");

		mTextures.resize(imageCount);
		mAquireImageSemaphoreAndFences.resize(imageCount);
		for (uint32_t i = 0; i < imageCount; ++i)
		{
			TextureDesc desc{};
			desc.dimension = TextureDimension::Texture2D;
			desc.width = mWidth;
			desc.height = mHeight;
			desc.format = mFormat;
			desc.usage = TextureUsage::RenderAttachment;
			mTextures[i].texture = SwapChainTexture::Create(device, desc, images[i]);
			mTextures[i].defualtView = mTextures[i].texture->APICreateView();

			VkSemaphoreCreateInfo semaphoreCI{};
			semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &mAquireImageSemaphoreAndFences[i].semaphore);
			vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &mTextures[i].renderingDoneSemaphore);
			VkFenceCreateInfo fenceCI{};
			fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &mAquireImageSemaphoreAndFences[i].fence);
		}

		return true;
	}

	void SwapChain::DestroySwapChain()
	{
		Surface* surface = checked_cast<Surface>(mSurface);
		Device* device = checked_cast<Device>(mDevice);
		Queue* queue = checked_cast<Queue>(device->GetQueue(QueueType::Graphics).Get());
		
		for (auto& [semaphore, fence] : mAquireImageSemaphoreAndFences)
		{
			queue->GetDeleter()->DeleteWhenUnused(semaphore);
			queue->GetDeleter()->DeleteWhenUnused(fence);
		}

		for (auto& perTexture : mTextures)
		{
			queue->GetDeleter()->DeleteWhenUnused(perTexture.renderingDoneSemaphore);
		}

		if (mHandle != VK_NULL_HANDLE)
		{
			//queue->GetDeleter()->DeleteWhenUnused(mHandle);
			vkDestroySwapchainKHR(device->GetHandle(), mHandle, nullptr);
			mHandle = VK_NULL_HANDLE;
		}
		mTextures.clear();
	}

	SurfaceAcquireNextTextureStatus SwapChain::AcquireNextTexture()
	{
		return AcquireNextTextureImpl(false);
	}

	SurfaceAcquireNextTextureStatus SwapChain::AcquireNextTextureImpl(bool isReentrant)
	{
		Device* device = checked_cast<Device>(mDevice);

		SurfaceAcquireNextTextureStatus status{};

		VkSemaphore semaphore = mAquireImageSemaphoreAndFences[mCurrentFrameIndex].semaphore;
		VkFence fence = mAquireImageSemaphoreAndFences[mCurrentFrameIndex].fence;

		VkResult err = vkWaitForFences(device->GetHandle(), 1, &fence, VK_TRUE, UINT64_MAX);
		CHECK_VK_RESULT(err, "vkWaitForFences");
		err = vkResetFences(device->GetHandle(), 1, &fence);
		CHECK_VK_RESULT(err, "vkResetFences");

		err = vkAcquireNextImageKHR(device->GetHandle(), mHandle, UINT64_MAX, semaphore, fence, &mImageIndex);
		switch (err)
		{
		case VK_SUCCESS:
		{
			status = SurfaceAcquireNextTextureStatus::Success;
			break;
		}
		case VK_SUBOPTIMAL_KHR:
		{
			status = SurfaceAcquireNextTextureStatus::Suboptimal;
			if (isReentrant)
			{
				status = SurfaceAcquireNextTextureStatus::SurfaceLost;
				break;
			}
			// Re-initialize the VkSwapchain and try getting the texture again.
			Initialize(this);
			return AcquireNextTextureImpl(true);
		}
		case VK_ERROR_OUT_OF_DATE_KHR:
		{
			status = SurfaceAcquireNextTextureStatus::Outdated;
			if (isReentrant)
			{
				status = SurfaceAcquireNextTextureStatus::SurfaceLost;
				break;
			}
			// Re-initialize the VkSwapchain and try getting the texture again.
			Initialize(this);
			return AcquireNextTextureImpl(true);
		}

		case VK_ERROR_SURFACE_LOST_KHR:
		{
			status = SurfaceAcquireNextTextureStatus::SurfaceLost;
			break;
		}
		default:
			CHECK_VK_RESULT(err, "AcquireNextImage");
			break;
		}

		VkSemaphoreSubmitInfo& waitInfo = checked_cast<Queue>(device->GetQueue(QueueType::Graphics))->
			GetPendingRecordingContext()->waitSemaphoreSubmitInfos.emplace_back();
		waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		waitInfo.pNext = nullptr;
		waitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		waitInfo.semaphore = semaphore;
		waitInfo.value = 0;
	}

	Ref<TextureViewBase> SwapChain::GetCurrentTextureView()
	{
		return mTextures[mImageIndex].defualtView;
	}

	Ref<TextureBase> SwapChain::GetCurrentTexture()
	{
		return mTextures[mImageIndex].texture;
	}

	void SwapChain::Present()
	{
		Device* device = checked_cast<Device>(mDevice);
		Queue* queue = checked_cast<Queue>(device->GetQueue(QueueType::Graphics).Get());

		VkSemaphore semaphore = mTextures[mImageIndex].renderingDoneSemaphore;

		VkSemaphoreSubmitInfo& signalInfo = queue->GetPendingRecordingContext()->signalSemaphoreSubmitInfos.emplace_back();
		signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalInfo.pNext = nullptr;
		signalInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		signalInfo.semaphore = semaphore;
		signalInfo.value = 0;

		// to ensure that all commandBuffers on this queue have been executed to completion and transition the colorattchment layout.
		mTextures[mImageIndex].texture->TransitionUsageNow(queue, cSwapChainImagePresentUsage, mTextures[mImageIndex].texture->GetAllSubresources());

		queue->SubmitPendingCommands();

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &semaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &mHandle;
		presentInfo.pImageIndices = &mImageIndex;

		VkResult err = vkQueuePresentKHR(queue->GetHandle(), &presentInfo);
		if (err == VK_ERROR_OUT_OF_DATE_KHR)
		{
			Initialize(this);
		}
		else
		{
			CHECK_VK_RESULT(err, "QueuePresent");
		}

		mCurrentFrameIndex = (mCurrentFrameIndex + 1) % mTextures.size();
	}

	VkSwapchainKHR SwapChain::GetHandle() const
	{
		return mHandle;
	}
}