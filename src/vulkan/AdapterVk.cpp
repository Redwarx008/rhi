#include "AdapterVk.h"
#include "DeviceVk.h"
#include "../common/Constants.h"

namespace rhi::impl::vulkan
{
	AdapterType AdapterTypeConvert(VkPhysicalDeviceType type)
	{
		switch (type)
		{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return AdapterType::IntegratedGPU;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return AdapterType::DiscreteGPU;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return AdapterType::VirtualGPU;
		default:
			return AdapterType::Unknown;
		}
	}

	Adapter::Adapter(InstanceBase* instance, VkPhysicalDevice vulkanPhysicalDevice)
		:
		AdapterBase(instance),
		mVulkanPhysicalDevice(vulkanPhysicalDevice)
	{

		VkPhysicalDeviceVulkan13Properties properties13{};
		properties13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;

		VkPhysicalDeviceProperties2 deviceProperties2{};
		deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		deviceProperties2.pNext = &properties13;

		vkGetPhysicalDeviceProperties2(mVulkanPhysicalDevice, &deviceProperties2);
		VkPhysicalDeviceProperties& properties = deviceProperties2.properties;

		mApiVersion = properties.apiVersion;
		mDriverVersion = properties.driverVersion;
		mVendorID = properties.vendorID;
		mDeviceID = properties.deviceID;
		mAdapterType = AdapterTypeConvert(properties.deviceType);
		mDeviceName = properties.deviceName;

		mLimits.maxTextureDimension1D = properties.limits.maxImageDimension1D;
		mLimits.maxTextureDimension2D = properties.limits.maxImageDimension2D;
		mLimits.maxTextureDimension3D = properties.limits.maxImageDimension3D;
		mLimits.maxTextureArrayLayers = properties.limits.maxImageArrayLayers;
		mLimits.maxBindSets = properties.limits.maxBoundDescriptorSets;
		mLimits.maxBufferSize = properties13.maxBufferSize;
		mLimits.maxBindingsPerBindSet = cMaxBindingsPerBindSet; // Todo: figure out this value
		mLimits.maxDynamicUniformBuffersPerPipelineLayout = properties.limits.maxDescriptorSetUniformBuffersDynamic;
		mLimits.maxDynamicStorageBuffersPerPipelineLayout = properties.limits.maxDescriptorSetStorageBuffersDynamic;
		mLimits.maxSampledTexturesPerShaderStage = properties.limits.maxPerStageDescriptorSampledImages;
		mLimits.maxSamplersPerShaderStage = properties.limits.maxPerStageDescriptorSamplers;
		mLimits.maxStorageBuffersPerShaderStage = properties.limits.maxPerStageDescriptorStorageBuffers;
		mLimits.maxStorageTexturesPerShaderStage = properties.limits.maxPerStageDescriptorStorageImages;
		mLimits.maxUniformBuffersPerShaderStage = properties.limits.maxPerStageDescriptorUniformBuffers;
		mLimits.maxUniformBufferBindingSize = properties.limits.maxUniformBufferRange;
		mLimits.maxStorageBufferBindingSize = properties.limits.maxStorageBufferRange;
		mLimits.minUniformBufferOffsetAlignment = properties.limits.minUniformBufferOffsetAlignment;
		mLimits.minStorageBufferOffsetAlignment = properties.limits.minStorageBufferOffsetAlignment;
		mLimits.maxVertexBuffers = properties.limits.maxVertexInputBindings;
		mLimits.maxVertexAttributes = properties.limits.maxVertexInputAttributes;
		mLimits.maxVertexBufferArrayStride = properties.limits.maxVertexInputBindingStride;
		mLimits.maxVertexOutputComponents = properties.limits.maxVertexOutputComponents;
		mLimits.maxFragmentInputComponents = properties.limits.maxFragmentInputComponents;
		mLimits.maxColorAttachments = properties.limits.maxColorAttachments;
		mLimits.maxComputeWorkgroupStorageSize = properties.limits.maxComputeSharedMemorySize;
		mLimits.maxComputeInvocationsPerWorkgroup = properties.limits.maxComputeWorkGroupInvocations;
		mLimits.maxComputeWorkgroupSizeX = properties.limits.maxComputeWorkGroupSize[0];
		mLimits.maxComputeWorkgroupSizeY = properties.limits.maxComputeWorkGroupSize[1];
		mLimits.maxComputeWorkgroupSizeZ = properties.limits.maxComputeWorkGroupSize[2];
		mLimits.maxComputeWorkgroupsPerDimension = (std::min)(
			{
				properties.limits.maxComputeWorkGroupCount[0],
				properties.limits.maxComputeWorkGroupCount[1],
				properties.limits.maxComputeWorkGroupCount[2],
			});
		mLimits.maxStorageBuffersPerShaderStage = properties.limits.maxPerStageDescriptorStorageBuffers;
		mLimits.maxStorageTexturesPerShaderStage = properties.limits.maxPerStageDescriptorStorageImages;
		mLimits.maxPushConstantsSize = properties.limits.maxPushConstantsSize;
		mLimits.maxDrawIndirectCount = properties.limits.maxDrawIndirectCount;
		mLimits.maxViewports = properties.limits.maxViewports;
		mLimits.maxSamplerLodBias = properties.limits.maxSamplerLodBias;
		mLimits.maxSamplerAnisotropy = properties.limits.maxSamplerAnisotropy;
	}



	DeviceBase* Adapter::APICreateDevice(const DeviceDesc& desc)
	{
		return Device::Create(this, desc).Detach();
	}

	VkPhysicalDevice Adapter::GetHandle() const
	{
		return mVulkanPhysicalDevice;
	}

}