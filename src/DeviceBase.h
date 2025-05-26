#pragma once

#include "RHIStruct.h"
#include "common/Ref.hpp"
#include "common/RefCounted.h"
#include "ResourceBase.h"
#include "FeatureSet.h"
#include "CallbackTaskManager.h"
#include "QueueBase.h"
#include <array>

namespace rhi::impl
{
    class DeviceBase : public RefCounted
    {
    public:
        AdapterBase* APIGetAdapter() const;
        QueueBase* APIGetQueue(QueueType queueType);
        PipelineLayoutBase* APICreatePipelineLayout(const PipelineLayoutDesc& desc);
        PipelineLayoutBase* APICreatePipelineLayout2(const PipelineLayoutDesc2& desc);
        RenderPipelineBase* APICreateRenderPipeline(const RenderPipelineDesc& desc);
        ComputePipelineBase* APICreateComputePipeline(const ComputePipelineDesc& desc);
        BindSetLayoutBase* APICreateBindSetLayout(const BindSetLayoutDesc& desc);
        BindSetBase* APICreateBindSet(const BindSetDesc& desc);
        TextureBase* APICreateTexture(const TextureDesc& desc);
        BufferBase* APICreateBuffer(const BufferDesc& desc);
        ShaderModuleBase* APICreateShader(const ShaderModuleDesc& desc);
        SamplerBase* APICreateSampler(const SamplerDesc& desc);
        CommandEncoder* APICreateCommandEncoder();
        void APITick();

        Ref<QueueBase> GetQueue(QueueType queueType);

        Ref<PipelineLayoutBase> GetOrCreatePipelineLayout(const PipelineLayoutDesc& desc);
        Ref<PipelineLayoutBase> GetOrCreatePipelineLayout2(const PipelineLayoutDesc2& desc);
        Ref<BindSetLayoutBase> GetOrCreateBindSetLayout(const BindSetLayoutDesc& desc);
        Ref<SamplerBase> GetOrCreateSampler(const SamplerDesc& desc);

        virtual Ref<SwapChainBase> CreateSwapChainImpl(SurfaceBase* surface,
                                                   SwapChainBase* previous,
                                                   const SurfaceConfiguration& config) = 0;
        virtual Ref<PipelineLayoutBase> CreatePipelineLayoutImpl(const PipelineLayoutDesc& desc) = 0;
        virtual Ref<PipelineLayoutBase> CreatePipelineLayout2Impl(const PipelineLayoutDesc2& desc) = 0;
        virtual Ref<RenderPipelineBase> CreateRenderPipelineImpl(const RenderPipelineDesc& desc) = 0;
        virtual Ref<ComputePipelineBase> CreateComputePipelineImpl(const ComputePipelineDesc& desc) = 0;
        virtual Ref<BindSetLayoutBase> CreateBindSetLayoutImpl(const BindSetLayoutDesc& desc) = 0;
        virtual Ref<BindSetBase> CreateBindSetImpl(const BindSetDesc& desc) = 0;
        virtual Ref<TextureBase> CreateTextureImpl(const TextureDesc& desc) = 0;
        virtual Ref<BufferBase> CreateBufferImpl(const BufferDesc& desc) = 0;
        virtual Ref<ShaderModuleBase> CreateShaderImpl(const ShaderModuleDesc& desc) = 0;
        virtual Ref<SamplerBase> CreateSamplerImpl(const SamplerDesc& desc) = 0;
        virtual Ref<CommandListBase> CreateCommandListImpl(CommandEncoder* encoder) = 0;
        virtual uint32_t GetOptimalBytesPerRowAlignment() const = 0;
        virtual uint32_t GetOptimalBufferToTextureCopyOffsetAlignment() const = 0;
        ResourceList* GetTrackedObjectList(ResourceType type);
        bool IsDebugLayerEnabled() const;
        BindSetLayoutBase* GetEmptyBindSetLayout();
        CallbackTaskManager& GetCallbackTaskManager();

    protected:
        explicit DeviceBase(AdapterBase* adapter, const DeviceDesc& desc);
        ~DeviceBase();
        void Initialize();
        bool HasRequiredFeature(FeatureName feature);
        void CreateEmptyBindSetLayout();
        void DestroyObjects();
        Ref<AdapterBase> mAdapter;
        std::array<Ref<QueueBase>, 3> mQueues;

    private:
        void SetFeatures(const DeviceDesc& desc);

        FeatureSet mRequiredFeatures;

        // The vulkan spec says that members in the VkPipelineLayoutCreateInfo.pSetLayouts array must not be nullptr.
        Ref<BindSetLayoutBase> mEmptyBindSetLayout;

        std::array<ResourceList, static_cast<uint32_t>(ResourceType::Count)> mTrackedResources;

        CallbackTaskManager mCallbackTaskManager;

        struct Cache;
        std::unique_ptr<Cache> mCaches;
    };
}
