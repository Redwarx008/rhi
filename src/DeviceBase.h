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
		virtual Ref<SwapChainBase> CreateSwapChain(SurfaceBase* surface, SwapChainBase* previous, const SurfaceConfiguration& config) = 0;
		virtual Ref<PipelineLayoutBase> CreatePipelineLayout(const PipelineLayoutDesc& desc) = 0;
		virtual Ref<RenderPipelineBase> CreateRenderPipeline(const RenderPipelineDesc& desc) = 0;
		virtual Ref<ComputePipelineBase> CreateComputePipeline(const ComputePipelineDesc& desc) = 0;
		virtual Ref<BindSetLayoutBase> CreateBindSetLayout(const BindSetLayoutDesc& desc) = 0;
		virtual Ref<BindSetBase> CreateBindSet(const BindSetDesc& desc) = 0;
		virtual Ref<TextureBase> CreateTexture(const TextureDesc& desc) = 0;
		virtual Ref<BufferBase> CreateBuffer(const BufferDesc& desc) = 0;
		virtual Ref<ShaderModuleBase> CreateShader(const ShaderModuleDesc& desc) = 0;
		virtual Ref<SamplerBase> CreateSampler(const SamplerDesc& desc) = 0;
		virtual Ref<CommandListBase> CreateCommandList(CommandEncoder* encoder) = 0;
		virtual uint32_t GetOptimalBytesPerRowAlignment() const = 0;
		virtual uint32_t GetOptimalBufferToTextureCopyOffsetAlignment() const = 0;
		ResourceList* GetTrackedObjectList(ResourceType type);
		bool IsDebugLayerEnabled() const;
		BindSetLayoutBase* GetEmptyBindSetLayout();
		CallbackTaskManager& GetCallbackTaskManager();
	protected:
		explicit DeviceBase(AdapterBase* adapter, const DeviceDesc& desc);
		~DeviceBase();
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
	};
}