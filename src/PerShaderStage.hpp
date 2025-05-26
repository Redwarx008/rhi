#pragma once

#include "RHIStruct.h"
#include "common/Constants.h"
#include "common/Error.h"
#include "common/Utils.h"
#include <cmath>
#include <array>

namespace rhi::impl
{
    template <typename T>
    class PerShaderStage
    {
        using iterator = typename std::array<T, cNumStages>::iterator;
        using const_iterator = typename std::array<T, cNumStages>::const_iterator;
    public:
        PerShaderStage()
        {};

        explicit PerShaderStage(const T& initialValue)
        {
            mData.fill(initialValue);
        }

        ~PerShaderStage()
        {}

        bool operator==(const PerShaderStage<T>& other) const
        {
            return mData == other.mData;
        }

        bool operator!=(const PerShaderStage<T>& other) const
        {
            return !(*this == other);
        }

        T& operator[](ShaderStage stageBit)
        {
            uint32_t bit = static_cast<uint32_t>(stageBit);
            ASSERT(bit != 0 && IsPowerOfTwo(bit) && bit <= (1 << cNumStages - 1));
            return mData[std::log2(bit)];
        }

        const T& operator[](ShaderStage stageBit) const
        {
            uint32_t bit = static_cast<uint32_t>(stageBit);
            ASSERT(bit != 0 && IsPowerOfTwo(bit) && bit <= (1 << cNumStages - 1));
            return mData[std::log2(bit)];
        }

        iterator begin() { return mData.begin(); }
        iterator end() { return mData.end(); }
        const_iterator begin() const { return mData.begin(); }
        const_iterator end() const { return mData.end(); }
        const_iterator cbegin() const { return mData.cbegin(); }
        const_iterator cend() const { return mData.cend(); }
    private:
        std::array<T, cNumStages> mData;
    };
}
