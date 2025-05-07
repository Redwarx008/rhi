#pragma once

#include "../DeviceBase.h"
#include "../common/Ref.hpp"
#include "CommandRecordContextVk.h"
#include "VulkanEXTFunctions.h"

#include <vk_mem_alloc.h>
#include <array>

namespace rhi::impl::vulkan
{
	class Adapter;

	struct VkDeviceInfo
	{
		VkPhysicalDeviceFeatures features;
		VkPhysicalDeviceProperties properties;
	};

	class Device final : public DeviceBase
	{
	public:
		static Ref<Device> Create(Adapter* adapter, const DeviceDesc& desc);
		// api implementation
		//void WaitIdle() override;
		Ref<SwapChainBase> CreateSwapChain(SurfaceBase* surface, SwapChainBase* previous, const SurfaceConfiguration& config) override;
		Ref<PipelineLayoutBase> CreatePipelineLayout(const PipelineLayoutDesc& desc) override;
		Ref<RenderPipelineBase> CreateRenderPipeline(const RenderPipelineDesc& desc) override;
		Ref<ComputePipelineBase> CreateComputePipeline(const ComputePipelineDesc& desc) override;
		Ref<BindSetLayoutBase> CreateBindSetLayout(const BindSetLayoutDesc& desc) override;
		Ref<BindSetBase> CreateBindSet(const BindSetDesc& desc) override;
		Ref<TextureBase> CreateTexture(const TextureDesc& desc) override;
		Ref<BufferBase> CreateBuffer(const BufferDesc& desc) override;
		Ref<ShaderModuleBase> CreateShader(const ShaderModuleDesc& desc) override;
		Ref<SamplerBase> CreateSampler(const SamplerDesc& desc) override;
		Ref<CommandListBase> CreateCommandList(CommandEncoder* encoder) override;

		VkDevice GetHandle() const;
		VmaAllocator GetMemoryAllocator() const;
		VkPhysicalDevice GetVkPhysicalDevice() const;
		const VkDeviceInfo& GetVkDeviceInfo() const;
		uint32_t GetOptimalBytesPerRowAlignment() const override;
		uint32_t GetOptimalBufferToTextureCopyOffsetAlignment() const override;

		VulkanExtFunctions Fn{};
	private:
		explicit Device(Adapter* adapter, const DeviceDesc& desc);
		~Device();
		bool Initialize(const DeviceDesc& desc);
		void LoadExtFunctions();
			
		VkDevice mHandle = VK_NULL_HANDLE;

		VmaAllocator mMemoryAllocator = VK_NULL_HANDLE;

		VkDeviceInfo mVkDeviceInfo{};
	};
}

