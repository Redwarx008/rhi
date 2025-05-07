#pragma once

#include "RHIStruct.h"
#include "ResourceBase.h"
#include "common/Constants.h"
#include "common/Ref.hpp"

#include <bitset>
#include <array>

namespace rhi::impl
{
	class DeviceBase;
	class BindSetLayoutBase;

	class PipelineLayoutBase : public ResourceBase
	{
	public:
		BindSetLayoutBase* APIGetBindSetLayout(uint32_t bindSetIndex) const;
		BindSetLayoutBase* GetBindSetLayout(uint32_t bindSetIndex) const;
		const std::bitset<cMaxBindSets>& GetBindSetMask() const;
		ResourceType GetType() const override;
		const PushConstantRange& GetPushConstants() const;
	protected:
		explicit PipelineLayoutBase(DeviceBase* device, const PipelineLayoutDesc& desc);
		~PipelineLayoutBase();
		std::bitset<cMaxBindSets> mBindSetMask = {};
		std::array<Ref<BindSetLayoutBase>, cMaxBindSets> mBindGroupLayouts = {};
		PushConstantRange mPushConstantRange;
	};
}