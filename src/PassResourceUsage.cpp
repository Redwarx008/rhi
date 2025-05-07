#include "PassResourceUsage.h"

namespace rhi::impl
{
	bool operator==(const TextureSyncInfo& a, const TextureSyncInfo& b)
	{
		return (a.usage == b.usage) && (a.shaderStages == b.shaderStages);
	}
}