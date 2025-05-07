#include "PipelineBase.h"
#include "PipelineLayoutBase.h"
#include "BindSetLayoutBase.h"
#include "ShaderModuleBase.h"
#include "common/Utils.h"
#include "common/Error.h"

namespace rhi::impl
{
	ShaderStageState::ShaderStageState() {}

	ShaderStageState::~ShaderStageState() {}

	PipelineBase::PipelineBase(DeviceBase* device, const RenderPipelineDesc& desc) :
		ResourceBase(device, desc.name),
		mPipelineLayout(desc.pipelineLayout)
	{

		AddShaderStageState(desc.vertexShader, ShaderStage::Vertex);
		AddShaderStageState(desc.fragmentShader, ShaderStage::Fragment);
		AddShaderStageState(desc.tessControlShader, ShaderStage::TessellationControl);
		AddShaderStageState(desc.tessEvaluationShader, ShaderStage::TessellationEvaluation);
		AddShaderStageState(desc.geometryShader, ShaderStage::Geometry);
	}

	PipelineBase::PipelineBase(DeviceBase* device, const ComputePipelineDesc& desc) :
		ResourceBase(device, desc.name),
		mPipelineLayout(desc.pipelineLayout)
	{
		AddShaderStageState(desc.computeShader, ShaderStage::Compute);
	}

	PipelineBase::~PipelineBase() {}

	void PipelineBase::AddShaderStageState(const ShaderState* shader, ShaderStage stage)
	{
		if (!shader)
		{
			return;
		}
		mShaderStageMask |= stage;
		ShaderStageState& state = mShaderStageStates[stage];
		state.shaderModule = shader->shaderModule;
		state.constants.reserve(shader->constantCount);
		for (uint32_t i = 0; i < shader->constantCount; ++i)
		{
			state.constants.emplace_back(shader->constants[i]);
		}
	}

	Ref<BindSetLayoutBase> PipelineBase::GetBindSetLayout(uint32_t setIndex)
	{
		ASSERT(mPipelineLayout != nullptr);
		Ref<BindSetLayoutBase> bindSetLayout = mPipelineLayout->GetBindSetLayout(setIndex);
		return bindSetLayout;
	}

	PipelineLayoutBase* PipelineBase::GetLayout()
	{
		return mPipelineLayout.Get();
	}

	BindSetLayoutBase* PipelineBase::APIGetBindSetLayout(uint32_t setIndex)
	{
		return GetBindSetLayout(setIndex).Detach();
	}

	ShaderStage PipelineBase::GetShaderStageMask() const
	{
		return mShaderStageMask;
	}

	bool PipelineBase::HasShaderStage(ShaderStage stage) const
	{
		return (mShaderStageMask & stage) != 0;
	}

	const ShaderStageState& PipelineBase::GetShaderStageState(ShaderStage stage) const
	{
		return mShaderStageStates[stage];
	}
}