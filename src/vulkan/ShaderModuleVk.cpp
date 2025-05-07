#include "ShaderModuleVk.h"
#include "DeviceVk.h"
#include "ErrorsVk.h"
#include "VulkanUtils.h"
#include "../common/Utils.h"

//#include <spirv_reflect.h>

namespace rhi::impl::vulkan
{
	ShaderModule::ShaderModule(Device* device, const ShaderModuleDesc& desc) :
		ShaderModuleBase(device, desc)
	{

	}

	ShaderModule::~ShaderModule() {}

	Ref<ShaderModule> ShaderModule::Create(Device* device, const ShaderModuleDesc& desc)
	{
		Ref<ShaderModule> shaderModule = AcquireRef(new ShaderModule(device, desc));
		if (!shaderModule->Initialize(desc))
		{
			return nullptr;
		}
		return std::move(shaderModule);
	}

	bool ShaderModule::Initialize(const ShaderModuleDesc& desc)
	{
		ShaderModuleBase::Initialize();

		mSpirvData.reserve(desc.code.size() / sizeof(uint32_t));
		memcpy_s(mSpirvData.data(), desc.code.size(), desc.code.data(), desc.code.size());

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pCode = mSpirvData.data();
		createInfo.codeSize = desc.code.size();

		Device* device = checked_cast<Device>(mDevice.Get());

		VkResult err = vkCreateShaderModule(device->GetHandle(), &createInfo, nullptr, &mHandle);
		CHECK_VK_RESULT_FALSE(err, "CreateShaderModule");

		SetDebugName(device, mHandle, "ShaderModule", GetName());

		return true;
	}

	void ShaderModule::DestroyImpl()
	{
		Device* device = checked_cast<Device>(mDevice.Get());

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
}