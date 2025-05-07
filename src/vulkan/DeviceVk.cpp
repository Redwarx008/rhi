#include "DeviceVk.h"
#include "InstanceVk.h"
#include "AdapterVk.h"
#include "QueueVk.h"
#include "SwapChainVk.h"
#include "CommandListVk.h"
#include "PipelineLayoutVk.h"
#include "RenderPipelineVk.h"
#include "ComputePipelineVk.h"
#include "BindSetLayoutVk.h"
#include "BindSetVk.h"
#include "TextureVk.h"
#include "BufferVk.h"
#include "ShaderModuleVk.h"
#include "SamplerVk.h"
#include "ErrorsVk.h"
#include "../common/Error.h"
#include "../common/Utils.h"

#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <optional>



namespace rhi::impl::vulkan
{
	Ref<Device> Device::Create(Adapter* adapter, const DeviceDesc& desc)
	{
		Ref<Device> device = AcquireRef(new Device(adapter, desc));
		if (!device->Initialize(desc))
		{
			return nullptr;
		}
		return device;
	}

	Device::Device(Adapter* adapter, const DeviceDesc& desc) :
		DeviceBase(adapter, desc)
	{

	}

	bool Device::Initialize(const DeviceDesc& desc)
	{
		std::vector<const char*> deviceExtensions;
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		// Get list of supported extensions
		std::vector<std::string> supportedExtensions;
		uint32_t extCount = 0;
		Adapter* adapter = checked_cast<Adapter>(mAdapter.Get());
		vkEnumerateDeviceExtensionProperties(adapter->GetHandle(), nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateDeviceExtensionProperties(checked_cast<Adapter>(mAdapter)->GetHandle(),
				nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				for (auto& ext : extensions)
				{
					supportedExtensions.push_back(ext.extensionName);
				}
			}
		}

		for (auto extension : deviceExtensions)
		{
			if (std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) == supportedExtensions.end())
			{
				LOG_ERROR(extension, "is not supported.");
				return false;
			}
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.fillModeNonSolid = true;
		deviceFeatures.imageCubeArray = true;
		deviceFeatures.textureCompressionBC = HasRequiredFeature(FeatureName::TextureCompressionBC);
		deviceFeatures.textureCompressionETC2 = HasRequiredFeature(FeatureName::TextureCompressionETC2);
		deviceFeatures.textureCompressionASTC_LDR = HasRequiredFeature(FeatureName::TextureCompressionASTC);
		deviceFeatures.multiViewport = HasRequiredFeature(FeatureName::MultiViewport);
		deviceFeatures.depthBiasClamp = HasRequiredFeature(FeatureName::DepthBiasClamp);
		deviceFeatures.depthClamp = HasRequiredFeature(FeatureName::DepthClamp);
		deviceFeatures.samplerAnisotropy = HasRequiredFeature(FeatureName::SamplerAnisotropy);
		deviceFeatures.sampleRateShading = HasRequiredFeature(FeatureName::SampleRateShading);
		deviceFeatures.shaderInt16 = HasRequiredFeature(FeatureName::ShaderInt16);
		deviceFeatures.shaderInt64 = HasRequiredFeature(FeatureName::ShaderInt64);
		deviceFeatures.shaderFloat64 = HasRequiredFeature(FeatureName::ShaderFloat64);
		deviceFeatures.multiDrawIndirect = HasRequiredFeature(FeatureName::MultiDrawIndirect);
		deviceFeatures.geometryShader = HasRequiredFeature(FeatureName::GeometryShader);
		deviceFeatures.tessellationShader = HasRequiredFeature(FeatureName::TessellationShader);
		deviceFeatures.shaderStorageImageExtendedFormats = HasRequiredFeature(FeatureName::R8UnormStorage);

		VkPhysicalDeviceVulkan13Features feature13{};
		feature13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		feature13.synchronization2 = true;
		feature13.dynamicRendering = true;

		VkPhysicalDeviceVulkan12Features feature12{};
		feature12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		feature12.timelineSemaphore = true;
		feature12.uniformBufferStandardLayout = true;
		feature12.scalarBlockLayout = true;
		feature12.separateDepthStencilLayouts = true;
		feature12.pNext = &feature13;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(checked_cast<Adapter>(mAdapter)->GetHandle(), &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyPropertieses(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(checked_cast<Adapter>(mAdapter)->GetHandle(), &queueFamilyCount, queueFamilyPropertieses.data());

		std::array<std::optional<uint32_t>, 3> queueFamlies;
		for (uint32_t i = 0; i < queueFamilyPropertieses.size(); i++)
		{
			const auto& queueFamilyProps = queueFamilyPropertieses[i];

			if (!queueFamlies[static_cast<uint32_t>(QueueType::Graphics)].has_value() &&
				queueFamilyProps.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT))
			{
				queueFamlies[static_cast<uint32_t>(QueueType::Graphics)] = i;
			}

			if (!queueFamlies[static_cast<uint32_t>(QueueType::Compute)].has_value() &&
				queueFamilyProps.queueFlags & VK_QUEUE_COMPUTE_BIT &&
				!(queueFamilyProps.queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				queueFamlies[static_cast<uint32_t>(QueueType::Compute)] = i;
			}

			if (!queueFamlies[static_cast<uint32_t>(QueueType::Transfer)].has_value() &&
				queueFamilyProps.queueFlags & VK_QUEUE_TRANSFER_BIT &&
				!(queueFamilyProps.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
				!(queueFamilyProps.queueFlags & VK_QUEUE_COMPUTE_BIT))
			{
				queueFamlies[static_cast<uint32_t>(QueueType::Transfer)] = i;
			}
		}

		float priority = 1.f;
		std::vector<VkDeviceQueueCreateInfo> queueCIs;

		for (uint32_t i = 0; i < queueFamlies.size(); ++i)
		{
			if (queueFamlies[i].has_value())
			{
				auto& queueCI = queueCIs.emplace_back();
				queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCI.queueFamilyIndex = queueFamlies[i].value();
				queueCI.queueCount = 1;
				queueCI.pQueuePriorities = &priority;
			}
		}

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.queueCreateInfoCount = queueCIs.size();
		deviceCreateInfo.pQueueCreateInfos = queueCIs.data();
		deviceCreateInfo.pNext = &feature12;

		VkResult err = vkCreateDevice(checked_cast<Adapter>(mAdapter)->GetHandle(), &deviceCreateInfo, nullptr, &mHandle);
		CHECK_VK_RESULT_FALSE(err);

		VmaAllocatorCreateInfo allocatorCreateInfo{};
		allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		allocatorCreateInfo.physicalDevice = checked_cast<Adapter>(mAdapter)->GetHandle();
		allocatorCreateInfo.device = mHandle;
		allocatorCreateInfo.instance = checked_cast<Instance>(checked_cast<Adapter>(mAdapter)->APIGetInstance())->GetHandle();
		err = vmaCreateAllocator(&allocatorCreateInfo, &mMemoryAllocator);
		CHECK_VK_RESULT_FALSE(err);

		// create queues
		for (uint32_t i = 0; i < mQueues.size(); ++i)
		{
			if (queueFamlies[i].has_value())
			{
				mQueues[i] = Queue::Create(this, queueFamlies[i].value(), static_cast<QueueType>(i));
			}
		}

		mVkDeviceInfo.features = deviceFeatures;
		vkGetPhysicalDeviceProperties(adapter->GetHandle(), &mVkDeviceInfo.properties);

		LoadExtFunctions();

		return true;
	}

	void Device::LoadExtFunctions()
	{
		Instance* instance = checked_cast<Instance>(checked_cast<Adapter>(APIGetAdapter())->APIGetInstance());
		if (instance->IsDebugLayerEnabled())
		{
			Fn.vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance->GetHandle(), "vkCmdBeginDebugUtilsLabelEXT"));
			Fn.vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance->GetHandle(), "vkCmdBeginDebugUtilsLabelEXT"));
			Fn.vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance->GetHandle(), "vkSetDebugUtilsObjectNameEXT"));
		}
	}

	VkDevice Device::GetHandle() const
	{
		return mHandle;
	}

	VkPhysicalDevice Device::GetVkPhysicalDevice() const
	{
		return checked_cast<Adapter>(mAdapter)->GetHandle();
	}

	const VkDeviceInfo& Device::GetVkDeviceInfo() const
	{
		return mVkDeviceInfo;
	}

	uint32_t Device::GetOptimalBytesPerRowAlignment() const
	{
		return mVkDeviceInfo.properties.limits.optimalBufferCopyRowPitchAlignment;
	}

	uint32_t Device::GetOptimalBufferToTextureCopyOffsetAlignment() const
	{
		return mVkDeviceInfo.properties.limits.optimalBufferCopyOffsetAlignment;
	}

	Device::~Device()
	{
		// We failed during initialization so early that we don't even have a VkDevice. There is
		// nothing to do.
		if (mHandle == VK_NULL_HANDLE)
		{
			return;
		}

		vkDeviceWaitIdle(mHandle);

		for (auto& queue : mQueues)
		{
			if (queue)
			{
				queue->AssumeCommandsComplete();
				ASSERT(queue->GetCompletedSerial() == queue->GetLastSubmittedSerial());
				queue->Tick();
				queue = nullptr;
			}
		}
		DestroyObjects();

		vmaDestroyAllocator(mMemoryAllocator);

		vkDestroyDevice(mHandle, nullptr);
	}

	VmaAllocator Device::GetMemoryAllocator() const
	{
		return mMemoryAllocator;
	}

	Ref<SwapChainBase> Device::CreateSwapChain(SurfaceBase* surface, SwapChainBase* previous, const SurfaceConfiguration& config)
	{
		return SwapChain::Create(this, surface, previous, config);
	}

	Ref<PipelineLayoutBase> Device::CreatePipelineLayout(const PipelineLayoutDesc& desc)
	{
		return PipelineLayout::Create(this, desc);
	}

	Ref<RenderPipelineBase> Device::CreateRenderPipeline(const RenderPipelineDesc& desc)
	{
		return RenderPipeline::Create(this, desc);
	}

	Ref<ComputePipelineBase> Device::CreateComputePipeline(const ComputePipelineDesc& desc)
	{
		return ComputePipeline::Create(this, desc);
	}

	Ref<BindSetLayoutBase> Device::CreateBindSetLayout(const BindSetLayoutDesc& desc)
	{
		return BindSetLayout::Create(this, desc);
	}

	Ref<BindSetBase> Device::CreateBindSet(const BindSetDesc& desc)
	{
		return BindSet::Create(this, desc);
	}

	Ref<TextureBase> Device::CreateTexture(const TextureDesc& desc)
	{
		return Texture::Create(this, desc);
	}

	
	Ref<BufferBase> Device::CreateBuffer(const BufferDesc& desc)
	{
		return Buffer::Create(this, desc);
	}

	Ref<ShaderModuleBase> Device::CreateShader(const ShaderModuleDesc& desc)
	{
		return ShaderModule::Create(this, desc);
	}

	Ref<SamplerBase> Device::CreateSampler(const SamplerDesc& desc)
	{
		return Sampler::Create(this, desc);
	}

	Ref<CommandListBase> Device::CreateCommandList(CommandEncoder* encoder)
	{
		return CommandList::Create(this, encoder);
	}

}