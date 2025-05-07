#pragma once

#include "common/NoCopyable.h"

#include <memory>
#include <vector>
#include <limits>

#if defined (max)
#undef max
#endif

namespace rhi::impl
{
	constexpr uint32_t cEndOfBlock = std::numeric_limits<uint32_t>::max();
	constexpr uint32_t cAdditionalData = std::numeric_limits<uint32_t>::max() - 1;

	struct Block
	{
		size_t size;
		std::unique_ptr<char[]> data;
	};

	using CommandBlocks = std::vector<Block>;

	class CommandAllocator;

	class CommandIterator : public NonCopyable
	{
	public:
		explicit CommandIterator(CommandAllocator& allocator);
		~CommandIterator();

		template <typename E>
		bool NextCommandId(E* commandId)
		{
			return NextCommandId(reinterpret_cast<uint32_t*>(commandId));
		}
		template <typename T>
		T* NextCommand()
		{
			return static_cast<T*>(NextCommand(sizeof(T), alignof(T)));
		}
		template <typename T>
		T* NextData(size_t count)
		{
			return static_cast<T*>(NextData(sizeof(T) * count, alignof(T)));
		}
		// Sets iterator to the beginning of the commands without emptying the list. This method can
		// be used if iteration was stopped early and the iterator needs to be restarted.
		void Reset();
		void Clear();
		bool IsEmpty() const;
	private:
		bool NextCommandId(uint32_t* commandId);
		void* NextCommand(size_t commandSize, size_t commandAlignment);
		void* NextData(size_t dataSize, size_t dataAlignment);
		CommandBlocks mBlocks;
		// This is an extremely hot pointer during command iteration, but always
		// points to at least a valid uint32_t, either inside a block, or at mEndOfBlock.
		char* mCurrentPtr = nullptr;
		size_t mCurrentBlockIndex = 0;
		// Used to avoid a special case for empty iterators.
		uint32_t mEndOfBlock = cEndOfBlock;

		CommandAllocator& mAllocator;
	};


	class CommandAllocator : public NonCopyable
	{
	public:
		CommandAllocator();
		~CommandAllocator();
		CommandAllocator(CommandAllocator&&);
		CommandAllocator& operator=(CommandAllocator&&);
		void Clear();
		template <typename T, typename E>
		T* Allocate(E commandId) {
			static_assert(sizeof(E) == sizeof(uint32_t));
			static_assert(alignof(E) == alignof(uint32_t));
			static_assert(alignof(T) <= cMaxSupportedAlignment);
			T* result =
				reinterpret_cast<T*>(Allocate(static_cast<uint32_t>(commandId), sizeof(T), alignof(T)));
			if (!result) {
				return nullptr;
			}
			new (result) T;
			return result;
		}

		template <typename T>
		T* AllocateData(size_t count) {
			static_assert(alignof(T) <= cMaxSupportedAlignment);
			T* result = reinterpret_cast<T*>(AllocateData(sizeof(T) * count, alignof(T)));
			if (!result) {
				return nullptr;
			}
			for (size_t i = 0; i < count; i++) {
				new (result + i) T;
			}
			return result;
		}
		void Recycle(CommandBlocks&& blocks);
	private:
		static constexpr uint32_t cMaxSupportedAlignment = 8;
		// To avoid checking for overflows at every step of the computations we compute an upper
		// bound of the space that will be needed in addition to the command data.
		static constexpr uint32_t cWorstCaseAdditionalSize =
			sizeof(uint32_t) + cMaxSupportedAlignment + alignof(uint32_t);

		static constexpr uint32_t cDefaultBaseAllocationSize = 2048;

		friend CommandIterator;
		CommandBlocks&& AcquireCurrentBlocks();

		char* Allocate(uint32_t commandId, size_t commandSize, size_t commandAlignment);
		char* AllocateData(size_t commandSize, size_t commandAlignment)
		{
			return Allocate(cAdditionalData, commandSize, commandAlignment);
		}
		bool GetNewBlock(size_t minimumSize);
		void Reset();
		std::vector<CommandBlocks> mBlocksPool;
		CommandBlocks mBlocks;
		int64_t mCurrentBlockIndex = -1;
		// Data used for the block range at initialization so that the first call to Allocate sees
		// there is not enough space and calls GetNewBlock. This avoids having to special case the
		// initialization in Allocate.
		uint32_t mPlaceholderSpace[1] = { 0 };
		size_t mLastAllocationSize = cDefaultBaseAllocationSize;
		char* mCurrentPtr = nullptr;
		char* mEndPtr = nullptr;
	};
}