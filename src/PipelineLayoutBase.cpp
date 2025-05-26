#include "PipelineLayoutBase.h"
#include <array>
#include <spirv_reflect.h>
#include <vector>
#include "BindSetLayoutBase.h"
#include "DeviceBase.h"
#include "ShaderModuleBase.h"
#include "common/BitSetUtils.h"
#include "common/Constants.h"
#include "common/Error.h"
#include "common/ObjectContentHasher.h"

namespace rhi::impl
{
    BindingType ToBindingType(SpvReflectDescriptorType descriptorType)
    {
        switch (descriptorType)
        {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
            return BindingType::Sampler;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return BindingType::CombinedTextureSampler;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return BindingType::SampledTexture;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return BindingType::StorageTexture;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            return BindingType::UniformBuffer;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            return BindingType::StorageBuffer;
        case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
            return BindingType::None; // todo : add ACCELERATION_STRUCTURE support?
        default:
            ASSERT(!"Unreachable");
            return BindingType::None;
        }
    }

    ShaderStage ToShaderStage(SpvReflectShaderStageFlagBits stage)
    {
        switch (stage)
        {
        case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
            return ShaderStage::Vertex;
        case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return ShaderStage::TessellationControl;
        case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return ShaderStage::TessellationEvaluation;
        case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
            return ShaderStage::Geometry;
        case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
            return ShaderStage::Fragment;
        case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
            return ShaderStage::Compute;
        case SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV:
            return ShaderStage::Task;
        case SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV:
            return ShaderStage::Mesh;
        default:
            ASSERT(!"Unreachable");
            return ShaderStage::None;
        }
    }

    PipelineLayoutBase::PipelineLayoutBase(DeviceBase* device, const PipelineLayoutDesc& desc) :
        ResourceBase(device, desc.name)
    {
        ASSERT(desc.bindSetLayoutCount <= cMaxBindSets);

        for (uint32_t i = 0; i < desc.bindSetLayoutCount; ++i)
        {
            if (desc.bindSetLayouts[i] == nullptr)
            {
                continue;
            }
            mBindSetLayouts[i] = desc.bindSetLayouts[i];
            mBindSetMask.set(i);
        }

        for (uint32_t i = 0; i < desc.pushConstantCount; ++i)
        {
            const PushConstantRange& pcr = desc.pushConstantRanges[i];
            INVALID_IF(mPushConstantRanges[pcr.visibility].has_value(),
                       "Each shader stage can only have one pushConstant");
            mPushConstantRanges[pcr.visibility] = pcr;
        }
    }

    PipelineLayoutBase::PipelineLayoutBase(DeviceBase* device, const PipelineLayoutDesc2& desc) :
        ResourceBase(device, desc.name)
    {
        std::array<std::vector<BindSetLayoutEntry>, cMaxBindSets> bindSetLayoutEntriesPerSet;

        for (uint32_t i = 0; i < desc.shaderCount; ++i)
        {
            ShaderModuleBase* shader = desc.shaders[i];
            SpvReflectShaderModule reflectModule{};
            SpvReflectResult result = spvReflectCreateShaderModule(
                    shader->GetSpirvData().size() * sizeof(uint32_t), shader->GetSpirvData().data(), &reflectModule);
            ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);


            for (uint32_t bindingIndex = 0; bindingIndex < reflectModule.descriptor_binding_count; ++bindingIndex)
            {
                const SpvReflectDescriptorBinding& reflectBinding = reflectModule.descriptor_bindings[bindingIndex];

                BindSetLayoutEntry entry{};
                entry.binding = reflectBinding.binding;
                entry.type = ToBindingType(reflectBinding.descriptor_type);
                entry.arrayElementCount = 1;
                for (uint32_t dim = 0; dim < reflectBinding.array.dims_count; ++dim)
                {
                    entry.arrayElementCount *= reflectBinding.array.dims[dim];
                }
                entry.visibleStages = ToShaderStage(reflectModule.shader_stage);
                entry.hasDynamicOffset =
                        reflectBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
                        reflectBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

                bindSetLayoutEntriesPerSet[reflectBinding.set].push_back(std::move(entry));
            }

            ASSERT(reflectModule.push_constant_block_count <= 1);
            for (uint32_t pushConstantIndex = 0; pushConstantIndex < reflectModule.push_constant_block_count;
                 ++pushConstantIndex)
            {
                PushConstantRange pcr{};
                pcr.size = reflectModule.push_constant_blocks[pushConstantIndex].size;
                pcr.visibility = ToShaderStage(reflectModule.shader_stage);

                mPushConstantRanges[pcr.visibility] = pcr;
            }

            spvReflectDestroyShaderModule(&reflectModule);
        }

        for (uint32_t setIndex = 0; setIndex < cMaxBindSets; ++setIndex)
        {
            if (!bindSetLayoutEntriesPerSet[setIndex].empty())
            {
                BindSetLayoutDesc desc{};
                desc.entryCount = static_cast<uint32_t>(bindSetLayoutEntriesPerSet[setIndex].size());
                desc.entries = bindSetLayoutEntriesPerSet[setIndex].data();


                mBindSetLayouts[setIndex] = mDevice->GetOrCreateBindSetLayout(desc);
                mBindSetMask.set(setIndex);
            }
        }
    }

    PipelineLayoutBase::~PipelineLayoutBase() = default;

    void PipelineLayoutBase::DestroyImpl()
    {
        Uncache();
    }

    ResourceType PipelineLayoutBase::GetType() const
    {
        return ResourceType::PipelineLayout;
    }

    const std::bitset<cMaxBindSets>& PipelineLayoutBase::GetBindSetMask() const
    {
        return mBindSetMask;
    }

    const std::optional<PushConstantRange>& PipelineLayoutBase::GetPushConstantRange(ShaderStage shaderStage) const
    {
        return mPushConstantRanges[shaderStage];
    }

    size_t PipelineLayoutBase::ComputeContentHash()
    {
        ObjectContentHasher recorder;
        recorder.Record(mBindSetMask);

        for (uint32_t set : IterateBitSet(mBindSetMask))
        {
            recorder.Record(GetBindSetLayout(set)->GetContentHash());
        }

        for (const std::optional<PushConstantRange>& pcr : mPushConstantRanges)
        {
            if (pcr.has_value())
            {
                recorder.Record(pcr.value().visibility, pcr.value().size);
            }
        }

        return recorder.GetContentHash();
    }

    bool PipelineLayoutBase::Equal::operator()(const PipelineLayoutBase* a, const PipelineLayoutBase* b) const
    {
        if (a->mBindSetMask != b->mBindSetMask)
        {
            return false;
        }

        for (uint32_t set : IterateBitSet(a->mBindSetMask))
        {
            if (a->GetBindSetLayout(set) != b->GetBindSetLayout(set))
            {
                return false;
            }
        }

        return a->mPushConstantRanges == b->mPushConstantRanges;
    }

    BindSetLayoutBase* PipelineLayoutBase::GetBindSetLayout(uint32_t bindSetIndex) const
    {
        ASSERT(bindSetIndex < cMaxBindSets);
        ASSERT(mBindSetLayouts[bindSetIndex] != nullptr);
        return mBindSetLayouts[bindSetIndex].Get();
    }

    BindSetLayoutBase* PipelineLayoutBase::APIGetBindSetLayout(uint32_t bindSetIndex) const
    {
        ASSERT(bindSetIndex < cMaxBindSets);
        ASSERT(mBindSetLayouts[bindSetIndex] != nullptr);
        Ref<BindSetLayoutBase> bindSetLayout = mBindSetLayouts[bindSetIndex];
        return bindSetLayout.Detach();
    }
} // namespace rhi::impl
