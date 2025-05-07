#pragma once

#include <bitset>
#include "RHIStruct.h"

namespace rhi::impl
{
	
	class FeatureSet
	{
	public:
		void EnableFeature(FeatureName feature)
		{
			mSet.set(static_cast<uint32_t>(feature));
		}
		bool IsEnabled(FeatureName feature)
		{
			return mSet[static_cast<uint32_t>(feature)];
		}
	private:
		static const uint32_t cFeatureCount = static_cast<uint32_t>(FeatureName::Count);
		std::bitset<cFeatureCount> mSet;
	};
}