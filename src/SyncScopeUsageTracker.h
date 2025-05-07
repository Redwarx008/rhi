#pragma once

#include "PassResourceUsage.h"
#include <absl/container/flat_hash_map.h>


namespace rhi::impl
{
	class BindSetBase;

	class SyncScopeUsageTracker
	{
	public:
		SyncScopeUsageTracker() = default;
		SyncScopeUsageTracker(SyncScopeUsageTracker&& other) = default;
		~SyncScopeUsageTracker() = default;

		SyncScopeUsageTracker& operator=(SyncScopeUsageTracker&& other) = default;

		void BufferUsedAs(BufferBase* buffer, 
			BufferUsage usage, 
			ShaderStage shaderStages = ShaderStage::None);
		void TextureViewUsedAs(TextureViewBase* view, 
			TextureUsage usage,
			ShaderStage shaderStages = ShaderStage::None);
		void TextureRangeUsedAs(TextureBase* texture, 
			const SubresourceRange& range,
			TextureUsage usage, 
			ShaderStage shaderStages = ShaderStage::None);
		void AddBindSet(BindSetBase* set);
		SyncScopeResourceUsage AcquireSyncScopeUsage();
	private:
		absl::flat_hash_map<BufferBase*, BufferSyncInfo> mBufferSyncInfos;
		absl::flat_hash_map<TextureBase*, SubresourceStorage<TextureSyncInfo>> mTextureSyncInfos;
	};
}