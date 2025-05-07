#pragma once

#include "RHIStruct.h"
#include "ResourceBase.h"

#include <array>

namespace rhi::impl
{
	class DeviceBase;

	class BufferBase : public ResourceBase
	{
	public:
		enum class State : uint8_t
		{
			Unmapped,
			PendingMap,
			Mapped,
			MappedAtCreation,
			Destroyed
		};
		// api 
		BufferUsage APIGetUsage() const;
		uint64_t APIGetSize() const;
		virtual void* APIGetMappedPointer() = 0;
		void APIDestroy();
		// internal methods
		ResourceType GetType() const override;
		void OnMapAsync(QueueBase* queue, MapMode usage, BufferMapCallback callback, void* userData);
		void OnMapCallbackCompleted(BufferMapAsyncStatus status);
	protected:
		explicit BufferBase(DeviceBase* device, const BufferDesc& desc);
		~BufferBase();
		void Initialize();
		struct UsageTrackInQueue;
		UsageTrackInQueue& GetTrackedUsagae(QueueType queueType);
		virtual void MapAsyncImpl(QueueBase* queue, MapMode mode) = 0;
		const BufferUsage mUsage = BufferUsage::None;
		const BufferUsage mInternalUsage = BufferUsage::None;
		const ShareMode mShareMode;
		const uint64_t mSize = 0;

		State mState = State::Unmapped;

		QueueType mLastUsedQueue = QueueType::Undefined;

		struct UsageTrackInQueue
		{
			uint64_t lastUsageSerial = 0;
			// Track which usage was the last to write to the buffer.
			BufferUsage lastWriteUsage = BufferUsage::None;
			ShaderStage lastWriteShaderStage = ShaderStage::None;

			// Track which usages have read the buffer since the last write.
			BufferUsage readUsage = BufferUsage::None;
			ShaderStage readShaderStages = ShaderStage::None;
		};
		std::array<UsageTrackInQueue, 3> mUsageTrackInQueues;
	};
}