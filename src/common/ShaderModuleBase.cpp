#include "ShaderModuleBase.h"

#include <cstring>

namespace rhi::impl
{
    ShaderModuleBase::ShaderModuleBase(DeviceBase* device, const ShaderModuleDesc& desc)
        : ResourceBase(device, desc.name)
        , mEntry(desc.entry)
    {
        mSpirvData.resize(desc.code.size() / sizeof(uint32_t));
        std::memcpy(mSpirvData.data(), desc.code.data(), desc.code.size());
    }

    ShaderModuleBase::~ShaderModuleBase() = default;

    ResourceType ShaderModuleBase::GetType() const
    {
        return ResourceType::ShaderModule;
    }

    std::string_view ShaderModuleBase::GetEntry() const
    {
        return mEntry;
    }
    const std::vector<uint32_t>& ShaderModuleBase::GetSpirvData() const
    {
        return mSpirvData;
    }
} // namespace rhi::impl
