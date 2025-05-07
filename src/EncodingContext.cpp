#include "EncodingContext.h"

namespace rhi::impl
{
	CommandAllocator& EncodingContext::GetCommandAllocator()
	{
		return mCommandAllocator;
	}

	CommandIterator EncodingContext::AcquireCommands()
	{
		return CommandIterator(mCommandAllocator);
	}

	std::vector<SyncScopeResourceUsage> EncodingContext::AcquireRenderPassUsages()
	{
		return std::move(mRenderPassUsages);
	}

	std::vector<SyncScopeResourceUsage> EncodingContext::AcquireComputePassUsages()
	{
		return std::move(mComputePassUsages);
	}

	void EncodingContext::ExitRenderPass(SyncScopeUsageTracker& usageTracker)
	{
		mRenderPassUsages.push_back(usageTracker.AcquireSyncScopeUsage());
	}

	void EncodingContext::ExitComputePass(SyncScopeUsageTracker& usageTracker)
	{
		mComputePassUsages.push_back(usageTracker.AcquireSyncScopeUsage());
	}
}