#include "UploadAllocator.h"
#include "DeviceBase.h"
#include "BufferBase.h"
#include "common/Utils.h"
#include "common/Error.h"

namespace rhi::impl
{
	UploadAllocator::RingBuffer::RingBuffer(uint64_t maxSize) :mMaxBlockSize(maxSize) {}

	UploadAllocator::RingBuffer::~RingBuffer() {};

	uint64_t UploadAllocator::RingBuffer::Allocate(uint64_t allocationSize, uint64_t serial, uint64_t offsetAlignment)
	{
		if (mUsedSize >= mMaxBlockSize)
		{
			return cInvalidOffset;
		}

		const uint64_t remainingSize = (mMaxBlockSize - mUsedSize);
		if (allocationSize > remainingSize)
		{
			return cInvalidOffset;
		}

		uint64_t startOffset = cInvalidOffset;
		uint64_t currentRequestSize = 0;

		// Compute an alignment offset for the buffer if allocating at the end.
		const uint64_t alignmentOffset = AlignUp(mUsedEndOffset, offsetAlignment) - mUsedEndOffset;
		const uint64_t alignedUsedEndOffset = mUsedEndOffset + alignmentOffset;

		// Check if the buffer is NOT split (i.e sub-alloc on ends)
		if (mUsedStartOffset <= mUsedEndOffset)
		{
			// Order is important (try to sub-alloc at end first).
			// This is due to FIFO order where sub-allocs are inserted from left-to-right (when not
			// wrapped).
			if (alignedUsedEndOffset + allocationSize <= mMaxBlockSize)
			{
				startOffset = alignedUsedEndOffset;
				mUsedSize += allocationSize + alignmentOffset;
				currentRequestSize = allocationSize + alignmentOffset;
			}
			else if (allocationSize <= mUsedStartOffset) // Try to sub-alloc at front.
			{
				// Count the space at the end so that a subsequent
				// sub-alloc cannot fail when the buffer is full.
				const uint64_t requestSize = (mMaxBlockSize - mUsedEndOffset) + allocationSize;

				startOffset = 0;
				mUsedSize += requestSize;
				currentRequestSize = requestSize;
			}
		}
		else if (alignedUsedEndOffset + allocationSize <= mUsedStartOffset)
		{
			// Otherwise, buffer is split where sub-alloc must be in-between.
			startOffset = alignedUsedEndOffset;
			mUsedSize += allocationSize + alignmentOffset;
			currentRequestSize = allocationSize + alignmentOffset;
		}

		if (startOffset != cInvalidOffset)
		{
			mUsedEndOffset = startOffset + allocationSize;

			Request request;
			request.endOffset = mUsedEndOffset;
			request.size = currentRequestSize;

			mInflightRequests.Push(serial, std::move(request));
		}

		return startOffset;
	}

	void UploadAllocator::RingBuffer::Deallocate(uint64_t lastCompletedSerial)
	{
		for (Request& request : mInflightRequests.IterateUpTo(lastCompletedSerial)) {
			mUsedStartOffset = request.endOffset;
			mUsedSize -= request.size;
		}

		// Dequeue previously recorded requests.
		mInflightRequests.ClearUpTo(lastCompletedSerial);
	}

	uint64_t UploadAllocator::RingBuffer::GetSize() const
	{
		return mMaxBlockSize;
	}

	uint64_t UploadAllocator::RingBuffer::GetUsedSize() const
	{
		return mUsedSize;
	}

	bool UploadAllocator::RingBuffer::Empty() const
	{
		return mInflightRequests.Empty();
	}

	UploadAllocator::UploadAllocator(DeviceBase* device) : mDevice(device) {}

	UploadAllocator::~UploadAllocator() {}

	UploadAllocation UploadAllocator::Allocate(uint64_t allocationSize, uint64_t serial, uint64_t offsetAlignment)
	{
		if (allocationSize > cRingBufferSize)
		{
			BufferDesc desc{};
			desc.usage = BufferUsage::CopySrc | BufferUsage::MapWrite;
			desc.size = AlignUp(allocationSize, 4u);
			desc.name = "UploadStageBuffer";

			Ref<BufferBase> buffer = mDevice->CreateBuffer(desc);
			UploadAllocation allocation{};
			allocation.buffer = buffer.Get();
			allocation.mappedAddress = buffer->APIGetMappedPointer();
			
			mLargeStageBuffersToDelete.Push(serial, std::move(buffer));
			return allocation;
		}

		if (mRingBuffers.empty())
		{
			mRingBuffers.emplace_back(std::make_unique<RingBuffer>(cRingBufferSize));
		}

		uint64_t startOffset = RingBuffer::cInvalidOffset;
		RingBuffer* targetRingBuffer;
		for (auto& ringBuffer : mRingBuffers)
		{
			ASSERT(ringBuffer->GetSize() >= ringBuffer->GetUsedSize());
			startOffset = ringBuffer->Allocate(allocationSize, serial, offsetAlignment);
			if (startOffset != RingBuffer::cInvalidOffset)
			{
				targetRingBuffer = ringBuffer.get();
				break;
			}
		}

		// append a newly created ring buffer to fulfill the request.
		if (startOffset == RingBuffer::cInvalidOffset)
		{
			mRingBuffers.emplace_back(std::make_unique<RingBuffer>(cRingBufferSize));
			targetRingBuffer = mRingBuffers.back().get();
			startOffset = targetRingBuffer->Allocate(allocationSize, serial);
		}

		ASSERT(startOffset != RingBuffer::cInvalidOffset);

		if (targetRingBuffer->buffer == nullptr)
		{
			BufferDesc desc{};
			desc.usage = BufferUsage::CopySrc | BufferUsage::MapWrite;
			desc.size = AlignUp(targetRingBuffer->GetSize(), 4u);
			desc.name = "UploadStageBuffer";

			Ref<BufferBase> buffer = mDevice->CreateBuffer(desc);
			targetRingBuffer->buffer = std::move(buffer);
		}

		ASSERT(targetRingBuffer->buffer != nullptr);

		UploadAllocation allocation{};
		allocation.buffer = targetRingBuffer->buffer.Get();
		allocation.mappedAddress = static_cast<uint8_t*>(targetRingBuffer->buffer->APIGetMappedPointer()) + startOffset;
		allocation.offset = startOffset;

		return allocation;
	}

	void UploadAllocator::Deallocate(uint64_t lastCompletedSerial)
	{
		for (auto ringBufferIter = mRingBuffers.begin(); ringBufferIter != mRingBuffers.end();)
		{
			(*ringBufferIter)->Deallocate(lastCompletedSerial);
			if ((*ringBufferIter)->Empty() &&  mRingBuffers.size() > 1)
			{
				// Never erase the last buffer as to prevent re-creating smaller buffers.
				ringBufferIter = mRingBuffers.erase(ringBufferIter);
			}
			else
			{
				++ringBufferIter;
			}
		}
		mLargeStageBuffersToDelete.CIterateUpTo(lastCompletedSerial);
	}
}