#include "PipelineLayoutBase.h"
#include "DeviceBase.h"
#include "BindSetLayoutBase.h"
#include "common/Error.h"

namespace rhi::impl
{
	PipelineLayoutBase::PipelineLayoutBase(DeviceBase* device, const PipelineLayoutDesc& desc) :
		ResourceBase(device, desc.name)
	{
		ASSERT(desc.bindSetLayoutCount <= cMaxBindSets);

		for (uint32_t i = 0; i < desc.bindSetLayoutCount; ++i)
		{
			if (desc.bindSetLayouts[i] == nullptr)
			{
				continue;
			}
			mBindGroupLayouts[i] = desc.bindSetLayouts[i];
			mBindSetMask.set(i);
		}
	}
	
	PipelineLayoutBase::~PipelineLayoutBase() {}

	ResourceType PipelineLayoutBase::GetType() const
	{
		return ResourceType::PipelineLayout;
	}

	const std::bitset<cMaxBindSets>& PipelineLayoutBase::GetBindSetMask() const
	{
		return mBindSetMask;
	}

	const PushConstantRange& PipelineLayoutBase::GetPushConstants() const
	{
		return mPushConstantRange;
	}

	BindSetLayoutBase* PipelineLayoutBase::GetBindSetLayout(uint32_t bindSetIndex) const
	{
		ASSERT(bindSetIndex < cMaxBindSets);
		ASSERT(mBindGroupLayouts[bindSetIndex] != nullptr);
		return mBindGroupLayouts[bindSetIndex].Get();
	}

	BindSetLayoutBase* PipelineLayoutBase::APIGetBindSetLayout(uint32_t bindSetIndex) const
	{
		ASSERT(bindSetIndex < cMaxBindSets);
		ASSERT(mBindGroupLayouts[bindSetIndex] != nullptr);
		Ref<BindSetLayoutBase> bindSetLayout = mBindGroupLayouts[bindSetIndex];
		return bindSetLayout.Detach();
	}
}