#include "vk_command_list.h"

#include "vk_pipeline.h"
#include "vk_render_device.h"
#include "vk_resource.h"
#include "rhi/common/Error.h"

#include <array>

namespace rhi
{
	CommandBuffer::~CommandBuffer()
	{
		vkDestroyCommandPool(m_Context.device, vkCmdPool, nullptr);
		if (!referencedStageBuffer.empty())
		{
			referencedStageBuffer.clear();
		}
	}

	CommandListVk::~CommandListVk()
	{

	}

	void CommandListVk::open()
	{
		m_CurrentCmdBuf = m_RenderDevice.getOrCreateCommandBuffer();

		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(m_CurrentCmdBuf->vkCmdBuf, &cmdBufferBeginInfo);

		//clear states
		m_LastGraphicsState = {};
		m_LastComputeState = {};
	}

	void CommandListVk::close()
	{
		endRendering();
		commitBarriers();
		vkEndCommandBuffer(m_CurrentCmdBuf->vkCmdBuf);
	}

	void CommandListVk::endRendering()
	{
		if (m_RenderingStarted)
		{
			vkCmdEndRendering(m_CurrentCmdBuf->vkCmdBuf);
			m_RenderingStarted = false;
		}
	}

	void CommandListVk::setResourceAutoTransition(bool enable)
	{
		m_EnableAutoTransition = enable;
	}

	inline bool resourceStateHasWriteAccess(ResourceState state)
	{
		const ResourceState writeAccessStates =
			ResourceState::RenderTarget |
			ResourceState::DepthWrite |
			ResourceState::UnorderedAccess |
			ResourceState::CopyDest |
			ResourceState::ResolveDest;
		return (state & writeAccessStates) == state;
	}

	void CommandListVk::transitionTextureState(ITexture& texture, ResourceState newState)
	{
		auto& textureVk = static_cast<TextureVk&>(texture);

		ResourceState oldState = textureVk.getState();

		// Always add barrier after writes.
		bool isAfterWrites = resourceStateHasWriteAccess(oldState);
		bool transitionNecessary = oldState != newState || isAfterWrites;

		if (transitionNecessary)
		{
			TextureBarrier& barrier = m_TextureBarriers.emplace_back();
			barrier.texture = &textureVk;
			barrier.stateBefore = oldState;
			barrier.stateAfter = newState;
		}

		textureVk.setState(newState);
	}

	void CommandListVk::transitionBufferState(IBuffer& buffer, ResourceState newState)
	{
		auto& bufferVk = static_cast<BufferVk&>(buffer);

		ResourceState oldState = bufferVk.getState();

		//if (bufferVk.getDesc().access != BufferAccess::GpuOnly)
		//{
		//	// host visible buffers can't change state
		//	return;
		//}

		// Always add barrier after writes.
		bool isAfterWrites = resourceStateHasWriteAccess(oldState);
		bool transitionNecessary = oldState != newState || isAfterWrites;

		if (transitionNecessary)
		{
			// See if this buffer is already used for a different purpose in this batch.
			// If it is, combine the state bits.
			// Example: same buffer used as index and vertex buffer, or as SRV and indirect arguments.
			for (BufferBarrier& barrier : m_BufferBarriers)
			{
				if (barrier.buffer == &bufferVk)
				{
					barrier.stateAfter = ResourceState(barrier.stateAfter | newState);
					bufferVk.setState(barrier.stateAfter);
					return;
				}
			}

			BufferBarrier& barrier = m_BufferBarriers.emplace_back();
			barrier.buffer = &bufferVk;
			barrier.stateBefore = oldState;
			barrier.stateAfter = newState;
		}
		bufferVk.setState(newState);
	}

	void CommandListVk::commitBarriers()
	{
		endRendering();
		if (m_BufferBarriers.empty() && m_TextureBarriers.empty())
		{
			return;
		}

		m_VkImageMemoryBarriers.resize(m_TextureBarriers.size());
		m_VkBufferMemoryBarriers.resize(m_BufferBarriers.size());

		for (int i = 0; i < m_TextureBarriers.size(); ++i)
		{
			const TextureBarrier& barrier = m_TextureBarriers[i];

			VkImageLayout oldLayout = resourceStateToVkImageLayout(barrier.stateBefore);
			VkImageLayout newLayout = resourceStateToVkImageLayout(barrier.stateAfter);
			assert(newLayout != VK_IMAGE_LAYOUT_UNDEFINED);

			VkPipelineStageFlags2 srcStage = resourceStatesToVkPipelineStageFlags2(barrier.stateBefore);
			VkPipelineStageFlags2 dstStage = resourceStatesToVkPipelineStageFlags2(barrier.stateAfter);

			VkAccessFlags2 srcAccessMask = resourceStatesToVkAccessFlags2(barrier.stateBefore);
			VkAccessFlags2 dstAccessMask = resourceStatesToVkAccessFlags2(barrier.stateAfter);

			VkImageSubresourceRange subresourceRange{};
			subresourceRange.aspectMask = getVkAspectMask(barrier.texture->format);
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.layerCount = barrier.texture->getDesc().arraySize;
			subresourceRange.levelCount = barrier.texture->getDesc().mipLevels;

			m_VkImageMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
			m_VkImageMemoryBarriers[i].pNext = nullptr;
			m_VkImageMemoryBarriers[i].srcStageMask = srcStage;
			m_VkImageMemoryBarriers[i].srcAccessMask = srcAccessMask;
			m_VkImageMemoryBarriers[i].dstStageMask = dstStage;
			m_VkImageMemoryBarriers[i].dstAccessMask = dstAccessMask;
			m_VkImageMemoryBarriers[i].oldLayout = oldLayout;
			m_VkImageMemoryBarriers[i].newLayout = newLayout;
			m_VkImageMemoryBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			m_VkImageMemoryBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			m_VkImageMemoryBarriers[i].image = barrier.texture->image;
			m_VkImageMemoryBarriers[i].subresourceRange = subresourceRange;
		}

		for (int i = 0; i < m_BufferBarriers.size(); ++i)
		{
			const BufferBarrier& barrier = m_BufferBarriers[i];
			// for buffer, if srcStage is VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, no effect
			//if (barrier.stateBefore == ResourceState::Undefined)
			//{
			//	continue;
			//}

			VkPipelineStageFlags2 srcStage = resourceStatesToVkPipelineStageFlags2(barrier.stateBefore);
			VkPipelineStageFlags2 dstStage = resourceStatesToVkPipelineStageFlags2(barrier.stateAfter);

			VkAccessFlags2 srcAccessMask = resourceStatesToVkAccessFlags2(barrier.stateBefore);
			VkAccessFlags2 dstAccessMask = resourceStatesToVkAccessFlags2(barrier.stateAfter);

			m_VkBufferMemoryBarriers[i].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
			m_VkBufferMemoryBarriers[i].pNext = nullptr;
			m_VkBufferMemoryBarriers[i].srcStageMask = srcStage;
			m_VkBufferMemoryBarriers[i].srcAccessMask = srcAccessMask;
			m_VkBufferMemoryBarriers[i].dstStageMask = dstStage;
			m_VkBufferMemoryBarriers[i].dstAccessMask = dstAccessMask;
			m_VkBufferMemoryBarriers[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			m_VkBufferMemoryBarriers[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			m_VkBufferMemoryBarriers[i].buffer = barrier.buffer->buffer;
			m_VkBufferMemoryBarriers[i].size = barrier.buffer->getDesc().size;
			m_VkBufferMemoryBarriers[i].offset = 0;
		}

		VkDependencyInfo dependencyInfo{};
		dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		dependencyInfo.pNext = nullptr;
		dependencyInfo.dependencyFlags = 0;
		dependencyInfo.imageMemoryBarrierCount = static_cast<uint32_t>(m_VkImageMemoryBarriers.size());
		dependencyInfo.pImageMemoryBarriers = m_VkImageMemoryBarriers.data();
		dependencyInfo.bufferMemoryBarrierCount = static_cast<uint32_t>(m_VkBufferMemoryBarriers.size());
		dependencyInfo.pBufferMemoryBarriers = m_VkBufferMemoryBarriers.data();

		vkCmdPipelineBarrier2(m_CurrentCmdBuf->vkCmdBuf, &dependencyInfo);

		m_BufferBarriers.clear();
		m_TextureBarriers.clear();
		m_VkBufferMemoryBarriers.clear();
		m_VkImageMemoryBarriers.clear();
	}

	void CommandListVk::setBufferBarrier(BufferVk& buffer, VkPipelineStageFlags2 dstStage, VkAccessFlags2 dstAccess)
	{
		auto& buf = static_cast<BufferVk&>(buffer);

		VkBufferMemoryBarrier2 barrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2 };
		barrier.pNext = nullptr;
		barrier.srcStageMask = resourceStatesToVkPipelineStageFlags2(buf.getState());
		barrier.srcAccessMask = resourceStatesToVkAccessFlags2(buf.getState());
		barrier.dstStageMask = dstStage;
		barrier.dstAccessMask = dstAccess;
		barrier.buffer = buf.buffer;
		barrier.size = buf.getDesc().size;
		barrier.offset = 0;

		VkDependencyInfo dependencyInfo{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
		dependencyInfo.pNext = nullptr;
		dependencyInfo.dependencyFlags = 0;
		dependencyInfo.bufferMemoryBarrierCount = 1;
		dependencyInfo.pBufferMemoryBarriers = &barrier;

		vkCmdPipelineBarrier2(m_CurrentCmdBuf->vkCmdBuf, &dependencyInfo);
	}

	void CommandListVk::copyBuffer(IBuffer& srcBuffer, uint64_t srcOffset, IBuffer& dstBuffer, uint64_t dstOffset, uint64_t dataSize)
	{
		assert(m_CurrentCmdBuf);

		BufferVk& srcBuf = static_cast<BufferVk&>(srcBuffer);
		BufferVk& dstBuf = static_cast<BufferVk&>(dstBuffer);

		assert(srcOffset + dataSize <= srcBuf.getDesc().size);
		assert(dstOffset + dataSize <= dstBuf.getDesc().size);

		if (m_EnableAutoTransition)
		{
			transitionBufferState(srcBuffer, ResourceState::CopySource);
			transitionBufferState(dstBuffer, ResourceState::CopyDest);
		}
		commitBarriers();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = srcOffset;
		copyRegion.dstOffset = dstOffset;
		copyRegion.size = dataSize;
		vkCmdCopyBuffer(m_CurrentCmdBuf->vkCmdBuf, srcBuf.buffer, dstBuf.buffer, 1, &copyRegion);
	}

	void CommandListVk::updateBuffer(IBuffer& buffer, const void* data, uint64_t dataSize, uint64_t offset)
	{
		assert(m_CurrentCmdBuf);
		BufferVk& buf = static_cast<BufferVk&>(buffer);

		if (buf.getDesc().access != BufferAccess::GpuOnly)
		{
			LOG_ERROR("this method only works with Buffer that tagged with BufferAccess::GpuOnly");
			return;
		}

		// vkCmdUpdateBuffer requires that the data size is smaller than or equal to 64 kB,
		// and offset is a multiple of 4.
		const uint64_t vkCmdUpdateBufferLimit = 65536;
		if ((offset & 3) == 0 && (dataSize & 3) == 0 && dataSize < vkCmdUpdateBufferLimit)
		{

			if (m_EnableAutoTransition)
			{
				transitionBufferState(buf, ResourceState::CopyDest);
			}
			commitBarriers();

			// Round up the write size to a multiple of 4
			const uint64_t sizeToWrite = (dataSize + 3) & ~uint64_t(3);
			vkCmdUpdateBuffer(m_CurrentCmdBuf->vkCmdBuf, buf.buffer, offset, sizeToWrite, data);
		}
		else
		{
			BufferDesc stageBufferDesc;
			stageBufferDesc.access = BufferAccess::CpuWrite;
			stageBufferDesc.usage = BufferUsage::None;
			auto& stageBuffer = m_CurrentCmdBuf->referencedStageBuffer.emplace_back();
			stageBuffer = std::unique_ptr<BufferVk>(checked_cast<BufferVk*>(m_RenderDevice.createBuffer(stageBufferDesc, data, dataSize)));
			copyBuffer(*stageBuffer, 0, buf, 0, dataSize);
		}
	}

	void* CommandListVk::mapBuffer(IBuffer& buffer)
	{
		assert(m_CurrentCmdBuf);
		auto& buf = static_cast<BufferVk&>(buffer);

		ASSERT_MSG(buf.getDesc().access != BufferAccess::CpuRead,
			"Only tagged with BufferAccess::CpuRead buffer can be mapped.");

		setBufferBarrier(buf, VK_PIPELINE_STAGE_2_HOST_BIT, VK_ACCESS_2_HOST_READ_BIT);

		return buf.allocaionInfo.pMappedData;
	}

	void CommandListVk::updateTexture(ITexture& texture, const void* data, uint64_t dataSize, const TextureUpdateInfo& updateInfo)
	{
		assert(m_CurrentCmdBuf);
		TextureVk& tex = static_cast<TextureVk&>(texture);

		TextureCopyInfo copyInfo = getTextureCopyInfo(tex.getDesc().format, updateInfo.dstRegion,
			(uint32_t)m_RenderDevice.getPhysicalDeviceProperties().limits.optimalBufferCopyRowPitchAlignment);

		ASSERT_MSG(updateInfo.dstRegion.maxX <= tex.getDesc().width << updateInfo.mipLevel &&
			updateInfo.dstRegion.maxY <= tex.getDesc().height << updateInfo.mipLevel &&
			updateInfo.dstRegion.maxZ <= tex.getDesc().depth << updateInfo.mipLevel, "dest region is out of bound for this miplevel.");

		ASSERT_MSG(updateInfo.srcRowPitch < copyInfo.rowBytesCount, "src row pitch is blow the dst region row pitch.");
		ASSERT_MSG(dataSize < copyInfo.regionBytesCount, "Not enough data was provided to update to the dst region.");

		BufferDesc stageBufferDesc;
		stageBufferDesc.access = BufferAccess::CpuWrite;
		stageBufferDesc.usage = BufferUsage::None;
		auto& stageBuffer = m_CurrentCmdBuf->referencedStageBuffer.emplace_back();
		stageBuffer = std::unique_ptr<BufferVk>(checked_cast<BufferVk*>(m_RenderDevice.createBuffer(stageBufferDesc, data, copyInfo.regionBytesCount)));

		uint32_t regionDepth = updateInfo.dstRegion.getDepth();

		for (uint32_t z = 0; z < regionDepth; ++z)
		{
			const uint8_t* srcPtr = reinterpret_cast<const uint8_t*>(data) + updateInfo.srcDepthPitch * z;
			const uint8_t* dstPtr = reinterpret_cast<const uint8_t*>(stageBuffer->allocaionInfo.pMappedData) + copyInfo.depthStride * z;
			for (uint32_t y = 0; y < copyInfo.rowCount; ++y)
			{
				memcpy(stageBuffer->allocaionInfo.pMappedData, srcPtr, copyInfo.rowBytesCount);
				srcPtr += updateInfo.srcRowPitch;
				dstPtr += copyInfo.rowStride;
			}
		}

		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = getVkAspectMask(tex.format);
		bufferCopyRegion.imageSubresource.baseArrayLayer = updateInfo.arrayLayer;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageSubresource.mipLevel = updateInfo.mipLevel;
		bufferCopyRegion.imageOffset = { static_cast<int32_t>(updateInfo.dstRegion.minX), static_cast<int32_t>(updateInfo.dstRegion.minY), static_cast<int32_t>(updateInfo.dstRegion.minZ) };
		bufferCopyRegion.imageExtent = { updateInfo.dstRegion.getWidth(), updateInfo.dstRegion.getHeight(), updateInfo.dstRegion.getDepth() };

		if (m_EnableAutoTransition)
		{
			transitionTextureState(tex, ResourceState::CopyDest);
		}
		commitBarriers();

		vkCmdCopyBufferToImage(m_CurrentCmdBuf->vkCmdBuf, stageBuffer->buffer, tex.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);
	}

	void CommandListVk::transitionResourceSet(IResourceSet& set, ShaderType dstVisibleStages)
	{
		auto& resourceSet = static_cast<ResourceSetVk&>(set);

		for (auto& itemWithVisibleStages : resourceSet.resourcesNeedStateTransition)
		{
			if ((itemWithVisibleStages.visibleStages & dstVisibleStages) == 0)
			{
				continue;
			}

			switch (itemWithVisibleStages.item.type)
			{
			case ShaderResourceType::TextureSampler:
			case ShaderResourceType::SampledTexture:
			{
				assert(itemWithVisibleStages.item.resource);
				auto& texture = static_cast<TextureVk&>(*itemWithVisibleStages.item.resource);
				transitionTextureState(texture, ResourceState::ShaderResource);
				break;
			}
			case ShaderResourceType::StorageTexture:
			{
				assert(itemWithVisibleStages.item.resource);
				auto& texture = static_cast<TextureVk&>(*itemWithVisibleStages.item.resource);
				transitionTextureState(texture, ResourceState::UnorderedAccess);
				break;
			}
			case ShaderResourceType::UniformBuffer:
			{
				assert(itemWithVisibleStages.item.resource);
				auto& buffer = static_cast<BufferVk&>(*itemWithVisibleStages.item.resource);
				transitionBufferState(buffer, ResourceState::ShaderResource);
				break;
			}
			case ShaderResourceType::StorageBuffer:
			{
				assert(itemWithVisibleStages.item.resource);
				auto& buffer = static_cast<BufferVk&>(*itemWithVisibleStages.item.resource);
				transitionBufferState(buffer, ResourceState::UnorderedAccess);
				break;
			}
			default:
				break;
			}
		}
	}

	static inline void fillVkRenderingInfo(const GraphicsState& state, VkRenderingInfo& renderingInfo,
		std::array<VkRenderingAttachmentInfo, g_MaxColorAttachments>& colorAttachments, 
		VkRenderingAttachmentInfo& depthStencilAttachment)
	{
		if (state.renderTargetCount > 0)
		{
			auto rt = checked_cast<TextureVk*>(state.renderTargetTextures[0]);
			renderingInfo.renderArea.extent = { rt->getDesc().width , rt->getDesc().height };
		}
		else
		{
			auto ds = checked_cast<TextureVk*>(state.depthStencilTexture);
			renderingInfo.renderArea.extent = { ds->getDesc().width , ds->getDesc().height };
		}
		renderingInfo.layerCount = 1;

		for (uint32_t i = 0; i < state.renderTargetCount; ++i)
		{
			auto renderTarget = checked_cast<TextureVk*>(state.renderTargetTextures[i]);
			auto& colorAttachment = colorAttachments[i];
			colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colorAttachment.pNext = nullptr;
			colorAttachment.imageView = renderTarget->view;
			colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachment.loadOp = state.clearRenderTarget ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.clearValue = { 0.0f, 0.0f, 0.f, 0.0f };
		}

		depthStencilAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthStencilAttachment.pNext = nullptr;
		depthStencilAttachment.imageView = checked_cast<TextureVk*>(state.depthStencilTexture)->view;
		depthStencilAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthStencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthStencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthStencilAttachment.clearValue.depthStencil = { 1.0f,  0 };

		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		renderingInfo.pNext = nullptr;
		renderingInfo.colorAttachmentCount = state.renderTargetCount;
		renderingInfo.pColorAttachments = colorAttachments.data();
		renderingInfo.pDepthAttachment = &depthStencilAttachment;
		renderingInfo.pStencilAttachment = &depthStencilAttachment;
		renderingInfo.viewMask = 0;
	}

	void CommandListVk::setGraphicsState(const GraphicsState& state)
	{
		assert(m_CurrentCmdBuf);
		bool pipelineTypeChanged = false;
		if (m_LastPipelineType != PipelineType::Graphics)
		{
			pipelineTypeChanged = true;
		}

		if (m_EnableAutoTransition)
		{
			constexpr ShaderType graphicsStages = ShaderType::Vertex | ShaderType::Fragment |
				ShaderType::Geometry | ShaderType::TessellationControl | ShaderType::TessellationEvaluation;

			// resource set
			for (uint32_t i = 0; i < state.resourceSetCount; ++i)
			{
				assert(state.resourceSets[i] != nullptr);
				transitionResourceSet(*state.resourceSets[i], graphicsStages);
			}

			// vertex buffer
			for (uint32_t i = 0; i < state.vertexBufferCount; ++i)
			{
				assert(state.vertexBuffers[i].buffer != nullptr);
				auto& buffer = static_cast<BufferVk&>(*state.vertexBuffers[i].buffer);
				transitionBufferState(buffer, ResourceState::VertexBuffer);
			}

			// index buffer
			assert(state.indexBuffer.buffer != nullptr);
			auto& buffer = static_cast<BufferVk&>(*state.indexBuffer.buffer);
			transitionBufferState(buffer, ResourceState::IndexBuffer);

			if (state.indirectBuffer)
			{
				auto& buffer = static_cast<BufferVk&>(*state.indirectBuffer);
				transitionBufferState(buffer, ResourceState::IndirectBuffer);
			}
		}

		assert(state.renderTargetCount > 0 || state.depthStencilTexture != nullptr);
		for (uint32_t i = 0; i < state.renderTargetCount; ++i)
		{
			assert(state.renderTargetTextures[i] != nullptr);
			auto& renderTarget = static_cast<TextureVk&>(*state.renderTargetTextures[i]);
			transitionTextureState(renderTarget, ResourceState::RenderTarget);
		}
		if (state.depthStencilTexture)
		{
			auto& depthStencil = static_cast<TextureVk&>(*state.depthStencilTexture);
			transitionTextureState(depthStencil, ResourceState::DepthWrite);
		}
		// Because once the graphics state is set, the render target is always changed. 
		// and Barrier must not be placed within a render section started with vkCmdBeginRendering
		endRendering();
		commitBarriers();

		std::array<VkRenderingAttachmentInfo, g_MaxColorAttachments> colorAttachments{};
		VkRenderingAttachmentInfo depthAttachment{};

		VkRenderingInfo renderingInfo{};
		fillVkRenderingInfo(state, renderingInfo, colorAttachments, depthAttachment);

		// Start a dynamic rendering section
		vkCmdBeginRendering(m_CurrentCmdBuf->vkCmdBuf, &renderingInfo);
		m_RenderingStarted = true;

		if (arraysAreDifferent(state.vertexBuffers, state.vertexBufferCount,
			m_LastGraphicsState.vertexBuffers, m_LastGraphicsState.vertexBufferCount))
		{
			VkBuffer buffers[g_MaxVertexInputBindings]{};
			VkDeviceSize bufferOffsets[g_MaxVertexInputBindings]{};
			uint32_t maxBindingSlot = 0;
			for (uint32_t i = 0; i < state.vertexBufferCount; ++i)
			{
				const VertexBufferBinding& binding = state.vertexBuffers[i];
				buffers[binding.bindingSlot] = checked_cast<BufferVk*>(binding.buffer)->buffer;
				bufferOffsets[binding.bindingSlot] = binding.offset;
				maxBindingSlot = (std::max)(maxBindingSlot, binding.bindingSlot);
			}
			vkCmdBindVertexBuffers(m_CurrentCmdBuf->vkCmdBuf, 0, maxBindingSlot + 1, buffers, bufferOffsets);
		}

		if (state.indexBuffer.buffer && state.indexBuffer != m_LastGraphicsState.indexBuffer)
		{
			auto* buffer = checked_cast<BufferVk*>(state.indexBuffer.buffer);
			vkCmdBindIndexBuffer(m_CurrentCmdBuf->vkCmdBuf, buffer->buffer, state.indexBuffer.offset,
				state.indexBuffer.format == Format::R16_UINT ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
		}
		// set viewports and scissors 
		ASSERT_MSG(state.viewportCount == checked_cast<GraphicsPipelineVk*>(state.pipeline)->getDesc().viewportCount,
			"The number of viewports provided does not match the number specified when the pipeline was created.");
		if (pipelineTypeChanged || arraysAreDifferent(state.viewports, state.viewportCount,
			m_LastGraphicsState.viewports, m_LastGraphicsState.viewportCount))
		{
			VkViewport viewports[g_MaxViewPorts]{};
			for (uint32_t i = 0; i < state.viewportCount; ++i)
			{
				viewports[i] = convertViewport(state.viewports[i]);
			}
			vkCmdSetViewport(m_CurrentCmdBuf->vkCmdBuf, 0, state.viewportCount, viewports);
		}
		ASSERT_MSG(state.scissorCount == checked_cast<GraphicsPipelineVk*>(state.pipeline)->getDesc().viewportCount, 
			"The number of scissors must be the same as the number of viewports.");
		if (pipelineTypeChanged || arraysAreDifferent(state.scissors, state.scissorCount,
			m_LastGraphicsState.scissors, m_LastGraphicsState.scissorCount))
		{
			VkRect2D scissors[g_MaxViewPorts]{};
			for (uint32_t i = 0; i < state.scissorCount; ++i)
			{
				scissors[i].offset = { state.scissors[i].minX, state.scissors[i].minY };
				scissors[i].extent = { static_cast<uint32_t>(std::abs(state.scissors[i].getWidth())), static_cast<uint32_t>(std::abs(state.scissors[i].getHeight())) };
			}
			vkCmdSetScissor(m_CurrentCmdBuf->vkCmdBuf, 0, state.scissorCount, scissors);
		}

		assert(state.pipeline != nullptr);
		GraphicsPipelineVk* pipeline = checked_cast<GraphicsPipelineVk*>(state.pipeline);

		if (pipelineTypeChanged || arraysAreDifferent(state.resourceSets, state.resourceSetCount,
			m_LastGraphicsState.resourceSets, m_LastGraphicsState.resourceSetCount))
		{
			VkDescriptorSet descriptorSets[g_MaxBoundDescriptorSets]{};
			for (uint32_t i = 0; i < state.resourceSetCount; ++i)
			{
				assert(state.resourceSets[i] != nullptr);
				auto resourceSet = checked_cast<ResourceSetVk*>(state.resourceSets[i]);
				descriptorSets[i] = resourceSet->descriptorSet;
			}
			vkCmdBindDescriptorSets(m_CurrentCmdBuf->vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, state.resourceSetCount, descriptorSets, 0, nullptr);
		}

		if (pipelineTypeChanged || state.pipeline != m_LastGraphicsState.pipeline)
		{
			vkCmdBindPipeline(m_CurrentCmdBuf->vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
		}

		m_LastPipelineType = PipelineType::Graphics;
		m_LastGraphicsState = state;
	}

	void CommandListVk::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		assert(m_CurrentCmdBuf);
		if (!m_RenderingStarted)
		{
			std::array<VkRenderingAttachmentInfo, g_MaxColorAttachments> colorAttachments{};
			VkRenderingAttachmentInfo depthAttachment{};

			VkRenderingInfo renderingInfo{};
			fillVkRenderingInfo(m_LastGraphicsState, renderingInfo, colorAttachments, depthAttachment);
			vkCmdBeginRendering(m_CurrentCmdBuf->vkCmdBuf, &renderingInfo);
		}

		vkCmdDraw(m_CurrentCmdBuf->vkCmdBuf, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void CommandListVk::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
	{
		assert(m_CurrentCmdBuf);
		if (!m_RenderingStarted)
		{
			std::array<VkRenderingAttachmentInfo, g_MaxColorAttachments> colorAttachments{};
			VkRenderingAttachmentInfo depthAttachment{};

			VkRenderingInfo renderingInfo{};
			vkCmdBeginRendering(m_CurrentCmdBuf->vkCmdBuf, &renderingInfo);
		}

		vkCmdDrawIndexed(m_CurrentCmdBuf->vkCmdBuf, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void CommandListVk::drawIndirect(uint64_t offset, uint32_t drawCount)
	{
		assert(m_CurrentCmdBuf);
		if (!m_RenderingStarted)
		{
			std::array<VkRenderingAttachmentInfo, g_MaxColorAttachments> colorAttachments{};
			VkRenderingAttachmentInfo depthAttachment{};

			VkRenderingInfo renderingInfo{};
			vkCmdBeginRendering(m_CurrentCmdBuf->vkCmdBuf, &renderingInfo);
		}
		auto indiectBuffer = checked_cast<BufferVk*>(m_LastGraphicsState.indirectBuffer);
		assert(indiectBuffer != nullptr);
		vkCmdDrawIndirect(m_CurrentCmdBuf->vkCmdBuf, indiectBuffer->buffer, offset, drawCount, sizeof(DrawIndirectCommand));
	}

	void CommandListVk::drawIndexedIndirect(uint64_t offset, uint32_t drawCount)
	{
		assert(m_CurrentCmdBuf);
		if (!m_RenderingStarted)
		{
			std::array<VkRenderingAttachmentInfo, g_MaxColorAttachments> colorAttachments{};
			VkRenderingAttachmentInfo depthAttachment{};

			VkRenderingInfo renderingInfo{};
			vkCmdBeginRendering(m_CurrentCmdBuf->vkCmdBuf, &renderingInfo);
		}
		auto indiectBuffer = checked_cast<BufferVk*>(m_LastGraphicsState.indirectBuffer);
		assert(indiectBuffer != nullptr);
		vkCmdDrawIndexedIndirect(m_CurrentCmdBuf->vkCmdBuf, indiectBuffer->buffer, offset, drawCount, sizeof(DrawIndexedIndirectCommand));
	}

	void CommandListVk::setComputeState(const ComputeState& state)
	{
		assert(m_CurrentCmdBuf);
		endRendering();

		bool pipelineChanged = m_LastPipelineType != PipelineType::Compute;

		auto pipeline = checked_cast<ComputePipelineVk*>(state.pipeline);

		if (m_EnableAutoTransition)
		{
			for (uint32_t i = 0; i < state.resourceSetCount; ++i)
			{
				assert(state.resourceSets[i] != nullptr);
				transitionResourceSet(*state.resourceSets[i], ShaderType::Compute);
			}

			if (state.indirectBuffer)
			{
				auto indirectBuffer = checked_cast<BufferVk*>(state.indirectBuffer);
				transitionBufferState(*indirectBuffer, ResourceState::IndirectBuffer);
			}
		}

		commitBarriers();

		if (pipelineChanged || state.pipeline != m_LastComputeState.pipeline)
		{
			vkCmdBindPipeline(m_CurrentCmdBuf->vkCmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->pipeline);
		}

		if (pipelineChanged || arraysAreDifferent(state.resourceSets, state.resourceSetCount,
			m_LastComputeState.resourceSets, m_LastComputeState.resourceSetCount))
		{
			VkDescriptorSet descriptorSets[g_MaxBoundDescriptorSets]{};
			for (uint32_t i = 0; i < state.resourceSetCount; ++i)
			{
				assert(state.resourceSets[i] != nullptr);
				auto resourceSet = checked_cast<ResourceSetVk*>(state.resourceSets[i]);
				descriptorSets[i] = resourceSet->descriptorSet;
			}
			vkCmdBindDescriptorSets(m_CurrentCmdBuf->vkCmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->pipelineLayout, 0, state.resourceSetCount, descriptorSets, 0, nullptr);
		}

		m_LastPipelineType = PipelineType::Compute;
		m_LastComputeState = state;
	}

	void CommandListVk::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	{
		assert(m_CurrentCmdBuf);

		vkCmdDispatch(m_CurrentCmdBuf->vkCmdBuf, groupCountX, groupCountY, groupCountZ);
	}

	void CommandListVk::dispatchIndirect(uint64_t offset)
	{
		assert(m_CurrentCmdBuf);

		auto indiectBuffer = checked_cast<BufferVk*>(m_LastComputeState.indirectBuffer);
		assert(indiectBuffer != nullptr);
		vkCmdDispatchIndirect(m_CurrentCmdBuf->vkCmdBuf, indiectBuffer->buffer, offset);
	}

	Object CommandListVk::getNativeObject(NativeObjectType type) const
	{
		if (type == NativeObjectType::VK_CommandBuffer)
		{
			return static_cast<Object>(m_CurrentCmdBuf->vkCmdBuf);
		}
		return nullptr;
	}
}