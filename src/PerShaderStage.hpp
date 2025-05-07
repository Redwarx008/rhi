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
    public:
        PerShaderStage() {};
        explicit PerShaderStage(const T& initialValue) { mData.fill(initialValue); }
        ~PerShaderStage() {}
        T& operator[](ShaderStage stageBit)
        {
            uint32_t bit = static_cast<uint32_t>(stageBit);
            ASSERT(bit != 0 && IsPowerOfTwo(bit) && bit <= (1 << cNumStages));
            return mData[std::log2(bit)];
        }
        const T& operator[](ShaderStage stageBit) const 
        {
            uint32_t bit = static_cast<uint32_t>(stageBit);
            ASSERT(bit != 0 && IsPowerOfTwo(bit) && bit <= (1 << cNumStages));
            return mData[std::log2(bit)];
        }

    private:
        std::array<T, cNumStages> mData;
    };
}