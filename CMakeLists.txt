cmake_minimum_required(VERSION 3.28)

project(rhi VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
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


# Clang 16's module support can be broken with extensions enabled
if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "16.0" )
	set_target_properties( ${TARGET_NAME} PROPERTIES CXX_EXTENSIONS OFF )
endif()

add_subdirectory(third_party)

set(interface_rhi 
	"include/rhi/rhi.h"
	"src/common/RHIStruct.h" "include/rhi/rhi_cpp.h" )

set(src_utils
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
	"src/common/RHI.cpp"
	"src/common/UploadAllocator.h"
	"src/common/UploadAllocator.cpp"
	"src/common/Commands.h"
	"src/common/CommandEncoder.h"
	"src/common/CommandEncoder.cpp"
	"src/common/CommandAllocator.h"
	"src/common/CommandAllocator.cpp"
	"src/common/ResourceBase.h"
	"src/common/ResourceBase.cpp"
	"src/common/DeviceBase.h" 
	"src/common/DeviceBase.cpp"
	"src/common/SyncScopeUsageTracker.h"
	"src/common/SubresourceStorage.hpp"
	"src/common/FeatureSet.h" 
	"src/common/Subresource.h" 
	"src/common/SubResource.cpp"
	"src/common/SyncScopeUsageTracker.cpp"
	"src/common/BufferBase.h"
	"src/common/BufferBase.cpp" 
	"src/common/TextureBase.h"
	"src/common/TextureBase.cpp" 
	"src/common/InstanceBase.h" 
	"src/common/InstanceBase.cpp" 
	"src/common/AdapterBase.h" 
	"src/common/AdapterBase.cpp"
	"src/common/BindSetBase.h" 
	"src/common/BindSetLayoutBase.h"
	"src/common/BindSetLayoutBase.cpp"
	"src/common/BindSetBase.cpp" 
	"src/common/SamplerBase.h" 
	"src/common/SamplerBase.cpp" 
	"src/common/ShaderModuleBase.h" 
	"src/common/ShaderModuleBase.cpp" 
	"src/common/PipelineLayoutBase.h" 
	"src/common/PipelineLayoutBase.cpp" 
	"src/common/PipelineBase.h"
	"src/common/PipelineBase.cpp"
	"src/common/RenderPipelineBase.h"
	"src/common/RenderPipelineBase.cpp"
	"src/common/PerShaderStage.hpp" 
	"src/common/Cached.hpp"
	"src/common/PassResourceUsage.h"
	"src/common/EncodingContext.h" 
	"src/common/EncodingContext.cpp"
	"src/common/RenderPassEncoder.h"
	"src/common/RenderPassEncoder.cpp" 
	"src/common/CommandListBase.h" 
	"src/common/CommandListBase.cpp"
	"src/common/PassEncoder.h" 
	"src/common/PassEncoder.cpp" 
	"src/common/QueueBase.h" 
	"src/common/QueueBase.cpp" 
	"src/common/CallbackTaskManager.h" 
	"src/common/CallbackTaskManager.cpp" 
	"src/common/ComputePipelineBase.h"
	"src/common/ComputePipelineBase.cpp" 
	"src/common/SwapChainBase.h"
	"src/common/SwapChainBase.cpp"
	"src/common/SurfaceBase.h"
	"src/common/SurfaceBase.cpp"
	"src/common/ComputePassEncoder.h" 
	"src/common/ComputePassEncoder.cpp" 
	"src/common/PassResourceUsage.cpp" 
	"src/common/Commands.cpp" 
	"src/common/PipelineCacheBase.h" 
	"src/common/PipelineCacheBase.cpp" )

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
				${src}
				${src_common}
				${src_vk} )

target_sources(rhi PUBLIC
        FILE_SET
		CXX_MODULES
        BASE_DIRS
        ${PROJECT_SOURCE_DIR}
        FILES
        "src/rhi.ixx"
  )


target_include_directories(rhi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

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