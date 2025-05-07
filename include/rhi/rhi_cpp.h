#pragma once

#include "rhi.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace rhi
{
#define ENUM_CLASS_FLAG_OPERATORS(EnumName) \
inline constexpr EnumName operator|(EnumName a, EnumName b) { \
    using Underlying = std::underlying_type_t<EnumName>; \
    return static_cast<EnumName>(static_cast<Underlying>(a) | static_cast<Underlying>(b)); \
} \
inline constexpr EnumName operator&(EnumName a, EnumName b) { \
    using Underlying = std::underlying_type_t<EnumName>; \
    return static_cast<EnumName>(static_cast<Underlying>(a) & static_cast<Underlying>(b)); \
} \
inline constexpr EnumName operator^(EnumName a, EnumName b) { \
    using Underlying = std::underlying_type_t<EnumName>; \
    return static_cast<EnumName>(static_cast<Underlying>(a) ^ static_cast<Underlying>(b)); \
} \
inline constexpr EnumName operator~(EnumName a) { \
    using Underlying = std::underlying_type_t<EnumName>; \
    return static_cast<EnumName>(~static_cast<Underlying>(a)); \
} \
inline constexpr EnumName& operator|=(EnumName& a, EnumName b) { \
    a = a | b; \
    return a; \
} \
inline constexpr EnumName& operator&=(EnumName& a, EnumName b) { \
    a = a & b; \
    return a; \
} \
inline constexpr bool operator==(EnumName a, uint32_t b) { \
	return static_cast<uint32_t>(a) == b; \
} \
inline constexpr bool operator!=(EnumName a, uint32_t b) { \
	return !(a == b); \
}\


	enum class MapMode : uint32_t
	{
		Read = RHIMapMode_Read,
		Write = RHIMapMode_Write
	};
	static_assert(sizeof(RHIMapMode) == sizeof(MapMode), "sizeof mismatch for MapMode");
	static_assert(alignof(RHIMapMode) == alignof(MapMode), "alignof mismatch for MapMode");

	enum class BufferMapAsyncStatus : uint32_t
	{
		Success = RHIBufferMapAsyncStatus_Success,
		None = RHIBufferMapAsyncStatus_None,
		DeviceLost = RHIBufferMapAsyncStatus_DeviceLost,
		DestroyedBeforeCallback = RHIBufferMapAsyncStatus_DestroyedBeforeCallback,
	};
	static_assert(sizeof(RHIBufferMapAsyncStatus) == sizeof(BufferMapAsyncStatus), "sizeof mismatch for BufferMapAsyncStatus");
	static_assert(alignof(RHIBufferMapAsyncStatus) == alignof(BufferMapAsyncStatus), "alignof mismatch for BufferMapAsyncStatus");

	enum class BufferUsage : uint32_t
	{
		None = RHIBufferUsage_None,
		Vertex = RHIBufferUsage_Vertex,
		Index = RHIBufferUsage_Index,
		Indirect = RHIBufferUsage_Indirect,
		Uniform = RHIBufferUsage_Uniform,
		Storage = RHIBufferUsage_Storage,
		MapRead = RHIBufferUsage_MapRead,
		MapWrite = RHIBufferUsage_MapWrite,
		CopySrc = RHIBufferUsage_CopySrc,
		CopyDst = RHIBufferUsage_CopyDst,
		QueryResolve = RHIBufferUsage_QueryResolve
	};
	ENUM_CLASS_FLAG_OPERATORS(BufferUsage);
	static_assert(sizeof(RHIBufferUsage) == sizeof(BufferUsage), "sizeof mismatch for BufferUsage");
	static_assert(alignof(RHIBufferUsage) == alignof(BufferUsage), "alignof mismatch for BufferUsage");

	enum class TextureDimension : uint32_t
	{
		Undefined = RHITextureDimension_Undefined,
		Texture1D = RHITextureDimension_Texture1D,
		Texture1DArray = RHITextureDimension_Texture1DArray,
		Texture2D = RHITextureDimension_Texture2D,
		Texture2DArray = RHITextureDimension_Texture2DArray,
		TextureCube = RHITextureDimension_TextureCube,
		TextureCubeArray = RHITextureDimension_TextureCubeArray,
		Texture3D = RHITextureDimension_Texture3D
	};
	static_assert(sizeof(RHITextureDimension) == sizeof(TextureDimension), "sizeof mismatch for TextureDimension");
	static_assert(alignof(RHITextureDimension) == alignof(TextureDimension), "alignof mismatch for TextureDimension");

	enum class TextureFormat : uint32_t
	{
		Undefined = RHITextureFormat_Undefined,

		R8_UINT = RHITextureFormat_R8_UINT,
		R8_SINT = RHITextureFormat_R8_SINT,
		R8_UNORM = RHITextureFormat_R8_UNORM,
		R8_SNORM = RHITextureFormat_R8_SNORM,
		RG8_UINT = RHITextureFormat_RG8_UINT,
		RG8_SINT = RHITextureFormat_RG8_SINT,
		RG8_UNORM = RHITextureFormat_RG8_UNORM,
		RG8_SNORM = RHITextureFormat_RG8_SNORM,
		R16_UINT = RHITextureFormat_R16_UINT,
		R16_SINT = RHITextureFormat_R16_SINT,
		R16_UNORM = RHITextureFormat_R16_UNORM,
		R16_SNORM = RHITextureFormat_R16_SNORM,
		R16_FLOAT = RHITextureFormat_R16_FLOAT,
		BGRA4_UNORM = RHITextureFormat_BGRA4_UNORM,
		B5G6R5_UNORM = RHITextureFormat_B5G6R5_UNORM,
		B5G5R5A1_UNORM = RHITextureFormat_B5G5R5A1_UNORM,
		RGBA8_UINT = RHITextureFormat_RGBA8_UINT,
		RGBA8_SINT = RHITextureFormat_RGBA8_SINT,
		RGBA8_UNORM = RHITextureFormat_RGBA8_UNORM,
		RGBA8_SNORM = RHITextureFormat_RGBA8_SNORM,
		BGRA8_UNORM = RHITextureFormat_BGRA8_UNORM,
		RGBA8_SRGB = RHITextureFormat_RGBA8_SRGB,
		BGRA8_SRGB = RHITextureFormat_BGRA8_SRGB,
		R10G10B10A2_UNORM = RHITextureFormat_R10G10B10A2_UNORM,
		R11G11B10_FLOAT = RHITextureFormat_R11G11B10_FLOAT,
		RG16_UINT = RHITextureFormat_RG16_UINT,
		RG16_SINT = RHITextureFormat_RG16_SINT,
		RG16_UNORM = RHITextureFormat_RG16_UNORM,
		RG16_SNORM = RHITextureFormat_RG16_SNORM,
		RG16_FLOAT = RHITextureFormat_RG16_FLOAT,
		R32_UINT = RHITextureFormat_R32_UINT,
		R32_SINT = RHITextureFormat_R32_SINT,
		R32_FLOAT = RHITextureFormat_R32_FLOAT,
		RGBA16_UINT = RHITextureFormat_RGBA16_UINT,
		RGBA16_SINT = RHITextureFormat_RGBA16_SINT,
		RGBA16_FLOAT = RHITextureFormat_RGBA16_FLOAT,
		RGBA16_UNORM = RHITextureFormat_RGBA16_UNORM,
		RGBA16_SNORM = RHITextureFormat_RGBA16_SNORM,
		RG32_UINT = RHITextureFormat_RG32_UINT,
		RG32_SINT = RHITextureFormat_RG32_SINT,
		RG32_FLOAT = RHITextureFormat_RG32_FLOAT,
		RGB32_UINT = RHITextureFormat_RGB32_UINT,
		RGB32_SINT = RHITextureFormat_RGB32_SINT,
		RGB32_FLOAT = RHITextureFormat_RGB32_FLOAT,
		RGBA32_UINT = RHITextureFormat_RGBA32_UINT,
		RGBA32_SINT = RHITextureFormat_RGBA32_SINT,
		RGBA32_FLOAT = RHITextureFormat_RGBA32_FLOAT,

		D16_UNORM = RHITextureFormat_D16_UNORM,
		D24_UNORM_S8_UINT = RHITextureFormat_D24_UNORM_S8_UINT,
		D32_UNORM = RHITextureFormat_D32_UNORM,
		D32_UNORM_S8_UINT = RHITextureFormat_D32_UNORM_S8_UINT,

		BC1_UNORM = RHITextureFormat_BC1_UNORM,
		BC1_UNORM_SRGB = RHITextureFormat_BC1_UNORM_SRGB,
		BC2_UNORM = RHITextureFormat_BC2_UNORM,
		BC2_UNORM_SRGB = RHITextureFormat_BC2_UNORM_SRGB,
		BC3_UNORM = RHITextureFormat_BC3_UNORM,
		BC3_UNORM_SRGB = RHITextureFormat_BC3_UNORM_SRGB,
		BC4_UNORM = RHITextureFormat_BC4_UNORM,
		BC4_SNORM = RHITextureFormat_BC4_SNORM,
		BC5_UNORM = RHITextureFormat_BC5_UNORM,
		BC5_SNORM = RHITextureFormat_BC5_SNORM,
		BC6H_UFLOAT = RHITextureFormat_BC6H_UFLOAT,
		BC6H_SFLOAT = RHITextureFormat_BC6H_SFLOAT,
		BC7_UNORM = RHITextureFormat_BC7_UNORM,
		BC7_UNORM_SRGB = RHITextureFormat_BC7_UNORM_SRGB,

		COUNT = RHITextureFormat_COUNT,
	};
	static_assert(sizeof(RHITextureFormat) == sizeof(TextureFormat), "sizeof mismatch for TextureFormat");
	static_assert(alignof(RHITextureFormat) == alignof(TextureFormat), "alignof mismatch for TextureFormat");

	enum class VertexFormat : uint32_t
	{
		Uint8 = RHIVertexFormat_Uint8,
		Uint8x2 = RHIVertexFormat_Uint8x2,
		Uint8x4 = RHIVertexFormat_Uint8x4,
		Sint8 = RHIVertexFormat_Sint8,
		Sint8x2 = RHIVertexFormat_Sint8x2,
		Sint8x4 = RHIVertexFormat_Sint8x4,
		Unorm8 = RHIVertexFormat_Unorm8,
		Unorm8x2 = RHIVertexFormat_Unorm8x2,
		Unorm8x4 = RHIVertexFormat_Unorm8x4,
		Snorm8 = RHIVertexFormat_Snorm8,
		Snorm8x2 = RHIVertexFormat_Snorm8x2,
		Snorm8x4 = RHIVertexFormat_Snorm8x4,
		Uint16 = RHIVertexFormat_Uint16,
		Uint16x2 = RHIVertexFormat_Uint16x2,
		Uint16x4 = RHIVertexFormat_Uint16x4,
		Sint16 = RHIVertexFormat_Sint16,
		Sint16x2 = RHIVertexFormat_Sint16x2,
		Sint16x4 = RHIVertexFormat_Sint16x4,
		Unorm16 = RHIVertexFormat_Unorm16,
		Unorm16x2 = RHIVertexFormat_Unorm16x2,
		Unorm16x4 = RHIVertexFormat_Unorm16x4,
		Snorm16 = RHIVertexFormat_Snorm16,
		Snorm16x2 = RHIVertexFormat_Snorm16x2,
		Snorm16x4 = RHIVertexFormat_Snorm16x4,
		Float16 = RHIVertexFormat_Float16,
		Float16x2 = RHIVertexFormat_Float16x2,
		Float16x4 = RHIVertexFormat_Float16x4,
		Float32 = RHIVertexFormat_Float32,
		Float32x2 = RHIVertexFormat_Float32x2,
		Float32x3 = RHIVertexFormat_Float32x3,
		Float32x4 = RHIVertexFormat_Float32x4,
		Uint32 = RHIVertexFormat_Uint32,
		Uint32x2 = RHIVertexFormat_Uint32x2,
		Uint32x3 = RHIVertexFormat_Uint32x3,
		Uint32x4 = RHIVertexFormat_Uint32x4,
		Sint32 = RHIVertexFormat_Sint32,
		Sint32x2 = RHIVertexFormat_Sint32x2,
		Sint32x3 = RHIVertexFormat_Sint32x3,
		Sint32x4 = RHIVertexFormat_Sint32x4,
		Unorm10_10_10_2 = RHIVertexFormat_Unorm10_10_10_2,
	};
	static_assert(sizeof(RHIVertexFormat) == sizeof(VertexFormat), "sizeof mismatch for VertexFormat");
	static_assert(alignof(RHIVertexFormat) == alignof(VertexFormat), "alignof mismatch for VertexFormat");

	enum class IndexFormat : uint32_t
	{
		Uint16 = RHIIndexFormat_Uint16,
		Uint32 = RHIIndexFormat_Uint32
	};
	static_assert(sizeof(RHIIndexFormat) == sizeof(IndexFormat), "sizeof mismatch for IndexFormat");
	static_assert(alignof(RHIIndexFormat) == alignof(IndexFormat), "alignof mismatch for IndexFormat");

	enum class TextureUsage : uint32_t
	{
		None = RHITextureUsage_None,
		CopySrc = RHITextureUsage_CopySrc,
		CopyDst = RHITextureUsage_CopyDst,
		SampledBinding = RHITextureUsage_SampledBinding,
		StorageBinding = RHITextureUsage_StorageBinding,
		RenderAttachment = RHITextureUsage_RenderAttachment
	};
	ENUM_CLASS_FLAG_OPERATORS(TextureUsage);
	static_assert(sizeof(RHITextureUsage) == sizeof(TextureUsage), "sizeof mismatch for TextureUsage");
	static_assert(alignof(RHITextureUsage) == alignof(TextureUsage), "alignof mismatch for TextureUsage");

	enum class TextureAspect : uint32_t
	{
		None = RHITextureAspect_None,
		All = RHITextureAspect_All,
		Depth = RHITextureAspect_Depth,
		Stencil = RHITextureAspect_Stencil,
		Plane0 = RHITextureAspect_Plane0,
		Plane1 = RHITextureAspect_Plane1,
		Plane2 = RHITextureAspect_Plane2
	};
	ENUM_CLASS_FLAG_OPERATORS(TextureAspect);
	static_assert(sizeof(RHITextureAspect) == sizeof(TextureAspect), "sizeof mismatch for TextureAspect");
	static_assert(alignof(RHITextureAspect) == alignof(TextureAspect), "alignof mismatch for TextureAspect");

	enum class BlendFactor : uint32_t
	{
		Zero = RHIBlendFactor_Zero,
		One = RHIBlendFactor_One,
		SrcColor = RHIBlendFactor_SrcColor,
		OneMinusSrcColor = RHIBlendFactor_OneMinusSrcColor,
		DstColor = RHIBlendFactor_DstColor,
		OneMinusDstColor = RHIBlendFactor_OneMinusDstColor,
		SrcAlpha = RHIBlendFactor_SrcAlpha,
		OneMinusSrcAlpha = RHIBlendFactor_OneMinusSrcAlpha,
		DstAlpha = RHIBlendFactor_DstAlpha,
		OneMinusDstAlpha = RHIBlendFactor_OneMinusDstAlpha,
		ConstantColor = RHIBlendFactor_ConstantColor,
		OneMinusConstantColor = RHIBlendFactor_OneMinusConstantColor,
		ConstantAlpha = RHIBlendFactor_ConstantAlpha,
		OneMinusConstantAlpha = RHIBlendFactor_OneMinusConstantAlpha,
		SrcAlphaSaturate = RHIBlendFactor_SrcAlphaSaturate,
		Src1Color = RHIBlendFactor_Src1Color,
		OneMinusSrc1Color = RHIBlendFactor_OneMinusSrc1Color,
		Src1Alpha = RHIBlendFactor_Src1Alpha,
		OneMinusSrc1Alpha = RHIBlendFactor_OneMinusSrc1Alpha
	};
	static_assert(sizeof(RHIBlendFactor) == sizeof(BlendFactor), "sizeof mismatch for BlendFactor");
	static_assert(alignof(RHIBlendFactor) == alignof(BlendFactor), "alignof mismatch for BlendFactor");

	enum class BlendOp : uint32_t
	{
		Add = RHIBlendOp_Add,
		Subrtact = RHIBlendOp_Subrtact,
		ReverseSubtract = RHIBlendOp_ReverseSubtract,
		Min = RHIBlendOp_Min,
		Max = RHIBlendOp_Max
	};
	static_assert(sizeof(RHIBlendOp) == sizeof(BlendOp), "sizeof mismatch for BlendOp");
	static_assert(alignof(RHIBlendOp) == alignof(BlendOp), "alignof mismatch for BlendOp");

	enum class ColorMask : uint32_t
	{
		// These values are equal to their counterparts in DX11, DX12, and Vulkan.
		Red = RHIColorMask_Red,
		Green = RHIColorMask_Green,
		Blue = RHIColorMask_Blue,
		Alpha = RHIColorMask_Alpha,
		All = RHIColorMask_All
	};
	ENUM_CLASS_FLAG_OPERATORS(ColorMask);
	static_assert(sizeof(RHIColorMask) == sizeof(ColorMask), "sizeof mismatch for ColorMask");
	static_assert(alignof(RHIColorMask) == alignof(ColorMask), "alignof mismatch for ColorMask");

	enum class SamplerAddressMode : uint32_t
	{
		ClampToEdge = RHISamplerAddressMode_ClampToEdge,
		Repeat = RHISamplerAddressMode_Repeat,
		ClampToBorder = RHISamplerAddressMode_ClampToBorder,
		MirroredRepeat = RHISamplerAddressMode_MirroredRepeat,
		MirrorClampToEdge = RHISamplerAddressMode_MirrorClampToEdge
	};
	ENUM_CLASS_FLAG_OPERATORS(SamplerAddressMode);
	static_assert(sizeof(RHISamplerAddressMode) == sizeof(SamplerAddressMode), "sizeof mismatch for SamplerAddressMode");
	static_assert(alignof(RHISamplerAddressMode) == alignof(SamplerAddressMode), "alignof mismatch for SamplerAddressMode");

	enum class FilterMode : uint32_t
	{
		Linear = RHIFilterMode_Linear,
		Nearest = RHIFilterMode_Nearest
	};
	static_assert(sizeof(RHIFilterMode) == sizeof(FilterMode), "sizeof mismatch for FilterMode");
	static_assert(alignof(RHIFilterMode) == alignof(FilterMode), "alignof mismatch for FilterMode");

	enum class BorderColor : uint32_t
	{
		FloatOpaqueBlack = RHIBorderColor_FloatOpaqueBlack,
		FloatOpaqueWhite = RHIBorderColor_FloatOpaqueWhite,
		FloatTransparentBlack = RHIBorderColor_FloatTransparentBlack
	};
	static_assert(sizeof(RHIBorderColor) == sizeof(BorderColor), "sizeof mismatch for BorderColor");
	static_assert(alignof(RHIBorderColor) == alignof(BorderColor), "alignof mismatch for BorderColor");

	enum class BindingType : uint32_t
	{
		None = RHIBindingType_None,
		SampledTexture = RHIBindingType_SampledTexture, //SRV
		StorageTexture = RHIBindingType_StorageTexture, //URV
		ReadOnlyStorageTexture = RHIBindingType_ReadOnlyStorageTexture,
		UniformBuffer = RHIBindingType_UniformBuffer, // CBV
		StorageBuffer = RHIBindingType_StorageBuffer, // UAV
		ReadOnlyStorageBuffer = RHIBindingType_ReadOnlyStorageBuffer,
		Sampler = RHIBindingType_Sampler,
		CombinedTextureSampler = RHIBindingType_CombinedTextureSampler
	};
	static_assert(sizeof(RHIBindingType) == sizeof(BindingType), "sizeof mismatch for BindingType");
	static_assert(alignof(RHIBindingType) == alignof(BindingType), "alignof mismatch for BindingType");

	enum class ShaderStage : uint32_t
	{
		None = RHIShaderStage_None,
		Vertex = RHIShaderStage_Vertex,
		TessellationControl = RHIShaderStage_TessellationControl,
		TessellationEvaluation = RHIShaderStage_TessellationEvaluation,
		Geometry = RHIShaderStage_Geometry,
		Fragment = RHIShaderStage_Fragment,
		Task = RHIShaderStage_Task,
		Mesh = RHIShaderStage_Mesh,
		Compute = RHIShaderStage_Compute,
		AllGraphics = RHIShaderStage_AllGraphics,
		All = RHIShaderStage_All
	};
	ENUM_CLASS_FLAG_OPERATORS(ShaderStage);
	static_assert(sizeof(RHIShaderStage) == sizeof(ShaderStage), "sizeof mismatch for ShaderStage");
	static_assert(alignof(RHIShaderStage) == alignof(ShaderStage), "alignof mismatch for ShaderStage");

	enum class FillMode : uint32_t
	{
		Fill = RHIFillMode_Fill,
		Line = RHIFillMode_Line,
		Point = RHIFillMode_Point
	};
	static_assert(sizeof(RHIFillMode) == sizeof(FillMode), "sizeof mismatch for FillMode");
	static_assert(alignof(RHIFillMode) == alignof(FillMode), "alignof mismatch for FillMode");

	enum class CullMode : uint32_t
	{
		None = RHICullMode_None,
		Front = RHICullMode_Front,
		Back = RHICullMode_Back
	};
	static_assert(sizeof(RHICullMode) == sizeof(CullMode), "sizeof mismatch for CullMode");
	static_assert(alignof(RHICullMode) == alignof(CullMode), "alignof mismatch for CullMode");

	enum class FrontFace : uint32_t
	{
		FrontCounterClockwise = RHIFrontFace_FrontCounterClockwise,
		FrontClockwise = RHIFrontFace_FrontClockwise
	};
	static_assert(sizeof(RHIFrontFace) == sizeof(FrontFace), "sizeof mismatch for FrontFace");
	static_assert(alignof(RHIFrontFace) == alignof(FrontFace), "alignof mismatch for FrontFace");

	enum class CompareOp : uint32_t
	{
		Never = RHICompareOp_Never,
		Less = RHICompareOp_Less,
		Equal = RHICompareOp_Equal,
		LessOrEqual = RHICompareOp_LessOrEqual,
		Greater = RHICompareOp_Greater,
		NotEqual = RHICompareOp_NotEqual,
		GreaterOrEqual = RHICompareOp_GreaterOrEqual,
		Always = RHICompareOp_Always
	};
	static_assert(sizeof(RHICompareOp) == sizeof(CompareOp), "sizeof mismatch for CompareOp");
	static_assert(alignof(RHICompareOp) == alignof(CompareOp), "alignof mismatch for CompareOp");

	enum class StencilOp : uint32_t
	{
		Keep = RHIStencilOp_Keep,
		Zero = RHIStencilOp_Zero,
		Replace = RHIStencilOp_Replace,
		IncrementAndClamp = RHIStencilOp_IncrementAndClamp,
		DecrementAndClamp = RHIStencilOp_DecrementAndClamp,
		Invert = RHIStencilOp_Invert,
		IncrementAndWrap = RHIStencilOp_IncrementAndWrap,
		DecrementAndWrap = RHIStencilOp_DecrementAndWrap
	};
	static_assert(sizeof(RHIStencilOp) == sizeof(StencilOp), "sizeof mismatch for StencilOp");
	static_assert(alignof(RHIStencilOp) == alignof(StencilOp), "alignof mismatch for StencilOp");

	enum class PrimitiveType : uint32_t
	{
		PointList = RHIPrimitiveType_PointList,
		LineList = RHIPrimitiveType_LineList,
		LineStrip = RHIPrimitiveType_LineStrip,
		TriangleList = RHIPrimitiveType_TriangleList,
		TriangleStrip = RHIPrimitiveType_TriangleStrip,
		TriangleFan = RHIPrimitiveType_TriangleFan,
		PatchList = RHIPrimitiveType_PatchList
	};
	static_assert(sizeof(RHIPrimitiveType) == sizeof(PrimitiveType), "sizeof mismatch for PrimitiveType");
	static_assert(alignof(RHIPrimitiveType) == alignof(PrimitiveType), "alignof mismatch for PrimitiveType");

	enum class VertexInputRate : uint32_t
	{
		Vertex = RHIVertexInputRate_Vertex,
		Instance = RHIVertexInputRate_Instance
	};
	static_assert(sizeof(RHIVertexInputRate) == sizeof(VertexInputRate), "sizeof mismatch for VertexInputRate");
	static_assert(alignof(RHIVertexInputRate) == alignof(VertexInputRate), "alignof mismatch for VertexInputRate");

	enum class QueueType : uint32_t
	{
		Graphics = RHIQueueType_Graphics,
		Compute = RHIQueueType_Compute,
		Transfer = RHIQueueType_Transfer,
		Undefined = RHIQueueType_Undefined
	};
	static_assert(sizeof(RHIQueueType) == sizeof(QueueType), "sizeof mismatch for QueueType");
	static_assert(alignof(RHIQueueType) == alignof(QueueType), "alignof mismatch for QueueType");

	enum class LoadOp : uint32_t
	{
		DontCare = RHILoadOp_DontCare,
		Load = RHILoadOp_Load,
		Clear = RHILoadOp_Clear
	};
	static_assert(sizeof(RHILoadOp) == sizeof(LoadOp), "sizeof mismatch for LoadOp");
	static_assert(alignof(RHILoadOp) == alignof(LoadOp), "alignof mismatch for LoadOp");

	enum class StoreOp : uint32_t
	{
		Store = RHIStoreOp_Store,
		Discard = RHIStoreOp_Discard
	};
	static_assert(sizeof(RHIStoreOp) == sizeof(StoreOp), "sizeof mismatch for StoreOp");
	static_assert(alignof(RHIStoreOp) == alignof(StoreOp), "alignof mismatch for StoreOp");

	enum class LoggingSeverity : uint32_t
	{
		Verbose = RHILoggingSeverity_Verbose,
		Info = RHILoggingSeverity_Info,
		Warning = RHILoggingSeverity_Warning,
		Error = RHILoggingSeverity_Error,
		Fatal = RHILoggingSeverity_Fatal
	};
	static_assert(sizeof(RHILoggingSeverity) == sizeof(LoggingSeverity), "sizeof mismatch for LoggingSeverity");
	static_assert(alignof(RHILoggingSeverity) == alignof(LoggingSeverity), "alignof mismatch for LoggingSeverity");

	enum class FeatureName : uint32_t
	{
		ShaderInt16 = RHIFeatureName_ShaderInt16,
		ShaderInt64 = RHIFeatureName_ShaderInt64,
		ShaderFloat64 = RHIFeatureName_ShaderFloat64,
		SampleRateShading = RHIFeatureName_SampleRateShading,
		SamplerAnisotropy = RHIFeatureName_SamplerAnisotropy,
		TextureCompressionBC = RHIFeatureName_TextureCompressionBC,
		TextureCompressionETC2 = RHIFeatureName_TextureCompressionETC2,
		TextureCompressionASTC = RHIFeatureName_TextureCompressionASTC,
		GeometryShader = RHIFeatureName_GeometryShader,
		TessellationShader = RHIFeatureName_TessellationShader,
		MultiViewport = RHIFeatureName_MultiViewport,
		MultiDrawIndirect = RHIFeatureName_MultiDrawIndirect,
		DepthBiasClamp = RHIFeatureName_DepthBiasClamp,
		DepthClamp = RHIFeatureName_DepthClamp,
		R8UnormStorage = RHIFeatureName_R8UnormStorage
	};
	static_assert(sizeof(RHIFeatureName) == sizeof(FeatureName), "sizeof mismatch for FeatureName");
	static_assert(alignof(RHIFeatureName) == alignof(FeatureName), "alignof mismatch for FeatureName");

	enum class BackendType : uint32_t
	{
		Vulkan = RHIBackendType_Vulkan
	};
	static_assert(sizeof(RHIBackendType) == sizeof(BackendType), "sizeof mismatch for BackendType");
	static_assert(alignof(RHIBackendType) == alignof(BackendType), "alignof mismatch for BackendType");

	enum class AdapterType : uint32_t
	{
		DiscreteGPU = RHIAdapterType_DiscreteGPU,
		IntegratedGPU = RHIAdapterType_IntegratedGPU,
		VirtualGPU = RHIAdapterType_VirtualGPU,
		Cpu = RHIAdapterType_Cpu,
		Unknown = RHIAdapterType_Unknown
	};
	static_assert(sizeof(RHIAdapterType) == sizeof(AdapterType), "sizeof mismatch for AdapterType");
	static_assert(alignof(RHIAdapterType) == alignof(AdapterType), "alignof mismatch for AdapterType");

	enum class ShareMode : uint32_t
	{
		Exclusive = RHIShareMode_Exclusive,
		Concurrent = RHIShareMode_Concurrent
	};
	static_assert(sizeof(RHIShareMode) == sizeof(ShareMode), "sizeof mismatch for ShareMode");
	static_assert(alignof(RHIShareMode) == alignof(ShareMode), "alignof mismatch for ShareMode");

	enum class PresentMode : uint32_t
	{
		Fifo = RHIPresentMode_Fifo,
		FifoRelaxed = RHIPresentMode_FifoRelaxed,
		Immediate = RHIPresentMode_Immediate,
		Mailbox = RHIPresentMode_Mailbox
	};
	static_assert(sizeof(RHIPresentMode) == sizeof(PresentMode), "sizeof mismatch for PresentMode");
	static_assert(alignof(RHIPresentMode) == alignof(PresentMode), "alignof mismatch for PresentMode");

	enum class SurfaceAcquireNextTextureStatus : uint32_t
	{
		Success = RHISurfaceAcquireNextTextureStatus_Success,
		Timeout = RHISurfaceAcquireNextTextureStatus_Timeout,
		Outdated = RHISurfaceAcquireNextTextureStatus_Outdated,
		SurfaceLost = RHISurfaceAcquireNextTextureStatus_SurfaceLost,
		Error = RHISurfaceAcquireNextTextureStatus_Error
	};
	static_assert(sizeof(RHISurfaceAcquireNextTextureStatus) == sizeof(SurfaceAcquireNextTextureStatus), "sizeof mismatch for SurfaceAcquireNextTextureStatus");
	static_assert(alignof(RHISurfaceAcquireNextTextureStatus) == alignof(SurfaceAcquireNextTextureStatus), "alignof mismatch for SurfaceAcquireNextTextureStatus");
#undef ENUM_CLASS_FLAG_OPERATORS

	using BufferMapCallback = RHIBufferMapCallback;
	using LoggingCallback = RHILoggingCallback;

	class Adapter;
	class BindSet;
	class BindSetLayout;
	class Buffer;
	class CommandList;
	class CommandEncoder;
	class ComputePassEncoder;
	class ComputePipeline;
	class Device;
	class Instance;
	class PipelineLayout;
	//class QuerySet;
	class Queue;
	class RenderPassEncoder;
	class RenderPipeline;
	class Sampler;
	class ShaderModule;
	class Surface;
	class Texture;
	class TextureView;

	struct DeviceDesc;
	struct BindSetDesc;
	struct BindSetLayoutDesc;
	struct BufferDesc;
	struct ComputePipelineDesc;
	struct DeviceDesc;
	struct InstanceDesc;
	struct PipelineLayoutDesc;
	struct RenderPassDesc;
	struct RenderPipelineDesc;
	struct SamplerDesc;
	struct ShaderModuleDesc;
	struct SurfaceConfiguration;
	struct TextureDesc;
	struct TextureViewDesc;
	struct Origin3D;
	struct Extent3D;
	struct Color;
	struct Viewport;
	struct Rect;
	struct TextureDataLayout;
	struct TextureSlice;
	struct SpecializationConstant;
	struct BindSetLayoutEntry;
	struct BindSetEntry;
	struct PushConstantRange;
	struct ShaderState;
	struct VertexInputAttribute;
	struct ColorAttachmentBlendState;
	struct BlendState;
	struct StencilOpState;
	struct DepthStencilState;
	struct RasterState;
	struct SampleState;
	struct ColorAttachment;
	struct DepthStencilAattachment;
	struct AdapterInfo;
	struct Limits;
	struct TextureSubresourceRange;
	struct TextureSubresources;
	struct ResourceTransfer;


	template<typename Derived, typename CType>
	class ObjectBase
	{
	public:
		ObjectBase() = default;
		ObjectBase(CType handle) : mHandle(handle)
		{
			if (mHandle) Derived::AddRef(mHandle);
		}
		~ObjectBase()
		{
			if (mHandle) Derived::Release(mHandle);
		}

		ObjectBase(ObjectBase const& other) :
			ObjectBase(other.Get())
		{

		}
		Derived& operator=(ObjectBase const& other)
		{
			if (&other != this)
			{
				if (mHandle) Derived::Release(mHandle);
				mHandle = other.mHandle;
				if (mHandle) Derived::AddRef(mHandle);
			}

			return static_cast<Derived&>(*this);
		}

		ObjectBase(ObjectBase&& other)
		{
			mHandle = other.mHandle;
			other.mHandle = 0;
		}
		Derived& operator=(ObjectBase&& other)
		{
			if (&other != this)
			{
				if (mHandle) Derived::Release(mHandle);
				mHandle = other.mHandle;
				other.mHandle = 0;
			}

			return static_cast<Derived&>(*this);
		}

		ObjectBase(std::nullptr_t) {}
		Derived& operator=(std::nullptr_t)
		{
			if (mHandle != nullptr)
			{
				Derived::Release(mHandle);
				mHandle = nullptr;
			}
			return static_cast<Derived&>(*this);
		}

		bool operator==(std::nullptr_t) const
		{
			return mHandle == nullptr;
		}
		bool operator!=(std::nullptr_t) const
		{
			return mHandle != nullptr;
		}

		explicit operator bool() const
		{
			return mHandle != nullptr;
		}
		CType Get() const
		{
			return mHandle;
		}
		CType MoveToCHandle()
		{
			CType result = mHandle;
			mHandle = 0;
			return result;
		}
		static Derived Acquire(CType handle)
		{
			Derived result;
			result.mHandle = handle;
			return result;
		}

	protected:
		CType mHandle = nullptr;
	};

	class Adapter : public ObjectBase<Adapter, RHIAdapter>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;

		inline Device CreateDevice(const DeviceDesc& desc);
		inline void GetInfo(AdapterInfo* info) const;
		inline void GetLimits(Limits* limits) const;
		inline Instance GetInstance() const;
	private:
		friend ObjectBase<Adapter, RHIAdapter>;
		static inline void AddRef(RHIAdapter handle);
		static inline void Release(RHIAdapter handle);
	};

	class BindSet : public ObjectBase<BindSet, RHIBindSet>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
	private:
		friend ObjectBase<BindSet, RHIBindSet>;
		static inline void AddRef(RHIBindSet handle);
		static inline void Release(RHIBindSet handle);
	};

	class BindSetLayout : public ObjectBase<BindSetLayout, RHIBindSetLayout>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
	private:
		friend ObjectBase<BindSetLayout, RHIBindSetLayout>;
		static inline void AddRef(RHIBindSetLayout handle);
		static inline void Release(RHIBindSetLayout handle);
	};

	class Buffer : public ObjectBase<Buffer, RHIBuffer>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
		inline BufferUsage GetUsage() const;
		inline uint64_t GetSize() const;
		inline void* GetMappedPointer() const;
		inline void Destroy();
	private:
		friend ObjectBase<Buffer, RHIBuffer>;
		static inline void AddRef(RHIBuffer handle);
		static inline void Release(RHIBuffer handle);
	};

	class CommandList : public ObjectBase<CommandList, RHICommandList>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
	private:
		friend ObjectBase<CommandList, RHICommandList>;
		static inline void AddRef(RHICommandList handle);
		static inline void Release(RHICommandList handle);
	};

	class CommandEncoder : public ObjectBase<CommandEncoder, RHICommandEncoder>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
		inline void ClearBuffer(Buffer& buffer, uint32_t value, uint64_t offset = 0, uint64_t size = WHOLE_SIZE);
		inline void CopyBufferToBuffer(Buffer& srcBuffer, uint64_t srcOffset, Buffer& dstBuffer, uint64_t dstOffset, uint64_t dataSize);
		inline void CopyBufferToTexture(Buffer& srcBuffer, const TextureDataLayout& dataLayout, const TextureSlice& dstTextureSlice);
		inline void CopyTextureToBuffer(const TextureSlice& srcTextureSlice, Buffer& dstBuffer, const TextureDataLayout& dataLayout);
		inline void CopyTextureToTexture(const TextureSlice& srcTextureSlice, const TextureSlice& dstTextureSlice);
		inline void MapBufferAsync(Buffer& buffer, MapMode usage, BufferMapCallback callback, void* userData);
		inline void BeginDebugLabel(std::string_view label, const Color* color = nullptr);
		inline void EndDebugLabel();
		inline RenderPassEncoder BeginRenderPass(const RenderPassDesc& desc);
		inline ComputePassEncoder BeginComputePass();
		inline CommandList Finish();
	private:
		friend ObjectBase<CommandEncoder, RHICommandEncoder>;
		static inline void AddRef(RHICommandEncoder handle);
		static inline void Release(RHICommandEncoder handle);
	};

	class ComputePassEncoder : public ObjectBase<ComputePassEncoder, RHIComputePassEncoder>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
		inline void SetPipeline(ComputePipeline& pipeline);
		inline void Dispatch(uint32_t groupCountX, uint32_t groupCountY = 1, uint32_t groupCountZ = 1);
		inline void DispatchIndirect(Buffer& indirectBuffer, uint64_t indirectOffset);
		inline void SetBindSet(BindSet& set, uint32_t setIndex, uint32_t dynamicOffsetCount = 0, const uint32_t* dynamicOffsets = nullptr);
		inline void End();
		inline void SetPushConstant(const void* data, uint32_t size);
		inline void BeginDebugLabel(std::string_view label, const Color* color = nullptr);
		inline void EndDebugLabel();
	private:
		friend ObjectBase<ComputePassEncoder, RHIComputePassEncoder>;
		static inline void AddRef(RHIComputePassEncoder handle);
		static inline void Release(RHIComputePassEncoder handle);
	};

	class ComputePipeline : public ObjectBase<ComputePipeline, RHIComputePipeline> 
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
	private:
		friend ObjectBase<ComputePipeline, RHIComputePipeline>;
		static inline void AddRef(RHIComputePipeline handle);
		static inline void Release(RHIComputePipeline handle);
	};

	class Device : public ObjectBase<Device, RHIDevice>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;

		inline Adapter GetAdapter() const;
		inline Queue GetQueue(QueueType queueType);
		inline PipelineLayout CreatePipelineLayout(const PipelineLayoutDesc& desc);
		inline RenderPipeline CreateRenderPipeline(const RenderPipelineDesc& desc);
		inline ComputePipeline CreateComputePipeline(const ComputePipelineDesc& desc);
		inline BindSetLayout CreateBindSetLayout(const BindSetLayoutDesc& desc);
		inline BindSet CreateBindSet(const BindSetDesc& desc);
		inline Texture CreateTexture(const TextureDesc& desc);
		inline Buffer CreateBuffer(const BufferDesc& desc);
		inline ShaderModule CreateShader(const ShaderModuleDesc& desc);
		inline Sampler CreateSampler(const SamplerDesc& desc);
		inline CommandEncoder CreateCommandEncoder();
		inline void Tick();
	private:
		friend ObjectBase<Device, RHIDevice>;
		static inline void AddRef(RHIDevice handle);
		static inline void Release(RHIDevice handle);
	};

	class Instance : public ObjectBase<Instance, RHIInstance>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
		inline void EnumerateAdapters(Adapter* const adapters, uint32_t* adapterCount);
		inline std::vector<Adapter> EnumerateAdapters();
		inline Surface CreateSurfaceFromWindowsHWND(void* hwnd, void* hinstance);
	private:
		friend ObjectBase<Instance, RHIInstance>;
		static inline void AddRef(RHIInstance handle);
		static inline void Release(RHIInstance handle);
	};

	class PipelineLayout : public ObjectBase<PipelineLayout, RHIPipelineLayout>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
		inline BindSetLayout GetBindSetLayout(uint32_t bindSetIndex) const;
	private:
		friend ObjectBase<PipelineLayout, RHIPipelineLayout>;
		static inline void AddRef(RHIPipelineLayout handle);
		static inline void Release(RHIPipelineLayout handle);
	};

	class Queue : public ObjectBase<Queue, RHIQueue>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
		inline void WriteBuffer(Buffer& buffer, const void* data, uint64_t dataSize, uint64_t offset);
		inline void WriteTexture(const TextureSlice& dstTexture, const void* data, size_t dataSize, const TextureDataLayout& dataLayout);
		inline void WaitFor(Queue queue, uint64_t submitSerial);
		inline uint64_t Submit(CommandList const* commands, uint32_t commandListCount, ResourceTransfer const* transfers = nullptr, uint32_t transferCount = 0);
	private:
		friend ObjectBase<Queue, RHIQueue>;
		static inline void AddRef(RHIQueue handle);
		static inline void Release(RHIQueue handle);
	};

	class RenderPassEncoder : public ObjectBase<RenderPassEncoder, RHIRenderPassEncoder>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
		inline void SetPipeline(RenderPipeline& pipeline);
		inline void SetVertexBuffers(uint32_t firstSlot, uint32_t bufferCount, Buffer const* buffers, uint64_t* offsets = nullptr);
		inline void SetIndexBuffer(Buffer& buffer, IndexFormat indexFormat, uint64_t offset = 0, uint64_t size = WHOLE_SIZE);
		inline void SetScissorRect(uint32_t firstScissor, const Rect* scissors, uint32_t scissorCount);
		inline void SetStencilReference(uint32_t reference);
		inline void SetBlendConstant(const Color& blendConstants);
		inline void SetViewport(uint32_t firstViewport, Viewport const* viewports, uint32_t viewportCount);
		inline void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
		inline void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t baseVertex = 0, uint32_t firstInstance = 0);
		inline void DrawIndirect(Buffer& indirectBuffer, uint64_t indirectOffset);
		inline void DrawIndexedIndirect(Buffer& indirectBuffer, uint64_t indirectOffset);
		inline void MultiDrawIndirect(Buffer& indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, Buffer drawCountBuffer = nullptr, uint64_t drawCountBufferOffset = 0);
		inline void MultiDrawIndexedIndirect(Buffer& indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, Buffer drawCountBuffer = nullptr, uint64_t drawCountBufferOffset = 0);
		inline void SetBindSet(BindSet& set, uint32_t setIndex, uint32_t dynamicOffsetCount = 0, const uint32_t* dynamicOffsets = nullptr);
		inline void End();
		inline void SetPushConstant(const void* data, uint32_t size);
		inline void BeginDebugLabel(std::string_view label, const Color* color = nullptr);
		inline void EndDebugLabel();
	private:
		friend ObjectBase<RenderPassEncoder, RHIRenderPassEncoder>;
		static inline void AddRef(RHIRenderPassEncoder handle);
		static inline void Release(RHIRenderPassEncoder handle);
	};

	class RenderPipeline : public ObjectBase<RenderPipeline, RHIRenderPipeline>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
	private:
		friend ObjectBase<RenderPipeline, RHIRenderPipeline>;
		static inline void AddRef(RHIRenderPipeline handle);
		static inline void Release(RHIRenderPipeline handle);
	};

	class Sampler : public ObjectBase<Sampler, RHISampler>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
	private:
		friend ObjectBase<Sampler, RHISampler>;
		static inline void AddRef(RHISampler handle);
		static inline void Release(RHISampler handle);
	};

	class ShaderModule : public ObjectBase<ShaderModule, RHIShaderModule>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
	private:
		friend ObjectBase<ShaderModule, RHIShaderModule>;
		static inline void AddRef(RHIShaderModule handle);
		static inline void Release(RHIShaderModule handle);
	};


	class Surface : public ObjectBase<Surface, RHISurface>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
		inline void Configure(const SurfaceConfiguration& config);
		inline SurfaceAcquireNextTextureStatus AcquireNextTexture();
		inline Texture GetCurrentTexture();
		inline TextureView GetCurrentTextureView();
		inline TextureFormat GetSwapChainFormat();
		inline void Present();
		inline void Unconfigure();
	private:
		friend ObjectBase<Surface, RHISurface>;
		static inline void AddRef(RHISurface handle);
		static inline void Release(RHISurface handle);
	};

	class Texture : public ObjectBase<Texture, RHITexture>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
		inline uint32_t GetWidth() const;
		inline uint32_t GetHeight() const;
		inline uint32_t GetDepthOrArrayLayers() const;
		inline uint32_t GetMipLevelCount() const;
		inline uint32_t GetSampleCount() const;
		inline TextureDimension GetDimension() const;
		inline TextureFormat GetFormat() const;
		inline TextureUsage GetUsage() const;
		inline TextureView CreateView(const TextureViewDesc* desc = nullptr);
	private:
		friend ObjectBase<Texture, RHITexture>;
		static inline void AddRef(RHITexture handle);
		static inline void Release(RHITexture handle);
	};

	class TextureView : public ObjectBase<TextureView, RHITextureView>
	{
	public:
		using ObjectBase::ObjectBase;
		using ObjectBase::operator=;
	private:
		friend ObjectBase<TextureView, RHITextureView>;
		static inline void AddRef(RHITextureView handle);
		static inline void Release(RHITextureView handle);
	};
	// Adapter implementations
	Device Adapter::CreateDevice(const DeviceDesc& desc)
	{
		RHIDevice result = rhiAdapterCreateDevice(Get(), reinterpret_cast<const RHIDeviceDesc*>(&desc));
		return Device::Acquire(result);
	}
	void Adapter::GetInfo(AdapterInfo* info) const
	{
		rhiAdapterGetInfo(Get(), reinterpret_cast<RHIAdapterInfo*>(info));
	}
	void Adapter::GetLimits(Limits* limits) const
	{
		rhiAdapterGetLimits(Get(), reinterpret_cast<RHILimits*>(limits));
	}
	Instance Adapter::GetInstance() const
	{
		RHIInstance result = rhiAdapterGetInstance(Get());
		return Instance::Acquire(result);
	}
	void Adapter::AddRef(RHIAdapter handle)
	{
		if (handle != nullptr)
		{
			rhiAdapterAddRef(handle);
		}
	}
	void Adapter::Release(RHIAdapter handle)
	{
		if (handle != nullptr)
		{
			rhiAdapterRelease(handle);
		}
	}
	// BindSet implementations
	void BindSet::AddRef(RHIBindSet handle)
	{
		if (handle != nullptr)
		{
			rhiBindSetAddRef(handle);
		}
	}
	void BindSet::Release(RHIBindSet handle)
	{
		if (handle != nullptr)
		{
			rhiBindSetRelease(handle);
		}
	}
	// BindSetLayout implementations
	void BindSetLayout::AddRef(RHIBindSetLayout handle)
	{
		if (handle != nullptr)
		{
			rhiBindSetLayoutAddRef(handle);
		}
	}
	void BindSetLayout::Release(RHIBindSetLayout handle)
	{
		if (handle != nullptr)
		{
			rhiBindSetLayoutRelease(handle);
		}
	}
	// Buffer implementations
	BufferUsage Buffer::GetUsage() const
	{
		RHIBufferUsage result = rhiBufferGetUsage(Get());
		return static_cast<BufferUsage>(result);
	}
	uint64_t Buffer::GetSize() const
	{
		return rhiBufferGetSize(Get());
	}
	void* Buffer::GetMappedPointer() const
	{
		return rhiBufferGetMappedPointer(Get());
	}
	void Buffer::Destroy()
	{
		rhiBufferDestroy(Get());
	}
	void Buffer::AddRef(RHIBuffer handle)
	{
		if (handle != nullptr)
		{
			rhiBufferAddRef(handle);
		}
	}
	void Buffer::Release(RHIBuffer handle)
	{
		if (handle != nullptr)
		{
			rhiBufferRelease(handle);
		}
	}
	// CommandList implementations
	void CommandList::AddRef(RHICommandList handle)
	{
		if (handle != nullptr)
		{
			rhiCommandListAddRef(handle);
		}
	}
	void CommandList::Release(RHICommandList handle)
	{
		if (handle != nullptr)
		{
			rhiCommandListRelease(handle);
		}
	}
	// CommandEncoder implementations
	void CommandEncoder::ClearBuffer(Buffer& buffer, uint32_t value, uint64_t offset, uint64_t size)
	{
		rhiCommandEncoderClearBuffer(Get(), buffer.Get(), value, offset, size);
	}
	void CommandEncoder::CopyBufferToBuffer(Buffer& srcBuffer, uint64_t srcOffset, Buffer& dstBuffer, uint64_t dstOffset, uint64_t dataSize)
	{
		rhiCommandEncoderCopyBufferToBuffer(Get(), srcBuffer.Get(), srcOffset, dstBuffer.Get(), dstOffset, dataSize);
	}
	void CommandEncoder::CopyBufferToTexture(Buffer& srcBuffer, const TextureDataLayout& dataLayout, const TextureSlice& dstTextureSlice)
	{
		rhiCommandEncoderCopyBufferToTexture(Get(), srcBuffer.Get(), reinterpret_cast<const RHITextureDataLayout*>(&dataLayout), reinterpret_cast<const RHITextureSlice*>(&dstTextureSlice));
	}
	void CommandEncoder::CopyTextureToBuffer(const TextureSlice& srcTextureSlice, Buffer& dstBuffer, const TextureDataLayout& dataLayout)
	{
		rhiCommandEncoderCopyTextureToBuffer(Get(), reinterpret_cast<const RHITextureSlice*>(&srcTextureSlice), dstBuffer.Get(), reinterpret_cast<const RHITextureDataLayout*>(&dataLayout));
	}
	void CommandEncoder::CopyTextureToTexture(const TextureSlice& srcTextureSlice, const TextureSlice& dstTextureSlice)
	{
		rhiCommandEncoderCopyTextureToTexture(Get(), reinterpret_cast<const RHITextureSlice*>(&srcTextureSlice), reinterpret_cast<const RHITextureSlice*>(&dstTextureSlice));
	}
	void CommandEncoder::MapBufferAsync(Buffer& buffer, MapMode usage, BufferMapCallback callback, void* userData)
	{
		rhiCommandEncoderMapBufferAsync(Get(), buffer.Get(), static_cast<RHIMapMode>(usage), callback, userData);
	}
	void CommandEncoder::BeginDebugLabel(std::string_view label, const Color* color)
	{
		rhiCommandEncoderBeginDebugLabel(Get(), { label.data(), label.size() }, reinterpret_cast<const RHIColor*>(color));
	}
	void CommandEncoder::EndDebugLabel()
	{
		rhiCommandEncoderEndDebugLabel(Get());
	}
	RenderPassEncoder CommandEncoder::BeginRenderPass(const RenderPassDesc& desc)
	{
		RHIRenderPassEncoder result = rhiCommandEncoderBeginRenderPass(Get(), reinterpret_cast<const RHIRenderPassDesc*>(&desc));
		return RenderPassEncoder::Acquire(result);
	}
	ComputePassEncoder CommandEncoder::BeginComputePass()
	{
		RHIComputePassEncoder result = rhiCommandEncoderBeginComputePass(Get());
		return ComputePassEncoder::Acquire(result);
	}
	inline CommandList CommandEncoder::Finish()
	{
		RHICommandList result = rhiCommandEncoderFinish(Get());
		return CommandList::Acquire(result);
	}
	void CommandEncoder::AddRef(RHICommandEncoder handle)
	{
		if (handle != nullptr)
		{
			rhiCommandEncoderAddRef(handle);
		}
	}
	void CommandEncoder::Release(RHICommandEncoder handle)
	{
		if (handle != nullptr)
		{
			rhiCommandEncoderRelease(handle);
		}
	}
	// ComputePassEncoder implementations
	void ComputePassEncoder::SetPipeline(ComputePipeline& pipeline)
	{
		rhiComputePassEncoderSetPipeline(Get(), pipeline.Get());
	}
	void ComputePassEncoder::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	{
		rhiComputePassEncoderDispatch(Get(), groupCountX, groupCountY, groupCountZ);
	}
	void ComputePassEncoder::DispatchIndirect(Buffer& indirectBuffer, uint64_t indirectOffset)
	{
		rhiComputePassEncoderDispatchIndirect(Get(), indirectBuffer.Get(), indirectOffset);
	}
	void ComputePassEncoder::SetBindSet(BindSet& set, uint32_t setIndex, uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets)
	{
		rhiComputePassEncoderSetBindSet(Get(), set.Get(), setIndex, dynamicOffsetCount, dynamicOffsets);
	}
	void ComputePassEncoder::End()
	{
		rhiComputePassEncoderEnd(Get());
	}
	void ComputePassEncoder::SetPushConstant(const void* data, uint32_t size)
	{
		rhiComputePassEncoderSetPushConstant(Get(), data, size);
	}
	void ComputePassEncoder::BeginDebugLabel(std::string_view label, const Color* color)
	{
		rhiComputePassEncoderBeginDebugLabel(Get(), { label.data(), label.size() }, reinterpret_cast<const RHIColor*>(color));
	}
	void ComputePassEncoder::EndDebugLabel()
	{
		rhiComputePassEncoderEnd(Get());
	}
	void ComputePassEncoder::AddRef(RHIComputePassEncoder handle)
	{
		if (handle != nullptr)
		{
			rhiComputePassEncoderAddRef(handle);
		}
	}
	void ComputePassEncoder::Release(RHIComputePassEncoder handle)
	{
		if (handle != nullptr)
		{
			rhiComputePassEncoderRelease(handle);
		}
	}
	// ComputePipeline implementations
	void ComputePipeline::AddRef(RHIComputePipeline handle)
	{
		if (handle != nullptr)
		{
			rhiComputePipelineAddRef(handle);
		}
	}
	void ComputePipeline::Release(RHIComputePipeline handle)
	{
		if (handle != nullptr)
		{
			rhiComputePipelineRelease(handle);
		}
	}

	// Device implementations
	Adapter Device::GetAdapter() const
	{
		RHIAdapter result = rhiDeviceGetAdapter(Get());
		return Adapter::Acquire(result);
	}
	Queue Device::GetQueue(QueueType queueType)
	{
		RHIQueue result = rhiDeviceGetQueue(Get(), static_cast<RHIQueueType>(queueType));
		return Queue::Acquire(result);
	}
	PipelineLayout Device::CreatePipelineLayout(const PipelineLayoutDesc& desc)
	{
		RHIPipelineLayout result = rhiCreatePipelineLayout(Get(), reinterpret_cast<const RHIPipelineLayoutDesc*>(&desc));
		return PipelineLayout::Acquire(result);
	}
	RenderPipeline Device::CreateRenderPipeline(const RenderPipelineDesc& desc)
	{
		RHIRenderPipeline result = rhiDeviceCreateRenderPipeline(Get(), reinterpret_cast<const RHIRenderPipelineDesc*>(&desc));
		return RenderPipeline::Acquire(result);
	}
	ComputePipeline Device::CreateComputePipeline(const ComputePipelineDesc& desc)
	{
		RHIComputePipeline result = rhiDeviceCreateComputePipeline(Get(), reinterpret_cast<const RHIComputePipelineDesc*>(&desc));
		return ComputePipeline::Acquire(result);
	}
	BindSetLayout Device::CreateBindSetLayout(const BindSetLayoutDesc& desc)
	{
		RHIBindSetLayout result = rhiDeviceCreateBindSetLayout(Get(), reinterpret_cast<const RHIBindSetLayoutDesc*>(&desc));
		return BindSetLayout::Acquire(result);
	}
	BindSet Device::CreateBindSet(const BindSetDesc& desc)
	{
		RHIBindSet result = rhiDeviceCreateBindSet(Get(), reinterpret_cast<const RHIBindSetDesc*>(&desc));
		return BindSet::Acquire(result);
	}
	Texture Device::CreateTexture(const TextureDesc& desc)
	{
		RHITexture result = rhiDeviceCreateTexture(Get(), reinterpret_cast<const RHITextureDesc*>(&desc));
		return Texture::Acquire(result);
	}
	Buffer Device::CreateBuffer(const BufferDesc& desc)
	{
		RHIBuffer result = rhiDeviceCreateBuffer(Get(), reinterpret_cast<const RHIBufferDesc*>(&desc));
		return Buffer::Acquire(result);
	}
	ShaderModule Device::CreateShader(const ShaderModuleDesc& desc)
	{
		RHIShaderModule result = rhiDeviceCreateShader(Get(), reinterpret_cast<const RHIShaderModuleDesc*>(&desc));
		return ShaderModule::Acquire(result);
	}
	Sampler Device::CreateSampler(const SamplerDesc& desc)
	{
		RHISampler result = rhiDeviceCreateSampler(Get(), reinterpret_cast<const RHISamplerDesc*>(&desc));
		return Sampler::Acquire(result);
	}
	CommandEncoder Device::CreateCommandEncoder()
	{
		RHICommandEncoder result = rhiDeviceCreateCommandEncoder(Get());
		return CommandEncoder::Acquire(result);
	}
	void Device::Tick()
	{
		rhiDeviceTick(Get());
	}
	void Device::AddRef(RHIDevice handle)
	{
		if (handle != nullptr)
		{
			rhiDeviceAddRef(handle);
		}
	}
	void Device::Release(RHIDevice handle)
	{
		if (handle != nullptr)
		{
			rhiDeviceRelease(handle);
		}
	}
	// Instance implementations
	void Instance::EnumerateAdapters(Adapter* const adapters, uint32_t* adapterCount)
	{
		rhiInstanceEnumerateAdapters(Get(), reinterpret_cast<RHIAdapter* const>(adapters), adapterCount);
	}
	std::vector<Adapter> Instance::EnumerateAdapters()
	{
		uint32_t adapterCount = 0;
		rhiInstanceEnumerateAdapters(Get(), nullptr, &adapterCount);
		std::vector<Adapter> adapters(adapterCount);
		rhiInstanceEnumerateAdapters(Get(), reinterpret_cast<RHIAdapter*>(adapters.data()), &adapterCount);
		return adapters;
	}
	Surface Instance::CreateSurfaceFromWindowsHWND(void* hwnd, void* hinstance)
	{
		RHISurface result = rhiCreateSurfaceFromWindowsHWND(Get(), hwnd, hinstance);
		return Surface::Acquire(result);
	}
	void Instance::AddRef(RHIInstance handle)
	{
		if (handle != nullptr)
		{
			rhiInstanceAddRef(handle);
		}
	}
	void Instance::Release(RHIInstance handle)
	{
		if (handle != nullptr)
		{
			rhiInstanceRelease(handle);
		}
	}
	// PipelineLayout implementations
	BindSetLayout PipelineLayout::GetBindSetLayout(uint32_t bindSetIndex) const
	{
		RHIBindSetLayout result = rhiPipelineLayoutIGetBindSetLayout(Get(), bindSetIndex);
		return BindSetLayout::Acquire(result);
	}
	void PipelineLayout::AddRef(RHIPipelineLayout handle)
	{
		if (handle != nullptr)
		{
			rhiPipelineLayoutAddRef(handle);
		}
	}
	void PipelineLayout::Release(RHIPipelineLayout handle)
	{
		if (handle != nullptr)
		{
			rhiPipelineLayoutRelease(handle);
		}
	}
	// Queue implementations
	void Queue::WriteBuffer(Buffer& buffer, const void* data, uint64_t dataSize, uint64_t offset)
	{
		rhiQueueWriteBuffer(Get(), buffer.Get(), data, dataSize, offset);
	}
	void Queue::WriteTexture(const TextureSlice& dstTexture, const void* data, size_t dataSize, const TextureDataLayout& dataLayout)
	{
		rhiQueueWriteTexture(Get(), reinterpret_cast<const RHITextureSlice*>(&dstTexture), data, dataSize, reinterpret_cast<const RHITextureDataLayout*>(&dataLayout));
	}
	void Queue::WaitFor(Queue queue, uint64_t submitSerial)
	{
		rhiQueueWaitFor(Get(), queue.Get(), submitSerial);
	}
	uint64_t Queue::Submit(CommandList const* commands, uint32_t commandListCount, ResourceTransfer const* transfers, uint32_t transferCount)
	{
		return rhiQueueSubmit(Get(), reinterpret_cast<RHICommandList const*>(commands), commandListCount, reinterpret_cast<RHIResourceTransfer const*>(transfers), transferCount);
	}
	void Queue::AddRef(RHIQueue handle)
	{
		if (handle != nullptr)
		{
			rhiQueueAddRef(handle);
		}
	}
	void Queue::Release(RHIQueue handle)
	{
		if (handle != nullptr)
		{
			rhiQueueRelease(handle);
		}
	}
	// RenderPassEncoder implementations
	void RenderPassEncoder::SetPipeline(RenderPipeline& pipeline)
	{
		rhiRenderPassEncoderSetPipeline(Get(), pipeline.Get());
	}
	void RenderPassEncoder::SetVertexBuffers(uint32_t firstSlot, uint32_t bufferCount, Buffer const* buffers, uint64_t* offsets)
	{
		rhiRenderPassEncoderSetVertexBuffers(Get(), firstSlot, bufferCount, reinterpret_cast<RHIBuffer const*>(buffers), offsets);
	}
	void RenderPassEncoder::SetIndexBuffer(Buffer& buffer, IndexFormat indexFormat, uint64_t offset, uint64_t size)
	{
		rhiRenderPassEncoderSetIndexBuffer(Get(), buffer.Get(), offset, size, static_cast<RHIIndexFormat>(indexFormat));
	}
	void RenderPassEncoder::SetScissorRect(uint32_t firstScissor, const Rect* scissors, uint32_t scissorCount)
	{
		rhiRenderPassEncoderSetScissorRect(Get(), firstScissor, reinterpret_cast<const RHIRect*>(scissors), scissorCount);
	}
	void RenderPassEncoder::SetStencilReference(uint32_t reference)
	{
		rhiRenderPassEncoderSetStencilReference(Get(), reference);
	}
	void RenderPassEncoder::SetBlendConstant(const Color& blendConstants)
	{
		rhiRenderPassEncoderSetBlendConstant(Get(), reinterpret_cast<const RHIColor*>(&blendConstants));
	}
	void RenderPassEncoder::SetViewport(uint32_t firstViewport, Viewport const* viewports, uint32_t viewportCount)
	{
		rhiRenderPassEncoderSetViewport(Get(), firstViewport, reinterpret_cast<RHIViewport const*>(viewports), viewportCount);
	}
	void RenderPassEncoder::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		rhiRenderPassEncoderDraw(Get(), vertexCount, instanceCount, firstVertex, firstInstance);
	}
	void RenderPassEncoder::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t baseVertex, uint32_t firstInstance)
	{
		rhiRenderPassEncoderDrawIndexed(Get(), indexCount, instanceCount, firstIndex, baseVertex, firstInstance);
	}
	void RenderPassEncoder::DrawIndirect(Buffer& indirectBuffer, uint64_t indirectOffset)
	{
		rhiRenderPassEncoderDrawIndirect(Get(), indirectBuffer.Get(), indirectOffset);
	}
	void RenderPassEncoder::DrawIndexedIndirect(Buffer& indirectBuffer, uint64_t indirectOffset)
	{
		rhiRenderPassEncoderDrawIndexedIndirect(Get(), indirectBuffer.Get(), indirectOffset);
	}
	void RenderPassEncoder::MultiDrawIndirect(Buffer& indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, Buffer drawCountBuffer, uint64_t drawCountBufferOffset)
	{
		rhiRenderPassEncoderMultiDrawIndirect(Get(), indirectBuffer.Get(), indirectOffset, maxDrawCount, drawCountBuffer.Get(), drawCountBufferOffset);
	}
	void RenderPassEncoder::MultiDrawIndexedIndirect(Buffer& indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, Buffer drawCountBuffer, uint64_t drawCountBufferOffset)
	{
		rhiRenderPassEncoderMultiDrawIndexedIndirect(Get(), indirectBuffer.Get(), indirectOffset, maxDrawCount, drawCountBuffer.Get(), drawCountBufferOffset);
	}
	void RenderPassEncoder::SetBindSet(BindSet& set, uint32_t setIndex, uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets)
	{
		rhiRenderPassEncoderSetBindSet(Get(), set.Get(), setIndex, dynamicOffsetCount, dynamicOffsets);
	}
	void RenderPassEncoder::End()
	{
		rhiRenderPassEncoderEnd(Get());
	}
	void RenderPassEncoder::SetPushConstant(const void* data, uint32_t size)
	{
		rhiRenderPassEncoderSetPushConstant(Get(), data, size);
	}
	void RenderPassEncoder::BeginDebugLabel(std::string_view label, const Color* color)
	{
		rhiRenderPassEncoderBeginDebugLabel(Get(), { label.data(), label.size() }, reinterpret_cast<const RHIColor*>(color));
	}
	void RenderPassEncoder::EndDebugLabel()
	{
		rhiRenderPassEncoderEndDebugLabel(Get());
	}
	void RenderPassEncoder::AddRef(RHIRenderPassEncoder handle)
	{
		if (handle != nullptr)
		{
			rhiRenderPassEncoderAddRef(handle);
		}
	}
	void RenderPassEncoder::Release(RHIRenderPassEncoder handle)
	{
		if (handle != nullptr)
		{
			rhiRenderPassEncoderRelease(handle);
		}
	}
	// RenderPipeline implementations
	void RenderPipeline::AddRef(RHIRenderPipeline handle)
	{
		if (handle != nullptr)
		{
			rhiRenderPipelineAddRef(handle);
		}
	}
	void RenderPipeline::Release(RHIRenderPipeline handle)
	{
		if (handle != nullptr)
		{
			rhiRenderPipelineRelease(handle);
		}
	}
	// Sampler implementations
	void Sampler::AddRef(RHISampler handle)
	{
		if (handle != nullptr)
		{
			rhiSamplerAddRef(handle);
		}
	}
	void Sampler::Release(RHISampler handle)
	{
		if (handle != nullptr)
		{
			rhiSamplerRelease(handle);
		}
	}
	// ShaderModule implementations
	void ShaderModule::AddRef(RHIShaderModule handle)
	{
		if (handle != nullptr)
		{
			rhiShaderModuleAddRef(handle);
		}
	}
	void ShaderModule::Release(RHIShaderModule handle)
	{
		if (handle != nullptr)
		{
			rhiShaderModuleRelease(handle);
		}
	}
	// Surface implementations
	void Surface::Configure(const SurfaceConfiguration& config)
	{
		rhiSurfaceConfigure(Get(), reinterpret_cast<const RHISurfaceConfiguration*>(&config));
	}
	SurfaceAcquireNextTextureStatus Surface::AcquireNextTexture()
	{
		RHISurfaceAcquireNextTextureStatus result = rhiSurfaceAcquireNextTexture(Get());
		return static_cast<SurfaceAcquireNextTextureStatus>(result);
	}
	Texture Surface::GetCurrentTexture()
	{
		RHITexture result = rhiSurfaceGetCurrentTexture(Get());
		return Texture::Acquire(result);
	}
	TextureView Surface::GetCurrentTextureView()
	{
		RHITextureView result = rhiSurfaceGetCurrentTextureView(Get());
		return TextureView::Acquire(result);
	}
	TextureFormat Surface::GetSwapChainFormat()
	{
		RHITextureFormat result = rhiSurfaceGetSwapChainFormat(Get());
		return static_cast<TextureFormat>(result);
	}
	void Surface::Present()
	{
		rhiSurfacePresent(Get());
	}
	void Surface::Unconfigure()
	{
		rhiSurfaceUnconfigure(Get());
	}
	void Surface::AddRef(RHISurface handle)
	{
		if (handle != nullptr)
		{
			rhiSurfaceAddRef(handle);
		}
	}
	void Surface::Release(RHISurface handle)
	{
		if (handle != nullptr)
		{
			rhiSurfaceRelease(handle);
		}
	}
	// Texture implementations
	uint32_t Texture::GetWidth() const
	{
		return rhiTextureGetWidth(Get());
	}
	uint32_t Texture::GetHeight() const
	{
		return rhiTextureGetHeight(Get());
	}
	uint32_t Texture::GetDepthOrArrayLayers() const
	{
		return rhiTextureGetDepthOrArrayLayers(Get());
	}
	uint32_t Texture::GetMipLevelCount() const
	{
		return rhiTextureGetMipLevelCount(Get());
	}
	uint32_t Texture::GetSampleCount() const
	{
		return rhiTextureGetDimension(Get());
	}
	TextureDimension Texture::GetDimension() const
	{
		RHITextureDimension result = rhiTextureGetDimension(Get());
		return static_cast<TextureDimension>(result);
	}
	TextureFormat Texture::GetFormat() const
	{
		RHITextureFormat result = rhiTextureGetFormat(Get());
		return static_cast<TextureFormat>(result);
	}
	TextureUsage Texture::GetUsage() const
	{
		RHITextureUsage result = rhiTextureGetUsage(Get());
		return static_cast<TextureUsage>(result);
	}
	TextureView Texture::CreateView(const TextureViewDesc* desc)
	{
		RHITextureView result = rhiTextureCreateView(Get(), reinterpret_cast<const RHITextureViewDesc*>(desc));
		return TextureView::Acquire(result);
	}
	void Texture::AddRef(RHITexture handle)
	{
		if (handle != nullptr)
		{
			rhiTextureAddRef(handle);
		}
	}
	void Texture::Release(RHITexture handle)
	{
		if (handle != nullptr)
		{
			rhiTextureRelease(handle);
		}
	}
	// TextureView implementations
	void TextureView::AddRef(RHITextureView handle)
	{
		if (handle != nullptr)
		{
			rhiTextureViewAddRef(handle);
		}
	}
	void TextureView::Release(RHITextureView handle)
	{
		if (handle != nullptr)
		{
			rhiTextureViewRelease(handle);
		}
	}
	// Free Functions
	inline Instance CreateInstance(const InstanceDesc& desc)
	{
		auto result = rhiCreateInstance(reinterpret_cast<const RHIInstanceDesc*>(&desc));
		return Instance::Acquire(result);
	}

	struct Origin3D
	{
		inline operator const RHIOrigin3D& () const noexcept
		{
			return *reinterpret_cast<const RHIOrigin3D*>(this);
		}
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t z = 0;
	};
	static_assert(sizeof(Origin3D) == sizeof(RHIOrigin3D), "sizeof mismatch for Origin3D");
	static_assert(alignof(Origin3D) == alignof(RHIOrigin3D), "alignof mismatch for Origin3D");
	static_assert(offsetof(Origin3D, x) == offsetof(RHIOrigin3D, x));
	static_assert(offsetof(Origin3D, y) == offsetof(RHIOrigin3D, y));
	static_assert(offsetof(Origin3D, z) == offsetof(RHIOrigin3D, z));

	struct Extent3D
	{
		inline operator const RHIExtent3D& () const noexcept
		{
			return *reinterpret_cast<const RHIExtent3D*>(this);
		}
		uint32_t width = 1;
		uint32_t height = 1;
		uint32_t depthOrArrayLayers = 1;
	};
	static_assert(sizeof(Extent3D) == sizeof(RHIExtent3D), "sizeof mismatch for Extent3D");
	static_assert(alignof(Extent3D) == alignof(RHIExtent3D), "alignof mismatch for Extent3D");
	static_assert(offsetof(Extent3D, width) == offsetof(RHIExtent3D, width));
	static_assert(offsetof(Extent3D, height) == offsetof(RHIExtent3D, height));
	static_assert(offsetof(Extent3D, depthOrArrayLayers) == offsetof(RHIExtent3D, depthOrArrayLayers));

	struct Color
	{
		inline operator const RHIColor& () const noexcept
		{
			return *reinterpret_cast<const RHIColor*>(this);
		}
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 0.0f;
	};
	static_assert(sizeof(Color) == sizeof(RHIColor), "sizeof mismatch for Color");
	static_assert(alignof(Color) == alignof(RHIColor), "alignof mismatch for Color");
	static_assert(offsetof(Color, r) == offsetof(RHIColor, r));
	static_assert(offsetof(Color, g) == offsetof(RHIColor, g));
	static_assert(offsetof(Color, b) == offsetof(RHIColor, b));
	static_assert(offsetof(Color, a) == offsetof(RHIColor, a));

	struct Viewport
	{
		inline operator const RHIViewport& () const noexcept
		{
			return *reinterpret_cast<const RHIViewport*>(this);
		}
		float x = 0.0f;
		float y = 0.0f;
		float width = 1.0f;
		float height = 1.0f;
		float minDepth = 0.0f;
		float maxDepth = 1.0f;
	};

	static_assert(sizeof(Viewport) == sizeof(RHIViewport), "sizeof mismatch for Viewport");
	static_assert(alignof(Viewport) == alignof(RHIViewport), "alignof mismatch for Viewport");
	static_assert(offsetof(Viewport, x) == offsetof(RHIViewport, x));
	static_assert(offsetof(Viewport, y) == offsetof(RHIViewport, y));
	static_assert(offsetof(Viewport, width) == offsetof(RHIViewport, width));
	static_assert(offsetof(Viewport, height) == offsetof(RHIViewport, height));
	static_assert(offsetof(Viewport, minDepth) == offsetof(RHIViewport, minDepth));
	static_assert(offsetof(Viewport, maxDepth) == offsetof(RHIViewport, maxDepth));

	struct Rect
	{
		inline operator const RHIRect& () const noexcept
		{
			return *reinterpret_cast<const RHIRect*>(this);
		}
		int32_t x = 0;
		int32_t y = 0;
		uint32_t width = 1;
		uint32_t height = 1;
	};
	static_assert(sizeof(Rect) == sizeof(RHIRect), "sizeof mismatch for Rect");
	static_assert(alignof(Rect) == alignof(RHIRect), "alignof mismatch for Rect");
	static_assert(offsetof(Rect, x) == offsetof(RHIRect, x));
	static_assert(offsetof(Rect, y) == offsetof(RHIViewport, y));
	static_assert(offsetof(Rect, width) == offsetof(RHIRect, width));
	static_assert(offsetof(Rect, height) == offsetof(RHIRect, height));

	struct TextureDataLayout
	{
		inline operator const RHITextureDataLayout& () const noexcept
		{
			return *reinterpret_cast<const RHITextureDataLayout*>(this);
		}
		uint64_t offset;
		uint32_t bytesPerRow;
		uint32_t rowsPerImage;
	};

	static_assert(sizeof(TextureDataLayout) == sizeof(RHITextureDataLayout), "sizeof mismatch for TextureDataLayout");
	static_assert(alignof(TextureDataLayout) == alignof(RHITextureDataLayout), "alignof mismatch for TextureDataLayout");
	static_assert(offsetof(TextureDataLayout, offset) == offsetof(RHITextureDataLayout, offset));
	static_assert(offsetof(TextureDataLayout, bytesPerRow) == offsetof(RHITextureDataLayout, bytesPerRow));
	static_assert(offsetof(TextureDataLayout, rowsPerImage) == offsetof(RHITextureDataLayout, rowsPerImage));

	// describes a 2D section of a single mip level + single slice of a texture
	struct TextureSlice
	{
		Texture texture;
		Origin3D origin;
		Extent3D size;
		uint32_t mipLevel = 0;
		TextureAspect aspect = TextureAspect::All;
	};
	static_assert(sizeof(TextureSlice) == sizeof(RHITextureSlice), "sizeof mismatch for TextureSlice");
	static_assert(alignof(TextureSlice) == alignof(RHITextureSlice), "alignof mismatch for TextureSlice");
	static_assert(offsetof(TextureSlice, texture) == offsetof(RHITextureSlice, texture));
	static_assert(offsetof(TextureSlice, origin) == offsetof(RHITextureSlice, origin));
	static_assert(offsetof(TextureSlice, size) == offsetof(RHITextureSlice, size));
	static_assert(offsetof(TextureSlice, mipLevel) == offsetof(RHITextureSlice, mipLevel));
	static_assert(offsetof(TextureSlice, aspect) == offsetof(RHITextureSlice, aspect));

	struct SpecializationConstant
	{
		uint32_t constantID = 0;
		union
		{
			uint32_t u = 0;
			int32_t i;
			float f;
		} value;

		static SpecializationConstant UInt32(uint32_t constantID, uint32_t value) {
			SpecializationConstant s;
			s.constantID = constantID;
			s.value.u = value;
			return s;
		}

		static SpecializationConstant Int32(uint32_t constantID, int32_t value) {
			SpecializationConstant s;
			s.constantID = constantID;
			s.value.i = value;
			return s;
		}

		static SpecializationConstant Float(uint32_t constantID, float value) {
			SpecializationConstant s;
			s.constantID = constantID;
			s.value.f = value;
			return s;
		}
	};
	static_assert(sizeof(SpecializationConstant) == sizeof(RHISpecializationConstant), "sizeof mismatch for SpecializationConstant");
	static_assert(alignof(SpecializationConstant) == alignof(RHISpecializationConstant), "alignof mismatch for SpecializationConstant");
	static_assert(offsetof(SpecializationConstant, constantID) == offsetof(RHISpecializationConstant, constantID));
	static_assert(offsetof(SpecializationConstant, value) == offsetof(RHISpecializationConstant, value));

	struct BindSetLayoutEntry
	{
		ShaderStage visibleStages = ShaderStage::None;
		BindingType type = BindingType::None;
		uint32_t binding = 0;
		uint32_t arrayElementCount = 1;
		bool hasDynamicOffset = false;

		static BindSetLayoutEntry SampledTexture(ShaderStage stage, uint32_t binding, uint32_t arrayElementCount = 1)
		{
			BindSetLayoutEntry entry{};
			entry.visibleStages = stage;
			entry.type = BindingType::SampledTexture;
			entry.binding = binding;
			entry.arrayElementCount = arrayElementCount;
			return entry;
		}
		static BindSetLayoutEntry StorageTexture(ShaderStage stage, uint32_t binding, uint32_t arrayElementCount = 1)
		{
			BindSetLayoutEntry entry{};
			entry.visibleStages = stage;
			entry.type = BindingType::StorageTexture;
			entry.binding = binding;
			entry.arrayElementCount = arrayElementCount;
			return entry;
		}
		static BindSetLayoutEntry UniformBuffer(ShaderStage stage, uint32_t binding, uint32_t arrayElementCount = 1, bool hasDynamicOffset = false)
		{
			BindSetLayoutEntry entry{};
			entry.visibleStages = stage;
			entry.type = BindingType::UniformBuffer;
			entry.binding = binding;
			entry.arrayElementCount = arrayElementCount;
			entry.hasDynamicOffset = hasDynamicOffset;
			return entry;
		}
		static BindSetLayoutEntry StorageBuffer(ShaderStage stage, uint32_t binding, uint32_t arrayElementCount = 1, bool hasDynamicOffset = false)
		{
			BindSetLayoutEntry entry{};
			entry.visibleStages = stage;
			entry.type = BindingType::StorageBuffer;
			entry.binding = binding;
			entry.arrayElementCount = arrayElementCount;
			entry.hasDynamicOffset = hasDynamicOffset;
			return entry;
		}
		static BindSetLayoutEntry Sampler(ShaderStage stage, uint32_t binding, uint32_t arrayElementCount = 1)
		{
			BindSetLayoutEntry entry{};
			entry.visibleStages = stage;
			entry.type = BindingType::Sampler;
			entry.binding = binding;
			entry.arrayElementCount = arrayElementCount;
			return entry;
		}
		static BindSetLayoutEntry CombinedTextureSampler(ShaderStage stage, uint32_t binding, uint32_t arrayElementCount = 1)
		{
			BindSetLayoutEntry entry{};
			entry.visibleStages = stage;
			entry.type = BindingType::CombinedTextureSampler;
			entry.binding = binding;
			entry.arrayElementCount = arrayElementCount;
			return entry;
		}
	};
	static_assert(sizeof(BindSetLayoutEntry) == sizeof(RHIBindSetLayoutEntry), "sizeof mismatch for BindSetLayoutEntry");
	static_assert(alignof(BindSetLayoutEntry) == alignof(RHIBindSetLayoutEntry), "alignof mismatch for BindSetLayoutEntry");
	static_assert(offsetof(BindSetLayoutEntry, visibleStages) == offsetof(RHIBindSetLayoutEntry, visibleStages));
	static_assert(offsetof(BindSetLayoutEntry, type) == offsetof(RHIBindSetLayoutEntry, type));
	static_assert(offsetof(BindSetLayoutEntry, binding) == offsetof(RHIBindSetLayoutEntry, binding));
	static_assert(offsetof(BindSetLayoutEntry, arrayElementCount) == offsetof(RHIBindSetLayoutEntry, arrayElementCount));
	static_assert(offsetof(BindSetLayoutEntry, hasDynamicOffset) == offsetof(RHIBindSetLayoutEntry, hasDynamicOffset));

	struct BindSetEntry
	{
		uint32_t binding = 0;
		uint32_t arrayElementIndex = 0;

		TextureView textureView = nullptr;
		Sampler sampler = nullptr;
		Buffer buffer = nullptr;

		//for buffer
		uint32_t bufferOffset = 0;
		uint64_t bufferRange = WHOLE_SIZE; 

		static BindSetEntry Buffer(Buffer& buffer, uint32_t binding, uint32_t offset = 0, uint64_t range = WHOLE_SIZE, uint32_t arrayElementIndex = 0)
		{
			BindSetEntry entry{};
			entry.buffer = buffer;
			entry.binding = binding;
			entry.bufferOffset = offset;
			entry.bufferRange = range;
			entry.arrayElementIndex = arrayElementIndex;
			return entry;
		}
		static BindSetEntry Texture(TextureView& textureView, uint32_t binding, Sampler sampler = nullptr, uint32_t arrayElementIndex = 0)
		{
			BindSetEntry entry{};
			entry.textureView = textureView;
			entry.binding = binding;
			entry.sampler = sampler;
			entry.arrayElementIndex = arrayElementIndex;
			return entry;
		}
		static BindSetEntry Sampler(Sampler& sampler, uint32_t binding, uint32_t arrayElementIndex = 0)
		{
			BindSetEntry entry{};
			entry.binding = binding;
			entry.sampler = sampler;
			entry.arrayElementIndex = arrayElementIndex;
			return entry;
		}
	};
	static_assert(sizeof(BindSetEntry) == sizeof(RHIBindSetEntry), "sizeof mismatch for BindSetEntry");
	static_assert(alignof(BindSetEntry) == alignof(RHIBindSetEntry), "alignof mismatch for BindSetEntry");
	static_assert(offsetof(BindSetEntry, binding) == offsetof(RHIBindSetEntry, binding));
	static_assert(offsetof(BindSetEntry, arrayElementIndex) == offsetof(RHIBindSetEntry, arrayElementIndex));
	static_assert(offsetof(BindSetEntry, textureView) == offsetof(RHIBindSetEntry, textureView));
	static_assert(offsetof(BindSetEntry, sampler) == offsetof(RHIBindSetEntry, sampler));
	static_assert(offsetof(BindSetEntry, buffer) == offsetof(RHIBindSetEntry, buffer));
	static_assert(offsetof(BindSetEntry, bufferOffset) == offsetof(RHIBindSetEntry, bufferOffset));
	static_assert(offsetof(BindSetEntry, bufferRange) == offsetof(RHIBindSetEntry, bufferRange));

	struct PushConstantRange
	{
		ShaderStage visibility = ShaderStage::None;
		uint32_t size = 0;
	};
	static_assert(sizeof(PushConstantRange) == sizeof(RHIPushConstantRange), "sizeof mismatch for PushConstantRange");
	static_assert(alignof(PushConstantRange) == alignof(RHIPushConstantRange), "alignof mismatch for PushConstantRange");
	static_assert(offsetof(PushConstantRange, visibility) == offsetof(RHIPushConstantRange, visibility));
	static_assert(offsetof(PushConstantRange, size) == offsetof(RHIPushConstantRange, size));

	struct ShaderState
	{
		ShaderModule shaderModule;
		SpecializationConstant const* constants;
		uint32_t constantCount;
	};
	static_assert(sizeof(ShaderState) == sizeof(RHIShaderState), "sizeof mismatch for ShaderState");
	static_assert(alignof(ShaderState) == alignof(RHIShaderState), "alignof mismatch for ShaderState");
	static_assert(offsetof(ShaderState, shaderModule) == offsetof(RHIShaderState, shaderModule));
	static_assert(offsetof(ShaderState, constants) == offsetof(RHIShaderState, constants));
	static_assert(offsetof(ShaderState, constantCount) == offsetof(RHIShaderState, constantCount));

	struct VertexInputAttribute
	{
		uint32_t bindingBufferSlot = 0;
		uint32_t location = 0;
		VertexFormat format = VertexFormat::Float32x3;
		VertexInputRate rate = VertexInputRate::Vertex;
		// If this value is set to UINT32_MAX (default value), the offset will
		// be computed automatically by placing the element right after the previous one.
		uint32_t offsetInElement = AUTO_COMPUTE;
		// Stride in bytes between two elements, for one buffer slot.
		// If this value is set to UINT32_MAX, the stride will be
		// computed automatically assuming that all elements in the same buffer slot that are
		// packed one by one. or must specify the same stride in the same buffer.
		uint32_t elementStride = AUTO_COMPUTE;
	};
	static_assert(sizeof(VertexInputAttribute) == sizeof(RHIVertexInputAttribute), "sizeof mismatch for VertexInputAttribute");
	static_assert(alignof(VertexInputAttribute) == alignof(RHIVertexInputAttribute), "alignof mismatch for VertexInputAttribute");
	static_assert(offsetof(VertexInputAttribute, bindingBufferSlot) == offsetof(RHIVertexInputAttribute, bindingBufferSlot));
	static_assert(offsetof(VertexInputAttribute, location) == offsetof(RHIVertexInputAttribute, location));
	static_assert(offsetof(VertexInputAttribute, format) == offsetof(RHIVertexInputAttribute, format));
	static_assert(offsetof(VertexInputAttribute, rate) == offsetof(RHIVertexInputAttribute, rate));
	static_assert(offsetof(VertexInputAttribute, offsetInElement) == offsetof(RHIVertexInputAttribute, offsetInElement));
	static_assert(offsetof(VertexInputAttribute, elementStride) == offsetof(RHIVertexInputAttribute, elementStride));

	struct ColorAttachmentBlendState
	{
		bool        blendEnable = false;
		BlendFactor srcColorBlend = BlendFactor::One;
		BlendFactor destColorBlend = BlendFactor::Zero;
		BlendOp     colorBlendOp = BlendOp::Add;
		BlendFactor srcAlphaBlend = BlendFactor::One;
		BlendFactor destAlphaBlend = BlendFactor::Zero;
		BlendOp     alphaBlendOp = BlendOp::Add;
		ColorMask   colorWriteMask = ColorMask::All;
	};
	static_assert(sizeof(ColorAttachmentBlendState) == sizeof(RHIColorAttachmentBlendState), "sizeof mismatch for ColorAttachmentBlendState");
	static_assert(alignof(ColorAttachmentBlendState) == alignof(RHIColorAttachmentBlendState), "alignof mismatch for ColorAttachmentBlendState");
	static_assert(offsetof(ColorAttachmentBlendState, blendEnable) == offsetof(RHIColorAttachmentBlendState, blendEnable));
	static_assert(offsetof(ColorAttachmentBlendState, srcColorBlend) == offsetof(RHIColorAttachmentBlendState, srcColorBlend));
	static_assert(offsetof(ColorAttachmentBlendState, destColorBlend) == offsetof(RHIColorAttachmentBlendState, destColorBlend));
	static_assert(offsetof(ColorAttachmentBlendState, colorBlendOp) == offsetof(RHIColorAttachmentBlendState, colorBlendOp));
	static_assert(offsetof(ColorAttachmentBlendState, srcAlphaBlend) == offsetof(RHIColorAttachmentBlendState, srcAlphaBlend));
	static_assert(offsetof(ColorAttachmentBlendState, destAlphaBlend) == offsetof(RHIColorAttachmentBlendState, destAlphaBlend));
	static_assert(offsetof(ColorAttachmentBlendState, alphaBlendOp) == offsetof(RHIColorAttachmentBlendState, alphaBlendOp));
	static_assert(offsetof(ColorAttachmentBlendState, colorWriteMask) == offsetof(RHIColorAttachmentBlendState, colorWriteMask));

	struct BlendState
	{
		bool alphaToCoverageEnable = false;

		ColorAttachmentBlendState colorAttachmentBlendStates[MAX_COLOR_ATTACHMENTS];
	};
	static_assert(sizeof(BlendState) == sizeof(RHIBlendState), "sizeof mismatch for BlendState");
	static_assert(alignof(BlendState) == alignof(RHIBlendState), "alignof mismatch for BlendState");
	static_assert(offsetof(BlendState, alphaToCoverageEnable) == offsetof(RHIBlendState, alphaToCoverageEnable));
	static_assert(offsetof(BlendState, colorAttachmentBlendStates) == offsetof(RHIBlendState, colorAttachmentBlendStates));

	struct StencilOpState
	{
		StencilOp failOp = StencilOp::Keep;
		StencilOp passOp = StencilOp::Keep;
		StencilOp depthFailOp = StencilOp::Keep;
		CompareOp compareOp = CompareOp::Always;
		uint8_t writeMask = 0xff;
		uint8_t compareMak = 0xff;
		uint32_t referenceValue = 0;
	};
	static_assert(sizeof(StencilOpState) == sizeof(RHIStencilOpState), "sizeof mismatch for StencilOpState");
	static_assert(alignof(StencilOpState) == alignof(RHIStencilOpState), "alignof mismatch for StencilOpState");
	static_assert(offsetof(StencilOpState, failOp) == offsetof(RHIStencilOpState, failOp));
	static_assert(offsetof(StencilOpState, passOp) == offsetof(RHIStencilOpState, passOp));
	static_assert(offsetof(StencilOpState, depthFailOp) == offsetof(RHIStencilOpState, depthFailOp));
	static_assert(offsetof(StencilOpState, compareOp) == offsetof(RHIStencilOpState, compareOp));
	static_assert(offsetof(StencilOpState, writeMask) == offsetof(RHIStencilOpState, writeMask));
	static_assert(offsetof(StencilOpState, compareMak) == offsetof(RHIStencilOpState, compareMak));
	static_assert(offsetof(StencilOpState, referenceValue) == offsetof(RHIStencilOpState, referenceValue));

	struct DepthStencilState
	{
		bool depthTestEnable = true;
		bool depthWriteEnable = true;
		CompareOp depthCompareOp = CompareOp::LessOrEqual;
		int32_t depthBias = 0;
		float depthBiasSlopeScale = 0.0f;
		float depthBiasClamp = 0.0f;
		bool stencilTestEnable = false;
		uint8_t stencilReadMask = 0xff;
		uint8_t stencilWriteMask = 0xff;
		StencilOpState frontFaceStencil;
		StencilOpState backFaceStencil;
	};
	static_assert(sizeof(DepthStencilState) == sizeof(RHIDepthStencilState), "sizeof mismatch for DepthStencilState");
	static_assert(alignof(DepthStencilState) == alignof(RHIDepthStencilState), "alignof mismatch for DepthStencilState");
	static_assert(offsetof(DepthStencilState, depthTestEnable) == offsetof(RHIDepthStencilState, depthTestEnable));
	static_assert(offsetof(DepthStencilState, depthWriteEnable) == offsetof(RHIDepthStencilState, depthWriteEnable));
	static_assert(offsetof(DepthStencilState, depthCompareOp) == offsetof(RHIDepthStencilState, depthCompareOp));
	static_assert(offsetof(DepthStencilState, depthBiasSlopeScale) == offsetof(RHIDepthStencilState, depthBiasSlopeScale));
	static_assert(offsetof(DepthStencilState, depthBiasClamp) == offsetof(RHIDepthStencilState, depthBiasClamp));
	static_assert(offsetof(DepthStencilState, stencilTestEnable) == offsetof(RHIDepthStencilState, stencilTestEnable));
	static_assert(offsetof(DepthStencilState, stencilReadMask) == offsetof(RHIDepthStencilState, stencilReadMask));
	static_assert(offsetof(DepthStencilState, stencilWriteMask) == offsetof(RHIDepthStencilState, stencilWriteMask));
	static_assert(offsetof(DepthStencilState, frontFaceStencil) == offsetof(RHIDepthStencilState, frontFaceStencil));
	static_assert(offsetof(DepthStencilState, backFaceStencil) == offsetof(RHIDepthStencilState, backFaceStencil));
	static_assert(offsetof(DepthStencilState, depthBias) == offsetof(RHIDepthStencilState, depthBias));

	struct RasterState
	{
		PrimitiveType primitiveType = PrimitiveType::TriangleList;
		FillMode fillMode = FillMode::Fill;
		CullMode cullMode = CullMode::Back;
		FrontFace frontFace = FrontFace::FrontCounterClockwise;
		bool depthClampEnable = false; //must be false if this feature is not enabled
		float lineWidth = 1.0f;
	};
	static_assert(sizeof(RasterState) == sizeof(RHIRasterState), "sizeof mismatch for RasterState");
	static_assert(alignof(RasterState) == alignof(RHIRasterState), "alignof mismatch for RasterState");
	static_assert(offsetof(RasterState, primitiveType) == offsetof(RHIRasterState, primitiveType));
	static_assert(offsetof(RasterState, fillMode) == offsetof(RHIRasterState, fillMode));
	static_assert(offsetof(RasterState, cullMode) == offsetof(RHIRasterState, cullMode));
	static_assert(offsetof(RasterState, frontFace) == offsetof(RHIRasterState, frontFace));
	static_assert(offsetof(RasterState, depthClampEnable) == offsetof(RHIRasterState, depthClampEnable));
	static_assert(offsetof(RasterState, lineWidth) == offsetof(RHIRasterState, lineWidth));

	struct SampleState
	{
		uint32_t count = 1;
		uint32_t quality = 0;
		uint32_t mask = 0xFFFFFFFF;
	};
	static_assert(sizeof(SampleState) == sizeof(RHISampleState), "sizeof mismatch for SampleState");
	static_assert(alignof(SampleState) == alignof(RHISampleState), "alignof mismatch for SampleState");
	static_assert(offsetof(SampleState, count) == offsetof(RHISampleState, count));
	static_assert(offsetof(SampleState, quality) == offsetof(RHISampleState, quality));
	static_assert(offsetof(SampleState, mask) == offsetof(RHISampleState, mask));

	struct ColorAttachment
	{
		TextureView view = nullptr;
		TextureView resolveView = nullptr;
		LoadOp loadOp = LoadOp::Clear;
		StoreOp storeOp = StoreOp::Store;
		Color clearValue = { 0.0f, 0.0f, 0.0f, 0.0f };
	};
	static_assert(sizeof(ColorAttachment) == sizeof(RHIColorAttachment), "sizeof mismatch for ColorAttachment");
	static_assert(alignof(ColorAttachment) == alignof(RHIColorAttachment), "alignof mismatch for ColorAttachment");
	static_assert(offsetof(ColorAttachment, view) == offsetof(RHIColorAttachment, view));
	static_assert(offsetof(ColorAttachment, resolveView) == offsetof(RHIColorAttachment, resolveView));
	static_assert(offsetof(ColorAttachment, loadOp) == offsetof(RHIColorAttachment, loadOp));
	static_assert(offsetof(ColorAttachment, storeOp) == offsetof(RHIColorAttachment, storeOp));
	static_assert(offsetof(ColorAttachment, clearValue) == offsetof(RHIColorAttachment, clearValue));

	struct DepthStencilAattachment
	{
		TextureView view;
		LoadOp depthLoadOp = LoadOp::Clear;
		StoreOp depthStoreOp = StoreOp::Store;
		LoadOp stencilLoadOp = LoadOp::Clear;
		StoreOp stencilStoreOp = StoreOp::Store;
		float depthClearValue = 1.0f;
		uint32_t stencilClearValue = 0;
	};
	static_assert(sizeof(DepthStencilAattachment) == sizeof(RHIDepthStencilAattachment), "sizeof mismatch for DepthStencilAattachment");
	static_assert(alignof(DepthStencilAattachment) == alignof(RHIDepthStencilAattachment), "alignof mismatch for DepthStencilAattachment");
	static_assert(offsetof(DepthStencilAattachment, view) == offsetof(RHIDepthStencilAattachment, view));
	static_assert(offsetof(DepthStencilAattachment, depthLoadOp) == offsetof(RHIDepthStencilAattachment, depthLoadOp));
	static_assert(offsetof(DepthStencilAattachment, depthStoreOp) == offsetof(RHIDepthStencilAattachment, depthStoreOp));
	static_assert(offsetof(DepthStencilAattachment, stencilLoadOp) == offsetof(RHIDepthStencilAattachment, stencilLoadOp));
	static_assert(offsetof(DepthStencilAattachment, stencilStoreOp) == offsetof(RHIDepthStencilAattachment, stencilStoreOp));
	static_assert(offsetof(DepthStencilAattachment, depthClearValue) == offsetof(RHIDepthStencilAattachment, depthClearValue));
	static_assert(offsetof(DepthStencilAattachment, stencilClearValue) == offsetof(RHIDepthStencilAattachment, stencilClearValue));

	struct AdapterInfo
	{
		uint32_t apiVersion;
		uint32_t driverVersion;
		uint32_t vendorID;
		uint32_t deviceID;
		AdapterType adapterType;
		std::string_view deviceName;
	};
	static_assert(sizeof(AdapterInfo) == sizeof(RHIAdapterInfo), "sizeof mismatch for AdapterInfo");
	static_assert(alignof(AdapterInfo) == alignof(RHIAdapterInfo), "alignof mismatch for AdapterInfo");
	static_assert(offsetof(AdapterInfo, apiVersion) == offsetof(RHIAdapterInfo, apiVersion));
	static_assert(offsetof(AdapterInfo, driverVersion) == offsetof(RHIAdapterInfo, driverVersion));
	static_assert(offsetof(AdapterInfo, vendorID) == offsetof(RHIAdapterInfo, vendorID));
	static_assert(offsetof(AdapterInfo, deviceID) == offsetof(RHIAdapterInfo, deviceID));
	static_assert(offsetof(AdapterInfo, adapterType) == offsetof(RHIAdapterInfo, adapterType));
	static_assert(offsetof(AdapterInfo, deviceName) == offsetof(RHIAdapterInfo, deviceName));

	struct Limits
	{
		uint32_t maxTextureDimension1D;
		uint32_t maxTextureDimension2D;
		uint32_t maxTextureDimension3D;
		uint32_t maxTextureArrayLayers;
		uint32_t maxBindSets;
		uint64_t maxBufferSize;
		uint32_t maxBindingsPerBindSet;
		uint32_t maxDynamicUniformBuffersPerPipelineLayout;
		uint32_t maxDynamicStorageBuffersPerPipelineLayout;
		uint32_t maxSampledTexturesPerShaderStage;
		uint32_t maxSamplersPerShaderStage;
		uint32_t maxStorageBuffersPerShaderStage;
		uint32_t maxStorageTexturesPerShaderStage;
		uint32_t maxUniformBuffersPerShaderStage;
		uint64_t maxUniformBufferBindingSize;
		uint64_t maxStorageBufferBindingSize;
		uint32_t minUniformBufferOffsetAlignment;
		uint32_t minStorageBufferOffsetAlignment;
		uint32_t maxVertexBuffers;
		uint32_t maxVertexAttributes;
		uint32_t maxVertexBufferArrayStride;
		uint32_t maxVertexOutputComponents;
		uint32_t maxFragmentInputComponents;
		uint32_t maxColorAttachments;
		uint32_t maxComputeWorkgroupStorageSize;
		uint32_t maxComputeInvocationsPerWorkgroup;
		uint32_t maxComputeWorkgroupSizeX;
		uint32_t maxComputeWorkgroupSizeY;
		uint32_t maxComputeWorkgroupSizeZ;
		uint32_t maxComputeWorkgroupsPerDimension;
		uint32_t maxPushConstantsSize;
		uint32_t maxDrawIndirectCount;
		uint32_t maxViewports;
		float maxSamplerLodBias;
		float maxSamplerAnisotropy;
	};
	// todo: 

	struct SurfaceConfiguration
	{
		Device device;
		TextureFormat format = TextureFormat::BGRA8_SRGB;
		uint32_t width;
		uint32_t height;
		PresentMode presentMode = PresentMode::Fifo;
	};
	static_assert(sizeof(SurfaceConfiguration) == sizeof(RHISurfaceConfiguration), "sizeof mismatch for SurfaceConfiguration");
	static_assert(alignof(SurfaceConfiguration) == alignof(RHISurfaceConfiguration), "alignof mismatch for SurfaceConfiguration");
	static_assert(offsetof(SurfaceConfiguration, device) == offsetof(RHISurfaceConfiguration, device));
	static_assert(offsetof(SurfaceConfiguration, format) == offsetof(RHISurfaceConfiguration, format));
	static_assert(offsetof(SurfaceConfiguration, width) == offsetof(RHISurfaceConfiguration, width));
	static_assert(offsetof(SurfaceConfiguration, height) == offsetof(RHISurfaceConfiguration, height));
	static_assert(offsetof(SurfaceConfiguration, presentMode) == offsetof(RHISurfaceConfiguration, presentMode));

	struct TextureSubresourceRange
	{
		TextureAspect aspect = TextureAspect::All;
		uint32_t baseMipLevel = 0;
		uint32_t mipLevelCount;
		uint32_t baseArrayLayer = 0;
		uint32_t arrayLayerCount;
	};
	static_assert(sizeof(TextureSubresourceRange) == sizeof(RHITextureSubresourceRange), "sizeof mismatch for TextureSubresourceRange");
	static_assert(alignof(TextureSubresourceRange) == alignof(RHITextureSubresourceRange), "alignof mismatch for TextureSubresourceRange");
	static_assert(offsetof(TextureSubresourceRange, aspect) == offsetof(RHITextureSubresourceRange, aspect));
	static_assert(offsetof(TextureSubresourceRange, baseMipLevel) == offsetof(RHITextureSubresourceRange, baseMipLevel));
	static_assert(offsetof(TextureSubresourceRange, mipLevelCount) == offsetof(RHITextureSubresourceRange, mipLevelCount));
	static_assert(offsetof(TextureSubresourceRange, baseArrayLayer) == offsetof(RHITextureSubresourceRange, baseArrayLayer));
	static_assert(offsetof(TextureSubresourceRange, arrayLayerCount) == offsetof(RHITextureSubresourceRange, arrayLayerCount));

	struct TextureSubresources
	{
		Texture texture;
		TextureSubresourceRange range;
	};
	static_assert(sizeof(TextureSubresources) == sizeof(RHITextureSubresources), "sizeof mismatch for TextureSubresources");
	static_assert(alignof(TextureSubresources) == alignof(RHITextureSubresources), "alignof mismatch for TextureSubresources");
	static_assert(offsetof(TextureSubresources, texture) == offsetof(RHITextureSubresources, texture));
	static_assert(offsetof(TextureSubresources, range) == offsetof(RHITextureSubresources, range));

	struct ResourceTransfer
	{
		Queue receivingQueue;
		Buffer const* buffers;
		uint32_t bufferCount;
		TextureSubresources const* textureSubresources;
		uint32_t textureSubresourceCount;
	};
	static_assert(sizeof(ResourceTransfer) == sizeof(RHIResourceTransfer), "sizeof mismatch for ResourceTransfer");
	static_assert(alignof(ResourceTransfer) == alignof(RHIResourceTransfer), "alignof mismatch for ResourceTransfer");
	static_assert(offsetof(ResourceTransfer, receivingQueue) == offsetof(RHIResourceTransfer, receivingQueue));
	static_assert(offsetof(ResourceTransfer, buffers) == offsetof(RHIResourceTransfer, buffers));
	static_assert(offsetof(ResourceTransfer, bufferCount) == offsetof(RHIResourceTransfer, bufferCount));
	static_assert(offsetof(ResourceTransfer, textureSubresources) == offsetof(RHIResourceTransfer, textureSubresources));
	static_assert(offsetof(ResourceTransfer, textureSubresourceCount) == offsetof(RHIResourceTransfer, textureSubresourceCount));


	struct BindSetLayoutDesc
	{
		std::string_view name;
		uint32_t entryCount;
		BindSetLayoutEntry const* entries;
	};
	static_assert(sizeof(BindSetLayoutDesc) == sizeof(RHIBindSetLayoutDesc), "sizeof mismatch for BindSetLayoutDesc");
	static_assert(alignof(BindSetLayoutDesc) == alignof(RHIBindSetLayoutDesc), "alignof mismatch for BindSetLayoutDesc");
	static_assert(offsetof(BindSetLayoutDesc, name) == offsetof(RHIBindSetLayoutDesc, name));
	static_assert(offsetof(BindSetLayoutDesc, entryCount) == offsetof(RHIBindSetLayoutDesc, entryCount));
	static_assert(offsetof(BindSetLayoutDesc, entries) == offsetof(RHIBindSetLayoutDesc, entries));

	struct BindSetDesc
	{
		std::string_view name;
		BindSetLayout layout;
		uint32_t entryCount;
		BindSetEntry const* entries;
	};
	static_assert(sizeof(BindSetDesc) == sizeof(RHIBindSetDesc), "sizeof mismatch for BindSetDesc");
	static_assert(alignof(BindSetDesc) == alignof(RHIBindSetDesc), "alignof mismatch for BindSetDesc");
	static_assert(offsetof(BindSetDesc, name) == offsetof(RHIBindSetDesc, name));
	static_assert(offsetof(BindSetDesc, layout) == offsetof(RHIBindSetDesc, layout));
	static_assert(offsetof(BindSetDesc, entryCount) == offsetof(RHIBindSetDesc, entryCount));
	static_assert(offsetof(BindSetDesc, entries) == offsetof(RHIBindSetDesc, entries));

	struct BufferDesc
	{
		size_t size = 0;
		std::string_view name;
		BufferUsage usage = BufferUsage::None;
		ShareMode shareMode = ShareMode::Exclusive;
	};
	static_assert(sizeof(BufferDesc) == sizeof(RHIBufferDesc), "sizeof mismatch for BufferDesc");
	static_assert(alignof(BufferDesc) == alignof(RHIBufferDesc), "alignof mismatch for BufferDesc");
	static_assert(offsetof(BufferDesc, name) == offsetof(RHIBufferDesc, name));
	static_assert(offsetof(BufferDesc, size) == offsetof(RHIBufferDesc, size));
	static_assert(offsetof(BufferDesc, usage) == offsetof(RHIBufferDesc, usage));
	static_assert(offsetof(BufferDesc, shareMode) == offsetof(RHIBufferDesc, shareMode));

	struct TextureDesc
	{
		TextureDimension dimension = TextureDimension::Texture2D;
		uint32_t width = 1;
		uint32_t height = 1;
		union
		{
			/// For a 1D array or 2D array, number of array slices
			uint32_t arraySize = 1;
			/// For a 3D texture, number of depth slices
			uint32_t depth;
		};
		uint32_t sampleCount = 1;
		uint32_t mipLevelCount = 1;
		TextureFormat format = TextureFormat::Undefined;

		TextureUsage usage = TextureUsage::None;

		std::string_view name;
	};
	static_assert(sizeof(TextureDesc) == sizeof(RHITextureDesc), "sizeof mismatch for TextureDesc");
	static_assert(alignof(TextureDesc) == alignof(RHITextureDesc), "alignof mismatch for TextureDesc");
	static_assert(offsetof(TextureDesc, name) == offsetof(RHITextureDesc, name));
	static_assert(offsetof(TextureDesc, dimension) == offsetof(RHITextureDesc, dimension));
	static_assert(offsetof(TextureDesc, width) == offsetof(RHITextureDesc, width));
	static_assert(offsetof(TextureDesc, height) == offsetof(RHITextureDesc, height));
	static_assert(offsetof(TextureDesc, arraySize) == offsetof(RHITextureDesc, arraySize));
	static_assert(offsetof(TextureDesc, sampleCount) == offsetof(RHITextureDesc, sampleCount));
	static_assert(offsetof(TextureDesc, mipLevelCount) == offsetof(RHITextureDesc, mipLevelCount));
	static_assert(offsetof(TextureDesc, format) == offsetof(RHITextureDesc, format));
	static_assert(offsetof(TextureDesc, usage) == offsetof(RHITextureDesc, usage));

	struct TextureViewDesc
	{
		std::string_view name;
		TextureFormat format = TextureFormat::Undefined;
		TextureDimension dimension = TextureDimension::Undefined;
		uint32_t baseMipLevel = 0;
		uint32_t mipLevelCount = UINT32_MAX;
		uint32_t baseArrayLayer = 0;
		uint32_t arrayLayerCount = UINT32_MAX;
		TextureAspect aspect = TextureAspect::All;
		TextureUsage usage = TextureUsage::None;
	};
	static_assert(sizeof(TextureViewDesc) == sizeof(RHITextureViewDesc), "sizeof mismatch for TextureViewDesc");
	static_assert(alignof(TextureViewDesc) == alignof(RHITextureViewDesc), "alignof mismatch for TextureViewDesc");
	static_assert(offsetof(TextureViewDesc, name) == offsetof(RHITextureViewDesc, name));
	static_assert(offsetof(TextureViewDesc, dimension) == offsetof(RHITextureViewDesc, dimension));
	static_assert(offsetof(TextureViewDesc, baseMipLevel) == offsetof(RHITextureViewDesc, baseMipLevel));
	static_assert(offsetof(TextureViewDesc, baseArrayLayer) == offsetof(RHITextureViewDesc, baseArrayLayer));
	static_assert(offsetof(TextureViewDesc, arrayLayerCount) == offsetof(RHITextureViewDesc, arrayLayerCount));
	static_assert(offsetof(TextureViewDesc, aspect) == offsetof(RHITextureViewDesc, aspect));
	static_assert(offsetof(TextureViewDesc, mipLevelCount) == offsetof(RHITextureViewDesc, mipLevelCount));
	static_assert(offsetof(TextureViewDesc, format) == offsetof(RHITextureViewDesc, format));
	static_assert(offsetof(TextureViewDesc, usage) == offsetof(RHITextureViewDesc, usage));

	struct ShaderModuleDesc
	{
		ShaderStage type = ShaderStage::None;
		std::string_view name;
		std::string_view entry;
		std::string_view code;
		SpecializationConstant const* specializationConstants;
		uint32_t specializationConstantCount = 0;
	};
	static_assert(sizeof(ShaderModuleDesc) == sizeof(RHIShaderModuleDesc), "sizeof mismatch for ShaderModuleDesc");
	static_assert(alignof(ShaderModuleDesc) == alignof(RHIShaderModuleDesc), "alignof mismatch for ShaderModuleDesc");
	static_assert(offsetof(ShaderModuleDesc, name) == offsetof(RHIShaderModuleDesc, name));
	static_assert(offsetof(ShaderModuleDesc, type) == offsetof(RHIShaderModuleDesc, type));
	static_assert(offsetof(ShaderModuleDesc, entry) == offsetof(RHIShaderModuleDesc, entry));
	static_assert(offsetof(ShaderModuleDesc, code) == offsetof(RHIShaderModuleDesc, code));
	static_assert(offsetof(ShaderModuleDesc, specializationConstants) == offsetof(RHIShaderModuleDesc, specializationConstants));
	static_assert(offsetof(ShaderModuleDesc, specializationConstantCount) == offsetof(RHIShaderModuleDesc, specializationConstantCount));

	struct SamplerDesc
	{
		float maxAnisotropy = 1.f;
		float mipLodBias = 0.f;
		float minLod = 0.f;
		float maxLod = 1000.f;

		CompareOp compareOp = CompareOp::Less;
		BorderColor borderColor = BorderColor::FloatOpaqueBlack;

		FilterMode magFilter = FilterMode::Linear;
		FilterMode minFilter = FilterMode::Linear;
		FilterMode mipmapFilter = FilterMode::Linear;

		SamplerAddressMode addressModeU = SamplerAddressMode::ClampToEdge;
		SamplerAddressMode addressModeV = SamplerAddressMode::ClampToEdge;
		SamplerAddressMode addressModeW = SamplerAddressMode::ClampToEdge;

		std::string_view name;
	};
	static_assert(sizeof(SamplerDesc) == sizeof(RHISamplerDesc), "sizeof mismatch for SamplerDesc");
	static_assert(alignof(SamplerDesc) == alignof(RHISamplerDesc), "alignof mismatch for SamplerDesc");
	static_assert(offsetof(SamplerDesc, name) == offsetof(RHISamplerDesc, name));
	static_assert(offsetof(SamplerDesc, maxAnisotropy) == offsetof(RHISamplerDesc, maxAnisotropy));
	static_assert(offsetof(SamplerDesc, mipLodBias) == offsetof(RHISamplerDesc, mipLodBias));
	static_assert(offsetof(SamplerDesc, minLod) == offsetof(RHISamplerDesc, minLod));
	static_assert(offsetof(SamplerDesc, maxLod) == offsetof(RHISamplerDesc, maxLod));
	static_assert(offsetof(SamplerDesc, compareOp) == offsetof(RHISamplerDesc, compareOp));
	static_assert(offsetof(SamplerDesc, borderColor) == offsetof(RHISamplerDesc, borderColor));
	static_assert(offsetof(SamplerDesc, magFilter) == offsetof(RHISamplerDesc, magFilter));
	static_assert(offsetof(SamplerDesc, minFilter) == offsetof(RHISamplerDesc, minFilter));
	static_assert(offsetof(SamplerDesc, mipmapFilter) == offsetof(RHISamplerDesc, mipmapFilter));
	static_assert(offsetof(SamplerDesc, addressModeW) == offsetof(RHISamplerDesc, addressModeW));
	static_assert(offsetof(SamplerDesc, addressModeU) == offsetof(RHISamplerDesc, addressModeU));
	static_assert(offsetof(SamplerDesc, addressModeV) == offsetof(RHISamplerDesc, addressModeV));

	struct PipelineLayoutDesc
	{
		std::string_view name;
		BindSetLayout const* bindSetLayouts;
		uint32_t bindSetLayoutCount;
		PushConstantRange pushConstantRange;
	};
	static_assert(sizeof(PipelineLayoutDesc) == sizeof(RHIPipelineLayoutDesc), "sizeof mismatch for PipelineLayoutDesc");
	static_assert(alignof(PipelineLayoutDesc) == alignof(RHIPipelineLayoutDesc), "alignof mismatch for PipelineLayoutDesc");
	static_assert(offsetof(PipelineLayoutDesc, name) == offsetof(RHIPipelineLayoutDesc, name));
	static_assert(offsetof(PipelineLayoutDesc, bindSetLayouts) == offsetof(RHIPipelineLayoutDesc, bindSetLayouts));
	static_assert(offsetof(PipelineLayoutDesc, bindSetLayoutCount) == offsetof(RHIPipelineLayoutDesc, bindSetLayoutCount));
	static_assert(offsetof(PipelineLayoutDesc, pushConstantRange) == offsetof(RHIPipelineLayoutDesc, pushConstantRange));

	struct PipelineLayoutDesc2
	{
		std::string_view name;
		ShaderState const* shaders;
		uint32_t shaderCount;
	};
	static_assert(sizeof(PipelineLayoutDesc2) == sizeof(RHIPipelineLayoutDesc2), "sizeof mismatch for PipelineLayoutDesc2");
	static_assert(alignof(PipelineLayoutDesc2) == alignof(RHIPipelineLayoutDesc2), "alignof mismatch for PipelineLayoutDesc2");
	static_assert(offsetof(PipelineLayoutDesc2, name) == offsetof(RHIPipelineLayoutDesc2, name));
	static_assert(offsetof(PipelineLayoutDesc2, shaders) == offsetof(RHIPipelineLayoutDesc2, shaders));
	static_assert(offsetof(PipelineLayoutDesc2, shaderCount) == offsetof(RHIPipelineLayoutDesc2, shaderCount));

	struct RenderPipelineDesc
	{
		std::string_view name;
		ShaderState* vertexShader = nullptr;
		ShaderState* fragmentShader = nullptr;
		ShaderState* tessControlShader = nullptr;
		ShaderState* tessEvaluationShader = nullptr;
		ShaderState* geometryShader = nullptr;

		PipelineLayout pipelineLayout;

		VertexInputAttribute const* vertexAttributes;
		uint32_t vertexAttributeCount = 0;

		BlendState blendState;
		RasterState rasterState;
		SampleState sampleState;
		DepthStencilState depthStencilState;

		uint32_t viewportCount = 1;
		uint32_t colorAttachmentCount = 0;
		TextureFormat colorAttachmentFormats[MAX_COLOR_ATTACHMENTS];
		TextureFormat depthStencilFormat = TextureFormat::Undefined;

		uint32_t patchControlPoints = 0;
	};
	static_assert(sizeof(RenderPipelineDesc) == sizeof(RHIRenderPipelineDesc), "sizeof mismatch for RenderPipelineDesc");
	static_assert(alignof(RenderPipelineDesc) == alignof(RHIRenderPipelineDesc), "alignof mismatch for RenderPipelineDesc");
	static_assert(offsetof(RenderPipelineDesc, name) == offsetof(RHIRenderPipelineDesc, name));
	static_assert(offsetof(RenderPipelineDesc, vertexShader) == offsetof(RHIRenderPipelineDesc, vertexShader));
	static_assert(offsetof(RenderPipelineDesc, fragmentShader) == offsetof(RHIRenderPipelineDesc, fragmentShader));
	static_assert(offsetof(RenderPipelineDesc, tessControlShader) == offsetof(RHIRenderPipelineDesc, tessControlShader));
	static_assert(offsetof(RenderPipelineDesc, tessEvaluationShader) == offsetof(RHIRenderPipelineDesc, tessEvaluationShader));
	static_assert(offsetof(RenderPipelineDesc, geometryShader) == offsetof(RHIRenderPipelineDesc, geometryShader));
	static_assert(offsetof(RenderPipelineDesc, pipelineLayout) == offsetof(RHIRenderPipelineDesc, pipelineLayout));
	static_assert(offsetof(RenderPipelineDesc, vertexAttributes) == offsetof(RHIRenderPipelineDesc, vertexAttributes));
	static_assert(offsetof(RenderPipelineDesc, vertexAttributeCount) == offsetof(RHIRenderPipelineDesc, vertexAttributeCount));
	static_assert(offsetof(RenderPipelineDesc, blendState) == offsetof(RHIRenderPipelineDesc, blendState));
	static_assert(offsetof(RenderPipelineDesc, rasterState) == offsetof(RHIRenderPipelineDesc, rasterState));
	static_assert(offsetof(RenderPipelineDesc, sampleState) == offsetof(RHIRenderPipelineDesc, sampleState));
	static_assert(offsetof(RenderPipelineDesc, depthStencilState) == offsetof(RHIRenderPipelineDesc, depthStencilState));
	static_assert(offsetof(RenderPipelineDesc, viewportCount) == offsetof(RHIRenderPipelineDesc, viewportCount));
	static_assert(offsetof(RenderPipelineDesc, colorAttachmentCount) == offsetof(RHIRenderPipelineDesc, colorAttachmentCount));
	static_assert(offsetof(RenderPipelineDesc, colorAttachmentFormats) == offsetof(RHIRenderPipelineDesc, colorAttachmentFormats));
	static_assert(offsetof(RenderPipelineDesc, depthStencilFormat) == offsetof(RHIRenderPipelineDesc, depthStencilFormat));
	static_assert(offsetof(RenderPipelineDesc, patchControlPoints) == offsetof(RHIRenderPipelineDesc, patchControlPoints));

	struct ComputePipelineDesc
	{
		std::string_view name;
		ShaderState* computeShader = nullptr;

		PipelineLayout pipelineLayout;
	};
	static_assert(sizeof(ComputePipelineDesc) == sizeof(RHIComputePipelineDesc), "sizeof mismatch for ComputePipelineDesc");
	static_assert(alignof(ComputePipelineDesc) == alignof(RHIComputePipelineDesc), "alignof mismatch for ComputePipelineDesc");
	static_assert(offsetof(ComputePipelineDesc, name) == offsetof(RHIComputePipelineDesc, name));
	static_assert(offsetof(ComputePipelineDesc, computeShader) == offsetof(RHIComputePipelineDesc, computeShader));
	static_assert(offsetof(ComputePipelineDesc, pipelineLayout) == offsetof(RHIComputePipelineDesc, pipelineLayout));

	struct RenderPassDesc
	{
		uint32_t colorAttachmentCount = 0;
		ColorAttachment const* colorAttachments;
		DepthStencilAattachment const* depthStencilAttachment = nullptr;
	};
	static_assert(sizeof(RenderPassDesc) == sizeof(RHIRenderPassDesc), "sizeof mismatch for RenderPassDesc");
	static_assert(alignof(RenderPassDesc) == alignof(RHIRenderPassDesc), "alignof mismatch for RenderPassDesc");
	static_assert(offsetof(RenderPassDesc, colorAttachmentCount) == offsetof(RHIRenderPassDesc, colorAttachmentCount));
	static_assert(offsetof(RenderPassDesc, colorAttachments) == offsetof(RHIRenderPassDesc, colorAttachments));
	static_assert(offsetof(RenderPassDesc, depthStencilAttachment) == offsetof(RHIRenderPassDesc, depthStencilAttachment));

	struct InstanceDesc
	{
		BackendType backend = BackendType::Vulkan;
		LoggingCallback loggingCallback;
		void* loggingCallbackUserData;
		bool enableDebugLayer;
	};
	static_assert(sizeof(InstanceDesc) == sizeof(RHIInstanceDesc), "sizeof mismatch for InstanceDesc");
	static_assert(alignof(InstanceDesc) == alignof(RHIInstanceDesc), "alignof mismatch for InstanceDesc");
	static_assert(offsetof(InstanceDesc, backend) == offsetof(RHIInstanceDesc, backend));
	static_assert(offsetof(InstanceDesc, loggingCallback) == offsetof(RHIInstanceDesc, loggingCallback));
	static_assert(offsetof(InstanceDesc, loggingCallbackUserData) == offsetof(RHIInstanceDesc, loggingCallbackUserData));
	static_assert(offsetof(InstanceDesc, enableDebugLayer) == offsetof(RHIInstanceDesc, enableDebugLayer));

	struct DeviceDesc
	{
		std::string_view name;
		uint32_t requiredFeatureCount = 0;
		FeatureName const* requiredFeatures;
	};
	static_assert(sizeof(DeviceDesc) == sizeof(RHIDeviceDesc), "sizeof mismatch for DeviceDesc");
	static_assert(alignof(DeviceDesc) == alignof(RHIDeviceDesc), "alignof mismatch for DeviceDesc");
	static_assert(offsetof(DeviceDesc, name) == offsetof(RHIDeviceDesc, name));
	static_assert(offsetof(DeviceDesc, requiredFeatureCount) == offsetof(RHIDeviceDesc, requiredFeatureCount));
	static_assert(offsetof(DeviceDesc, requiredFeatures) == offsetof(RHIDeviceDesc, requiredFeatures));

}