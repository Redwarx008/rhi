#include "CommandEncoder.h"
#include "Commands.h"
#include "common/Utils.h"
#include "common/Error.h"
#include "BufferBase.h"
#include "TextureBase.h"
#include "RenderPassEncoder.h"
#include "ComputePassEncoder.h"
#include "DeviceBase.h"
#include "Subresource.h"

namespace rhi::impl
{
	CommandEncoder::CommandEncoder(DeviceBase* device) :
		mDevice(device)
	{

	}

	Ref<CommandEncoder> CommandEncoder::Create(DeviceBase* device)
	{
		Ref<CommandEncoder> encoder = AcquireRef(new CommandEncoder(device));
		return encoder;
	}

	void CommandEncoder::APIClearBuffer(BufferBase* buffer, uint32_t value, uint64_t offset, uint64_t size)
	{
		INVALID_IF(mState != State::OutsideOfPass, "The command must be outside of the compute pass and render pass.");
		//ASSERT(HasFlag(buffer->APIGetUsage(), BufferUsage::CopyDst));

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		ClearBufferCmd* cmd = allocator.Allocate<ClearBufferCmd>(Command::ClearBuffer);
		cmd->buffer = buffer;
		cmd->value = value;
		cmd->offset = offset;
		cmd->size = size;
	}

	void CommandEncoder::APICopyBufferToBuffer(BufferBase* srcBuffer, uint64_t srcOffset, BufferBase* dstBuffer, uint64_t dstOffset, uint64_t dataSize)
	{
		INVALID_IF(mState != State::OutsideOfPass, "The command must be outside of the compute pass and render pass.");
		ASSERT(HasFlag(srcBuffer->APIGetUsage(), BufferUsage::CopySrc));
		//ASSERT(HasFlag(srcBuffer->APIGetUsage(), BufferUsage::CopyDst));

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		CopyBufferToBufferCmd* cmd = allocator.Allocate<CopyBufferToBufferCmd>(Command::CopyBufferToBuffer);
		cmd->srcBuffer = srcBuffer;
		cmd->srcOffset = srcOffset;
		cmd->dstBuffer = dstBuffer;
		cmd->dstOffset = dstOffset;
		cmd->size = dataSize;
	}

	void CommandEncoder::APICopyBufferToTexture(BufferBase* srcBuffer, const TextureDataLayout& dataLayout, const TextureSlice& dstTextureSlice)
	{
		INVALID_IF(mState != State::OutsideOfPass, "The command must be outside of the compute pass and render pass.");
		ASSERT(HasFlag(srcBuffer->APIGetUsage(), BufferUsage::CopySrc));
		ASSERT(HasFlag(dstTextureSlice.texture->APIGetUsage(), TextureUsage::CopyDst));
		ASSERT(dataLayout.bytesPerRow != 0 && dataLayout.rowsPerImage != 0);


		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		CopyBufferToTextureCmd* cmd = allocator.Allocate<CopyBufferToTextureCmd>(Command::CopyBufferToTexture);
		cmd->srcBuffer = srcBuffer;
		cmd->dataLayout = dataLayout;
		cmd->dstTexture = dstTextureSlice.texture;
		cmd->origin = dstTextureSlice.origin;
		cmd->size = dstTextureSlice.size;
		cmd->mipLevel = dstTextureSlice.mipLevel;
		cmd->aspect = AspectConvert(cmd->dstTexture->APIGetFormat(), dstTextureSlice.aspect);
	}

	void CommandEncoder::APICopyTextureToBuffer(const TextureSlice& srcTextureSlice, BufferBase* dstBuffer, const TextureDataLayout& dataLayout)
	{
		INVALID_IF(mState != State::OutsideOfPass, "The command must be outside of the compute pass and render pass.");
		ASSERT(HasFlag(srcTextureSlice.texture->APIGetUsage(), TextureUsage::CopySrc));
		ASSERT(HasFlag(dstBuffer->APIGetUsage(), BufferUsage::CopyDst));
		ASSERT(dataLayout.bytesPerRow != 0 && dataLayout.rowsPerImage != 0);

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		CopyTextureToBufferCmd* cmd = allocator.Allocate<CopyTextureToBufferCmd>(Command::CopyTextureToBuffer);
		cmd->srcTexture = srcTextureSlice.texture;
		cmd->dataLayout = dataLayout;
		cmd->aspect = AspectConvert(cmd->srcTexture->APIGetFormat(), srcTextureSlice.aspect);
		cmd->origin = srcTextureSlice.origin;
		cmd->size = srcTextureSlice.size;
		cmd->mipLevel = srcTextureSlice.mipLevel;
	}

	void CommandEncoder::APICopyTextureToTexture(const TextureSlice& srcTextureSlice, const TextureSlice& dstTextureSlice)
	{
		INVALID_IF(mState != State::OutsideOfPass, "The command must be outside of the compute pass and render pass.");
		ASSERT(HasFlag(srcTextureSlice.texture->APIGetUsage(), TextureUsage::CopySrc));
		ASSERT(HasFlag(dstTextureSlice.texture->APIGetUsage(), TextureUsage::CopyDst));

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		CopyTextureToTextureCmd* cmd = allocator.Allocate<CopyTextureToTextureCmd>(Command::CopyTextureToTexture);
		cmd->srcTexture = srcTextureSlice.texture;
		cmd->srcOrigin = srcTextureSlice.origin;
		cmd->srcSize = srcTextureSlice.size;
		cmd->srcAspect = AspectConvert(cmd->srcTexture->APIGetFormat(), srcTextureSlice.aspect);
		cmd->srcMipLevel = srcTextureSlice.mipLevel;

		cmd->dstTexture = dstTextureSlice.texture;
		cmd->dstOrigin = dstTextureSlice.origin;
		cmd->dstSize = dstTextureSlice.size;
		cmd->dstAspect = AspectConvert(cmd->dstTexture->APIGetFormat(), dstTextureSlice.aspect);
		cmd->dstMipLevel = dstTextureSlice.mipLevel;
	}


	void CommandEncoder::APIMapBufferAsync(BufferBase* buffer, MapMode usage, BufferMapCallback callback, void* userData)
	{
		INVALID_IF(mState != State::OutsideOfPass, "The command must be outside of the compute pass and render pass.");
		ASSERT(HasFlag(buffer->APIGetUsage(), BufferUsage::MapRead | BufferUsage::MapWrite));

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		MapBufferAsyncCmd* cmd = allocator.Allocate<MapBufferAsyncCmd>(Command::MapBufferAsync);
		cmd->buffer = buffer;
		cmd->callback = callback;
		cmd->mapMode = usage;
		cmd->userData = userData;
	}

	void CommandEncoder::APIBeginDebugLabel(std::string_view label, const Color* color)
	{
		INVALID_IF(mState != State::OutsideOfPass, "The command must be outside of the compute pass and render pass.");
		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		BeginDebugLabelCmd* cmd = allocator.Allocate<BeginDebugLabelCmd>(Command::BeginDebugLabel);
		EnsureValidString(allocator, label, &cmd->labelLength);
		cmd->color = color == nullptr ? Color() : *color;
		++mDebugLabelCount;
	}

	void CommandEncoder::APIEndDebugLabel()
	{
		INVALID_IF(mState != State::OutsideOfPass, "The command must be outside of the compute pass and render pass.");
		INVALID_IF(mDebugLabelCount == 0, "EndDebugLabel called when no DebugLabels are begin.");

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		allocator.Allocate<EndDebugLabelCmd>(Command::EndDebugLabel);
		--mDebugLabelCount;
	}

	Ref<RenderPassEncoder> CommandEncoder::BeginRenderPass(const RenderPassDesc& desc)
	{
		INVALID_IF(mState != State::OutsideOfPass, "The command must be outside of the compute pass and render pass.");

		SyncScopeUsageTracker usageTracker;

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		BeginRenderPassCmd* cmd = allocator.Allocate<BeginRenderPassCmd>(Command::BeginRenderPass);
		cmd->colorAttachmentCount = desc.colorAttachmentCount;
		auto& colorAttachments = cmd->colorAttachments;
		for (uint32_t i = 0; i < cmd->colorAttachmentCount; ++i)
		{
			colorAttachments[i].view = desc.colorAttachments[i].view;
			colorAttachments[i].resolveView = desc.colorAttachments[i].resolveView;
			colorAttachments[i].loadOp = desc.colorAttachments[i].loadOp;
			colorAttachments[i].storeOp = desc.colorAttachments[i].storeOp;
			colorAttachments[i].clearColor = desc.colorAttachments[i].clearValue;

			usageTracker.TextureViewUsedAs(colorAttachments[i].view.Get(), TextureUsage::RenderAttachment);
			if (colorAttachments[i].resolveView)
			{
				usageTracker.TextureViewUsedAs(colorAttachments[i].resolveView.Get(), TextureUsage::RenderAttachment);
			}
		}

		if (desc.depthStencilAttachment)
		{
			cmd->depthStencilAttachment.view = desc.depthStencilAttachment->view;
			cmd->depthStencilAttachment.depthClearValue = desc.depthStencilAttachment->depthClearValue;
			cmd->depthStencilAttachment.depthLoadOp = desc.depthStencilAttachment->depthLoadOp;
			cmd->depthStencilAttachment.depthStoreOp = desc.depthStencilAttachment->depthStoreOp;
			cmd->depthStencilAttachment.stencilClearValue = desc.depthStencilAttachment->stencilClearValue;
			cmd->depthStencilAttachment.stencilLoadOp = desc.depthStencilAttachment->stencilLoadOp;
			cmd->depthStencilAttachment.stencilStoreOp = desc.depthStencilAttachment->stencilStoreOp;

			usageTracker.TextureViewUsedAs(cmd->depthStencilAttachment.view.Get(), TextureUsage::RenderAttachment);
		}

		mState = State::InRenderPass;
		Ref<RenderPassEncoder> renderPassEncoder = RenderPassEncoder::Create(this, mEncodingContext, std::move(usageTracker));
		return renderPassEncoder;

	}


	Ref<ComputePassEncoder> CommandEncoder::BeginComputePass()
	{
		INVALID_IF(mState != State::OutsideOfPass, "The command must be outside of the compute pass and render pass.");

		CommandAllocator& allocator = mEncodingContext.GetCommandAllocator();
		BeginComputePassCmd* cmd = allocator.Allocate<BeginComputePassCmd>(Command::BeginComputePass);
		mState = State::InComputePass;
		Ref<ComputePassEncoder> computePassEncoder = ComputePassEncoder::Create(this, mEncodingContext);
		return computePassEncoder;
	}

	RenderPassEncoder* CommandEncoder::APIBeginRenderPass(const RenderPassDesc& desc)
	{
		return BeginRenderPass(desc).Detach();
	}

	ComputePassEncoder* CommandEncoder::APIBeginComputePass()
	{
		return BeginComputePass().Detach();
	}

	CommandListBase* CommandEncoder::APIFinish()
	{
		Ref<CommandListBase> commandBuffer = mDevice->CreateCommandList(this);
		return commandBuffer.Detach();
	}

	CommandIterator CommandEncoder::AcquireCommands()
	{
		return mEncodingContext.AcquireCommands();
	}

	CommandListResourceUsage CommandEncoder::AcquireResourceUsages()
	{
		return CommandListResourceUsage{ mEncodingContext.AcquireRenderPassUsages(), mEncodingContext.AcquireComputePassUsages() };
	}

	void CommandEncoder::OnRenderPassEnd()
	{
		mState = State::OutsideOfPass;
	}

	void CommandEncoder::OnComputePassEnd()
	{
		mState = State::OutsideOfPass;
	}
}

