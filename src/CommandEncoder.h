#pragma once

#include "RHIStruct.h"
#include "Commands.h"
#include "EncodingContext.h"
#include "CommandListBase.h"
#include "common/Ref.hpp"
#include "common/RefCounted.h"
#include <memory>

namespace rhi::impl
{
	class CommandEncoder : public RefCounted
	{
	public:
		static Ref<CommandEncoder> Create(DeviceBase* device);

		//void ClearColorTexture(TextureViewBase* textureView, const ClearColor& color);
		//void ClearDepthStencil(TextureViewBase* textureView, ClearDepthStencilFlag flag, float depthVal, uint8_t stencilVal);
		void APIClearBuffer(BufferBase* buffer, uint32_t value, uint64_t offset = 0, uint64_t size = ~0ull);
		void APICopyBufferToBuffer(BufferBase* srcBuffer, uint64_t srcOffset, BufferBase* dstBuffer, uint64_t dstOffset, uint64_t dataSize);
		void APICopyBufferToTexture(BufferBase* srcBuffer, const TextureDataLayout& dataLayout, const TextureSlice& dstTextureSlice);
		void APICopyTextureToBuffer(const TextureSlice& srcTextureSlice, BufferBase* dstBuffer, const TextureDataLayout& dataLayout);
		void APICopyTextureToTexture(const TextureSlice& srcTextureSlice, const TextureSlice& dstTextureSlice);
		void APIMapBufferAsync(BufferBase* buffer, MapMode usage, BufferMapCallback callback, void* userData);
		void APIBeginDebugLabel(std::string_view label, const Color* color);
		void APIEndDebugLabel();
		RenderPassEncoder* APIBeginRenderPass(const RenderPassDesc& desc);
		ComputePassEncoder* APIBeginComputePass();
		Ref<RenderPassEncoder> BeginRenderPass(const RenderPassDesc& desc);
		Ref<ComputePassEncoder> BeginComputePass();
		CommandListBase* APIFinish();

		CommandIterator AcquireCommands();
		CommandListResourceUsage AcquireResourceUsages();
		void OnRenderPassEnd();
		void OnComputePassEnd();
	private:
		explicit CommandEncoder(DeviceBase* device);
		enum class State
		{
			OutsideOfPass,
			InRenderPass,
			InComputePass
		};
		DeviceBase* mDevice;
		EncodingContext mEncodingContext;
		uint64_t mDebugLabelCount = 0;
		State mState = State::OutsideOfPass;
	};
}