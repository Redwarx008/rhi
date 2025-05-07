#include "CommandAllocator.h"
#include "common/Utils.h"

#include <cassert>

namespace rhi::impl
{
    CommandAllocator::CommandAllocator()
    {
        mCurrentPtr = reinterpret_cast<char*>(&mPlaceholderSpace[0]);
        mEndPtr = reinterpret_cast<char*>(&mPlaceholderSpace[1]); // just get address. no visit
    }

    CommandAllocator::~CommandAllocator() = default;

    CommandAllocator::CommandAllocator(CommandAllocator&& other)
        : mBlocks(std::move(other.mBlocks)), mLastAllocationSize(other.mLastAllocationSize)
    {
        mCurrentPtr = other.mCurrentPtr;
        mEndPtr = other.mEndPtr;
        mCurrentBlockIndex = other.mCurrentBlockIndex;
        other.Clear();
    }
    CommandAllocator& CommandAllocator::operator=(CommandAllocator&& other)
    {
        Clear();
        std::swap(mBlocks, other.mBlocks);
        mLastAllocationSize = other.mLastAllocationSize;
        mCurrentPtr = other.mCurrentPtr;
        mEndPtr = other.mEndPtr;
        other.Clear();
        return *this;
    }

	char* CommandAllocator::Allocate(uint32_t commandId, size_t commandSize, size_t commandAlignment)
	{
        assert(mCurrentPtr != nullptr);
        assert(mEndPtr != nullptr);
        assert(commandId != cEndOfBlock);

        // It should always be possible to allocate one id, for kEndOfBlock tagging,
        assert(IsPtrAligned(mCurrentPtr, alignof(uint32_t)));
        assert(mEndPtr >= mCurrentPtr);
        assert(static_cast<size_t>(mEndPtr - mCurrentPtr) >= sizeof(uint32_t));

        // The memory after the ID will contain the following:
        //   - the current ID
        //   - padding to align the command, maximum kMaxSupportedAlignment
        //   - the command of size commandSize
        //   - padding to align the next ID, maximum alignof(uint32_t)
        //   - the next ID of size sizeof(uint32_t)

        // This can't overflow because by construction mCurrentPtr always has space for the next
        // ID.
        size_t remainingSize = static_cast<size_t>(mEndPtr - mCurrentPtr);

        // The good case were we have enough space for the command data and upper bound of the
        // extra required space.
        if ((remainingSize >= cWorstCaseAdditionalSize) &&
            (remainingSize - cWorstCaseAdditionalSize >= commandSize))
        {
            uint32_t* idAlloc = reinterpret_cast<uint32_t*>(mCurrentPtr);
            *idAlloc = commandId;

            char* commandAlloc = AlignPtr(mCurrentPtr + sizeof(uint32_t), commandAlignment);
            mCurrentPtr = AlignPtr(commandAlloc + commandSize, alignof(uint32_t));

            return commandAlloc;
        }
       
        // When there is not enough space, we signal the kEndOfBlock, so that the iterator knows
        // to move to the next one. kEndOfBlock on the last block means the end of the commands.
        uint32_t* idAlloc = reinterpret_cast<uint32_t*>(mCurrentPtr);
        *idAlloc = cEndOfBlock;

        if (mBlocks.empty() && mBlocksPool.size() > 0)
        {
            mBlocks = std::move((*(mBlocksPool.end() - 1)));
            mBlocksPool.erase((mBlocksPool.end() - 1));
        }

        // We can reuse the allocated memory.
        if (int32_t lastBlockIndex = (static_cast<int>(mBlocks.size()) - 1) > mCurrentBlockIndex)
        {
            ++mCurrentBlockIndex;
            mCurrentPtr = AlignPtr(mBlocks[mCurrentBlockIndex].data.get(), alignof(uint32_t));
            mEndPtr = mBlocks[mCurrentBlockIndex].data.get() + mBlocks[mCurrentBlockIndex].size;
        }
        else
        {
            // We'll request a block that can contain at least the command ID, the command and an
            // additional ID to contain the kEndOfBlock tag.
            size_t requestedBlockSize = commandSize + cWorstCaseAdditionalSize;

            // The computation of the request could overflow.
            if (requestedBlockSize <= commandSize)
            {
                return nullptr;
            }

            if (!GetNewBlock(requestedBlockSize))
            {
                return nullptr;
            }
        }

        return Allocate(commandId, commandSize, commandAlignment);
	}

    bool CommandAllocator::GetNewBlock(size_t minimumSize)
    {
        // Allocate blocks doubling sizes each time, to a maximum of 16k (or at least minimumSize).
        mLastAllocationSize = std::max(minimumSize, std::min(mLastAllocationSize * 2, size_t(16384)));

        auto block = std::unique_ptr<char[]>(new (std::nothrow) char[mLastAllocationSize]);
        if (block == nullptr)
        {
            return false;
        }

        mCurrentPtr = AlignPtr(block.get(), alignof(uint32_t));
        mEndPtr = block.get() + mLastAllocationSize;
        mBlocks.push_back({ mLastAllocationSize, std::move(block) });
        return true;
    }

    void CommandAllocator::Reset()
    {

        mCurrentPtr = reinterpret_cast<char*>(&mPlaceholderSpace[0]);
        mEndPtr = reinterpret_cast<char*>(&mPlaceholderSpace[1]); // just get address. no visit
        mCurrentBlockIndex = -1;
    }

    void CommandAllocator::Clear()
    {
        Reset();
        mLastAllocationSize = cDefaultBaseAllocationSize;
    }

    CommandBlocks&& CommandAllocator::AcquireCurrentBlocks()
    {
        assert(mCurrentPtr != nullptr && mEndPtr != nullptr);
        assert(IsPtrAligned(mCurrentPtr, alignof(uint32_t)));
        assert(mCurrentPtr + sizeof(uint32_t) <= mEndPtr);
        *reinterpret_cast<uint32_t*>(mCurrentPtr) = cEndOfBlock;

        Clear();
        return std::move(mBlocks);
    }

    void CommandAllocator::Recycle(CommandBlocks&& blocks)
    {
        mBlocksPool.push_back(std::move(blocks));
    }

    CommandIterator::CommandIterator(CommandAllocator& allocator) :
        mBlocks(allocator.AcquireCurrentBlocks()),
        mAllocator(allocator)
    {
        Reset();
    }

    CommandIterator::~CommandIterator()
    {
        mAllocator.Recycle(std::move(mBlocks));
    }

    bool CommandIterator::NextCommandId(uint32_t* commandId)
    {
        char* idPtr = AlignPtr(mCurrentPtr, alignof(uint32_t));
        assert(idPtr == reinterpret_cast<char*>(&mEndOfBlock) ||
            idPtr + sizeof(uint32_t) <=
            mBlocks[mCurrentBlockIndex].data.get() + mBlocks[mCurrentBlockIndex].size);

        uint32_t id = *reinterpret_cast<uint32_t*>(idPtr);

        if (id != cEndOfBlock)
        {
            mCurrentPtr = idPtr + sizeof(uint32_t);
            *commandId = id;
            return true;
        }

        mCurrentBlockIndex++;
        if (mCurrentBlockIndex >= mBlocks.size())
        {
            Reset();
            *commandId = cEndOfBlock;
            return false;
        }
        mCurrentPtr = AlignPtr(mBlocks[mCurrentBlockIndex].data.get(), alignof(uint32_t));
        return NextCommandId(commandId);
    }

    void CommandIterator::Reset()
    {
        mCurrentBlockIndex = 0;

        if (mBlocks.empty())
        {
            // This will case the first NextCommandId call to try to move to the next block and stop
            // the iteration immediately, without special casing the initialization.
            mCurrentPtr = reinterpret_cast<char*>(&mEndOfBlock);
        }
        else
        {
            mCurrentPtr = AlignPtr(mBlocks[0].data.get(), alignof(uint32_t));
        }
    }

    void CommandIterator::Clear()
    {
        if (IsEmpty())
        {
            return;
        }

        mCurrentPtr = reinterpret_cast<char*>(&mEndOfBlock);
        Reset();
    }

    bool CommandIterator::IsEmpty() const
    {
        return mBlocks.empty();
    }

    void* CommandIterator::NextCommand(size_t commandSize, size_t commandAlignment)
    {
        char* commandPtr = AlignPtr(mCurrentPtr, commandAlignment);
        assert(commandPtr + sizeof(commandSize) <=
            mBlocks[mCurrentBlockIndex].data.get() + mBlocks[mCurrentBlockIndex].size);

        mCurrentPtr = commandPtr + commandSize;
        return commandPtr;
    }

    void* CommandIterator::NextData(size_t dataSize, size_t dataAlignment)
    {
        uint32_t id;
        bool hasId = NextCommandId(&id);
        assert(hasId);
        assert(id == cAdditionalData);

        return NextCommand(dataSize, dataAlignment);
    }
}