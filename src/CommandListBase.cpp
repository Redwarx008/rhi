#include "CommandListBase.h"

#include "CommandEncoder.h"

namespace rhi::impl
{
	CommandListBase::CommandListBase(DeviceBase* device, CommandEncoder* encoder) :
		mDevice(device),
		mCommandIter(encoder->AcquireCommands()),
		mResourceUsages(encoder->AcquireResourceUsages())
	{

	}

	const CommandListResourceUsage& CommandListBase::GetResourceUsages() const
	{
		return mResourceUsages;
	}

	CommandListBase::~CommandListBase()
	{
		FreeCommands(&mCommandIter);
	}
}