#pragma once

#include "../common/Ref.hpp"
#include "../CommandListBase.h"

namespace rhi::impl
{
	class BeginRenderPassCmd;
	class BeginComputePassCmd;
}

namespace rhi::impl::vulkan
{
	struct CommandRecordContext;
	class Queue;
	class Device;

	class CommandList final : public CommandListBase
	{
	public:
		static Ref<CommandList> Create(Device* device, CommandEncoder* encoder);
		void RecordCommands(Queue* queue);
	private:
		explicit CommandList(Device* device, CommandEncoder* encoder);
		void RecordRenderPass(Queue* queue, BeginRenderPassCmd* renderPassCmd);
		void RecordComputePass(Queue* queue, BeginComputePassCmd* computePassCmd);
	};
}