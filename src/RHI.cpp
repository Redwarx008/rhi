#include "rhi/rhi.h"

#include "InstanceBase.h"
#include "BindSetBase.h"
#include "BindSetLayoutBase.h"
#include "QueueBase.h"
#include "DeviceBase.h"
#include "AdapterBase.h"
#include "SurfaceBase.h"
#include "RenderPipelineBase.h"
#include "ComputePipelineBase.h"
#include "CommandEncoder.h"
#include "RenderPassEncoder.h"
#include "ComputePassEncoder.h"
#include "CommandListBase.h"
#include "PipelineLayoutBase.h"
#include "SamplerBase.h"
#include "ShaderModuleBase.h"
#include "BufferBase.h"
#include "TextureBase.h"
#include "vulkan/InstanceVk.h"

using namespace rhi::impl;

struct AdapterImpl : public AdapterBase {};
struct BindSetImpl : public BindSetBase {};
struct BindSetLayoutImpl : public BindSetLayoutBase {};
struct QueueImpl : public QueueBase {};
struct DeviceImpl : public DeviceBase {};
struct InstanceImpl : public InstanceBase {};
struct SurfaceImpl : public SurfaceBase {};
struct RenderPipelineImpl : public RenderPipelineBase {};
struct ComputePipelineImpl : public ComputePipelineBase {};
struct CommandEncoderImpl : public CommandEncoder {};
struct RenderPassEncoderImpl : public RenderPassEncoder {};
struct ComputePassEncoderImpl : public ComputePassEncoder {};
struct CommandListImpl : public CommandListBase {};
struct PipelineLayoutImpl : public PipelineLayoutBase {};
struct SamplerImpl : public SamplerBase {};
struct ShaderModuleImpl : public ShaderModuleBase {};
struct BufferImpl : public BufferBase {};
struct TextureImpl : public TextureBase {};
struct TextureViewImpl : public TextureViewBase {};

RHIInstance rhiCreateInstance(const RHIInstanceDesc* desc)
{
	InstanceBase* result;
	switch (desc->backend)
	{
	case RHIBackendType_Vulkan:
		result = rhi::impl::vulkan::Instance::Create(reinterpret_cast<const InstanceDesc&>(*desc)).Detach();
		break;
	default:
		break;
	}
	return static_cast<RHIInstance>(result);
}

void rhiInstanceEnumerateAdapters(RHIInstance instance, RHIAdapter* const pAdapters, uint32_t* adapterCount)
{
	instance->APIEnumerateAdapters(reinterpret_cast<AdapterBase** const>(pAdapters), adapterCount);
}
// methods of Instance
void rhiInstanceAddRef(RHIInstance instance)
{
	instance->AddRef();
}
void rhiInstanceRelease(RHIInstance instance)
{
	instance->Release();
}
// methods of Adapter
RHIDevice rhiAdapterCreateDevice(RHIAdapter adapter, const RHIDeviceDesc* desc)
{
	DeviceBase* result = adapter->APICreateDevice(*reinterpret_cast<const DeviceDesc*>(desc));
	return static_cast<RHIDevice>(result);
}
void rhiAdapterGetInfo(RHIAdapter adapter, RHIAdapterInfo* info)
{
	adapter->APIGetInfo(reinterpret_cast<AdapterInfo*>(info));
}
void rhiAdapterGetLimits(RHIAdapter adapter, RHILimits* limits)
{
	adapter->APIGetLimits(reinterpret_cast<Limits*>(limits));
}
RHIInstance rhiAdapterGetInstance(RHIAdapter adapter)
{
	InstanceBase* result = adapter->APIGetInstance();
	return static_cast<RHIInstance>(result);
}
void rhiAdapterAddRef(RHIAdapter adapter)
{
	adapter->AddRef();
}
void rhiAdapterRelease(RHIAdapter adapter)
{
	adapter->Release();
}
// methods of Device
RHIAdapter rhiDeviceGetAdapter(RHIDevice device)
{
	auto result = device->APIGetAdapter();
	return static_cast<RHIAdapter>(result);
}
RHIQueue rhiDeviceGetQueue(RHIDevice device, RHIQueueType queueType)
{
	auto result = device->APIGetQueue(static_cast<QueueType>(queueType));
	return static_cast<RHIQueue>(result);
}
RHIPipelineLayout rhiCreatePipelineLayout(RHIDevice device, const RHIPipelineLayoutDesc* desc)
{
	auto result = device->APICreatePipelineLayout(*reinterpret_cast<const PipelineLayoutDesc*>(desc));
	return static_cast<RHIPipelineLayout>(result);
}
RHIRenderPipeline rhiDeviceCreateRenderPipeline(RHIDevice device, const RHIRenderPipelineDesc* desc)
{
	auto result = device->APICreateRenderPipeline(*reinterpret_cast<const RenderPipelineDesc*>(desc));
	return static_cast<RHIRenderPipeline>(result);
}
RHIComputePipeline rhiDeviceCreateComputePipeline(RHIDevice device, const RHIComputePipelineDesc* desc)
{
	auto result = device->APICreateComputePipeline(*reinterpret_cast<const ComputePipelineDesc*>(desc));
	return static_cast<RHIComputePipeline>(result);
}
RHIBindSetLayout rhiDeviceCreateBindSetLayout(RHIDevice device, const RHIBindSetLayoutDesc* desc)
{
	auto result = device->APICreateBindSetLayout(*reinterpret_cast<const BindSetLayoutDesc*>(desc));
	return static_cast<RHIBindSetLayout>(result);
}
RHIBindSet rhiDeviceCreateBindSet(RHIDevice device, const RHIBindSetDesc* desc)
{
	auto result = device->APICreateBindSet(*reinterpret_cast<const BindSetDesc*>(desc));
	return static_cast<RHIBindSet>(result);
}
RHITexture rhiDeviceCreateTexture(RHIDevice device, const RHITextureDesc* desc)
{
	auto result = device->APICreateTexture(*reinterpret_cast<const TextureDesc*>(desc));
	return static_cast<RHITexture>(result);
}
RHIBuffer rhiDeviceCreateBuffer(RHIDevice device, const RHIBufferDesc* desc)
{
	auto result = device->APICreateBuffer(*reinterpret_cast<const BufferDesc*>(desc));
	return static_cast<RHIBuffer>(result);
}
RHIShaderModule rhiDeviceCreateShader(RHIDevice device, const RHIShaderModuleDesc* desc)
{
	auto result = device->APICreateShader(*reinterpret_cast<const ShaderModuleDesc*>(desc));
	return static_cast<RHIShaderModule>(result);
}
RHISampler rhiDeviceCreateSampler(RHIDevice device, const RHISamplerDesc* desc)
{
	auto result = device->APICreateSampler(*reinterpret_cast<const SamplerDesc*>(desc));
	return static_cast<RHISampler>(result);
}
RHICommandEncoder rhiDeviceCreateCommandEncoder(RHIDevice device)
{
	auto result = device->APICreateCommandEncoder();
	return static_cast<RHICommandEncoder>(result);
}
void rhiDeviceTick(RHIDevice device)
{
	device->APITick();
}
void rhiDeviceAddRef(RHIDevice device)
{
	device->AddRef();
}
void rhiDeviceRelease(RHIDevice device)
{
	device->Release();
}
// methods of Queue
void rhiQueueWriteBuffer(RHIQueue queue, RHIBuffer buffer, const void* data, uint64_t dataSize, uint64_t offset)
{
	queue->APIWriteBuffer(buffer, data, dataSize, offset);
}
void rhiQueueWriteTexture(RHIQueue queue, const RHITextureSlice* dstTexture, const void* data, size_t dataSize, const RHITextureDataLayout* dataLayout)
{
	queue->APIWriteTexture(*reinterpret_cast<const TextureSlice*>(dstTexture), data, dataSize, *reinterpret_cast<const TextureDataLayout*>(dataLayout));
}
void rhiQueueWaitFor(RHIQueue queue, RHIQueue waitQueue, uint64_t submitSerial)
{
	queue->APIWaitFor(waitQueue, submitSerial);
}
uint64_t rhiQueueSubmit(RHIQueue queue, RHICommandList const* commands, uint32_t commandListCount, RHIResourceTransfer const* transfers, uint32_t transferCount)
{
	return queue->APISubmit(reinterpret_cast<CommandListBase* const*>(commands), commandListCount, reinterpret_cast<ResourceTransfer const*>(transfers), transferCount);
}
void rhiQueueAddRef(RHIQueue queue)
{
	queue->AddRef();
}
void rhiQueueRelease(RHIQueue queue)
{
	queue->Release();
}
// methods of Surface
RHISurface rhiCreateSurfaceFromWindowsHWND(RHIInstance instance, void* hwnd, void* hinstance)
{
	auto result = instance->APICreateSurface(hwnd, hinstance);
	return static_cast<RHISurface>(result);
}
void rhiSurfaceConfigure(RHISurface surface, const RHISurfaceConfiguration* config)
{
	surface->APIConfigure(*reinterpret_cast<const SurfaceConfiguration*>(config));
}
RHISurfaceAcquireNextTextureStatus rhiSurfaceAcquireNextTexture(RHISurface surface)
{
	auto result = surface->APIAcquireNextTexture();
	return static_cast<RHISurfaceAcquireNextTextureStatus>(result);
}
RHITexture rhiSurfaceGetCurrentTexture(RHISurface surface)
{
	auto result = surface->APIGetCurrentTexture();
	return static_cast<RHITexture>(result);
}
RHITextureView rhiSurfaceGetCurrentTextureView(RHISurface surface)
{
	auto result = surface->APIGetCurrentTextureView();
	return static_cast<RHITextureView>(result);
}
RHITextureFormat rhiSurfaceGetSwapChainFormat(RHISurface surface)
{
	auto result = surface->APIGetSwapChainFormat();
	return static_cast<RHITextureFormat>(result);
}
void rhiSurfacePresent(RHISurface surface)
{
	surface->APIPresent();
}
void rhiSurfaceUnconfigure(RHISurface surface)
{
	surface->APIUnconfigure();
}
void rhiSurfaceAddRef(RHISurface surface)
{
	surface->AddRef();
}
void rhiSurfaceRelease(RHISurface surface)
{
	surface->Release();
}
// methods of CommandEncoder
void rhiCommandEncoderClearBuffer(RHICommandEncoder encoder, RHIBuffer buffer, uint32_t value, uint64_t offset, uint64_t size)
{
	encoder->APIClearBuffer(buffer, value, offset, size);
}
void rhiCommandEncoderCopyBufferToBuffer(RHICommandEncoder encoder, RHIBuffer srcBuffer, uint64_t srcOffset, RHIBuffer dstBuffer, uint64_t dstOffset, uint64_t dataSize)
{
	encoder->APICopyBufferToBuffer(srcBuffer, srcOffset, dstBuffer, dstOffset, dataSize);
}
void rhiCommandEncoderCopyBufferToTexture(RHICommandEncoder encoder, RHIBuffer srcBuffer, const RHITextureDataLayout* dataLayout, const RHITextureSlice* dstTextureSlice)
{
	encoder->APICopyBufferToTexture(srcBuffer, *reinterpret_cast<const TextureDataLayout*>(dataLayout), *reinterpret_cast<const TextureSlice*>(dstTextureSlice));
}
void rhiCommandEncoderCopyTextureToBuffer(RHICommandEncoder encoder, const RHITextureSlice* srcTextureSlice, RHIBuffer dstBuffer, const RHITextureDataLayout* dataLayout)
{
	encoder->APICopyTextureToBuffer(*reinterpret_cast<const TextureSlice*>(srcTextureSlice), dstBuffer, *reinterpret_cast<const TextureDataLayout*>(dataLayout));
}
void rhiCommandEncoderCopyTextureToTexture(RHICommandEncoder encoder, const RHITextureSlice* srcTextureSlice, const RHITextureSlice* dstTextureSlice)
{
	encoder->APICopyTextureToTexture(*reinterpret_cast<const TextureSlice*>(srcTextureSlice), *reinterpret_cast<const TextureSlice*>(dstTextureSlice));
}
void rhiCommandEncoderMapBufferAsync(RHICommandEncoder encoder, RHIBuffer buffer, RHIMapMode usage, RHIBufferMapCallback callback, void* userData)
{
	encoder->APIMapBufferAsync(buffer, static_cast<MapMode>(usage), reinterpret_cast<BufferMapCallback>(callback), userData);
}
void rhiCommandEncoderBeginDebugLabel(RHICommandEncoder encoder, RHIStringView label, const RHIColor* color)
{
	encoder->APIBeginDebugLabel({ label.data, label.length }, reinterpret_cast<const Color*>(color));
}
void rhiCommandEncoderEndDebugLabel(RHICommandEncoder encoder)
{
	encoder->APIEndDebugLabel();
}
RHIRenderPassEncoder rhiCommandEncoderBeginRenderPass(RHICommandEncoder encoder, const RHIRenderPassDesc* desc)
{
	auto result = encoder->APIBeginRenderPass(*reinterpret_cast<const RenderPassDesc*>(desc));
	return static_cast<RHIRenderPassEncoder>(result);
}
RHIComputePassEncoder rhiCommandEncoderBeginComputePass(RHICommandEncoder encoder)
{
	auto result = encoder->APIBeginComputePass();
	return static_cast<RHIComputePassEncoder>(result);
}
RHICommandList rhiCommandEncoderFinish(RHICommandEncoder encoder)
{
	auto result = encoder->APIFinish();
	return static_cast<RHICommandList>(result);
}
void rhiCommandEncoderAddRef(RHICommandEncoder encoder)
{
	encoder->AddRef();
}
void rhiCommandEncoderRelease(RHICommandEncoder encoder)
{
	encoder->Release();
}
// methods of RenderPassEncoder
void rhiRenderPassEncoderSetPipeline(RHIRenderPassEncoder encoder, RHIRenderPipeline pipeline)
{
	encoder->APISetPipeline(pipeline);
}
void rhiRenderPassEncoderSetVertexBuffers(RHIRenderPassEncoder encoder, uint32_t firstSlot, uint32_t bufferCount, RHIBuffer const* buffers, uint64_t* offsets)
{
	encoder->APISetVertexBuffers(firstSlot, bufferCount, reinterpret_cast<BufferBase* const*>(buffers), offsets);
}
void rhiRenderPassEncoderSetIndexBuffer(RHIRenderPassEncoder encoder, RHIBuffer buffer, uint64_t offset, uint64_t size, RHIIndexFormat indexFormat)
{
	encoder->APISetIndexBuffer(buffer, static_cast<IndexFormat>(indexFormat), offset, size);
}
void rhiRenderPassEncoderSetScissorRect(RHIRenderPassEncoder encoder, uint32_t firstScissor, const RHIRect* scissors, uint32_t scissorCount)
{
	encoder->APISetScissorRect(firstScissor, reinterpret_cast<const Rect*>(scissors), scissorCount);
}
void rhiRenderPassEncoderSetStencilReference(RHIRenderPassEncoder encoder, uint32_t reference)
{
	encoder->APISetStencilReference(reference);
}
void rhiRenderPassEncoderSetBlendConstant(RHIRenderPassEncoder encoder, const RHIColor* blendConstants)
{
	encoder->APISetBlendConstant(*reinterpret_cast<const Color*>(blendConstants));
}
void rhiRenderPassEncoderSetViewport(RHIRenderPassEncoder encoder, uint32_t firstViewport, RHIViewport const* viewports, uint32_t viewportCount)
{
	encoder->APISetViewport(firstViewport, reinterpret_cast<Viewport const*>(viewports), viewportCount);
}
void rhiRenderPassEncoderSetBindSet(RHIRenderPassEncoder encoder, RHIBindSet set, uint32_t setIndex, uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets)
{
	encoder->APISetBindSet(set, setIndex, dynamicOffsetCount, dynamicOffsets);
}
void rhiRenderPassEncoderDraw(RHIRenderPassEncoder encoder, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	encoder->APIDraw(vertexCount, instanceCount, firstVertex, firstInstance);
}
void rhiRenderPassEncoderDrawIndexed(RHIRenderPassEncoder encoder, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t baseVertex, uint32_t firstInstance)
{
	encoder->APIDrawIndexed(indexCount, instanceCount, firstIndex, baseVertex, firstInstance);
}
void rhiRenderPassEncoderDrawIndirect(RHIRenderPassEncoder encoder, RHIBuffer indirectBuffer, uint64_t indirectOffset)
{
	encoder->APIDrawIndirect(indirectBuffer, indirectOffset);
}
void rhiRenderPassEncoderDrawIndexedIndirect(RHIRenderPassEncoder encoder, RHIBuffer indirectBuffer, uint64_t indirectOffset)
{
	encoder->APIDrawIndexedIndirect(indirectBuffer, indirectOffset);
}
void rhiRenderPassEncoderMultiDrawIndirect(RHIRenderPassEncoder encoder, RHIBuffer indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, RHIBuffer drawCountBuffer, uint64_t drawCountBufferOffset)
{
	encoder->APIMultiDrawIndexedIndirect(indirectBuffer, indirectOffset, maxDrawCount, drawCountBuffer, drawCountBufferOffset);
}
void rhiRenderPassEncoderMultiDrawIndexedIndirect(RHIRenderPassEncoder encoder, RHIBuffer indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, RHIBuffer drawCountBuffer, uint64_t drawCountBufferOffset)
{
	encoder->APIMultiDrawIndirect(indirectBuffer, indirectOffset, maxDrawCount, drawCountBuffer, drawCountBufferOffset);
}
void rhiRenderPassEncoderSetPushConstant(RHIRenderPassEncoder encoder, const void* data, uint32_t size)
{
	encoder->APISetPushConstant(data, size);
}
void rhiRenderPassEncoderBeginDebugLabel(RHIRenderPassEncoder encoder, RHIStringView label, const RHIColor* color)
{
	encoder->APIBeginDebugLabel({ label.data, label.length }, reinterpret_cast<const Color*>(color));
}
void rhiRenderPassEncoderEndDebugLabel(RHIRenderPassEncoder encoder)
{
	encoder->APIEndDebugLabel();
}
void rhiRenderPassEncoderEnd(RHIRenderPassEncoder encoder)
{
	encoder->APIEnd();
}
void rhiRenderPassEncoderAddRef(RHIRenderPassEncoder encoder)
{
	encoder->AddRef();
}
void rhiRenderPassEncoderRelease(RHIRenderPassEncoder encoder)
{
	encoder->Release();
}
// methods of ComputePassEncoder
void rhiComputePassEncoderSetPipeline(RHIComputePassEncoder encoder, RHIComputePipeline pipeline)
{
	encoder->APISetPipeline(pipeline);
}
void rhiComputePassEncoderDispatch(RHIComputePassEncoder encoder, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	encoder->APIDispatch(groupCountX, groupCountY, groupCountZ);
}
void rhiComputePassEncoderDispatchIndirect(RHIComputePassEncoder encoder, RHIBuffer indirectBuffer, uint64_t indirectOffset)
{
	encoder->APIDispatchIndirect(indirectBuffer, indirectOffset);
}
void rhiComputePassEncoderSetBindSet(RHIComputePassEncoder encoder, RHIBindSet set, uint32_t setIndex, uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets)
{
	encoder->APISetBindSet(set, setIndex, dynamicOffsetCount, dynamicOffsets);
}
void rhiComputePassEncoderSetPushConstant(RHIComputePassEncoder encoder, const void* data, uint32_t size)
{
	encoder->APISetPushConstant(data, size);
}
void rhiComputePassEncoderBeginDebugLabel(RHIComputePassEncoder encoder, RHIStringView label, const RHIColor* color)
{
	encoder->APIBeginDebugLabel({ label.data, label.length }, reinterpret_cast<const Color*>(color));
}
void rhiComputePassEncoderEndDebugLabel(RHIComputePassEncoder encoder)
{
	encoder->APIEndDebugLabel();
}
void rhiComputePassEncoderEnd(RHIComputePassEncoder encoder)
{
	encoder->APIEnd();
}
void rhiComputePassEncoderAddRef(RHIComputePassEncoder encoder)
{
	encoder->AddRef();
}
void rhiComputePassEncoderRelease(RHIComputePassEncoder encoder)
{
	encoder->Release();
}
// methods of ComputePipeline
void rhiComputePipelineAddRef(RHIComputePipeline pipeline)
{
	pipeline->AddRef();
}
void rhiComputePipelineRelease(RHIComputePipeline pipeline)
{
	pipeline->Release();
}
// methods of CommandList
void rhiCommandListAddRef(RHICommandList commandList)
{
	commandList->AddRef();
}
void rhiCommandListRelease(RHICommandList commandList)
{
	commandList->Release();
}
// methods of RenderPipeline
void rhiRenderPipelineAddRef(RHIRenderPipeline pipeline)
{
	pipeline->AddRef();
}
void rhiRenderPipelineRelease(RHIRenderPipeline pipeline)
{
	pipeline->Release();
}
// methods of PipelineLayout
RHIBindSetLayout rhiPipelineLayoutIGetBindSetLayout(RHIPipelineLayout pipelineLayout, uint32_t bindSetIndex)
{
	auto result = pipelineLayout->APIGetBindSetLayout(bindSetIndex);
	return static_cast<RHIBindSetLayout>(result);
}
void rhiPipelineLayoutAddRef(RHIPipelineLayout pipelineLayout)
{
	pipelineLayout->AddRef();
}
void rhiPipelineLayoutRelease(RHIPipelineLayout pipelineLayout)
{
	pipelineLayout->Release();
}
// methods of BindSetLayout
void rhiBindSetLayoutAddRef(RHIBindSetLayout bindSetLayout)
{
	bindSetLayout->AddRef();
}
void rhiBindSetLayoutRelease(RHIBindSetLayout bindSetLayout)
{
	bindSetLayout->Release();
}
// methods of BindSet
void rhiBindSetDestroy(RHIBindSet bindSet)
{
	bindSet->APIDestroy();
}
void rhiBindSetAddRef(RHIBindSet bindSet)
{
	bindSet->AddRef();
}
void rhiBindSetRelease(RHIBindSet bindSet)
{
	bindSet->Release();
}
// methods of Buffer
RHIBufferUsage rhiBufferGetUsage(RHIBuffer buffer)
{
	auto result = buffer->APIGetUsage();
	return static_cast<RHIBufferUsage>(result);
}
uint64_t rhiBufferGetSize(RHIBuffer buffer)
{
	return buffer->APIGetSize();
}
void* rhiBufferGetMappedPointer(RHIBuffer buffer)
{
	return buffer->APIGetMappedPointer();
}
void rhiBufferDestroy(RHIBuffer buffer)
{
	buffer->APIDestroy();
}
void rhiBufferAddRef(RHIBuffer buffer)
{
	buffer->AddRef();
}
void rhiBufferRelease(RHIBuffer buffer)
{
	buffer->Release();
}
// methods of Texture
uint32_t rhiTextureGetWidth(RHITexture texture)
{
	return texture->APIGetWidth();
}
uint32_t rhiTextureGetHeight(RHITexture texture)
{
	return texture->APIGetHeight();
}
uint32_t rhiTextureGetDepthOrArrayLayers(RHITexture texture)
{
	return texture->APIGetDepthOrArrayLayers();
}
uint32_t rhiTextureGetMipLevelCount(RHITexture texture)
{
	return texture->APIGetMipLevelCount();
}
uint32_t rhiTextureGetSampleCount(RHITexture texture)
{
	return texture->APIGetSampleCount();
}
RHITextureDimension rhiTextureGetDimension(RHITexture texture)
{
	auto result = texture->APIGetDimension();
	return static_cast<RHITextureDimension>(result);
}
RHITextureFormat rhiTextureGetFormat(RHITexture texture)
{
	auto result = texture->APIGetFormat();
	return static_cast<RHITextureFormat>(result);
}
RHITextureUsage rhiTextureGetUsage(RHITexture texture)
{
	auto result = texture->APIGetUsage();
	return static_cast<RHITextureUsage>(result);
}
RHITextureView rhiTextureCreateView(RHITexture texture, const RHITextureViewDesc* desc)
{
	auto result = texture->APICreateView(reinterpret_cast<const TextureViewDesc*>(desc));
	return static_cast<RHITextureView>(result);
}
void rhiTextureDestroy(RHITexture texture)
{
	texture->APIDestroy();
}
void rhiTextureAddRef(RHITexture texture)
{
	texture->AddRef();
}
void rhiTextureRelease(RHITexture texture)
{
	texture->Release();
}
// methods of TextureView
void rhiTextureViewAddRef(RHITextureView textureView)
{
	textureView->AddRef();
}
void rhiTextureViewRelease(RHITextureView textureView)
{
	textureView->Release();
}
// methods of Sampler
//void rhiSamplerDestroy(RHISampler sampler);
void rhiSamplerAddRef(RHISampler sampler)
{
	sampler->AddRef();
}
void rhiSamplerRelease(RHISampler sampler)
{
	sampler->Release();
}
// methods of ShaderModule
void rhiShaderModuleAddRef(RHIShaderModule shaderModule)
{
	shaderModule->AddRef();
}
void rhiShaderModuleRelease(RHIShaderModule shaderModule)
{
	shaderModule->Release();
}
namespace rhi::impl
{
	LoggingCallback gDebugMessageCallback;
	void* gDebugMessageCallbackUserData;
}