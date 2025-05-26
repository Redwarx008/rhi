#pragma once

#include "PerShaderStage.hpp"
#include "RHIStruct.h"
#include "ResourceBase.h"
#include "common/Cached.hpp"
#include "common/Constants.h"
#include "common/Ref.hpp"

#include <array>
#include <bitset>
#include <optional>

namespace rhi::impl
{
    class PipelineLayoutBase : public ResourceBase, public Cached<PipelineLayoutBase>
    {
    public:
        explicit PipelineLayoutBase(DeviceBase* device, const PipelineLayoutDesc& desc);
        explicit PipelineLayoutBase(DeviceBase* device, const PipelineLayoutDesc2& desc);
        ~PipelineLayoutBase();
        BindSetLayoutBase* APIGetBindSetLayout(uint32_t bindSetIndex) const;
        BindSetLayoutBase* GetBindSetLayout(uint32_t bindSetIndex) const;
        const std::bitset<cMaxBindSets>& GetBindSetMask() const;
        ResourceType GetType() const override;
        const std::optional<PushConstantRange>& GetPushConstantRange(ShaderStage shaderStage) const;
        size_t ComputeContentHash() override;
        struct Equal
        {
            bool operator()(const PipelineLayoutBase* a, const PipelineLayoutBase* b) const;
        };


    protected:
        void DestroyImpl() override;
        std::bitset<cMaxBindSets> mBindSetMask = {};
        std::array<Ref<BindSetLayoutBase>, cMaxBindSets> mBindSetLayouts = {};
        PerShaderStage<std::optional<PushConstantRange>> mPushConstantRanges;
    };
} // namespace rhi::impl
