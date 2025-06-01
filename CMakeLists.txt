cmake_minimum_required(VERSION 3.13)

project(rhi VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

OPTION(USE_D2D_WSI "Build the project using Direct to Display swapchain" OFF)
OPTION(USE_DIRECTFB_WSI "Build the project using DirectFB swapchain" OFF)
OPTION(USE_WAYLAND_WSI "Build the project using Wayland swapchain" OFF)
OPTION(USE_HEADLESS "Build the project using headless extension swapchain" OFF)

IF(UNIX AND NOT APPLE)
	set(LINUX TRUE)
ENDIF()

IF(WIN32)
	IF (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/libs/vulkan)
		IF (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			MESSAGE("Using bundled Vulkan library version")
		ENDIF()
	ENDIF()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
ELSEIF(LINUX)
	IF (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" "${CMAKE_SOURCE_DIR}/libs/vulkan" REQUIRED)
		IF (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			MESSAGE("Using bundled Vulkan library version")
		ENDIF()
	ENDIF()
	find_package(Threads REQUIRED)
	IF(USE_D2D_WSI)
		MESSAGE("Using direct to display extension...")
		add_definitions(-D_DIRECT2DISPLAY)
	ELSEIF(USE_DIRECTFB_WSI)
		find_package(DirectFB REQUIRED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_DIRECTFB_EXT")
		include_directories(${DIRECTFB_INCLUDE_DIR})
	ELSEIF(USE_WAYLAND_WSI)
		find_program(PKG_CONFIG pkg-config)
		if (NOT PKG_CONFIG)
			message(FATAL_ERROR "pkg-config binary not found")
		endif ()
		find_package(Wayland REQUIRED)
		if (NOT WAYLAND_FOUND)
			message(FATAL_ERROR "Wayland development package not found")
		endif ()
		pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
		if (NOT WAYLAND_PROTOCOLS_FOUND)
			message(FATAL_ERROR "Wayland protocols package not found")
		endif ()
		find_program(WAYLAND_SCANNER wayland-scanner)
		if (NOT WAYLAND_SCANNER)
			message(FATAL_ERROR "wayland-scanner binary not found")
		endif ()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WAYLAND_KHR")
		include_directories(${WAYLAND_INCLUDE_DIR})
		pkg_get_variable(protocol_dir wayland-protocols pkgdatadir)
		execute_process(COMMAND ${WAYLAND_SCANNER} client-header ${protocol_dir}/stable/xdg-shell/xdg-shell.xml ${CMAKE_BINARY_DIR}/xdg-shell-client-protocol.h
				COMMAND ${WAYLAND_SCANNER} private-code ${protocol_dir}/stable/xdg-shell/xdg-shell.xml ${CMAKE_BINARY_DIR}/xdg-shell-protocol.c)
		include_directories(${CMAKE_BINARY_DIR})
	ELSEIF(USE_HEADLESS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_HEADLESS_EXT")
	ELSE(USE_D2D_WSI)
		find_package(XCB REQUIRED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR")
	ENDIF(USE_D2D_WSI)
	# Todo : android?
ENDIF(WIN32)

IF (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
ELSE()
	message(STATUS ${Vulkan_LIBRARY})
ENDIF()

add_subdirectory(third_party)

set(interface_rhi 
	"include/rhi/rhi.h"
	"src/RHIStruct.h" "include/rhi/rhi_cpp.h" )

set(src_common
	"src/common/Error.h"
	"src/common/Utils.h"
	"src/common/NoCopyable.h"
	"src/common/Ref.hpp"
	"src/common/WeakRef.hpp"
	"src/common/LinkedList.h"
	"src/common/ObjectContentHasher.h" 
	"src/common/RefCounted.h"
	"src/common/SerialQueue.hpp" 
	"src/common/SerialMap.hpp"
	"src/common/EnumFlagIterator.hpp"
	"src/common/MutexProtected.hpp"
	"src/common/Log.h" 
	"src/common/Log.cpp" 
	"src/common/Constants.h"
	"src/common/BitSetUtils.h")

set (src
	"src/RHI.cpp"
	"src/UploadAllocator.h"
	"src/UploadAllocator.cpp"
	"src/Commands.h"
	"src/CommandEncoder.h"
	"src/CommandEncoder.cpp"
	"src/CommandAllocator.h"
	"src/CommandAllocator.cpp"
	"src/ResourceBase.h"
	"src/ResourceBase.cpp"
	"src/DeviceBase.h" 
	"src/DeviceBase.cpp"
	"src/SyncScopeUsageTracker.h"
	"src/SubresourceStorage.hpp"
	"src/FeatureSet.h" 
	"src/Subresource.h" 
	"src/SubResource.cpp"
	"src/SyncScopeUsageTracker.cpp"
	"src/BufferBase.h"
	"src/BufferBase.cpp" 
	"src/TextureBase.h"
	"src/TextureBase.cpp" 
	"src/InstanceBase.h" 
	"src/InstanceBase.cpp" 
	"src/AdapterBase.h" 
	"src/AdapterBase.cpp"
	"src/BindSetBase.h" 
	 "src/BindSetLayoutBase.h"
	 "src/BindSetLayoutBase.cpp"
	 "src/BindSetBase.cpp" 
	 "src/SamplerBase.h" 
	 "src/SamplerBase.cpp" 
	 "src/ShaderModuleBase.h" 
	 "src/ShaderModuleBase.cpp" 
	 "src/PipelineLayoutBase.h" 
	 "src/PipelineLayoutBase.cpp" 
	 "src/PipelineBase.h"
	 "src/PipelineBase.cpp"
	 "src/RenderPipelineBase.h"
	 "src/RenderPipelineBase.cpp"
	 "src/PerShaderStage.hpp" 
	 "src/common/Cached.hpp"
	 
	 "src/PassResourceUsage.h"
	 "src/EncodingContext.h" 
	 "src/EncodingContext.cpp"
	 "src/RenderPassEncoder.h"
	 "src/RenderPassEncoder.cpp" 
	 "src/CommandListBase.h" 
	 "src/CommandListBase.cpp"
	 "src/PassEncoder.h" 
	 "src/PassEncoder.cpp" 
	 "src/QueueBase.h" 
	 "src/QueueBase.cpp" 
	 "src/CallbackTaskManager.h" 
	 "src/CallbackTaskManager.cpp" 
	 "src/ComputePipelineBase.h"
	 "src/ComputePipelineBase.cpp" 
	 "src/SwapChainBase.h"
	 "src/SwapChainBase.cpp"
	 "src/SurfaceBase.h"
	 "src/SurfaceBase.cpp"
	 "src/ComputePassEncoder.h" 
	 "src/ComputePassEncoder.cpp" 
	 "src/PassResourceUsage.cpp" 
	 "src/Commands.cpp" 
	 "src/PipelineCacheBase.h" 
	 "src/PipelineCacheBase.cpp" )

set(src_vk
	"src/vulkan/VMA.cpp"
	"src/vulkan/DeviceVk.h"
	"src/vulkan/DeviceVk.cpp"
	"src/vulkan/SwapChainVk.h"
	"src/vulkan/SwapChainVk.cpp"
	"src/vulkan/ErrorsVk.h"
	"src/vulkan/CommandListVk.h"
	"src/vulkan/CommandListVk.cpp"
	"src/vulkan/QueueVk.h"
	"src/vulkan/QueueVk.cpp"
	"src/vulkan/BufferVk.h"
	"src/vulkan/BufferVk.cpp"
	"src/vulkan/TextureVk.h" 
	"src/vulkan/InstanceVk.h" 
	"src/vulkan/InstanceVk.cpp" 
	"src/vulkan/AdapterVk.h"
	"src/vulkan/AdapterVk.cpp" 
	"src/vulkan/CommandRecordContextVk.h"
	"src/vulkan/CommandRecordContextVk.cpp" 
	"src/vulkan/TextureVk.cpp"
	"src/vulkan/VkResourceDeleter.h"
	"src/vulkan/VkResourceDeleter.cpp" 
	"src/vulkan/ResourceToDelete.h"
	"src/vulkan/VulkanUtils.h"
	"src/vulkan/VulkanUtils.cpp" 
	"src/vulkan/DescriptorSetAllocator.h"
	"src/vulkan/DescriptorSetAllocator.cpp" 
	"src/vulkan/BindSetLayoutVk.h"
	"src/vulkan/BindSetLayoutVk.cpp" 
	"src/vulkan/BindSetVk.h"
	"src/vulkan/BindSetVk.cpp" 
	"src/vulkan/DescriptorSetAllocation.h"
	"src/vulkan/SamplerVk.h" 
	"src/vulkan/SamplerVk.cpp" 
	"src/vulkan/ShaderModuleVk.h"
	"src/vulkan/ShaderModuleVk.cpp" 
	"src/vulkan/PipelineLayoutVk.h"
	"src/vulkan/PipelineLayoutVk.cpp" 
	"src/vulkan/RenderPipelineVk.h" 
	"src/vulkan/RenderPipelineVk.cpp" 
	"src/vulkan/ComputePipelineVk.h" 
	"src/vulkan/ComputePipelineVk.cpp" 
	"src/vulkan/RefCountedHandle.h"  
	"src/vulkan/SurfaceVk.h"
	"src/vulkan/SurfaceVk.cpp" 
	"src/vulkan/VulkanEXTFunctions.h" 
	"src/vulkan/PipelineCacheVk.h"
	"src/vulkan/PipelineCacheVk.cpp")

add_library(rhi "")

target_sources(rhi	PRIVATE
				${interface_rhi}
				${src}
				${src_common}
				${src_vk} )

target_include_directories(rhi PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> )

set_target_properties(rhi PROPERTIES FOLDER "RHI")

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(rhi PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(rhi PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

IF(WIN32)
	# Nothing here (yet)
ELSE(WIN32)
	target_link_libraries(rhi ${XCB_LIBRARIES} ${Vulkan_LIBRARY} ${Vulkan_LIBRARY} ${DIRECTFB_LIBRARIES} ${WAYLAND_CLIENT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
ENDIF(WIN32)

target_link_libraries(rhi Vulkan::Vulkan vma absl::inlined_vector absl::flat_hash_map absl::strings spirv-reflect)