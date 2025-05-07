#include "ComputePipelineVk.h"
#include "ShaderModuleVk.h"
#include "DeviceVk.h"
#include "PipelineLayoutVk.h"
#include "ErrorsVk.h"
#include "VulkanUtils.h"

namespace rhi::impl::vulkan
{
	ComputePipeline::ComputePipeline(Device* device, const ComputePipelineDesc& desc) :
		ComputePipelineBase(device, desc)
	{

	}

	ComputePipeline::~ComputePipeline() {}

	Ref<ComputePipeline> ComputePipeline::Create(Device* device, const ComputePipelineDesc& desc)
	{
		Ref<ComputePipeline> pipeline = AcquireRef(new ComputePipeline(device, desc));
		if (!pipeline->Initialize())
		{
			return nullptr;
		}
		return pipeline;
	}

	void ComputePipeline::DestroyImpl()
	{
		Device* device = checked_cast<Device>(mDevice.Get());

		if (mHandle != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(device->GetHandle(), mHandle, nullptr);
			mHandle = VK_NULL_HANDLE;
		}
	}

	bool ComputePipeline::Initialize()
	{
		ASSERT(HasShaderStage(ShaderStage::Compute));

		const ShaderStageState& stageState = GetShaderStageState(ShaderStage::Compute);

		std::vector<VkSpecializationMapEntry> specMapEntries(stageState.constants.size());
		std::vector<uint32_t> specDatas(stageState.constants.size());
		uint32_t dataOffset = 0;
		for (uint32_t i = 0; i < stageState.constants.size(); ++i)
		{
			const SpecializationConstant& constant = stageState.constants[i];
			VkSpecializationMapEntry& specMapEntry = specMapEntries[i];
			specMapEntry.constantID = constant.constantID;
			specMapEntry.offset = dataOffset;
			specMapEntry.size = sizeof(uint32_t);
			specDatas[i] = constant.value.u;
			dataOffset += specMapEntry.size;
		}
		VkSpecializationInfo specInfo{};
		specInfo.dataSize = stageState.constants.size() * sizeof(uint32_t);
		specInfo.pData = specDatas.data();
		specInfo.pMapEntries = specMapEntries.data();
		specInfo.mapEntryCount = specMapEntries.size();

		VkComputePipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		createInfo.layout = checked_cast<PipelineLayout>(mPipelineLayout)->GetHandle();

		createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		createInfo.stage.pNext = nullptr;
		createInfo.stage.flags = 0;
		createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		if (!stageState.constants.empty())
		{
			createInfo.stage.pSpecializationInfo = &specInfo;
		}

		Device* device = checked_cast<Device>(mDevice.Get());
		VkResult err = vkCreateComputePipelines(device->GetHandle(), nullptr, 1, &createInfo, nullptr, &mHandle);
		CHECK_VK_RESULT_FALSE(err, "CreateComputePipelines");

		SetDebugName(device, mHandle, "ComputePipeline", GetName());

		return true;
	}

	VkPipeline ComputePipeline::GetHandle() const
	{
		return mHandle;
	}
}