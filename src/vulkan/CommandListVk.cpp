#include "CommandListVk.h"

#include "DeviceVk.h"
#include "QueueVk.h"
#include "BufferVk.h"
#include "TextureVk.h"
#include "RenderPipelineVk.h"
#include "ComputePipelineVk.h"
#include "PipelineLayoutVk.h"
#include "BindSetVk.h"
#include "ErrorsVk.h"
#include "VulkanUtils.h"
#include "CommandRecordContextVk.h"
#include "../PassResourceUsage.h"
#include "../Commands.h"

#include <array>
#include <optional>

namespace rhi::impl::vulkan
{
	CommandList::CommandList(Device* device, CommandEncoder* encoder) :
		CommandListBase(device, encoder)
	{

	}

	Ref<CommandList> CommandList::Create(Device* device, CommandEncoder* encoder)
	{
		Ref<CommandList> commandList = AcquireRef(new CommandList(device, encoder));
		return commandList;
	}

	VkIndexType VulkanIndexType(IndexFormat format)
	{
		switch (format)
		{
		case IndexFormat::Uint16:
			return VK_INDEX_TYPE_UINT16;
		case IndexFormat::Uint32:
			return VK_INDEX_TYPE_UINT32;
		}
		ASSERT(!"Unreachable");
	}

	VkAttachmentLoadOp VulkanAttachmentLoadOp(LoadOp op)
	{
		switch (op)
		{
		case LoadOp::Load:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
		case LoadOp::Clear:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case LoadOp::DontCare:
		default:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}
	}

	VkAttachmentStoreOp VulkanAttachmentStoreOp(StoreOp op)
	{
		switch (op)
		{
		case StoreOp::Store:
			return VK_ATTACHMENT_STORE_OP_STORE;
		case StoreOp::Discard:
		default:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}
	}

	void TransitionSyncScope(Queue* queue, const SyncScopeResourceUsage& scopeUsage)
	{
		for (uint32_t i = 0; i < scopeUsage.buffers.size(); ++i)
		{
			auto buffer = checked_cast<Buffer>(scopeUsage.buffers[i]);
			buffer->TrackUsageAndGetResourceBarrier(queue, scopeUsage.bufferSyncInfos[i].usage, scopeUsage.bufferSyncInfos[i].shaderStages);
		}

		for (uint32_t i = 0; i < scopeUsage.textures.size(); ++i)
		{
			auto texture = checked_cast<Texture>(scopeUsage.textures[i]);
			texture->TransitionUsageForMultiRange(queue, scopeUsage.textureSyncInfos[i]);
		}

		queue->GetPendingRecordingContext()->EmitBarriers();
	}

	void CommandList::RecordRenderPass(Queue* queue, BeginRenderPassCmd* renderPassCmd)
	{
		Device* device = checked_cast<Device>(mDevice);

		VkCommandBuffer commandBuffer = queue->GetPendingRecordingContext()->commandBufferAndPool.bufferHandle;

		VkRenderingInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;

		uint32_t renderWidth = checked_cast<TextureView>(renderPassCmd->colorAttachments[0].view)->GetTexture()->APIGetWidth();
		uint32_t renderHeight = checked_cast<TextureView>(renderPassCmd->colorAttachments[0].view)->GetTexture()->APIGetHeight();
		std::array<VkRenderingAttachmentInfo, cMaxColorAttachments> colorAttachmentInfos;

		for (uint32_t i = 0; i < renderPassCmd->colorAttachmentCount; ++i)
		{
			TextureView* view = checked_cast<TextureView>(renderPassCmd->colorAttachments[i].view.Get());
			INVALID_IF(view->GetTexture()->APIGetWidth() != renderWidth || view->GetTexture()->APIGetHeight() != renderHeight,
				"The color attachment size (width: %u, height: %u) does not match the size of the other attachments (width: %u, height: %u).",
				view->GetTexture()->APIGetWidth(), view->GetTexture()->APIGetHeight(), renderWidth, renderHeight);

			VkRenderingAttachmentInfo& attachment = colorAttachmentInfos[i];
			attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			attachment.pNext = nullptr;
			attachment.imageView = view->GetHandle();
			attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			attachment.loadOp = VulkanAttachmentLoadOp(renderPassCmd->colorAttachments[i].loadOp);
			attachment.storeOp = VulkanAttachmentStoreOp(renderPassCmd->colorAttachments[i].storeOp);
			attachment.clearValue.color =
			{
				renderPassCmd->colorAttachments[i].clearColor.r,
				renderPassCmd->colorAttachments[i].clearColor.g,
				renderPassCmd->colorAttachments[i].clearColor.b,
				renderPassCmd->colorAttachments[i].clearColor.a,
			};
			attachment.resolveMode = VK_RESOLVE_MODE_NONE;
			attachment.resolveImageView = nullptr;
			attachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			if (renderPassCmd->colorAttachments[i].resolveView != nullptr)
			{
				attachment.resolveImageView = view->GetHandle();
				attachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
				attachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
		}

		renderingInfo.renderArea = { 0, 0, renderWidth, renderHeight };
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = renderPassCmd->colorAttachmentCount;
		renderingInfo.pColorAttachments = colorAttachmentInfos.data();

		// A single depth stencil attachment info can be used, but they can also be specified separately.
		// When both are specified separately, the only requirement is that the image view is identical.	
		VkRenderingAttachmentInfo depthAttachment{};
		VkRenderingAttachmentInfo stencilAttachment{};
		if (renderPassCmd->depthStencilAttachment.view != nullptr)
		{
			TextureView* view = checked_cast<TextureView>(renderPassCmd->depthStencilAttachment.view.Get());
			INVALID_IF(view->GetTexture()->APIGetWidth() < renderWidth || view->GetTexture()->APIGetHeight() < renderHeight,
				"The depth stencil attachment size (width: %u, height: %u) is less than the size of the color attachments (width: %u, height: %u).",
				view->GetTexture()->APIGetWidth(), view->GetTexture()->APIGetHeight(), renderWidth, renderHeight);

			depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			depthAttachment.imageView = checked_cast<TextureView>(renderPassCmd->depthStencilAttachment.view)->GetHandle();
			depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			depthAttachment.loadOp = VulkanAttachmentLoadOp(renderPassCmd->depthStencilAttachment.depthLoadOp);
			depthAttachment.storeOp = VulkanAttachmentStoreOp(renderPassCmd->depthStencilAttachment.depthStoreOp);
			depthAttachment.clearValue.depthStencil.depth = renderPassCmd->depthStencilAttachment.depthClearValue;

			stencilAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			stencilAttachment.imageView = checked_cast<TextureView>(renderPassCmd->depthStencilAttachment.view)->GetHandle();
			stencilAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			stencilAttachment.loadOp = VulkanAttachmentLoadOp(renderPassCmd->depthStencilAttachment.stencilLoadOp);
			stencilAttachment.storeOp = VulkanAttachmentStoreOp(renderPassCmd->depthStencilAttachment.stencilStoreOp);
			stencilAttachment.clearValue.depthStencil.stencil = renderPassCmd->depthStencilAttachment.stencilClearValue;

			renderingInfo.pDepthAttachment = &depthAttachment;
			renderingInfo.pStencilAttachment = &stencilAttachment;
		}

		vkCmdBeginRendering(commandBuffer, &renderingInfo);
		
		// Set the default value for the dynamic state
		vkCmdSetDepthBounds(commandBuffer, 0.0f, 1.0f);
		vkCmdSetStencilReference(commandBuffer, VK_STENCIL_FRONT_AND_BACK, 0);

		float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		vkCmdSetBlendConstants(commandBuffer, blendConstants);

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(renderHeight);
		viewport.width = static_cast<float>(renderWidth);
		viewport.height = -static_cast<float>(renderHeight);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissorRect;
		scissorRect.offset.x = 0;
		scissorRect.offset.y = 0;
		scissorRect.extent.width = renderWidth;
		scissorRect.extent.height = renderHeight;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);

		RenderPipeline* lastPipeline = nullptr;
		Command type;
		while (mCommandIter.NextCommandId(&type))
		{
			switch (type)
			{
			case Command::SetRenderPipeline:
			{
				SetRenderPipelineCmd* cmd = mCommandIter.NextCommand<SetRenderPipelineCmd>();
				RenderPipeline* pipeline = checked_cast<RenderPipeline>(cmd->pipeline.Get());
				lastPipeline = pipeline;
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetHandle());
				break;
			}
			case Command::SetBindSet:
			{
				SetBindSetCmd* cmd = mCommandIter.NextCommand<SetBindSetCmd>();
				BindSet* bindSet = checked_cast<BindSet>(cmd->set.Get());
				bindSet->MarkUsedInQueue(queue->GetType());
				VkDescriptorSet set = bindSet->GetHandle();
				uint32_t* dynamicOffsets = nullptr;
				if (cmd->dynamicOffsetCount > 0) {
					dynamicOffsets = mCommandIter.NextData<uint32_t>(cmd->dynamicOffsetCount);
				}
				ASSERT(lastPipeline != nullptr);
				VkPipelineLayout layout = checked_cast<PipelineLayout>(lastPipeline->GetLayout())->GetHandle();
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, cmd->setIndex, 1,
					&set, cmd->dynamicOffsetCount, dynamicOffsets);
				break;
			}
			case Command::SetIndexBuffer:
			{
				SetIndexBufferCmd* cmd = mCommandIter.NextCommand<SetIndexBufferCmd>();
				Buffer* indexBuffer = checked_cast<Buffer>(cmd->buffer.Get());
				vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetHandle(), cmd->offset, VulkanIndexType(cmd->format));
				break;
			}
			case Command::SetVertexBuffer:
			{
				SetVertexBufferCmd* cmd = mCommandIter.NextCommand<SetVertexBufferCmd>();
				std::array<VkBuffer, cMaxVertexBuffers> buffers;
				std::array<VkDeviceSize, cMaxVertexBuffers> offsets;
				for (uint32_t i = 0; i < cmd->bufferCount; ++i)
				{
					buffers[i] = checked_cast<Buffer>(cmd->buffers[i].buffer)->GetHandle();
					offsets[i] = cmd->buffers[i].offset;
				}
				vkCmdBindVertexBuffers(commandBuffer, cmd->firstSlot, cmd->bufferCount, buffers.data(), offsets.data());
				break;
			}
			case Command::Draw:
			{
				DrawCmd* cmd = mCommandIter.NextCommand<DrawCmd>();
				vkCmdDraw(commandBuffer, cmd->vertexCount, cmd->instanceCount, cmd->firstVertex, cmd->firstInstance);
				break;
			}
			case Command::DrawIndexed:
			{
				DrawIndexedCmd* cmd = mCommandIter.NextCommand<DrawIndexedCmd>();
				vkCmdDrawIndexed(commandBuffer, cmd->indexCount, cmd->instanceCount, cmd->firstIndex, cmd->baseVertex, cmd->firstInstance);
				break;
			}
			case Command::DrawIndirect:
			{
				DrawIndirectCmd* cmd = mCommandIter.NextCommand<DrawIndirectCmd>();
				Buffer* buffer = checked_cast<Buffer>(cmd->indirectBuffer.Get());
				vkCmdDrawIndirect(commandBuffer, buffer->GetHandle(), static_cast<VkDeviceSize>(cmd->indirectOffset), 1, 0);
				break;
			}
			case Command::DrawIndexedIndirect:
			{
				DrawIndexedIndirectCmd* cmd = mCommandIter.NextCommand<DrawIndexedIndirectCmd>();
				Buffer* buffer = checked_cast<Buffer>(cmd->indirectBuffer.Get());
				vkCmdDrawIndexedIndirect(commandBuffer, buffer->GetHandle(), static_cast<VkDeviceSize>(cmd->indirectOffset), 1, 0);
				break;
			}
			case Command::MultiDrawIndirect:
			{
				MultiDrawIndirectCmd* cmd = mCommandIter.NextCommand<MultiDrawIndirectCmd>();
				Buffer* indirectBuffer = checked_cast<Buffer>(cmd->indirectBuffer.Get());
				// Count buffer is optional
				Buffer* countBuffer = checked_cast<Buffer>(cmd->drawCountBuffer.Get());
				if (countBuffer == nullptr)
				{
					vkCmdDrawIndirect(commandBuffer, indirectBuffer->GetHandle(), static_cast<VkDeviceSize>(cmd->indirectOffset), cmd->maxDrawCount, cDrawIndirectSize);
				}
				else
				{
					vkCmdDrawIndirectCount(commandBuffer, indirectBuffer->GetHandle(),
						static_cast<VkDeviceSize>(cmd->indirectOffset), countBuffer->GetHandle(),
						static_cast<VkDeviceSize>(cmd->drawCountOffset), cmd->maxDrawCount, cDrawIndirectSize);
				}
				break;
			}
			case Command::MultiDrawIndexedIndirect:
			{
				MultiDrawIndexedIndirectCmd* cmd = mCommandIter.NextCommand<MultiDrawIndexedIndirectCmd>();
				Buffer* indirectBuffer = checked_cast<Buffer>(cmd->indirectBuffer.Get());

				// Count buffer is optional
				Buffer* countBuffer = checked_cast<Buffer>(cmd->drawCountBuffer.Get());
				if (countBuffer == nullptr)
				{
					vkCmdDrawIndexedIndirect(commandBuffer, indirectBuffer->GetHandle(), static_cast<VkDeviceSize>(cmd->indirectOffset), cmd->maxDrawCount, cDrawIndexedIndirectSize);
				}
				else
				{
					vkCmdDrawIndexedIndirectCount(commandBuffer, indirectBuffer->GetHandle(),
						static_cast<VkDeviceSize>(cmd->indirectOffset), countBuffer->GetHandle(),
						static_cast<VkDeviceSize>(cmd->drawCountOffset), cmd->maxDrawCount, cDrawIndexedIndirectSize);
				}
				break;
			}
			case Command::SetPushConstant:
			{
				SetPushConstantCmd* cmd = mCommandIter.NextCommand<SetPushConstantCmd>();
				void* data = mCommandIter.NextData<uint32_t>(cmd->size);
				PipelineLayout* pipelineLayout = checked_cast<PipelineLayout>(lastPipeline->GetLayout());

				vkCmdPushConstants(commandBuffer, pipelineLayout->GetHandle(), pipelineLayout->GetPushConstantVisibility(), 0, cmd->size, data);
				break;
			}
			case Command::EndRenderPass:
			{
				EndRenderPassCmd* cmd = mCommandIter.NextCommand<EndRenderPassCmd>();
				vkCmdEndRendering(commandBuffer);
				return; //to void mCommandIter.reset()
			}
			case Command::SetViewport:
			{
				SetViewportCmd* cmd = mCommandIter.NextCommand<SetViewportCmd>();
				std::array<VkViewport, cMaxViewports> viewports;
				for (uint32_t i = 0; i < cmd->viewportCount; ++i)
				{
					VkViewport& viewport = viewports[i];
					viewport.x = cmd->viewports[i].x;
					viewport.y = cmd->viewports[i].y + cmd->viewports[i].height;
					viewport.width = cmd->viewports[i].width;
					viewport.height = -cmd->viewports[i].height;
					viewport.minDepth = cmd->viewports[i].minDepth;
					viewport.maxDepth = cmd->viewports[i].maxDepth;
				}

				vkCmdSetViewport(commandBuffer, cmd->firstViewport, cmd->viewportCount, viewports.data());
				break;
			}
			case Command::SetScissorRects:
			{
				SetScissorRectsCmd* cmd = mCommandIter.NextCommand<SetScissorRectsCmd>();
				std::array<VkRect2D, cMaxViewports> scissorRect;
				for (uint32_t i = 0; i < cmd->scissorCount; ++i)
				{
					VkRect2D& rect = scissorRect[i];
					rect.offset.x = cmd->scissors[i].x;
					rect.offset.y = cmd->scissors[i].y;
					rect.extent.width = cmd->scissors[i].width;
					rect.extent.height = cmd->scissors[i].height;
				}

				vkCmdSetScissor(commandBuffer, cmd->firstScissor, cmd->scissorCount, scissorRect.data());
				break;
			}
			case Command::SetStencilReference:
			{
				SetStencilReferenceCmd* cmd = mCommandIter.NextCommand<SetStencilReferenceCmd>();
				vkCmdSetStencilReference(commandBuffer, VK_STENCIL_FRONT_AND_BACK, cmd->reference);
				break;
			}
			case Command::SetBlendConstant:
			{
				SetBlendConstantCmd* cmd = mCommandIter.NextCommand<SetBlendConstantCmd>();
				const std::array<float, 4> blendConstants =
				{
					cmd->color.r,
					cmd->color.g,
					cmd->color.b,
					cmd->color.a,
				};

				vkCmdSetBlendConstants(commandBuffer, blendConstants.data());
				break;
			}

			case Command::BeginDebugLabel:
			{
				BeginDebugLabelCmd* cmd = mCommandIter.NextCommand<BeginDebugLabelCmd>();
				const char* label = mCommandIter.NextData<char>(cmd->labelLength);
				if (mDevice->IsDebugLayerEnabled())
				{
					VkDebugUtilsLabelEXT utilsLabel;
					utilsLabel.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
					utilsLabel.pNext = nullptr;
					utilsLabel.pLabelName = label;
					utilsLabel.color[0] = cmd->color.r;
					utilsLabel.color[1] = cmd->color.g;
					utilsLabel.color[2] = cmd->color.b;
					utilsLabel.color[3] = cmd->color.a;

					device->Fn.vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &utilsLabel);
				}
				break;
			}
			case Command::EndDebugLabel:
			{
				EndDebugLabelCmd* cmd = mCommandIter.NextCommand<EndDebugLabelCmd>();
				if (mDevice->IsDebugLayerEnabled())
				{
					device->Fn.vkCmdEndDebugUtilsLabelEXT(commandBuffer);
				}
				break;
			}

			default:
				ASSERT(!"Unreachable");
				break;
			}
		}
	}

	void CommandList::RecordComputePass(Queue* queue, BeginComputePassCmd* computePassCmd)
	{
		Device* device = checked_cast<Device>(mDevice);

		VkCommandBuffer commandBuffer = queue->GetPendingRecordingContext()->commandBufferAndPool.bufferHandle;
		ComputePipeline* lastPipeline = nullptr;
		Command type;
		while (mCommandIter.NextCommandId(&type))
		{
			switch (type)
			{
			case Command::SetComputePipeline:
			{
				SetComputePipelineCmd* cmd = mCommandIter.NextCommand<SetComputePipelineCmd>();
				ComputePipeline* pipeline = checked_cast<ComputePipeline>(cmd->pipeline.Get());
				lastPipeline = pipeline;
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->GetHandle());
				break;
			}
			case Command::SetBindSet:
			{
				SetBindSetCmd* cmd = mCommandIter.NextCommand<SetBindSetCmd>();
				BindSet* bindSet = checked_cast<BindSet>(cmd->set.Get());
				bindSet->MarkUsedInQueue(queue->GetType());
				VkDescriptorSet set = bindSet->GetHandle();
				uint32_t* dynamicOffsets = nullptr;
				if (cmd->dynamicOffsetCount > 0) {
					dynamicOffsets = mCommandIter.NextData<uint32_t>(cmd->dynamicOffsetCount);
				}
				ASSERT(lastPipeline != nullptr);
				VkPipelineLayout layout = checked_cast<PipelineLayout>(lastPipeline->GetLayout())->GetHandle();
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, cmd->setIndex, 1,
					&set, cmd->dynamicOffsetCount, dynamicOffsets);
				break;
			}
			case Command::Dispatch:
			{
				DispatchCmd* cmd = mCommandIter.NextCommand<DispatchCmd>();
				vkCmdDispatch(commandBuffer, cmd->x, cmd->y, cmd->z);
				break;
			}
			case Command::DispatchIndirect:
			{
				DispatchIndirectCmd* cmd = mCommandIter.NextCommand<DispatchIndirectCmd>();
				Buffer* indirectBuffer = checked_cast<Buffer>(cmd->indirectBuffer.Get());
				vkCmdDispatchIndirect(commandBuffer, indirectBuffer->GetHandle(), cmd->indirectOffset);
				break;
			}
			case Command::SetPushConstant:
			{
				SetPushConstantCmd* cmd = mCommandIter.NextCommand<SetPushConstantCmd>();
				void* data = mCommandIter.NextData<uint32_t>(cmd->size);
				PipelineLayout* pipelineLayout = checked_cast<PipelineLayout>(lastPipeline->GetLayout());

				vkCmdPushConstants(commandBuffer, pipelineLayout->GetHandle(), pipelineLayout->GetPushConstantVisibility(), 0, cmd->size, data);
				break;
			}
			case Command::EndComputePass:
			{
				mCommandIter.NextCommand<EndComputePassCmd>();
				break;
			}
			case Command::BeginDebugLabel:
				{
					BeginDebugLabelCmd* cmd = mCommandIter.NextCommand<BeginDebugLabelCmd>();
					const char* label = mCommandIter.NextData<char>(cmd->labelLength);
					if (mDevice->IsDebugLayerEnabled())
					{
						VkDebugUtilsLabelEXT utilsLabel;
						utilsLabel.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
						utilsLabel.pNext = nullptr;
						utilsLabel.pLabelName = label;
						utilsLabel.color[0] = cmd->color.r;
						utilsLabel.color[1] = cmd->color.g;
						utilsLabel.color[2] = cmd->color.b;
						utilsLabel.color[3] = cmd->color.a;

						device->Fn.vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &utilsLabel);
					}
					break;
				}
			case Command::EndDebugLabel:
			{
				EndDebugLabelCmd* cmd = mCommandIter.NextCommand<EndDebugLabelCmd>();
				if (mDevice->IsDebugLayerEnabled())
				{
					device->Fn.vkCmdEndDebugUtilsLabelEXT(commandBuffer);
				}
				break;
			}
			default:
				ASSERT(!"Unreachable");
				break;
			}
		}
	}

	void CommandList::RecordCommands(Queue* queue)
	{
		Device* device = checked_cast<Device>(mDevice);

		CommandRecordContext* recordContext = queue->GetPendingRecordingContext();

		VkCommandBuffer commandBuffer = recordContext->commandBufferAndPool.bufferHandle;

		uint32_t nextRenderPassIndex = 0;
		uint32_t nextComputePassIndex = 0;

		Command type;
		while (mCommandIter.NextCommandId(&type))
		{
			switch (type)
			{
			case Command::ClearBuffer:
			{
				ClearBufferCmd* cmd = mCommandIter.NextCommand<ClearBufferCmd>();
				if (cmd->size == 0)
				{
					break;
				}

				Buffer* buffer = checked_cast<Buffer>(cmd->buffer.Get());

				vkCmdFillBuffer(commandBuffer, buffer->GetHandle(), cmd->offset, cmd->size, cmd->value);
				break;
			}

			case Command::BeginRenderPass:
			{
				BeginRenderPassCmd* cmd = mCommandIter.NextCommand<BeginRenderPassCmd>();
				TransitionSyncScope(queue, GetResourceUsages().renderPassUsages[nextRenderPassIndex]);
				RecordRenderPass(queue, cmd);
				++nextRenderPassIndex;
				break;
			}
			case Command::BeginComputePass:
			{
				BeginComputePassCmd* cmd = mCommandIter.NextCommand<BeginComputePassCmd>();
				TransitionSyncScope(queue, GetResourceUsages().computePassUsages[nextComputePassIndex]);
				++nextComputePassIndex;
				break;
			}
			case Command::BeginDebugLabel:
			{
				BeginDebugLabelCmd* cmd = mCommandIter.NextCommand<BeginDebugLabelCmd>();
				const char* label = mCommandIter.NextData<char>(cmd->labelLength);
				if (mDevice->IsDebugLayerEnabled())
				{
					VkDebugUtilsLabelEXT utilsLabel;
					utilsLabel.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
					utilsLabel.pNext = nullptr;
					utilsLabel.pLabelName = label;
					utilsLabel.color[0] = cmd->color.r;
					utilsLabel.color[1] = cmd->color.g;
					utilsLabel.color[2] = cmd->color.b;
					utilsLabel.color[3] = cmd->color.a;

					device->Fn.vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &utilsLabel);
				}
				break;
			}

			case Command::CopyBufferToBuffer:
			{
				CopyBufferToBufferCmd* cmd = mCommandIter.NextCommand<CopyBufferToBufferCmd>();
				if (cmd->size == 0)
				{
					break;
				}
				Buffer* src = checked_cast<Buffer>(cmd->srcBuffer.Get());
				Buffer* dst = checked_cast<Buffer>(cmd->dstBuffer.Get());

				src->TrackUsageAndGetResourceBarrier(queue, BufferUsage::CopySrc);
				dst->TrackUsageAndGetResourceBarrier(queue, BufferUsage::CopyDst);
				recordContext->EmitBarriers();

				VkBufferCopy region{};
				region.srcOffset = cmd->srcOffset;
				region.dstOffset = cmd->dstOffset;
				region.size = cmd->size;

				vkCmdCopyBuffer(commandBuffer, src->GetHandle(), dst->GetHandle(), 1, &region);
				break;
			}

			case Command::CopyBufferToTexture:
			{
				CopyBufferToTextureCmd* cmd = mCommandIter.NextCommand<CopyBufferToTextureCmd>();
				if (cmd->size.width == 0 || cmd->size.height == 0 || cmd->size.depthOrArrayLayers == 0)
				{
					break;
				}

				Buffer* srcBuffer = checked_cast<Buffer>(cmd->srcBuffer.Get());
				Texture* dstTexture = checked_cast<Texture>(cmd->dstTexture.Get());

				VkBufferImageCopy region = ComputeBufferImageCopyRegion(cmd->dataLayout, cmd->size, dstTexture, cmd->mipLevel, cmd->origin, cmd->aspect);

				SubresourceRange range = { cmd->aspect, cmd->origin.z, cmd->size.depthOrArrayLayers, cmd->mipLevel, 1 };

				srcBuffer->TrackUsageAndGetResourceBarrier(queue, BufferUsage::CopySrc);
				dstTexture->TransitionUsageAndGetResourceBarrier(queue, TextureUsage::CopyDst, ShaderStage::None, range);
				recordContext->EmitBarriers();

				vkCmdCopyBufferToImage(commandBuffer, srcBuffer->GetHandle(), dstTexture->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
				break;
			}
			case Command::CopyTextureToBuffer:
			{
				CopyTextureToBufferCmd* cmd = mCommandIter.NextCommand<CopyTextureToBufferCmd>();
				if (cmd->size.width == 0 || cmd->size.height == 0 || cmd->size.depthOrArrayLayers == 0)
				{
					break;
				}


				Texture* srcTexture = checked_cast<Texture>(cmd->srcTexture.Get());
				Buffer* dstBuffer = checked_cast<Buffer>(cmd->dstBuffer.Get());

				VkBufferImageCopy region = ComputeBufferImageCopyRegion(cmd->dataLayout, cmd->size, srcTexture, cmd->mipLevel, cmd->origin, cmd->aspect);

				SubresourceRange range = { cmd->aspect, cmd->origin.z, cmd->size.depthOrArrayLayers, cmd->mipLevel,  1};

				srcTexture->TransitionUsageAndGetResourceBarrier(queue, TextureUsage::CopySrc, ShaderStage::None, range);
				dstBuffer->TrackUsageAndGetResourceBarrier(queue, BufferUsage::CopyDst);
				recordContext->EmitBarriers();

				vkCmdCopyImageToBuffer(commandBuffer, srcTexture->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstBuffer->GetHandle(), 1, &region);
				break;
			}

			case Command::CopyTextureToTexture:
			{
				CopyTextureToTextureCmd* cmd = mCommandIter.NextCommand<CopyTextureToTextureCmd>();
				
				Extent3D size =
				{
					(std::min)(cmd->srcSize.width, cmd->dstSize.width),
					(std::min)(cmd->srcSize.height, cmd->dstSize.height),
					(std::min)(cmd->srcSize.depthOrArrayLayers, cmd->dstSize.depthOrArrayLayers)
				};

				if (size.width == 0 || size.height == 0 || size.depthOrArrayLayers == 0)
				{
					break;
				}

				Texture* srcTexture = checked_cast<Texture>(cmd->srcTexture.Get());
				Texture* dstTexture = checked_cast<Texture>(cmd->dstTexture.Get());

				SubresourceRange srcRange = { cmd->srcAspect, cmd->srcOrigin.z, size.depthOrArrayLayers, cmd->srcMipLevel, 1 };
				SubresourceRange dstRange = { cmd->dstAspect, cmd->dstOrigin.z, size.depthOrArrayLayers, cmd->dstMipLevel, 1 };

				srcTexture->TransitionUsageAndGetResourceBarrier(queue, TextureUsage::CopySrc, ShaderStage::None, srcRange);
				dstTexture->TransitionUsageAndGetResourceBarrier(queue, TextureUsage::CopyDst, ShaderStage::None, dstRange);
				recordContext->EmitBarriers();

				VkImageCopy region{};
				region.srcSubresource.aspectMask = ImageAspectFlagsConvert(cmd->srcAspect);
				region.srcSubresource.mipLevel = cmd->srcMipLevel;
				region.srcSubresource.baseArrayLayer = cmd->srcOrigin.z;
				region.srcSubresource.layerCount = size.depthOrArrayLayers;

				region.dstSubresource.aspectMask = ImageAspectFlagsConvert(cmd->dstAspect);
				region.dstSubresource.mipLevel = cmd->dstMipLevel;
				region.dstSubresource.baseArrayLayer = cmd->dstOrigin.z;
				region.dstSubresource.layerCount = size.depthOrArrayLayers;

				region.srcOffset.x = cmd->srcOrigin.x;
				region.srcOffset.y = cmd->srcOrigin.y;
				region.srcOffset.z = cmd->srcOrigin.z;


				region.dstOffset.x = cmd->dstOrigin.x;
				region.dstOffset.y = cmd->dstOrigin.y;
				region.dstOffset.z = cmd->dstOrigin.z;

				region.extent.width = size.width;
				region.extent.height = size.height;
				region.extent.depth = size.depthOrArrayLayers;

				vkCmdCopyImage(commandBuffer, srcTexture->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					dstTexture->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
				break;
			}
			case Command::EndComputePass:
				return;
			case Command::EndDebugLabel:
			{
				EndDebugLabelCmd* cmd = mCommandIter.NextCommand<EndDebugLabelCmd>();
				if (mDevice->IsDebugLayerEnabled())
				{
					device->Fn.vkCmdEndDebugUtilsLabelEXT(commandBuffer);
				}
				break;
			}

			case Command::MapBufferAsync:
			{
				MapBufferAsyncCmd* cmd = mCommandIter.NextCommand<MapBufferAsyncCmd>();

				break;
			}
			default:
				break;
			}
		}
	}
}