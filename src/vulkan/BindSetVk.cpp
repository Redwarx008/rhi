#include "BindSetVk.h"

#include "../common/Error.h"
#include "../common/Constants.h"
#include "DeviceVk.h"
#include "BindSetLayoutVk.h"
#include "TextureVk.h"
#include "BufferVk.h"
#include "SamplerVk.h"
#include <array>
#include <absl/container/inlined_vector.h>

namespace rhi::impl::vulkan
{
	Ref<BindSet> BindSet::Create(Device* device, const BindSetDesc& desc)
	{
		return checked_cast<BindSetLayout>(desc.layout)->AllocateBindSet(desc);
	}

	BindSet::BindSet(Device* device, const BindSetDesc& desc, DescriptorSetAllocation descriptorSetAllocation) :
		BindSetBase(device, desc),
		mDescriptorSetAllocation(descriptorSetAllocation)
	{
		BindSetBase::Initialize();

		absl::InlinedVector<VkWriteDescriptorSet, cMaxOptimalBindingsPerGroup> writes(desc.entryCount);
		absl::InlinedVector<VkDescriptorBufferInfo, cMaxOptimalBindingsPerGroup> writeBufferInfo(desc.entryCount);
		absl::InlinedVector<VkDescriptorImageInfo, cMaxOptimalBindingsPerGroup> writeImageInfo(desc.entryCount);

		for (uint32_t i = 0; i < desc.entryCount; ++i)
		{
			VkWriteDescriptorSet& write = writes[i];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;
			write.dstSet = GetHandle();
			write.dstBinding = desc.entries[i].binding;
			write.dstArrayElement = desc.entries[i].arrayElementIndex;
			write.descriptorCount = 1;
			
			BindingType bindingType = desc.layout->GetBindingType(desc.entries[i].binding);

			write.descriptorType = ToVkDescriptorType(bindingType, desc.layout->HasDynamicOffset(desc.entries[i].binding));

			switch (bindingType)
			{
			case BindingType::SampledTexture:
			{
				VkDescriptorImageInfo& imageInfo = writeImageInfo[i];
				imageInfo.imageView = checked_cast<TextureView>(desc.entries[i].textureView)->GetHandle();
				imageInfo.imageLayout = ImageLayoutConvert(TextureUsage::SampledBinding, desc.entries[i].textureView->GetTexture()->APIGetFormat());
				write.pImageInfo = &imageInfo;
				break;
			}
			case BindingType::StorageTexture:
			{
				VkDescriptorImageInfo& imageInfo = writeImageInfo[i];
				imageInfo.imageView = checked_cast<TextureView>(desc.entries[i].textureView)->GetHandle();
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
				write.pImageInfo = &imageInfo;
				break;
			}
			case BindingType::StorageBuffer:
			case BindingType::UniformBuffer:
			{
				VkDescriptorBufferInfo& bufferInfo = writeBufferInfo[i];
				bufferInfo.buffer = checked_cast<Buffer>(desc.entries[i].buffer)->GetHandle();
				bufferInfo.offset = desc.entries[i].bufferOffset;
				bufferInfo.range = desc.entries[i].bufferRange;
				write.pBufferInfo = &bufferInfo;
				break;
			}
			case BindingType::Sampler:
			{
				VkDescriptorImageInfo& imageInfo = writeImageInfo[i];
				imageInfo.sampler = checked_cast<Sampler>(desc.entries[i].sampler)->GetHandle();
				write.pImageInfo = &imageInfo;
				break;
			}
			case BindingType::CombinedTextureSampler:
			{
				VkDescriptorImageInfo& imageInfo = writeImageInfo[i];
				imageInfo.imageView = checked_cast<TextureView>(desc.entries[i].textureView)->GetHandle();
				imageInfo.imageLayout = ImageLayoutConvert(TextureUsage::StorageBinding, desc.entries[i].textureView->GetTexture()->APIGetFormat());
				imageInfo.sampler = checked_cast<Sampler>(desc.entries[i].sampler)->GetHandle();
				write.pImageInfo = &imageInfo;
				break;
			}
			default:
				break;
			}
		}

		vkUpdateDescriptorSets(device->GetHandle(), desc.entryCount, writes.data(), 0, nullptr);
	}

	BindSet::~BindSet() {}

	VkDescriptorSet BindSet::GetHandle() const
	{
		return mDescriptorSetAllocation.set;
	}

	void BindSet::MarkUsedInQueue(QueueType queueType)
	{
		static_assert(static_cast<uint32_t>(QueueType::Graphics) == 0 && static_cast<uint32_t>(QueueType::Compute) == 1);
		ASSERT(queueType != QueueType::Transfer);
		ASSERT(mDevice->GetQueue(queueType) != nullptr);
		mUsedInQueues[static_cast<uint32_t>(queueType)] = true;
	}

	bool BindSet::IsUsedInQueue(QueueType queueType)
	{
		static_assert(static_cast<uint32_t>(QueueType::Graphics) == 0 && static_cast<uint32_t>(QueueType::Compute) == 1);
		ASSERT(queueType != QueueType::Transfer);

		return mUsedInQueues[static_cast<uint32_t>(queueType)];
	}

	void BindSet::DestroyImpl()
	{
		checked_cast<BindSetLayout>(GetLayout())->DeallocateBindSet(this, &mDescriptorSetAllocation);
	}

}