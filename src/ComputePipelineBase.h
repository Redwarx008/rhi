#pragma once

#include "RHIStruct.h"
#include "PipelineBase.h"

namespace rhi::impl
{
	class ComputePipelineBase : public PipelineBase
	{
	public:
		explicit ComputePipelineBase(DeviceBase* device, const ComputePipelineDesc& desc);
		~ComputePipelineBase();
		ResourceType GetType() const override;
	};
}