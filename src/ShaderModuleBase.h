#pragma once

#include "ResourceBase.h"
#include "RHIStruct.h"
#include <string>
#include <string_view>

namespace rhi::impl
{
	class ShaderModuleBase : public ResourceBase
	{
	public:
		ResourceType GetType() const override;
		std::string_view GetEntry() const;
	protected:
		explicit ShaderModuleBase(DeviceBase* device, const ShaderModuleDesc);
		~ShaderModuleBase();

		std::string mEntry;
	};
}