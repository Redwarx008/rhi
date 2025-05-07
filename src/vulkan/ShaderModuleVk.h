#pragma once

#include "../ShaderModuleBase.h"
#include "../common/Ref.hpp"

#include <vulkan/vulkan.h>
#include <vector>

namespace rhi::impl::vulkan
{
	class Device;

	class ShaderModule : public ShaderModuleBase
	{
	public:
		static Ref<ShaderModule> Create(Device* device, const ShaderModuleDesc& desc);
		VkShaderModule GetHandle() const;
	private: 
		explicit ShaderModule(Device* device, const ShaderModuleDesc& desc);
		~ShaderModule();
		bool Initialize(const ShaderModuleDesc& desc);
		void DestroyImpl() override;

		VkShaderModule mHandle = VK_NULL_HANDLE;
		std::vector<uint32_t> mSpirvData;
	};
}