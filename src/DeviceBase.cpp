#include "DeviceBase.h"
#include "AdapterBase.h"
#include "InstanceBase.h"
#include "QueueBase.h"
#include "ShaderModuleBase.h"
#include "SamplerBase.h"
#include "BufferBase.h"
#include "TextureBase.h"
#include "CommandEncoder.h"
#include "BindSetBase.h"
#include "BindSetLayoutBase.h"
#include "RenderPipelineBase.h"
#include "ComputePipelineBase.h"
#include "PipelineLayoutBase.h"


namespace rhi::impl
{
	DeviceBase::DeviceBase(AdapterBase* adapter, const DeviceDesc& desc)
		:mAdapter(adapter)
	{
		SetFeatures(desc);
		// Todo: create cache object.
	}

	DeviceBase::~DeviceBase() {}

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
		static constexpr std::array<ResourceType, static_cast<uint32_t>(ResourceType::Count)> cResourceTypeDependencyOrder =
		{
			ResourceType::RenderPipeline,
			ResourceType::ComputePipeline,
			ResourceType::PipelineLayout,
			ResourceType::BindSet,
			ResourceType::BindSetLayout,
			ResourceType::ShaderModule,
			ResourceType::Texture,  // Note that Textures own the TextureViews.
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
		Ref<RenderPipelineBase> pipeline = CreateRenderPipeline(desc);
		return pipeline.Detach();
	}

	ComputePipelineBase* DeviceBase::APICreateComputePipeline(const ComputePipelineDesc& desc)
	{
		Ref<ComputePipelineBase> pipeline = CreateComputePipeline(desc);
		return pipeline.Detach();
	}

	BindSetLayoutBase* DeviceBase::APICreateBindSetLayout(const BindSetLayoutDesc& desc)
	{
		Ref<BindSetLayoutBase> bindSetLayout = CreateBindSetLayout(desc);
		return bindSetLayout.Detach();
	}

	BindSetBase* DeviceBase::APICreateBindSet(const BindSetDesc& desc)
	{
		Ref<BindSetBase> bindSet = CreateBindSet(desc);
		return bindSet.Detach();
	}

	TextureBase* DeviceBase::APICreateTexture(const TextureDesc& desc)
	{
		Ref<TextureBase> texture = CreateTexture(desc);
		return texture.Detach();
	}

	BufferBase* DeviceBase::APICreateBuffer(const BufferDesc& desc)
	{
		Ref<BufferBase> buffer = CreateBuffer(desc);
		return buffer.Detach();
	}

	ShaderModuleBase* DeviceBase::APICreateShader(const ShaderModuleDesc& desc)
	{
		Ref<ShaderModuleBase> shader = CreateShader(desc);
		return shader.Detach();
	}

	SamplerBase* DeviceBase::APICreateSampler(const SamplerDesc& desc)
	{
		Ref<SamplerBase> sampler = CreateSampler(desc);
		return sampler.Detach();
	}

	QueueBase* DeviceBase::APIGetQueue(QueueType queueType)
	{
		return GetQueue(queueType).Detach();
	}

	PipelineLayoutBase* DeviceBase::APICreatePipelineLayout(const PipelineLayoutDesc& desc)
	{
		Ref<PipelineLayoutBase> layout = CreatePipelineLayout(desc);
		return layout.Detach();
	}

	AdapterBase* DeviceBase::APIGetAdapter() const
	{
		Ref<AdapterBase> adapter = mAdapter;
		return adapter.Detach();
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
		mEmptyBindSetLayout = CreateBindSetLayout(desc);
	}
}