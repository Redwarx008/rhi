#pragma once
#include <cstdint>

#include "RHIStruct.h"

namespace rhi::impl
{
    enum class Aspect : uint8_t 
    {
        None = 0x0,
        Color = 0x1,
        Depth = 0x2,
        Stencil = 0x4,
        // Aspects used to select individual planes in a multi-planar format.
        Plane0 = 0x8,
        Plane1 = 0x10,
        Plane2 = 0x20,
    };
    ENUM_CLASS_FLAG_OPERATORS(Aspect);

    struct SubresourceRange 
    {
        Aspect aspects = Aspect::None;
        uint32_t baseArrayLayer = 0;
        uint32_t layerCount = 0;
        uint32_t baseMipLevel = 0;
        uint32_t levelCount = 0;

        SubresourceRange() = default;
        SubresourceRange(Aspect _aspects, uint32_t _baseArrayLayer, uint32_t _layerCount, uint32_t _baseMipLevel, uint32_t _levelCount);

        static SubresourceRange MakeSingle(Aspect aspect, uint32_t baseArrayLayer, uint32_t baseMipLevel);

        static SubresourceRange MakeFull(Aspect aspects, uint32_t layerCount, uint32_t levelCount);
    };

    uint8_t GetAspectIndex(Aspect aspect);

    Aspect GetAspectFromFormat(TextureFormat format);

    uint8_t GetAspectCount(Aspect aspects);

    Aspect AspectConvert(TextureFormat format, TextureAspect aspect);

    Aspect ViewAspectConvert(TextureFormat format, TextureAspect aspect);

}