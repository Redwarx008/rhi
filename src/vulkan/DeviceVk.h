#pragma once

#include "../DeviceBase.h"
#include "../common/Ref.hpp"
#include "CommandRecordContextVk.h"
#include "VulkanEXTFunctions.h"

#include <array>
#include <vk_mem_alloc.h>

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
        // void WaitIdle() override;
        Ref<SwapChainBase> CreateSwapChainImpl(SurfaceBase* surface,
                                               SwapChainBase* previous,
                                               const SurfaceConfiguration& config) override;
        Ref<PipelineLayoutBase> CreatePipelineLayoutImpl(const PipelineLayoutDesc& desc) override;
        Ref<PipelineLayoutBase> CreatePipelineLayout2Impl(const PipelineLayoutDesc2& desc) override;
        Ref<RenderPipelineBase> CreateRenderPipelineImpl(const RenderPipelineDesc& desc) override;
        Ref<ComputePipelineBase> CreateComputePipelineImpl(const ComputePipelineDesc& desc) override;
        Ref<PipelineCacheBase> CreatePipelineCacheImpl(const PipelineCacheDesc& desc) override;
        Ref<BindSetLayoutBase> CreateBindSetLayoutImpl(const BindSetLayoutDesc& desc) override;
        Ref<BindSetBase> CreateBindSetImpl(const BindSetDesc& desc) override;
        Ref<TextureBase> CreateTextureImpl(const TextureDesc& desc) override;
        Ref<BufferBase> CreateBufferImpl(const BufferDesc& desc, QueueType initialQueueOwner = QueueType::Undefined) override;
        Ref<ShaderModuleBase> CreateShaderImpl(const ShaderModuleDesc& desc) override;
        Ref<SamplerBase> CreateSamplerImpl(const SamplerDesc& desc) override;
        Ref<CommandListBase> CreateCommandListImpl(CommandEncoder* encoder) override;

        VkDevice GetHandle() const;
        VmaAllocator GetMemoryAllocator() const;
        VkPhysicalDevice GetVkPhysicalDevice() const;
        const VkDeviceInfo& GetVkDeviceInfo() const;
        uint32_t GetOptimalBytesPerRowAlignment() const override;
        uint32_t GetOptimalBufferToTextureCopyOffsetAlignment() const override;

        VulkanExtFunctions Fn{};

    private:
        explicit Device(Adapter* adapter, const DeviceDesc& desc);
        ~Device() override;
        bool Initialize(const DeviceDesc& desc);
        void LoadExtFunctions();

        VkDevice mHandle = VK_NULL_HANDLE;

        VmaAllocator mMemoryAllocator = VK_NULL_HANDLE;

        VkDeviceInfo mVkDeviceInfo{};
    };
} // namespace rhi::impl::vulkan
