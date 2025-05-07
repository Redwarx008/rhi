#pragma once

#include "RHIStruct.h"
#include "SubresourceStorage.hpp"
#include "Subresource.h"

namespace rhi::impl
{
	class BufferBase;
	class TextureBase;
	class TextureViewBase;

	struct BufferSyncInfo
	{
		BufferUsage usage;
		ShaderStage shaderStages;
	};

	struct TextureSyncInfo
	{
		TextureUsage usage;
		ShaderStage shaderStages;
		QueueType queue;
	};
	// SubresourceStorage require the storage type has equality operator.
	bool operator==(const TextureSyncInfo& a, const TextureSyncInfo& b);

	struct SyncScopeResourceUsage
	{
		std::vector<BufferBase*> buffers;
		std::vector<BufferSyncInfo> bufferSyncInfos;

		std::vector<TextureBase*> textures;
		std::vector<SubresourceStorage<TextureSyncInfo>> textureSyncInfos;
	};

	struct CommandListResourceUsage
	{
		std::vector<SyncScopeResourceUsage> renderPassUsages;
		std::vector<SyncScopeResourceUsage> computePassUsages;
	};
}