#include "Commands.h"
#include "TextureBase.h"
#include "BindSetLayoutBase.h"
#include "RenderPipelineBase.h"
#include "ComputePipelineBase.h"
#include "common/Error.h"

namespace rhi::impl
{
	RenderPassColorAttachment::RenderPassColorAttachment() {}
	RenderPassColorAttachment::~RenderPassColorAttachment() {}

	RenderPassDepthStencilAttachment::RenderPassDepthStencilAttachment() {}
	RenderPassDepthStencilAttachment::~RenderPassDepthStencilAttachment() {}

	BeginRenderPassCmd::BeginRenderPassCmd() {}
	BeginRenderPassCmd::~BeginRenderPassCmd() {}

	BeginComputePassCmd::BeginComputePassCmd() {}
	BeginComputePassCmd::~BeginComputePassCmd() {}

	ClearBufferCmd::ClearBufferCmd() {}
	ClearBufferCmd::~ClearBufferCmd() {}

	CopyBufferToBufferCmd::CopyBufferToBufferCmd() {}
	CopyBufferToBufferCmd::~CopyBufferToBufferCmd() {}

	CopyBufferToTextureCmd::CopyBufferToTextureCmd() {}
	CopyBufferToTextureCmd::~CopyBufferToTextureCmd() {}

	CopyTextureToBufferCmd::CopyTextureToBufferCmd() {}
	CopyTextureToBufferCmd::~CopyTextureToBufferCmd() {}

	CopyTextureToTextureCmd::CopyTextureToTextureCmd() {}
	CopyTextureToTextureCmd::~CopyTextureToTextureCmd() {}

	MapBufferAsyncCmd::MapBufferAsyncCmd() {}
	MapBufferAsyncCmd::~MapBufferAsyncCmd() {}

	BeginDebugLabelCmd::BeginDebugLabelCmd() {}
	BeginDebugLabelCmd::~BeginDebugLabelCmd() {}

	EndDebugLabelCmd::EndDebugLabelCmd() {}
	EndDebugLabelCmd::~EndDebugLabelCmd() {}

	SetRenderPipelineCmd::SetRenderPipelineCmd() {}
	SetRenderPipelineCmd::~SetRenderPipelineCmd() {}

	VertexBuffer::VertexBuffer() {}
	VertexBuffer::~VertexBuffer() {}

	SetVertexBufferCmd::SetVertexBufferCmd() {}
	SetVertexBufferCmd::~SetVertexBufferCmd() {}

	SetIndexBufferCmd::SetIndexBufferCmd() {}
	SetIndexBufferCmd::~SetIndexBufferCmd() {}

	SetScissorRectsCmd::SetScissorRectsCmd() {}
	SetScissorRectsCmd::~SetScissorRectsCmd() {}

	SetBindSetCmd::SetBindSetCmd() {}
	SetBindSetCmd::~SetBindSetCmd() {}

	SetPushConstantCmd::SetPushConstantCmd() {}
	SetPushConstantCmd::~SetPushConstantCmd() {}

	DrawIndirectCmd::DrawIndirectCmd() {}
	DrawIndirectCmd::~DrawIndirectCmd() {}

	MultiDrawIndirectCmd::MultiDrawIndirectCmd() {}
	MultiDrawIndirectCmd::~MultiDrawIndirectCmd() {}

	EndRenderPassCmd::EndRenderPassCmd(){}
	EndRenderPassCmd::~EndRenderPassCmd() {}

	SetViewportCmd::SetViewportCmd() {}
	SetViewportCmd::~SetViewportCmd(){}

	SetComputePipelineCmd::SetComputePipelineCmd() {}
	SetComputePipelineCmd::~SetComputePipelineCmd() {}

	DispatchIndirectCmd::DispatchIndirectCmd() {}
	DispatchIndirectCmd::~DispatchIndirectCmd() {}

	EndComputePassCmd::EndComputePassCmd() {}
	EndComputePassCmd::~EndComputePassCmd() {}


	const char* EnsureValidString(CommandAllocator& allocator, std::string_view s, uint32_t* length)
	{
		* length = s.length() + 1;

		// Include extra null-terminator character. The string_view may not be null-terminated. It also
		// may already have a null-terminator inside of it, in which case adding the null-terminator is
		// unnecessary. However, this is unlikely, so always include the extra character.
		char* out = allocator.AllocateData<char>(s.length() + 1);
		memcpy(out, s.data(), s.length());
		out[s.length()] = '\0';

		return out;
	}

	void FreeCommands(CommandIterator* commands)
	{
		commands->Reset();
		Command type;
		while (commands->NextCommandId(&type))
		{
			switch (type)
			{
			case Command::ClearBuffer:
			{
				ClearBufferCmd* begin = commands->NextCommand<ClearBufferCmd>();
				begin->~ClearBufferCmd();
				break;
			}
			case Command::BeginRenderPass:
			{
				BeginRenderPassCmd* begin = commands->NextCommand<BeginRenderPassCmd>();
				begin->~BeginRenderPassCmd();
				break;
			}
			case Command::BeginComputePass:
			{
				BeginComputePassCmd* begin = commands->NextCommand<BeginComputePassCmd>();
				begin->~BeginComputePassCmd();
				break;
			}
			case Command::BeginDebugLabel:
			{
				BeginDebugLabelCmd* begin = commands->NextCommand<BeginDebugLabelCmd>();
				begin->~BeginDebugLabelCmd();
				break;
			}
			case Command::CopyBufferToBuffer:
			{
				CopyBufferToBufferCmd* begin = commands->NextCommand<CopyBufferToBufferCmd>();
				begin->~CopyBufferToBufferCmd();
				break;
			}
			case Command::CopyBufferToTexture:
			{
				CopyBufferToTextureCmd* begin = commands->NextCommand<CopyBufferToTextureCmd>();
				begin->~CopyBufferToTextureCmd();
				break;
			}
			case Command::CopyTextureToBuffer:
			{
				CopyTextureToBufferCmd* begin = commands->NextCommand<CopyTextureToBufferCmd>();
				begin->~CopyTextureToBufferCmd();
				break;
			}
			case Command::CopyTextureToTexture:
			{
				CopyTextureToTextureCmd* begin = commands->NextCommand<CopyTextureToTextureCmd>();
				begin->~CopyTextureToTextureCmd();
				break;
			}
			case Command::Dispatch:
			{
				DispatchCmd* begin = commands->NextCommand<DispatchCmd>();
				begin->~DispatchCmd();
				break;
			}
			case Command::DispatchIndirect:
			{
				DispatchIndirectCmd* begin = commands->NextCommand<DispatchIndirectCmd>();
				begin->~DispatchIndirectCmd();
				break;
			}
			case Command::Draw:
			{
				DrawCmd* begin = commands->NextCommand<DrawCmd>();
				begin->~DrawCmd();
				break;
			}
			case Command::DrawIndexed:
			{
				DrawIndexedCmd* begin = commands->NextCommand<DrawIndexedCmd>();
				begin->~DrawIndexedCmd();
				break;
			}
			case Command::DrawIndirect:
			{
				DrawIndirectCmd* begin = commands->NextCommand<DrawIndirectCmd>();
				begin->~DrawIndirectCmd();
				break;
			}
			case Command::DrawIndexedIndirect:
			{
				DrawIndexedIndirectCmd* begin = commands->NextCommand<DrawIndexedIndirectCmd>();
				begin->~DrawIndexedIndirectCmd();
				break;
			}
			case Command::MultiDrawIndirect:
			{
				MultiDrawIndirectCmd* begin = commands->NextCommand<MultiDrawIndirectCmd>();
				begin->~MultiDrawIndirectCmd();
				break;
			}
			case Command::MultiDrawIndexedIndirect:
			{
				MultiDrawIndexedIndirectCmd* begin = commands->NextCommand<MultiDrawIndexedIndirectCmd>();
				begin->~MultiDrawIndexedIndirectCmd();
				break;
			}
			case Command::SetRenderPipeline:
			{
				SetRenderPipelineCmd* begin = commands->NextCommand<SetRenderPipelineCmd>();
				begin->~SetRenderPipelineCmd();
				break;
			}
			case Command::SetComputePipeline:
			{
				SetComputePipelineCmd* begin = commands->NextCommand<SetComputePipelineCmd>();
				begin->~SetComputePipelineCmd();
				break;
			}
			case Command::SetViewport:
			{
				SetViewportCmd* begin = commands->NextCommand<SetViewportCmd>();
				begin->~SetViewportCmd();
				break;
			}
			case Command::SetScissorRects:
			{
				SetScissorRectsCmd* begin = commands->NextCommand<SetScissorRectsCmd>();
				begin->~SetScissorRectsCmd();
				break;
			}
			case Command::SetIndexBuffer:
			{
				SetIndexBufferCmd* begin = commands->NextCommand<SetIndexBufferCmd>();
				begin->~SetIndexBufferCmd();
				break;
			}
			case Command::SetVertexBuffer:
			{
				SetVertexBufferCmd* begin = commands->NextCommand<SetVertexBufferCmd>();
				begin->~SetVertexBufferCmd();
				break;
			}
			case Command::SetPushConstant:
			{
				SetPushConstantCmd* begin = commands->NextCommand<SetPushConstantCmd>();
				begin->~SetPushConstantCmd();
				break;
			}
			case Command::SetStencilReference:
			{
				SetStencilReferenceCmd* begin = commands->NextCommand<SetStencilReferenceCmd>();
				begin->~SetStencilReferenceCmd();
				break;
			}
			case Command::SetBlendConstant:
			{
				SetBlendConstantCmd* begin = commands->NextCommand<SetBlendConstantCmd>();
				begin->~SetBlendConstantCmd();
				break;
			}
			case Command::SetBindSet:
			{
				SetBindSetCmd* begin = commands->NextCommand<SetBindSetCmd>();
				begin->~SetBindSetCmd();
				break;
			}
			case Command::EndRenderPass:
			{
				EndRenderPassCmd* begin = commands->NextCommand<EndRenderPassCmd>();
				begin->~EndRenderPassCmd();
				break;
			}
			case Command::EndComputePass:
			{
				EndComputePassCmd* begin = commands->NextCommand<EndComputePassCmd>();
				begin->~EndComputePassCmd();
				break;
			}
			case Command::EndDebugLabel:
			{
				EndDebugLabelCmd* begin = commands->NextCommand<EndDebugLabelCmd>();
				begin->~EndDebugLabelCmd();
				break;
			}
			case Command::MapBufferAsync:
			{
				MapBufferAsyncCmd* begin = commands->NextCommand<MapBufferAsyncCmd>();
				begin->~MapBufferAsyncCmd();
				break;
			}
			default:
				ASSERT(!"Unreachable");
				break;
			}
		}

		commands->Reset();
	}
}