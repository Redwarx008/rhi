#pragma once

#include "PerShaderStage.hpp"
#include "RHIStruct.h"
#include "ResourceBase.h"
#include "common/Ref.hpp"

namespace rhi::impl
{
    class DeviceBase;
    class BindSetLayoutBase;
    class PipelineLayoutBase;

    struct ShaderStageState
    {
        ShaderStageState();
        ~ShaderStageState();
        Ref<ShaderModuleBase> shaderModule;
        std::vector<SpecializationConstant> constants;
    };

    class PipelineBase : public ResourceBase
    {
    public:
        BindSetLayoutBase* APIGetBindSetLayout(uint32_t setIndex);
        Ref<BindSetLayoutBase> GetBindSetLayout(uint32_t setIndex);
        PipelineLayoutBase* GetLayout();
        ShaderStage GetShaderStageMask() const;
        bool HasShaderStage(ShaderStage stage) const;
        const ShaderStageState& GetShaderStageState(ShaderStage stage) const;

    protected:
        explicit PipelineBase(DeviceBase* device, const RenderPipelineDesc& desc);
        explicit PipelineBase(DeviceBase* device, const ComputePipelineDesc& desc);
        ~PipelineBase() override;
        void AddShaderStageState(const ShaderState* shader, ShaderStage stage);

        Ref<PipelineLayoutBase> mPipelineLayout;

        ShaderStage mShaderStageMask = ShaderStage::None;

        PerShaderStage<ShaderStageState> mShaderStageStates;
    };
} // namespace rhi::impl
