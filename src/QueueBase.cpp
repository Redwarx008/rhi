#include "QueueBase.h"
#include "DeviceBase.h"
#include "BufferBase.h"
#include "TextureBase.h"
#include "common/Utils.h"
#include "common/Error.h"

namespace rhi::impl
{
	QueueBase::QueueBase(DeviceBase* device, QueueType type) :
		mDevice(device),
		mQueueType(type),
		mUploadAllocator(std::make_unique<UploadAllocator>(device))
	{

	}

	QueueBase::~QueueBase() {}

	QueueType QueueBase::GetType() const
	{
		return mQueueType;
	}

	uint64_t QueueBase::GetLastSubmittedSerial() const
	{
		return mLastSubmittedSerial.load(std::memory_order_acquire);
	}

	uint64_t QueueBase::GetPendingSubmitSerial() const
	{
		return mLastSubmittedSerial.load(std::memory_order_acquire) + 1;
	}

	uint64_t QueueBase::GetCompletedSerial() const
	{
		return mCompletedSerial.load(std::memory_order_acquire);
	}

	void QueueBase::AssumeCommandsComplete()
	{
		uint64_t prev = mLastSubmittedSerial.fetch_add(1ull, std::memory_order_release);
		mCompletedSerial.store(prev + 1u, std::memory_order_release);
	}

	void QueueBase::TrackTask(std::unique_ptr<CallbackTask> callback, uint64_t serial)
	{
		if (serial < GetCompletedSerial())
		{
			mDevice->GetCallbackTaskManager().AddCallbackTask(std::move(callback));
		}
		else
		{
			mTasksInFlight.Push(serial, std::move(callback));
		}
	}

	void QueueBase::Tick()
	{
		uint64_t completedSerial = GetCompletedSerial();
		// Tasks' serials have passed. Move them to the callback task manager. They
		// are ready to be called.
		for (auto& task : mTasksInFlight.IterateUpTo(completedSerial))
		{
			mDevice->GetCallbackTaskManager().AddCallbackTask(std::move(task));
		}
		mTasksInFlight.ClearUpTo(completedSerial);

		mUploadAllocator->Deallocate(completedSerial);

		TickImpl(completedSerial);
	}

	uint64_t QueueBase::APISubmit(CommandListBase* const* commands, uint32_t commandListCount, ResourceTransfer const* transfers, uint32_t transferCount)
	{
		return SubmitImpl(commands, commandListCount, transfers, transferCount);
		//Tick();
	}

	void QueueBase::CheckAndUpdateCompletedSerial()
	{
		uint64_t completedSerial = QueryCompletedSerial();
		uint64_t current = mCompletedSerial.load(std::memory_order_acquire);
		while (uint64_t(completedSerial) > current &&
			!mCompletedSerial.compare_exchange_weak(current, uint64_t(completedSerial),
				std::memory_order_acq_rel)) {}

	}

	bool QueueBase::HasScheduledCommands() const
	{
		return mLastSubmittedSerial.load(std::memory_order_acquire) >
			mCompletedSerial.load(std::memory_order_acquire);
	}

	bool QueueBase::NeedsTick() const
	{
		return HasScheduledCommands() || !mTasksInFlight.Empty();
	}

	void QueueBase::CopyFromStagingToBuffer(BufferBase* src, uint64_t srcOffset, BufferBase* dst, uint64_t dstOffset, uint64_t size)
	{
		CopyFromStagingToBufferImpl(src, srcOffset, dst, dstOffset, size);
		MarkRecordingContextIsUsed();
	}

	void QueueBase::APIWriteBuffer(BufferBase* buffer, const void* data, uint64_t dataSize, uint64_t offset)
	{
		//ASSERT(HasFlag(buffer->APIGetUsage(), BufferUsage::CopyDst));
		INVALID_IF(dataSize > buffer->APIGetSize() - offset || offset > buffer->APIGetSize(),
			"Write range (bufferOffset: %u, size: %u) does not fit in Buffer(%s) size (%u).",
			offset, dataSize, buffer->GetName(), buffer->APIGetSize());

		if (HasFlag(buffer->APIGetUsage(), BufferUsage::MapWrite | BufferUsage::MapRead))
		{
			ASSERT(buffer->APIGetMappedPointer() != nullptr);
			uint8_t* src = static_cast<uint8_t*>(buffer->APIGetMappedPointer()) + offset;
			memcpy(src, data, dataSize);
		}
		else
		{
			// For device visible buffer, we use stage buffer to upload.
			UploadAllocation allocation = mUploadAllocator->Allocate(dataSize, GetPendingSubmitSerial(), 4);
			ASSERT(allocation.mappedAddress != nullptr);
			memcpy(allocation.mappedAddress, data, dataSize);

			CopyFromStagingToBuffer(allocation.buffer, allocation.offset, buffer, offset, dataSize);
		}
	}

	uint64_t ComputeRequiredBytesInCopy(TextureFormat format, const Extent3D& size, uint32_t alignedBytesPerRow, uint32_t rowsPerImage)
	{
		const FormatInfo& formatInfo = GetFormatInfo(format);
		uint32_t widthInBlocks = size.width / formatInfo.blockSize;
		uint32_t heightInBlocks = size.height / formatInfo.blockSize;
		// The number of bytes in the last row may be different because there is alignment padding in the memory layout
		// and the padding bytes are not used in the last row.
		uint64_t bytesInLastRow = static_cast<uint64_t>(widthInBlocks) * static_cast<uint64_t>(formatInfo.bytesPerBlock);

		if (size.depthOrArrayLayers == 0)
		{
			return 0;
		}

		uint64_t bytesPerImage = static_cast<uint64_t>(alignedBytesPerRow) * static_cast<uint64_t>(rowsPerImage);
		uint64_t requiredBytesInCopy = bytesPerImage * (uint64_t(size.depthOrArrayLayers) - 1);
		if (heightInBlocks > 0)
		{
			uint64_t bytesInLastImage = static_cast<uint64_t>(alignedBytesPerRow) * (heightInBlocks - 1) + bytesInLastRow;
			requiredBytesInCopy += bytesInLastImage;
		}

		return requiredBytesInCopy;
	}

	void CopyTextureData(uint8_t* dstPointer,
		const uint8_t* srcPointer,
		uint32_t depth,
		uint32_t rowsPerImage,
		uint64_t additionalStridePerImage,
		uint32_t actualBytesPerRow,
		uint32_t dstBytesPerRow,
		uint32_t srcBytesPerRow)
	{
		bool copyWholeLayer = actualBytesPerRow == dstBytesPerRow && dstBytesPerRow == srcBytesPerRow;
		bool copyWholeData = copyWholeLayer && additionalStridePerImage == 0;

		if (!copyWholeLayer) 
		{  // copy row by row
			for (uint32_t d = 0; d < depth; ++d) {
				for (uint32_t h = 0; h < rowsPerImage; ++h) 
				{
					memcpy(dstPointer, srcPointer, actualBytesPerRow);
					dstPointer += dstBytesPerRow;
					srcPointer += srcBytesPerRow;
				}
				srcPointer += additionalStridePerImage;
			}
		}
		else 
		{
			uint64_t layerSize = uint64_t(rowsPerImage) * actualBytesPerRow;
			if (!copyWholeData)
			{  // copy layer by layer
				for (uint32_t d = 0; d < depth; ++d)
				{
					memcpy(dstPointer, srcPointer, layerSize);
					dstPointer += layerSize;
					srcPointer += layerSize + additionalStridePerImage;
				}
			}
			else 
			{  // do a single copy
				memcpy(dstPointer, srcPointer, layerSize * depth);
			}
		}
	}

	void QueueBase::APIWriteTexture(const TextureSlice& dstTexture, const void* data, size_t dataSize, const TextureDataLayout& dataLayout)
	{
		ASSERT(HasFlag(dstTexture.texture->APIGetUsage(), TextureUsage::CopyDst));
		ASSERT(dataLayout.bytesPerRow != 0 && dataLayout.rowsPerImage != 0);
		INVALID_IF(dataLayout.offset > dataSize, "Data offset (%u) is greater than the data size (%u).", dataLayout.offset, dataSize);
		TextureFormat format = dstTexture.texture->APIGetFormat();
		ASSERT(dstTexture.size.width % GetFormatInfo(format).blockSize == 0);
		ASSERT(dstTexture.size.height % GetFormatInfo(format).blockSize == 0);

		uint32_t alignedBytesPerRow = dstTexture.size.width / GetFormatInfo(format).blockSize * GetFormatInfo(format).bytesPerBlock;
		uint32_t alignedRowsPerImage = dstTexture.size.height / GetFormatInfo(format).blockSize;

		uint64_t optimalOffsetAlignment = mDevice->GetOptimalBufferToTextureCopyOffsetAlignment();
		// We need the offset to be aligned to both optimalOffsetAlignment and blockByteSize,
		// since both of them are powers of two, we only need to align to the max value.
		uint64_t offsetAlignment = std::max(optimalOffsetAlignment, uint64_t(GetFormatInfo(format).bytesPerBlock));
		uint32_t optimalBytesPerRowAlignment = mDevice->GetOptimalBytesPerRowAlignment();
		uint32_t optimallyAlignedBytesPerRow = AlignUp(alignedBytesPerRow, optimalBytesPerRowAlignment);

		uint64_t requiredBytesInCopy = ComputeRequiredBytesInCopy(dstTexture.texture->APIGetFormat(), dstTexture.size, optimallyAlignedBytesPerRow, alignedRowsPerImage);

		UploadAllocation allocation = mUploadAllocator->Allocate(requiredBytesInCopy, GetPendingSubmitSerial(), offsetAlignment);
		ASSERT(allocation.mappedAddress != nullptr);

		const uint8_t* copySrc = static_cast<const uint8_t*>(data) + dataLayout.offset;
		uint8_t* copyDst = static_cast<uint8_t*>(allocation.mappedAddress);

		ASSERT(dataLayout.rowsPerImage >= alignedRowsPerImage);

		uint64_t additionalStridePerImage = dataLayout.bytesPerRow * (dataLayout.rowsPerImage - alignedRowsPerImage);

		CopyTextureData(copyDst, copySrc, dstTexture.size.depthOrArrayLayers, alignedRowsPerImage,
			additionalStridePerImage, alignedBytesPerRow, optimallyAlignedBytesPerRow,
			dataLayout.bytesPerRow);

		TextureDataLayout alignedDataLayout{};
		alignedDataLayout.offset = allocation.offset;
		alignedDataLayout.bytesPerRow = optimallyAlignedBytesPerRow;
		alignedDataLayout.rowsPerImage = alignedRowsPerImage;

		CopyFromStagingToTextureImpl(allocation.buffer, dstTexture, alignedDataLayout);
	}

	void QueueBase::APIWaitFor(QueueBase* queue, uint64_t submitSerial)
	{
		WaitForImpl(queue, submitSerial);
	}
}