#include "BindSetLayoutBase.h"
#include "DeviceBase.h"

#include "common/Constants.h"
#include "common/Error.h"
#include "common/ObjectContentHasher.h"

#include <algorithm>

namespace rhi::impl
{
    bool BindSetLayoutBase::BindingInfo::operator==(const BindingInfo& other) const
    {
        return this->type == other.type && this->visibility == other.visibility &&
                this->arrayElementCount == other.arrayElementCount && this->hasDynamicOffset == other.hasDynamicOffset;
    }

    bool BindSetLayoutBase::BindingInfo::operator!=(const BindingInfo& other) const
    {
        return !(*this == other);
    }

    BindSetLayoutBase::BindSetLayoutBase(DeviceBase* device, const BindSetLayoutDesc& desc)
        : ResourceBase(device, desc.name)
    {
        uint32_t maxBinding = 0;
        for (uint32_t i = 0; i < desc.entryCount; ++i)
        {
            maxBinding = (std::max(maxBinding, desc.entries[i].binding));
        }

        ASSERT(maxBinding < cMaxBindingsPerBindSet);
        mBindingIndexToInfoMap.resize(maxBinding + 1);

        for (uint32_t i = 0; i < desc.entryCount; ++i)
        {
            auto& entry = desc.entries[i];
            INVALID_IF(mBindingIndexToInfoMap[entry.binding].has_value(),
                       "Multiple BindSetLayoutEntry are defined in the same slot (%d)",
                       entry.binding);

            mBindingIndexToInfoMap[entry.binding] = {
                    entry.type,
                    entry.visibleStages,
                    entry.arrayElementCount,
                    entry.hasDynamicOffset,
            };
        }
    }

    BindSetLayoutBase::~BindSetLayoutBase()
    {}

    void BindSetLayoutBase::DestroyImpl()
    {
        Uncache();
    }

    ResourceType BindSetLayoutBase::GetType() const
    {
        return ResourceType::BindSetLayout;
    }

    BindingType BindSetLayoutBase::GetBindingType(uint32_t binding) const
    {
        ASSERT(binding < mBindingIndexToInfoMap.size());
        ASSERT(mBindingIndexToInfoMap[binding].has_value());
        return mBindingIndexToInfoMap[binding].value().type;
    }

    ShaderStage BindSetLayoutBase::GetVisibility(uint32_t binding) const
    {
        ASSERT(binding < mBindingIndexToInfoMap.size());
        ASSERT(mBindingIndexToInfoMap[binding].has_value());
        return mBindingIndexToInfoMap[binding].value().visibility;
    }

    bool BindSetLayoutBase::HasDynamicOffset(uint32_t binding) const
    {
        ASSERT(binding < mBindingIndexToInfoMap.size());
        ASSERT(mBindingIndexToInfoMap[binding].has_value());
        return mBindingIndexToInfoMap[binding].value().hasDynamicOffset;
    }

    size_t BindSetLayoutBase::ComputeContentHash()
    {
        ObjectContentHasher recorder;

        for (uint32_t i = 0; i < mBindingIndexToInfoMap.size(); ++i)
        {
            if (mBindingIndexToInfoMap[i].has_value())
            {
                uint32_t bindingIndex = i;
                const BindingInfo& bindingInfo = mBindingIndexToInfoMap[i].value();

                recorder.Record(bindingIndex);
                recorder.Record(bindingInfo.type,
                                bindingInfo.visibility,
                                bindingInfo.arrayElementCount,
                                bindingInfo.hasDynamicOffset);
            }
        }

        return recorder.GetContentHash();
    }

    bool BindSetLayoutBase::Equal::operator()(const BindSetLayoutBase* a, const BindSetLayoutBase* b) const
    {
        return a->mBindingIndexToInfoMap == b->mBindingIndexToInfoMap;
    }
} // namespace rhi::impl
