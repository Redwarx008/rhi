#include "PipelineLayoutVk.h"

#include "../common/BitSetUtils.h"
#include "../common/Constants.h"
#include "../common/Utils.h"
#include "BindSetLayoutVk.h"
#include "DeviceVk.h"
#include "ErrorsVk.h"
#include "ShaderModuleVk.h"
#include "VulkanUtils.h"

#include <array>

namespace rhi::impl::vulkan
{
    PipelineLayout::PipelineLayout(Device* device, const PipelineLayoutDesc& desc)
        : PipelineLayoutBase(device, desc)
    {}

    PipelineLayout::PipelineLayout(Device* device, const PipelineLayoutDesc2& desc)
        : PipelineLayoutBase(device, desc)
    {}

    PipelineLayout::~PipelineLayout() {}

    Ref<PipelineLayout> PipelineLayout::Create(Device* device, const PipelineLayoutDesc& desc)
    {
        Ref<PipelineLayout> pipelineLayout = AcquireRef(new PipelineLayout(device, desc));
        if (!pipelineLayout->Initialize())
        {
            return nullptr;
        }
        pipelineLayout->TrackResource();
        return pipelineLayout;
    }

    Ref<PipelineLayout> PipelineLayout::Create(Device* device, const PipelineLayoutDesc2& desc)
    {
        Ref<PipelineLayout> pipelineLayout = AcquireRef(new PipelineLayout(device, desc));
        if (!pipelineLayout->Initialize())
        {
            return nullptr;
        }
        pipelineLayout->TrackResource();
        return pipelineLayout;
    }

    bool PipelineLayout::Initialize()
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
                descriptorSetLayouts[index] =
                        checked_cast<BindSetLayout>(mDevice->GetEmptyBindSetLayout())->GetHandle();
            }
        }

        VkPipelineLayoutCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.setLayoutCount = highestBindSetIndex;
        createInfo.pSetLayouts = descriptorSetLayouts.data();

        std::vector<VkPushConstantRange> pushConstantRanges;
        uint32_t offset = 0;
        for (const std::optional<PushConstantRange>& pcr : mPushConstantRanges)
        {
            if (pcr.has_value())
            {
                VkPushConstantRange& vkpcr = pushConstantRanges.emplace_back();
                vkpcr.stageFlags = ToVkShaderStageFlags(pcr.value().visibility);
                vkpcr.offset = offset;
                vkpcr.size = pcr.value().size;
                offset += vkpcr.size;
            }
        }
        createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
        createInfo.pPushConstantRanges = pushConstantRanges.data();

        Device* device = checked_cast<Device>(mDevice);

        VkResult err = vkCreatePipelineLayout(device->GetHandle(), &createInfo, nullptr, &mHandle);
        CHECK_VK_RESULT_FALSE(err, "CreatePipelineLayout");

        SetDebugName(device, mHandle, "PipelineLayout", GetName());

        return true;
    }

    void PipelineLayout::DestroyImpl()
    {
        PipelineLayoutBase::DestroyImpl();
        Device* device = checked_cast<Device>(mDevice);

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
} // namespace rhi::impl::vulkan
