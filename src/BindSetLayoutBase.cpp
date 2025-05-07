#include "BindSetLayoutBase.h"
#include "DeviceBase.h"

#include "common/Error.h"
#include "common/Constants.h"

#include <algorithm>

namespace rhi::impl
{

	BindSetLayoutBase::BindSetLayoutBase(DeviceBase* device, const BindSetLayoutDesc& desc) :
		ResourceBase(device, desc.name)
	{
	}

	BindSetLayoutBase::~BindSetLayoutBase() {}

	void BindSetLayoutBase::Initialize(const BindSetLayoutDesc& desc)
	{
		ResourceBase::Initialize();

		uint32_t maxBinding = 0;
		for (uint32_t i = 0; i < desc.entryCount; ++i)
		{
			maxBinding = (std::max(maxBinding, desc.entries[i].binding));
		}

		ASSERT(maxBinding < cMaxBindingsPerBindSet);
		mBindingIndexToInfoMap.resize(maxBinding + 1);


		for (uint32_t i = 0; i < desc.entryCount; ++i)
		{
			auto& entry = desc.entries[i];
			mBindingIndexToInfoMap[entry.binding].type = entry.type;
			mBindingIndexToInfoMap[entry.binding].visibility = entry.visibleStages;
			mBindingIndexToInfoMap[entry.binding].hasDynamicOffset = entry.hasDynamicOffset;
		}
	}

	ResourceType BindSetLayoutBase::GetType() const
	{
		return ResourceType::BindSetLayout;
	}

	BindingType BindSetLayoutBase::GetBindingType(uint32_t binding) const
	{
		ASSERT(binding < mBindingIndexToInfoMap.size());
		return mBindingIndexToInfoMap[binding].type;
	}

	ShaderStage BindSetLayoutBase::GetVisibility(uint32_t binding) const
	{
		ASSERT(binding < mBindingIndexToInfoMap.size());
		return mBindingIndexToInfoMap[binding].visibility;
	}

	bool BindSetLayoutBase::HasDynamicOffset(uint32_t binding) const
	{
		ASSERT(binding < mBindingIndexToInfoMap.size());
		return mBindingIndexToInfoMap[binding].hasDynamicOffset;
	}
}