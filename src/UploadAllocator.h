#pragma once

#include "RHIStruct.h"
#include "common/Ref.hpp"
#include "common/SerialQueue.hpp"
#include <cstdint>
#include <limits>
#include <list>
#include <memory>

#if defined (max)
#undef max
#endif

namespace rhi::impl
{
	struct UploadAllocation
	{
		BufferBase* buffer;
		uint64_t offset;
		void* mappedAddress;
	};

	class UploadAllocator
	{
	public:
		explicit UploadAllocator(DeviceBase* device);
		~UploadAllocator();

		UploadAllocation Allocate(uint64_t allocationSize, uint64_t serial, uint64_t offsetAlignment);
		void Deallocate(uint64_t lastCompletedSerial);
	private:
		class RingBuffer
		{
		public:
			explicit RingBuffer(uint64_t maxSize);
			~RingBuffer();

			// Sub-allocate the ring-buffer by requesting a chunk of the specified size. 
			// return the starting offset. 
			uint64_t Allocate(uint64_t allocationSize, uint64_t serial, uint64_t offsetAlignment = 1);
			void Deallocate(uint64_t lastCompletedSerial);

			uint64_t GetSize() const;
			uint64_t GetUsedSize() const;
			bool Empty() const;

			static constexpr uint64_t cInvalidOffset = std::numeric_limits<uint64_t>::max();

			Ref<BufferBase> buffer;
		private:
			struct Request
			{
				uint64_t endOffset;
				uint64_t size;
			};

			SerialQueue<uint64_t, Request> mInflightRequests;

			uint64_t mUsedEndOffset = 0;    // Tail of used sub-alloc requests (in bytes).
			uint64_t mUsedStartOffset = 0;  // Head of used sub-alloc requests (in bytes).
			uint64_t mMaxBlockSize = 0;     // Max size of the ring buffer (in bytes).
			uint64_t mUsedSize = 0;         // Size of the sub-alloc requests (in bytes) of the ring buffer.
		};

		static constexpr uint64_t cRingBufferSize = 4 * 1024 * 1024;

		std::list<std::unique_ptr<RingBuffer>> mRingBuffers;

		SerialQueue<uint64_t, Ref<BufferBase>> mLargeStageBuffersToDelete;

		DeviceBase* mDevice;

	};
}