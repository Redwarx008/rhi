#include "ShaderModuleBase.h"

namespace rhi::impl
{
	ShaderModuleBase::ShaderModuleBase(DeviceBase* device, const ShaderModuleDesc desc) :
		ResourceBase(device, desc.name),
		mEntry(desc.entry)
	{

	}

	ShaderModuleBase::~ShaderModuleBase() {}

	ResourceType ShaderModuleBase::GetType() const
	{
		return ResourceType::ShaderModule;
	}

	std::string_view ShaderModuleBase::GetEntry() const
	{
		return mEntry;
	}
}