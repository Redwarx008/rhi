#pragma once

#include <vector>
#include "RHIStruct.h"
#include "ResourceBase.h"
#include "common/Ref.hpp"

namespace rhi::impl
{
    class BindSetBase : public ResourceBase
    {
    public:
        void APIDestroy();
        ResourceType GetType() const override;
        BindSetLayoutBase* GetLayout();
        const std::vector<BindSetEntry>& GetBindingEntries() const;

    protected:
        explicit BindSetBase(DeviceBase* device, const BindSetDesc& desc);
        ~BindSetBase() override;

    private:
        Ref<BindSetLayoutBase> mLayout;
        std::vector<BindSetEntry> mEntries;
    };
} // namespace rhi::impl
