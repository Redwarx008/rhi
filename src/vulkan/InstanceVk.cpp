#include "InstanceVk.h"
#include "AdapterVk.h"
#include "SurfaceVk.h"
#include "ErrorsVk.h"

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

namespace rhi::impl::vulkan
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		LoggingSeverity serverity = LoggingSeverity::Info;

		std::string prefix;
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		{
#if defined(_WIN32)
			prefix = "\033[32m" + prefix + "\033[0m";
#endif
			prefix = "VERBOSE: ";
			serverity = LoggingSeverity::Verbose;
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		{
			prefix = "INFO: ";
#if defined(_WIN32)
			prefix = "\033[36m" + prefix + "\033[0m";
#endif
			serverity = LoggingSeverity::Info;
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			prefix = "WARNING: ";
#if defined(_WIN32)
			prefix = "\033[33m" + prefix + "\033[0m";
#endif
			serverity = LoggingSeverity::Warning;
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			prefix = "ERROR: ";
#if defined(_WIN32)
			prefix = "\033[31m" + prefix + "\033[0m";
#endif
			serverity = LoggingSeverity::Error;
		}

		std::stringstream debugMessage;

		if (!gDebugMessageCallback)
		{
			debugMessage << prefix;
		}

		debugMessage << pCallbackData->pMessage;

		if (gDebugMessageCallback)
		{
			gDebugMessageCallback(serverity, debugMessage.str().c_str(), gDebugMessageCallbackUserData);
		}
		else
		{
			std::cerr << debugMessage.str().c_str() << std::endl;
		}
		// The return value of this callback controls whether the Vulkan call that caused the validation message will be aborted or not
		// We return VK_FALSE as we DON'T want Vulkan calls that cause a validation message to abort
		// If you instead want to have calls abort, pass in VK_TRUE and the function will return VK_ERROR_VALIDATION_FAILED_EXT
		return VK_FALSE;
	}

	Ref<Instance> Instance::Create(const InstanceDesc& desc)
	{
		gDebugMessageCallback = desc.loggingCallback;
		gDebugMessageCallbackUserData = desc.loggingCallbackUserData;

		Ref<Instance> instance = AcquireRef(new Instance());
		if (!instance->Initialize(desc))
		{
			return nullptr;
		}
		return instance;
	}

	bool Instance::Initialize(const InstanceDesc& desc)
	{
		std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

#if defined(VK_USE_PLATFORM_WIN32_KHR)
		instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
		instanceExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
		instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
		instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
		instanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif
		instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

		std::vector<std::string> supportedInstanceExtensions;
		// Get extensions supported by the instance and store for later use
		uint32_t extCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateInstanceExtensionProperties(nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				for (VkExtensionProperties& extension : extensions)
				{
					supportedInstanceExtensions.push_back(extension.extensionName);
				}
			}
		}

		for (auto extensionName : instanceExtensions)
		{
			if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), extensionName) == supportedInstanceExtensions.end())
			{
				LOG_ERROR(extensionName, " is not supported.");
				return false;
			}
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = nullptr;
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;

		if (desc.enableDebugLayer)
		{
			VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{};
			debugUtilsMessengerCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugUtilsMessengerCI.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugUtilsMessengerCI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
			debugUtilsMessengerCI.pfnUserCallback = DebugMessageCallback;
			debugUtilsMessengerCI.pUserData = nullptr;
			instanceCreateInfo.pNext = &debugUtilsMessengerCI;

			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			// The VK_LAYER_KHRONOS_validation contains all current validation functionality.
			// Note that on Android this layer requires at least NDK r20
			const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
			// Check if this layer is available at instance level
			uint32_t instanceLayerCount;
			vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
			std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
			vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());
			bool validationLayerPresent = false;
			for (VkLayerProperties& layer : instanceLayerProperties)
			{
				if (strcmp(layer.layerName, validationLayerName) == 0) {
					validationLayerPresent = true;
					break;
				}
			}
			if (validationLayerPresent)
			{

				const VkBool32 setting_validate_core = VK_TRUE;
				const VkBool32 setting_validate_sync = VK_TRUE;
				const VkBool32 setting_thread_safety = VK_TRUE;
				const char* setting_debug_action[] = { "VK_DBG_LAYER_ACTION_LOG_MSG" };
				const char* setting_report_flags[] = { "info", "warn", "perf", "error", "debug" };
				const VkBool32 setting_enable_message_limit = VK_TRUE;
				const int32_t setting_duplicate_message_limit = 10;

				const VkLayerSettingEXT settings[] = {
					{validationLayerName, "validate_core", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &setting_validate_core},
					{validationLayerName, "validate_sync", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &setting_validate_sync},
					{validationLayerName, "thread_safety", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &setting_thread_safety},
					{validationLayerName, "debug_action", VK_LAYER_SETTING_TYPE_STRING_EXT, 1, setting_debug_action},
					{validationLayerName, "report_flags", VK_LAYER_SETTING_TYPE_STRING_EXT, static_cast<uint32_t>(std::size(setting_report_flags)), setting_report_flags},
					{validationLayerName, "enable_message_limit", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1,&setting_enable_message_limit },
					{validationLayerName, "duplicate_message_limit", VK_LAYER_SETTING_TYPE_INT32_EXT, 1, &setting_duplicate_message_limit} };

				const VkLayerSettingsCreateInfoEXT layerSettingsCreateInfo =
				{
					VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT, nullptr,
					static_cast<uint32_t>(std::size(settings)), settings
				};
				debugUtilsMessengerCI.pNext = &layerSettingsCreateInfo;

				instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
				instanceCreateInfo.enabledLayerCount = 1;
			}
			else {
				LOG_ERROR("Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled");
			}
		}

		instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &mHandle);
		CHECK_VK_RESULT_FALSE(result, "Failed to create a Vulkan instance");
		
		if (!RegisterDebugUtils())
		{
			return false;
		}
		return true;
	}

	bool Instance::RegisterDebugUtils()
	{
		auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mHandle, "vkCreateDebugUtilsMessengerEXT"));

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{};
		debugUtilsMessengerCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugUtilsMessengerCI.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugUtilsMessengerCI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		debugUtilsMessengerCI.pfnUserCallback = DebugMessageCallback;
		debugUtilsMessengerCI.pUserData = nullptr;
		VkResult result = vkCreateDebugUtilsMessengerEXT(mHandle, &debugUtilsMessengerCI, nullptr, &mDebugUtilsMessenger);
		CHECK_VK_RESULT_FALSE(result, "Failed to create vulkan debugUtilsMessenger");
		mDebugLayerEnabled = true;
		return true;
	}

	void Instance::APIEnumerateAdapters(AdapterBase** const adapters, uint32_t* adapterCount)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(mHandle, &deviceCount, nullptr);

		*adapterCount = deviceCount;
		if (adapters == nullptr)
		{
			return;
		}

		std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
		vkEnumeratePhysicalDevices(mHandle, &deviceCount, physicalDevices.data());

		for (int i = 0; i < deviceCount; ++i)
		{
			adapters[i] = new Adapter(this, physicalDevices[i]);
		}
	}

	SurfaceBase* Instance::APICreateSurface(void* hwnd, void* hinstance)
	{
		Ref<SurfaceBase> surface = Surface::CreateFromWindowsHWND(this, hwnd, hinstance);
		return surface.Detach();
	}

	VkInstance Instance::GetHandle() const
	{
		return mHandle;
	}

	Instance::~Instance()
	{
		if (mDebugUtilsMessenger)
		{
			auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(mHandle, "vkDestroyDebugUtilsMessengerEXT"));
			vkDestroyDebugUtilsMessengerEXT(mHandle, mDebugUtilsMessenger, nullptr);
			mDebugUtilsMessenger = VK_NULL_HANDLE;
		}
		if (mHandle)
		{
			vkDestroyInstance(mHandle, nullptr);
			mHandle = VK_NULL_HANDLE;
		}
	}

}