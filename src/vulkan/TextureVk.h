#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <vector>

#include "rhi/RHI.h"
#include "../common/Ref.hpp"
#include "../TextureBase.h"
#include "../Subresource.h"
#include "../SubresourceStorage.hpp"
#include "../SyncScopeUsageTracker.h"

namespace rhi::impl::vulkan
{
	class Device;
	class Queue;
	class TextureView;

	class Texture : public TextureBase
	{
	public:
		static Ref<Texture> Create(Device* device, const TextureDesc& desc);

		Ref<TextureViewBase> CreateView(const TextureViewDesc& desc) override;

		// internal 
		VkImage GetHandle() const;

		void TransitionOwnership(Queue* queue, const SubresourceRange& range, Queue* recevingQueue);
		void TransitionUsageAndGetResourceBarrier(Queue* queue,
			TextureUsage usage,
			ShaderStage shaderStages,
			const SubresourceRange& range);
		void TransitionUsageForMultiRange(Queue* queue, const SubresourceStorage<TextureSyncInfo>& syncInfos);

		void TransitionUsageNow(Queue* queue, TextureUsage usage, const SubresourceRange& range, ShaderStage shaderStages = ShaderStage::None);
	protected:
		explicit Texture(Device* device, const TextureDesc& desc);
		~Texture();
		VkImage mHandle = VK_NULL_HANDLE;
		SubresourceStorage<TextureSyncInfo> mSubresourceLastSyncInfos;
	private:
		bool Initialize();
		void DestroyImpl() override;

		VmaAllocation mAllocation = VK_NULL_HANDLE;
		VkFormat mVkFormat; // we will get it in the hot path, so cache it.

		friend class TextureView;
	};

	class SwapChainTexture final : public Texture
	{
	public:
		static Ref<SwapChainTexture> Create(Device* device, const TextureDesc& desc, VkImage nativeImage);
	private:
		explicit SwapChainTexture(Device* device, const TextureDesc& desc);
		~SwapChainTexture();
		void Initialize(VkImage nativeImage);
		void DestroyImpl() override;
	};

	class TextureView final : public TextureViewBase
	{
	public:
		static Ref<TextureView> Create(TextureBase* texture, const TextureViewDesc& desc);
		// internal 
		VkImageView GetHandle() const;
	private:
		explicit TextureView(TextureBase* texture, const TextureViewDesc& desc);
		bool Initialize();
		void DestroyImpl() override;

		VkImageView mHandle = VK_NULL_HANDLE;
	};

	VkFormat ToVkFormat(TextureFormat format);

	TextureFormat ToTextureFormat(VkFormat format);

	VkImageUsageFlags GetVkImageUsageFlags(TextureUsage usage, TextureFormat format);

	VkSampleCountFlagBits SampleCountConvert(uint32_t sampleCount);

	VkImageType GetVkImageType(TextureDimension dimension);

	VkImageViewType GetVkImageViewType(TextureDimension dimension);

	VkImageAspectFlags GetVkAspectMask(VkFormat format);

	VkImageLayout ImageLayoutConvert(TextureUsage usage, TextureFormat format);
}