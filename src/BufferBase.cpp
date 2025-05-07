#pragma once

#include "BufferBase.h"
#include "CallbackTaskManager.h"
#include "QueueBase.h"
#include "common/Constants.h"
#include "common/Ref.hpp"

#include <memory>

namespace rhi::impl
{
	class MapAsyncCallbackTask : public CallbackTask
	{
	public:
		MapAsyncCallbackTask(Ref<BufferBase> buffer, 
			BufferMapCallback callback, void* userData) :
			mBuffer(buffer),
			mMapCallback(callback),
			mMapUserdata(userData)
		{ }
	private:
		void FinishImpl() override 
		{
			mMapCallback(BufferMapAsyncStatus::Success, mBuffer->APIGetMappedPointer(), mMapUserdata);
			mBuffer->OnMapCallbackCompleted(BufferMapAsyncStatus::Success);
		}
		void HandleDeviceLossImpl() override
		{
			mMapCallback(BufferMapAsyncStatus::DeviceLost, mBuffer->APIGetMappedPointer(), mMapUserdata);
			mBuffer->OnMapCallbackCompleted(BufferMapAsyncStatus::DeviceLost);
		}
		void HandleShutDownImpl() override
		{
			mMapCallback(BufferMapAsyncStatus::DestroyedBeforeCallback, mBuffer->APIGetMappedPointer(), mMapUserdata);
			mBuffer->OnMapCallbackCompleted(BufferMapAsyncStatus::DestroyedBeforeCallback);
		}
		Ref<BufferBase> mBuffer;
		BufferMapCallback mMapCallback;
		void* mMapUserdata;
	};

	BufferUsage AddInternalUsage(BufferUsage usage)
	{
		if ((usage & BufferUsage::Storage) != 0)
		{
			usage |= cReadOnlyStorageBuffer;
		}
		return usage;
	}

	BufferBase::BufferBase(DeviceBase* device, const BufferDesc& desc):
		mSize(desc.size),
		mUsage(desc.usage),
		mInternalUsage(AddInternalUsage(desc.usage)),
		mShareMode(desc.shareMode),
		ResourceBase(device, desc.name)
	{

	}

	BufferBase::~BufferBase() {}

	void BufferBase::Initialize()
	{
		ResourceBase::Initialize();
	}

	BufferUsage BufferBase::APIGetUsage() const
	{
		return mUsage;
	}

	uint64_t BufferBase::APIGetSize() const
	{
		return mSize;
	}

	void BufferBase::APIDestroy()
	{
		Destroy();
	}

	ResourceType BufferBase::GetType() const
	{
		return ResourceType::Buffer;
	}

	BufferBase::UsageTrackInQueue& BufferBase::GetTrackedUsagae(QueueType queueType)
	{
		static_assert(static_cast<uint32_t>(QueueType::Graphics) == 0);
		static_assert(static_cast<uint32_t>(QueueType::Compute) == 1);
		static_assert(static_cast<uint32_t>(QueueType::Transfer) == 2);

		return mUsageTrackInQueues[static_cast<uint32_t>(queueType)];
	}

	void BufferBase::OnMapAsync(QueueBase* queue, MapMode usage, BufferMapCallback callback, void* userData)
	{
		if (mState == State::PendingMap)
		{
			return;
		}

		mState = State::PendingMap;

		MapAsyncImpl(queue, usage);

		uint64_t lastUsageSerial = GetTrackedUsagae(queue->GetType()).lastUsageSerial;
		std::unique_ptr<MapAsyncCallbackTask> task 
			= std::make_unique<MapAsyncCallbackTask>(this, callback, userData);
		queue->TrackTask(std::move(task), lastUsageSerial);
	}

	void BufferBase::OnMapCallbackCompleted(BufferMapAsyncStatus status)
	{
		if (mState == State::PendingMap && status == BufferMapAsyncStatus::Success)
		{
			mState = State::Mapped;
		}
	}
}