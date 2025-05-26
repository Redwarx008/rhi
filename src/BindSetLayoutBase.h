#pragma once

#include "RHIStruct.h"
#include "ResourceBase.h"
#include "common/Cached.hpp"

#include <optional>
#include <vector>

namespace rhi::impl
{
    class DeviceBase;

    class BindSetLayoutBase : public ResourceBase, public Cached<BindSetLayoutBase>
    {
    public:
        explicit BindSetLayoutBase(DeviceBase* device, const BindSetLayoutDesc& desc);
        ~BindSetLayoutBase();
        ResourceType GetType() const override;
        BindingType GetBindingType(uint32_t binding) const;
        bool HasDynamicOffset(uint32_t binding) const;
        ShaderStage GetVisibility(uint32_t binding) const;
        size_t ComputeContentHash() override;

        struct Equal
        {
            bool operator()(const BindSetLayoutBase* a, const BindSetLayoutBase* b) const;
        };

    protected:
        void DestroyImpl() override;
        //  A mirror of BindSetLayoutEntry
        struct BindingInfo
        {
            BindingType type;
            ShaderStage visibility;
            uint32_t arrayElementCount;
            bool hasDynamicOffset;
            bool operator==(const BindingInfo& other) const;
            bool operator!=(const BindingInfo& other) const;
        };

        std::vector<std::optional<BindingInfo>> mBindingIndexToInfoMap;
    };
} // namespace rhi::impl
