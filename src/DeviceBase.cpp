#include "DeviceBase.h"
#include <functional>
#include <type_traits>
#include "AdapterBase.h"
#include "BindSetBase.h"
#include "BindSetLayoutBase.h"
#include "BufferBase.h"
#include "CommandEncoder.h"
#include "ComputePipelineBase.h"
#include "InstanceBase.h"
#include "PipelineLayoutBase.h"
#include "QueueBase.h"
#include "RenderPipelineBase.h"
#include "SamplerBase.h"
#include "ShaderModuleBase.h"
#include "TextureBase.h"
#include "PipelineCacheBase.h"
#include "common/Cached.hpp"

namespace rhi::impl
{
    struct DeviceBase::Cache
    {
        CachedObjects<BindSetLayoutBase> bindSetLayouts;
        CachedObjects<PipelineLayoutBase> pipelineLayouts;
        CachedObjects<SamplerBase> samplers;
    };

    template <typename T>
    Ref<T> GetOrCreate(CachedObjects<T>& cache, T* key, std::function<Ref<T>()> createFunc)
    {
        static_assert(std::is_base_of_v<RefCounted, T>, "Type must be refcounted.");
        static_assert(std::is_base_of_v<Cached<T>, T>, "Type must be Cached.");

        Ref<T> result = cache.Find(key);
        if (result != nullptr)
        {
            return result;
        }

        result = createFunc();
        ASSERT(result != nullptr);
        bool inserted = false;
        std::tie(result, inserted) = cache.Insert(result.Get());
        ASSERT(inserted);
        return result;
    }

    DeviceBase::DeviceBase(AdapterBase* adapter, const DeviceDesc& desc)
        : mAdapter(adapter)
    {
        SetFeatures(desc);
        // Todo: create cache object.
    }

    DeviceBase::~DeviceBase() {}

    void DeviceBase::Initialize()
    {
        mCaches = std::make_unique<DeviceBase::Cache>();
        CreateEmptyBindSetLayout();
    }

    void DeviceBase::SetFeatures(const DeviceDesc& desc)
    {
        for (uint32_t i = 0; i < desc.requiredFeatureCount; ++i)
        {
            mRequiredFeatures.EnableFeature(desc.requiredFeatures[i]);
        }
    }

    bool DeviceBase::HasRequiredFeature(FeatureName feature)
    {
        return mRequiredFeatures.IsEnabled(feature);
    }

    bool DeviceBase::IsDebugLayerEnabled() const
    {
        return mAdapter->GetInstance()->IsDebugLayerEnabled();
    }

    ResourceList* DeviceBase::GetTrackedObjectList(ResourceType type)
    {
        return &mTrackedResources[static_cast<uint32_t>(type)];
    }

    void DeviceBase::APITick()
    {
        for (auto& queue : mQueues)
        {
            if (queue && queue->NeedsTick())
            {
                queue->CheckAndUpdateCompletedSerial();
                queue->Tick();
            }
        }
        mCallbackTaskManager.Flush();
    }

    void DeviceBase::DestroyObjects()
    {
        static constexpr std::array<ResourceType, static_cast<uint32_t>(ResourceType::Count)>
                cResourceTypeDependencyOrder = {
                        ResourceType::RenderPipeline,
                        ResourceType::ComputePipeline,
                        ResourceType::PipelineLayout,
                        ResourceType::PipelineCache,
                        ResourceType::BindSet,
                        ResourceType::BindSetLayout,
                        ResourceType::ShaderModule,
                        ResourceType::Texture, // Note that Textures own the TextureViews.
                        ResourceType::Sampler,
                        ResourceType::Buffer,
                };

        for (ResourceType type : cResourceTypeDependencyOrder)
        {
            mTrackedResources[static_cast<uint32_t>(type)].Destroy();
        }
    }

    Ref<QueueBase> DeviceBase::GetQueue(QueueType queueType)
    {
        static_assert(static_cast<uint32_t>(QueueType::Graphics) == 0);
        static_assert(static_cast<uint32_t>(QueueType::Compute) == 1);
        static_assert(static_cast<uint32_t>(QueueType::Transfer) == 2);
        return mQueues[static_cast<uint32_t>(queueType)];
    }

    CommandEncoder* DeviceBase::APICreateCommandEncoder()
    {
        Ref<CommandEncoder> encoder = CommandEncoder::Create(this);
        return encoder.Detach();
    }

    RenderPipelineBase* DeviceBase::APICreateRenderPipeline(const RenderPipelineDesc& desc)
    {
        Ref<RenderPipelineBase> pipeline = CreateRenderPipelineImpl(desc);
        return pipeline.Detach();
    }

    ComputePipelineBase* DeviceBase::APICreateComputePipeline(const ComputePipelineDesc& desc)
    {
        Ref<ComputePipelineBase> pipeline = CreateComputePipelineImpl(desc);
        return pipeline.Detach();
    }

    PipelineCacheBase* DeviceBase::APICreatePipelineCache(const PipelineCacheDesc& desc)
    {
        Ref<PipelineCacheBase> cache = CreatePipelineCacheImpl(desc);
        return cache.Detach();
    }

    BindSetLayoutBase* DeviceBase::APICreateBindSetLayout(const BindSetLayoutDesc& desc)
    {
        Ref<BindSetLayoutBase> bindSetLayout = GetOrCreateBindSetLayout(desc);
        return bindSetLayout.Detach();
    }

    BindSetBase* DeviceBase::APICreateBindSet(const BindSetDesc& desc)
    {
        Ref<BindSetBase> bindSet = CreateBindSetImpl(desc);
        return bindSet.Detach();
    }

    TextureBase* DeviceBase::APICreateTexture(const TextureDesc& desc)
    {
        Ref<TextureBase> texture = CreateTextureImpl(desc);
        return texture.Detach();
    }

    BufferBase* DeviceBase::APICreateBuffer(const BufferDesc& desc)
    {
        Ref<BufferBase> buffer = CreateBufferImpl(desc);
        return buffer.Detach();
    }

    ShaderModuleBase* DeviceBase::APICreateShader(const ShaderModuleDesc& desc)
    {
        Ref<ShaderModuleBase> shader = CreateShaderImpl(desc);
        return shader.Detach();
    }

    SamplerBase* DeviceBase::APICreateSampler(const SamplerDesc& desc)
    {
        Ref<SamplerBase> sampler = GetOrCreateSampler(desc);
        return sampler.Detach();
    }

    QueueBase* DeviceBase::APIGetQueue(QueueType queueType)
    {
        return GetQueue(queueType).Detach();
    }

    PipelineLayoutBase* DeviceBase::APICreatePipelineLayout(const PipelineLayoutDesc& desc)
    {
        Ref<PipelineLayoutBase> layout = GetOrCreatePipelineLayout(desc);
        return layout.Detach();
    }

    PipelineLayoutBase* DeviceBase::APICreatePipelineLayout2(const PipelineLayoutDesc2& desc)
    {
        Ref<PipelineLayoutBase> layout = GetOrCreatePipelineLayout2(desc);
        return layout.Detach();
    }

    AdapterBase* DeviceBase::APIGetAdapter() const
    {
        Ref<AdapterBase> adapter = mAdapter;
        return adapter.Detach();
    }

    Ref<PipelineLayoutBase> DeviceBase::GetOrCreatePipelineLayout(const PipelineLayoutDesc& desc)
    {
        PipelineLayoutBase key(this, desc);
        const size_t hash = key.ComputeContentHash();
        key.SetContentHash(hash);

        Ref<PipelineLayoutBase> result = GetOrCreate<PipelineLayoutBase>(mCaches->pipelineLayouts,
                                                                         &key,
                                                                         [&]() -> Ref<PipelineLayoutBase>
                                                                         {
                                                                             Ref<PipelineLayoutBase> pipelineLayout =
                                                                                     CreatePipelineLayoutImpl(desc);
                                                                             pipelineLayout->SetContentHash(hash);
                                                                             return pipelineLayout;
                                                                         });
        return result;
    }

    Ref<PipelineLayoutBase> DeviceBase::GetOrCreatePipelineLayout2(const PipelineLayoutDesc2& desc)
    {
        PipelineLayoutBase key(this, desc);
        const size_t hash = key.ComputeContentHash();
        key.SetContentHash(hash);

        Ref<PipelineLayoutBase> result = GetOrCreate<PipelineLayoutBase>(mCaches->pipelineLayouts,
                                                                         &key,
                                                                         [&]() -> Ref<PipelineLayoutBase>
                                                                         {
                                                                             Ref<PipelineLayoutBase> pipelineLayout =
                                                                                     CreatePipelineLayout2Impl(desc);
                                                                             pipelineLayout->SetContentHash(hash);
                                                                             return pipelineLayout;
                                                                         });
        return result;
    }

    Ref<BindSetLayoutBase> DeviceBase::GetOrCreateBindSetLayout(const BindSetLayoutDesc& desc)
    {
        BindSetLayoutBase key(this, desc);
        const size_t hash = key.ComputeContentHash();
        key.SetContentHash(hash);

        Ref<BindSetLayoutBase> result = GetOrCreate<BindSetLayoutBase>(mCaches->bindSetLayouts,
                                                                       &key,
                                                                       [&]() -> Ref<BindSetLayoutBase>
                                                                       {
                                                                           Ref<BindSetLayoutBase> bindSetLayout =
                                                                                   CreateBindSetLayoutImpl(desc);
                                                                           bindSetLayout->SetContentHash(hash);
                                                                           return bindSetLayout;
                                                                       });
        return result;
    }

    Ref<SamplerBase> DeviceBase::GetOrCreateSampler(const SamplerDesc& desc)
    {
        SamplerBase key(this, desc);
        const size_t hash = key.ComputeContentHash();
        key.SetContentHash(hash);

        Ref<SamplerBase> result = GetOrCreate<SamplerBase>(mCaches->samplers,
                                                           &key,
                                                           [&]() -> Ref<SamplerBase>
                                                           {
                                                               Ref<SamplerBase> sampler = CreateSamplerImpl(desc);
                                                               sampler->SetContentHash(hash);
                                                               return sampler;
                                                           });
        return result;
    }

    BindSetLayoutBase* DeviceBase::GetEmptyBindSetLayout()
    {
        return mEmptyBindSetLayout.Get();
    }

    CallbackTaskManager& DeviceBase::GetCallbackTaskManager()
    {
        return mCallbackTaskManager;
    }

    void DeviceBase::CreateEmptyBindSetLayout()
    {
        BindSetLayoutDesc desc{};
        desc.name = "EmptyBindSetLayout";
        desc.entryCount = 0;
        desc.entries = nullptr;
        mEmptyBindSetLayout = CreateBindSetLayoutImpl(desc);
    }
} // namespace rhi::impl
