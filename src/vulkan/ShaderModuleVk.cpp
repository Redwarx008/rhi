#include "ShaderModuleVk.h"
#include "../common/Utils.h"
#include "DeviceVk.h"
#include "ErrorsVk.h"
#include "VulkanUtils.h"

// #include <spirv_reflect.h>

namespace rhi::impl::vulkan
{
    ShaderModule::ShaderModule(Device* device, const ShaderModuleDesc& desc)
        : ShaderModuleBase(device, desc)
    {}

    ShaderModule::~ShaderModule() = default;

    Ref<ShaderModule> ShaderModule::Create(Device* device, const ShaderModuleDesc& desc)
    {
        Ref<ShaderModule> shaderModule = AcquireRef(new ShaderModule(device, desc));
        if (!shaderModule->Initialize(desc))
        {
            return nullptr;
        }
        shaderModule->TrackResource();
        return shaderModule;
    }

    bool ShaderModule::Initialize(const ShaderModuleDesc& desc)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pCode = mSpirvData.data();
        createInfo.codeSize = desc.code.size();

        Device* device = checked_cast<Device>(mDevice);

        VkResult err = vkCreateShaderModule(device->GetHandle(), &createInfo, nullptr, &mHandle);
        CHECK_VK_RESULT_FALSE(err, "CreateShaderModule");

        SetDebugName(device, mHandle, "ShaderModule", GetName());

        return true;
    }

    void ShaderModule::DestroyImpl()
    {
        Device* device = checked_cast<Device>(mDevice);

        if (mHandle != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device->GetHandle(), mHandle, nullptr);
            mHandle = VK_NULL_HANDLE;
        }
    }

    VkShaderModule ShaderModule::GetHandle() const
    {
        return mHandle;
    }

} // namespace rhi::impl::vulkan
