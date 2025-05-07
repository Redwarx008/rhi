#include "InstanceBase.h"

namespace rhi::impl
{
	bool InstanceBase::IsDebugLayerEnabled() const
	{
		return mDebugLayerEnabled;
	}
}