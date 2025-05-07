#pragma once

#include "CommandAllocator.h"
#include "SyncScopeUsageTracker.h"

#include <vector>

namespace rhi::impl
{
	class RenderPassEncoder;

	class EncodingContext
	{
	public:
		CommandAllocator& GetCommandAllocator();
		CommandIterator AcquireCommands();
		std::vector<SyncScopeResourceUsage> AcquireRenderPassUsages();
		std::vector<SyncScopeResourceUsage> AcquireComputePassUsages();
		void ExitRenderPass(SyncScopeUsageTracker& usageTracker);
		void ExitComputePass(SyncScopeUsageTracker& usageTracker);
	private:
		std::vector<SyncScopeResourceUsage> mRenderPassUsages;
		std::vector<SyncScopeResourceUsage> mComputePassUsages;
		CommandAllocator mCommandAllocator;
	};
}