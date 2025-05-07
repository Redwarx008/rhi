#include "BindSetBase.h"

#include "BindSetLayoutBase.h"

namespace rhi::impl
{
	BindSetBase::BindSetBase(DeviceBase* device, const BindSetDesc& desc) :
		ResourceBase(device, desc.name),
		mLayout(desc.layout)
	{
		mEntries.resize(desc.entryCount);
		for (uint32_t i = 0; i < desc.entryCount; ++i)
		{
			mEntries[i] = desc.entries[i];
		}
	}

	BindSetBase::~BindSetBase() {}

	void BindSetBase::APIDestroy()
	{
		Destroy();
	}

	ResourceType BindSetBase::GetType() const
	{
		return ResourceType::BindSet;
	}

	BindSetLayoutBase* BindSetBase::GetLayout()
	{
		return mLayout.Get();
	}

	const std::vector<BindSetEntry>& BindSetBase::GetBindingEntries() const
	{
		return mEntries;
	}
}