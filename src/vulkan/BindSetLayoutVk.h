#pragma once

#include "../BindSetLayoutBase.h"
#include "../common/Ref.hpp"
#include "../common/MutexProtected.hpp"
#include "DescriptorSetAllocator.h"

#include <vulkan/vulkan.h>

namespace rhi::impl::vulkan
{
	class BindSet;

	class BindSetLayout final: public BindSetLayoutBase
	{
	public:
		static Ref<BindSetLayout> Create(DeviceBase* device, const BindSetLayoutDesc& desc);
		VkDescriptorSetLayout GetHandle() const;

		Ref<BindSet> AllocateBindSet(const BindSetDesc& desc);

		void DeallocateBindSet(BindSet* bindSet,
			DescriptorSetAllocation* descriptorSetAllocation);
	private:
		explicit BindSetLayout(DeviceBase* device, const BindSetLayoutDesc& desc);
		~BindSetLayout();
		bool Initialize(const BindSetLayoutDesc& desc);
		void DestroyImpl() override;
		VkDescriptorSetLayout mHandle = VK_NULL_HANDLE;
		Ref<DescriptorSetAllocator> mDescriptorSetAllocator;
	};

	VkDescriptorType ToVkDescriptorType(BindingType bindType, bool hasDynamicOffset);
}