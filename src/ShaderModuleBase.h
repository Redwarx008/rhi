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
        const std::vector<uint32_t>& GetSpirvData() const;
    protected:
        explicit ShaderModuleBase(DeviceBase* device, const ShaderModuleDesc& desc);
        ~ShaderModuleBase();

        std::string mEntry;
        std::vector<uint32_t> mSpirvData;
    };
}
