#pragma once

namespace rhi::impl
{
    template <typename Enum>
    class EnumFlagIterator {
    public:
        using UnderlyingType = std::underlying_type_t<Enum>;

        EnumFlagIterator(Enum mCombination, UnderlyingType offset)
            : mCombination(mCombination), mOffset(offset)
        {
            moveToNextValidFlag();
        }

        Enum operator*() const
        {
            return static_cast<Enum>(static_cast<UnderlyingType>(1) << mOffset);
        }

        EnumFlagIterator& operator++()
        {
            ++mOffset;
            moveToNextValidFlag();
            return *this;
        }

        bool operator!=(const EnumFlagIterator& other) const
        {
            return mOffset != other.mOffset;
        }

    private:
        Enum mCombination;
        UnderlyingType mOffset;

        void moveToNextValidFlag()
        {
            while (mOffset < sizeof(UnderlyingType) * 8 &&
                (mCombination & static_cast<Enum>(static_cast<UnderlyingType>(1) << mOffset)) == static_cast<Enum>(0))
            {
                ++mOffset;
            }
        }
    };

    template <typename Enum>
    class EnumFlagRange {
    public:
        EnumFlagRange(Enum combination) : mCombination(combination) {}

        EnumFlagIterator<Enum> begin() const
        {
            return EnumFlagIterator<Enum>(mCombination, 0);
        }

        EnumFlagIterator<Enum> end() const
        {
            return EnumFlagIterator<Enum>(mCombination, sizeof(std::underlying_type_t<Enum>) * 8);
        }

    private:
        Enum mCombination;
    };

    // Usage: for(auto component : IterateEnumComponent(combination)).
    template<typename Enum>
    EnumFlagRange<Enum> IterateEnumFlags(Enum combination)
    {
        return EnumFlagRange<Enum>(combination);
    }

}