#pragma once

#include "ResourceBase.h"
#include "RHIStruct.h"

#include <vector>

namespace rhi::impl
{
	class DeviceBase;
	class BindSetLayoutBase : public ResourceBase
	{
	public:
		ResourceType GetType() const override;
		BindingType GetBindingType(uint32_t binding) const;
		bool HasDynamicOffset(uint32_t binding) const;
		ShaderStage GetVisibility(uint32_t binding) const;
	protected:
		explicit BindSetLayoutBase(DeviceBase* device, const BindSetLayoutDesc& desc);
		~BindSetLayoutBase();
		void Initialize(const BindSetLayoutDesc& desc);

		struct BindingInfo
		{
			BindingType type;
			ShaderStage visibility;
			bool hasDynamicOffset;
		};
		std::vector<BindingInfo> mBindingIndexToInfoMap;
	};
}