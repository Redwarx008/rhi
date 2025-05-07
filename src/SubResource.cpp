#include "Subresource.h"

#include "common/Utils.h"

namespace rhi::impl
{
    SubresourceRange::SubresourceRange(Aspect _aspects, uint32_t _baseArrayLayer, uint32_t _layerCount, uint32_t _baseMipLevel, uint32_t _levelCount)
        :
        aspects(_aspects),
        baseArrayLayer(_baseArrayLayer),
        layerCount(_layerCount),
        baseMipLevel(_baseArrayLayer),
        levelCount(_levelCount)
    {

    }

	SubresourceRange SubresourceRange::MakeSingle(Aspect aspect, uint32_t baseArrayLayer, uint32_t baseMipLevel)
	{
        SubresourceRange res;
        res.aspects = aspect;
        res.baseMipLevel = baseMipLevel;
        res.levelCount = 1;
        res.baseArrayLayer = baseArrayLayer;
        res.layerCount = 1;

        return res;
	}

    SubresourceRange SubresourceRange::MakeFull(Aspect aspects, uint32_t layerCount, uint32_t levelCount)
    {
        SubresourceRange res;
        res.aspects = aspects;
        res.baseMipLevel = 0;
        res.levelCount = levelCount;
        res.baseArrayLayer = 0;
        res.layerCount = layerCount;

        return res;
    }

    uint8_t GetAspectIndex(Aspect aspect)
    {
        assert(HasOneFlag(aspect));
        switch (aspect)
        {
        case Aspect::Color:
        case Aspect::Depth:
        case Aspect::Plane0:
            return 0;
        case Aspect::Stencil:
        case Aspect::Plane1:
            return 1;
        case Aspect::Plane2:
            return 2;
        default:
            assert(!"unreachable");
        }
    }

    Aspect GetAspectFromFormat(TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::R8_UINT:
        case TextureFormat::R8_SINT:
        case TextureFormat::R8_UNORM:
        case TextureFormat::R8_SNORM:
        case TextureFormat::RG8_UINT:
        case TextureFormat::RG8_SINT:
        case TextureFormat::RG8_UNORM:
        case TextureFormat::RG8_SNORM:
        case TextureFormat::R16_UINT:
        case TextureFormat::R16_SINT:
        case TextureFormat::R16_UNORM:
        case TextureFormat::R16_SNORM:
        case TextureFormat::R16_FLOAT:
        case TextureFormat::BGRA4_UNORM:
        case TextureFormat::B5G6R5_UNORM:
        case TextureFormat::B5G5R5A1_UNORM:
        case TextureFormat::RGBA8_UINT:
        case TextureFormat::RGBA8_SINT:
        case TextureFormat::RGBA8_UNORM:
        case TextureFormat::RGBA8_SNORM:
        case TextureFormat::BGRA8_UNORM:
        case TextureFormat::RGBA8_SRGB:
        case TextureFormat::BGRA8_SRGB:
        case TextureFormat::R10G10B10A2_UNORM:
        case TextureFormat::R11G11B10_FLOAT:
        case TextureFormat::RG16_UINT:
        case TextureFormat::RG16_SINT:
        case TextureFormat::RG16_UNORM:
        case TextureFormat::RG16_SNORM:
        case TextureFormat::RG16_FLOAT:
        case TextureFormat::R32_UINT:
        case TextureFormat::R32_SINT:
        case TextureFormat::R32_FLOAT:
        case TextureFormat::RGBA16_UINT:
        case TextureFormat::RGBA16_SINT:
        case TextureFormat::RGBA16_FLOAT:
        case TextureFormat::RGBA16_UNORM:
        case TextureFormat::RGBA16_SNORM:
        case TextureFormat::RG32_UINT:
        case TextureFormat::RG32_SINT:
        case TextureFormat::RG32_FLOAT:
        case TextureFormat::RGB32_UINT:
        case TextureFormat::RGB32_SINT:
        case TextureFormat::RGB32_FLOAT:
        case TextureFormat::RGBA32_UINT:
        case TextureFormat::RGBA32_SINT:
        case TextureFormat::RGBA32_FLOAT:
        case TextureFormat::BC1_UNORM:
        case TextureFormat::BC1_UNORM_SRGB:
        case TextureFormat::BC2_UNORM:
        case TextureFormat::BC2_UNORM_SRGB:
        case TextureFormat::BC3_UNORM:
        case TextureFormat::BC3_UNORM_SRGB:
        case TextureFormat::BC4_UNORM:
        case TextureFormat::BC4_SNORM:
        case TextureFormat::BC5_UNORM:
        case TextureFormat::BC5_SNORM:
        case TextureFormat::BC6H_UFLOAT:
        case TextureFormat::BC6H_SFLOAT:
        case TextureFormat::BC7_UNORM:
        case TextureFormat::BC7_UNORM_SRGB:
            return Aspect::Color;
        case TextureFormat::D16_UNORM:
        case TextureFormat::D32_UNORM:
            return Aspect::Depth;
        case TextureFormat::D24_UNORM_S8_UINT:
        case TextureFormat::D32_UNORM_S8_UINT:
            return Aspect::Depth | Aspect::Stencil;
        default:
            assert(!"Invalid Enumeration Value");
            break;
        } // todo: add  multi-planar formats support.
    }

    uint8_t GetAspectCount(Aspect aspects)
    {
        if (aspects == Aspect::Stencil)
        {
            // Fake a the existence of a depth aspect so that the stencil data stays at index 1.
            assert(GetAspectIndex(Aspect::Stencil) == 1);
            return 2;
        }
        return PopCount(static_cast<uint8_t>(aspects));
    }

    Aspect AspectConvert(TextureFormat format, TextureAspect aspect)
    {
        Aspect aspectsFromFormat = GetAspectFromFormat(format);
        switch (aspect)
        {
        case TextureAspect::All:
            return aspectsFromFormat;
        case TextureAspect::Depth:
            return aspectsFromFormat & Aspect::Depth;
        case TextureAspect::Stencil:
            return aspectsFromFormat & Aspect::Stencil;
        case TextureAspect::Plane0:
            return aspectsFromFormat & Aspect::Plane0;
        case TextureAspect::Plane1:
            return aspectsFromFormat & Aspect::Plane1;
        case TextureAspect::Plane2:
            return aspectsFromFormat & Aspect::Plane2;
        default:
            break;
        }
        assert(!"Unreachable");
    }

    Aspect ViewAspectConvert(TextureFormat format, TextureAspect aspect)
    {
        // Color view |format| must be treated as the same plane |aspect|.
        if (GetAspectFromFormat(format) == Aspect::Color)
        {
            switch (aspect)
            {
            case TextureAspect::Plane0:
                return Aspect::Plane0;
            case TextureAspect::Plane1:
                return Aspect::Plane1;
            case TextureAspect::Plane2:
                return Aspect::Plane2;
            default:
                break;
            }
        }

        return AspectConvert(format, aspect);
    }
}