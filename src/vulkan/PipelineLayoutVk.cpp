#include "PipelineLayoutVk.h"

#include "BindSetLayoutVk.h"
#include "DeviceVk.h"
#include "VulkanUtils.h"
#include "ErrorsVk.h"
#include "../common/BitSetUtils.h"
#include "../common/Constants.h"
#include "../common/Utils.h"
#include <array>

namespace rhi::impl::vulkan
{
	PipelineLayout::PipelineLayout(Device* device, const PipelineLayoutDesc& desc) :
		PipelineLayoutBase(device, desc)
	{

	}

	PipelineLayout::~PipelineLayout() {}

	Ref<PipelineLayout> PipelineLayout::Create(Device* device, const PipelineLayoutDesc& desc)
	{
		Ref<PipelineLayout> pipelineLayout = AcquireRef(new PipelineLayout(device, desc));
		if (!pipelineLayout->Initialize(desc))
		{
			return nullptr;
		}
		return pipelineLayout;
	}

	bool PipelineLayout::Initialize(const PipelineLayoutDesc& desc)
	{
		auto bindSetMask = GetBindSetMask();
		uint32_t highestBindSetIndex = GetHighestBitSetIndex(bindSetMask) + 1;
		std::array<VkDescriptorSetLayout, cMaxBindSets> descriptorSetLayouts{};
		for (uint32_t index = 0; index < highestBindSetIndex; ++index)
		{
			if (bindSetMask[index])
			{
				descriptorSetLayouts[index] = checked_cast<BindSetLayout>(GetBindSetLayout(index))->GetHandle();
			}
			else
			{
				descriptorSetLayouts[index] = checked_cast<BindSetLayout>(mDevice->GetEmptyBindSetLayout())->GetHandle();
			}
		}

		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.setLayoutCount = highestBindSetIndex;
		createInfo.pSetLayouts = descriptorSetLayouts.data();

		VkPushConstantRange pushConstantRange;
		if (desc.pushConstantRange.size != 0)
		{
			pushConstantRange.stageFlags = ShaderStageFlagsConvert(desc.pushConstantRange.visibility);
			pushConstantRange.offset = 0;
			pushConstantRange.size = desc.pushConstantRange.size;

			createInfo.pushConstantRangeCount = 1;
			createInfo.pPushConstantRanges = &pushConstantRange;
		}

		Device* device = checked_cast<Device>(mDevice.Get());

		VkResult err = vkCreatePipelineLayout(device->GetHandle(), &createInfo, nullptr, &mHandle);
		CHECK_VK_RESULT_FALSE(err, "CreatePipelineLayout");

		SetDebugName(device, mHandle, "PipelineLayout", GetName());

		return true;
	}

	void PipelineLayout::DestroyImpl()
	{
		Device* device = checked_cast<Device>(mDevice.Get());

		if (mHandle != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(device->GetHandle(), mHandle, nullptr);
			mHandle = VK_NULL_HANDLE;
		}
	}

	VkPipelineLayout PipelineLayout::GetHandle() const
	{
		return mHandle;
	}

	VkShaderStageFlags PipelineLayout::GetPushConstantVisibility() const
	{
		return mPushConstantVisibility;
	}
}