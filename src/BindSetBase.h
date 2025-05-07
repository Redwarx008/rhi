#pragma once

#include "RHIStruct.h"
#include "ResourceBase.h"
#include "common/Ref.hpp"
#include <vector>

namespace rhi::impl
{
	class DeviceBase;
	class BindSetLayoutBase;

	class BindSetBase : public ResourceBase
	{
	public:
		void APIDestroy();
		ResourceType GetType() const override;
		BindSetLayoutBase* GetLayout();
		const std::vector<BindSetEntry>& GetBindingEntries() const;
	protected:
		explicit BindSetBase(DeviceBase* device, const BindSetDesc& desc);
		~BindSetBase();

	private:
		Ref<BindSetLayoutBase> mLayout;
		std::vector<BindSetEntry> mEntries;
	};
}