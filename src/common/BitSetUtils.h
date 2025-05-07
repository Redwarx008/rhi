#pragma once

#include <bitset>

namespace rhi::impl
{
    template <size_t N>
    class BitSetIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type;

        explicit BitSetIterator(const std::bitset<N>& bits, size_t pos = 0)
            : mBits(bits), mPos(find_next(pos))
        {

        }

        value_type operator*() const { return mPos; }

        BitSetIterator& operator++()
        {
            mPos = find_next(mPos + 1);
            return *this;
        }

        BitSetIterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const BitSetIterator& other) const { return mPos == other.mPos; }
        bool operator!=(const BitSetIterator& other) const { return !(*this == other); }

    private:
        const std::bitset<N>& mBits;
        size_t mPos;

        size_t find_next(size_t start) const 
        {
            for (size_t i = start; i < N; ++i) 
            {
                if (mBits.test(i)) return i;
            }
            return N; 
        }
    };

    template <size_t N>
    class BitSetView
    {
    public:
        explicit BitSetView(const std::bitset<N>& bits) : mBits(bits) {}

        BitSetIterator<N> begin() const { return BitSetIterator<N>(mBits, 0); }
        BitSetIterator<N> end() const { return BitSetIterator<N>(mBits, N); }

    private:
        const std::bitset<N>& mBits;
    };

    template <size_t N>
    BitSetView<N> IterateBitSet(const std::bitset<N>& bitSet)
    {
        return BitSetView<N>(bitSet);
    }

    template <size_t N>
    uint32_t GetHighestBitSetIndex(const std::bitset<N>& bitSet)
    {
        for (uint32_t i = N - 1; i >= 0; --i)
        {
            if (bitSet.test(i))
            {
                return i;
            }
        }

        return static_cast<uint32_t>(-1);
    }

}