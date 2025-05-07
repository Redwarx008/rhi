#pragma once

#include <cstdint>
#include "../RHIStruct.h"

namespace rhi::impl
{
	static constexpr uint32_t cMaxBindSets = 4u;
	static constexpr uint32_t cMaxBindingsPerBindSet = 1000u;
	static constexpr uint8_t cMaxVertexAttributes = 32u;
	static constexpr uint8_t cMaxVertexBuffers = 32u;
	static constexpr uint32_t cMaxVertexBufferArrayStride = 2048u;
	static constexpr uint8_t cMaxColorAttachments = 8u;
	static constexpr uint8_t cMaxViewports = 8;
	static constexpr uint32_t cNumStages = 5;

	// Per stage maximum limits used to optimized rhi internals.
	static constexpr uint32_t cMaxSampledTexturesPerShaderStage = 16;
	static constexpr uint32_t cMaxSamplersPerShaderStage = 16;
	static constexpr uint32_t cMaxStorageBuffersPerShaderStage = 10;
	static constexpr uint32_t cMaxStorageTexturesPerShaderStage = 8;
	static constexpr uint32_t cMaxUniformBuffersPerShaderStage = 12;
	static constexpr uint32_t cMaxOptimalBindingsPerGroup = 32;

	// Indirect command sizes
	static constexpr uint64_t cDispatchIndirectSize = 3 * sizeof(uint32_t);
	static constexpr uint64_t cDrawIndirectSize = 4 * sizeof(uint32_t);
	static constexpr uint64_t cDrawIndexedIndirectSize = 5 * sizeof(uint32_t);

	constexpr TextureUsage cSwapChainImageAcquireUsage = static_cast<TextureUsage>(1u << 5);
	constexpr TextureUsage cSwapChainImagePresentUsage = static_cast<TextureUsage>(1u << 6);

	constexpr BufferUsage cReadOnlyStorageBuffer = static_cast<BufferUsage>(1u << 10);
	constexpr TextureUsage cReadOnlyStorageTexture = static_cast<TextureUsage>(1u << 7);
}