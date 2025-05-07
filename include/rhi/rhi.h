#pragma once
#include <cstdint>

#define DEFINE_RHI_OBJECT(name) typedef struct name##Impl* RHI##name

DEFINE_RHI_OBJECT(Adapter);
DEFINE_RHI_OBJECT(BindSet);
DEFINE_RHI_OBJECT(BindSetLayout);
DEFINE_RHI_OBJECT(Queue);
DEFINE_RHI_OBJECT(Device);
DEFINE_RHI_OBJECT(Instance);
DEFINE_RHI_OBJECT(Surface);
DEFINE_RHI_OBJECT(RenderPipeline);
DEFINE_RHI_OBJECT(ComputePipeline);
DEFINE_RHI_OBJECT(CommandEncoder);
DEFINE_RHI_OBJECT(RenderPassEncoder);
DEFINE_RHI_OBJECT(ComputePassEncoder);
DEFINE_RHI_OBJECT(CommandList);
DEFINE_RHI_OBJECT(PipelineLayout);
DEFINE_RHI_OBJECT(Sampler);
DEFINE_RHI_OBJECT(ShaderModule);
DEFINE_RHI_OBJECT(Buffer);
DEFINE_RHI_OBJECT(Texture);
DEFINE_RHI_OBJECT(TextureView);

#define MAX_COLOR_ATTACHMENTS 8
#define WHOLE_SIZE ~0ull
#define AUTO_COMPUTE uint32_t(-1)
#define ARRAY_SIZE_UNDEFINE uint32_t(-1)
#define MIPLEVEL_COUNT_UNDEFINE uint32_t(-1)

struct RHIAdapterInfo;
struct RHILimits;

struct RHIRect;
struct RHIViewport;
struct RHIColor;
struct RHIExtent3D;
struct RHIOrigin3D;
struct RHITextureDataLayout;
struct RHITextureSlice;
struct RHIBindSetLayoutEntry;
struct RHIBindSetEntry;
struct RHIPushConstantRange;
struct RHISpecializationConstant;
struct RHIStringView;
struct RHIShaderState;
struct RHIVertexInputAttribute;
struct RHIBlendState;
struct RHIStencilOpState;
struct RHIDepthStencilState;
struct RHIRasterState;
struct RHISampleState;
struct RHIColorAttachment;
struct RHIDepthStencilAattachment;
struct RHISurfaceConfiguration;
struct RHITextureSubresourceRange;
struct RHITextureSubresources;
struct RHIResourceTransfer;
struct RHIDrawIndirectCommand;
struct RHIDrawIndexedIndirectCommand;
struct RHIDispatchIndirectCommand;

struct RHIDeviceDesc;
struct RHIInstanceDesc;
struct RHIBufferDesc;
struct RHITextureDesc;
struct RHITextureViewDesc;
struct RHISamplerDesc;
struct RHIShaderModuleDesc;
struct RHIBindSetDesc;
struct RHIBindSetLayoutDesc;
struct RHIPipelineLayoutDesc;
struct RHIRenderPipelineDesc;
struct RHIComputePipelineDesc;
struct RHIRenderPassDesc;

typedef enum RHIMapMode
{
	RHIMapMode_Read,
	RHIMapMode_Write
}RHIMapMode;

typedef enum RHIBufferMapAsyncStatus
{
	RHIBufferMapAsyncStatus_Success,
	RHIBufferMapAsyncStatus_None,
	RHIBufferMapAsyncStatus_DeviceLost,
	RHIBufferMapAsyncStatus_DestroyedBeforeCallback
}RHIBufferMapAsyncStatus;

typedef enum RHIBufferUsage
{
	RHIBufferUsage_None = 0 << 0,
	RHIBufferUsage_Vertex = 1 << 0,
	RHIBufferUsage_Index = 1 << 1,
	RHIBufferUsage_Indirect = 1 << 2,
	RHIBufferUsage_Uniform = 1 << 3,
	RHIBufferUsage_Storage = 1 << 4,
	RHIBufferUsage_MapRead = 1 << 5,
	RHIBufferUsage_MapWrite = 1 << 6,
	RHIBufferUsage_CopySrc = 1 << 7,
	RHIBufferUsage_CopyDst = 1 << 8,
	RHIBufferUsage_QueryResolve = 1 << 9
}RHIBufferUsage;


typedef enum RHITextureDimension
{
	RHITextureDimension_Undefined,
	RHITextureDimension_Texture1D,
	RHITextureDimension_Texture1DArray,
	RHITextureDimension_Texture2D,
	RHITextureDimension_Texture2DArray,
	RHITextureDimension_TextureCube,
	RHITextureDimension_TextureCubeArray,
	RHITextureDimension_Texture3D
}RHITextureDimension;

typedef enum RHITextureFormat
{
	RHITextureFormat_Undefined,

	RHITextureFormat_R8_UINT,
	RHITextureFormat_R8_SINT,
	RHITextureFormat_R8_UNORM,
	RHITextureFormat_R8_SNORM,
	RHITextureFormat_RG8_UINT,
	RHITextureFormat_RG8_SINT,
	RHITextureFormat_RG8_UNORM,
	RHITextureFormat_RG8_SNORM,
	RHITextureFormat_R16_UINT,
	RHITextureFormat_R16_SINT,
	RHITextureFormat_R16_UNORM,
	RHITextureFormat_R16_SNORM,
	RHITextureFormat_R16_FLOAT,
	RHITextureFormat_BGRA4_UNORM,
	RHITextureFormat_B5G6R5_UNORM,
	RHITextureFormat_B5G5R5A1_UNORM,
	RHITextureFormat_RGBA8_UINT,
	RHITextureFormat_RGBA8_SINT,
	RHITextureFormat_RGBA8_UNORM,
	RHITextureFormat_RGBA8_SNORM,
	RHITextureFormat_BGRA8_UNORM,
	RHITextureFormat_RGBA8_SRGB,
	RHITextureFormat_BGRA8_SRGB,
	RHITextureFormat_R10G10B10A2_UNORM,
	RHITextureFormat_R11G11B10_FLOAT,
	RHITextureFormat_RG16_UINT,
	RHITextureFormat_RG16_SINT,
	RHITextureFormat_RG16_UNORM,
	RHITextureFormat_RG16_SNORM,
	RHITextureFormat_RG16_FLOAT,
	RHITextureFormat_R32_UINT,
	RHITextureFormat_R32_SINT,
	RHITextureFormat_R32_FLOAT,
	RHITextureFormat_RGBA16_UINT,
	RHITextureFormat_RGBA16_SINT,
	RHITextureFormat_RGBA16_FLOAT,
	RHITextureFormat_RGBA16_UNORM,
	RHITextureFormat_RGBA16_SNORM,
	RHITextureFormat_RG32_UINT,
	RHITextureFormat_RG32_SINT,
	RHITextureFormat_RG32_FLOAT,
	RHITextureFormat_RGB32_UINT,
	RHITextureFormat_RGB32_SINT,
	RHITextureFormat_RGB32_FLOAT,
	RHITextureFormat_RGBA32_UINT,
	RHITextureFormat_RGBA32_SINT,
	RHITextureFormat_RGBA32_FLOAT,

	RHITextureFormat_D16_UNORM,
	RHITextureFormat_D24_UNORM_S8_UINT,
	RHITextureFormat_D32_UNORM,
	RHITextureFormat_D32_UNORM_S8_UINT,

	RHITextureFormat_BC1_UNORM,
	RHITextureFormat_BC1_UNORM_SRGB,
	RHITextureFormat_BC2_UNORM,
	RHITextureFormat_BC2_UNORM_SRGB,
	RHITextureFormat_BC3_UNORM,
	RHITextureFormat_BC3_UNORM_SRGB,
	RHITextureFormat_BC4_UNORM,
	RHITextureFormat_BC4_SNORM,
	RHITextureFormat_BC5_UNORM,
	RHITextureFormat_BC5_SNORM,
	RHITextureFormat_BC6H_UFLOAT,
	RHITextureFormat_BC6H_SFLOAT,
	RHITextureFormat_BC7_UNORM,
	RHITextureFormat_BC7_UNORM_SRGB,

	RHITextureFormat_COUNT,
}RHITextureFormat;

typedef enum RHIVertexFormat
{
	RHIVertexFormat_Uint8,
	RHIVertexFormat_Uint8x2,
	RHIVertexFormat_Uint8x4,
	RHIVertexFormat_Sint8,
	RHIVertexFormat_Sint8x2,
	RHIVertexFormat_Sint8x4,
	RHIVertexFormat_Unorm8,
	RHIVertexFormat_Unorm8x2,
	RHIVertexFormat_Unorm8x4,
	RHIVertexFormat_Snorm8,
	RHIVertexFormat_Snorm8x2,
	RHIVertexFormat_Snorm8x4,
	RHIVertexFormat_Uint16,
	RHIVertexFormat_Uint16x2,
	RHIVertexFormat_Uint16x4,
	RHIVertexFormat_Sint16,
	RHIVertexFormat_Sint16x2,
	RHIVertexFormat_Sint16x4,
	RHIVertexFormat_Unorm16,
	RHIVertexFormat_Unorm16x2,
	RHIVertexFormat_Unorm16x4,
	RHIVertexFormat_Snorm16,
	RHIVertexFormat_Snorm16x2,
	RHIVertexFormat_Snorm16x4,
	RHIVertexFormat_Float16,
	RHIVertexFormat_Float16x2,
	RHIVertexFormat_Float16x4,
	RHIVertexFormat_Float32,
	RHIVertexFormat_Float32x2,
	RHIVertexFormat_Float32x3,
	RHIVertexFormat_Float32x4,
	RHIVertexFormat_Uint32,
	RHIVertexFormat_Uint32x2,
	RHIVertexFormat_Uint32x3,
	RHIVertexFormat_Uint32x4,
	RHIVertexFormat_Sint32,
	RHIVertexFormat_Sint32x2,
	RHIVertexFormat_Sint32x3,
	RHIVertexFormat_Sint32x4,
	RHIVertexFormat_Unorm10_10_10_2,
}RHIVertexFormat;

typedef enum RHIIndexFormat
{
	RHIIndexFormat_Uint16,
	RHIIndexFormat_Uint32
}RHIIndexFormat;

typedef enum RHITextureUsage
{
	RHITextureUsage_None = 0 << 0,
	RHITextureUsage_CopySrc = 1 << 0,
	RHITextureUsage_CopyDst = 1 << 1,
	RHITextureUsage_SampledBinding = 1 << 2,
	RHITextureUsage_StorageBinding = 1 << 3,
	RHITextureUsage_RenderAttachment = 1 << 4
}RHITextureUsage;

typedef enum RHITextureAspect
{
	RHITextureAspect_None = 0 << 0,
	RHITextureAspect_All = 1 << 0,
	RHITextureAspect_Depth = 1 << 1,
	RHITextureAspect_Stencil = 1 << 2,
	RHITextureAspect_Plane0 = 1 << 3,
	RHITextureAspect_Plane1 = 1 << 4,
	RHITextureAspect_Plane2 = 1 << 5
}RHITextureAspect;

typedef enum RHIBlendFactor
{
	RHIBlendFactor_Zero,
	RHIBlendFactor_One,
	RHIBlendFactor_SrcColor,
	RHIBlendFactor_OneMinusSrcColor,
	RHIBlendFactor_DstColor,
	RHIBlendFactor_OneMinusDstColor,
	RHIBlendFactor_SrcAlpha,
	RHIBlendFactor_OneMinusSrcAlpha,
	RHIBlendFactor_DstAlpha,
	RHIBlendFactor_OneMinusDstAlpha,
	RHIBlendFactor_ConstantColor,
	RHIBlendFactor_OneMinusConstantColor,
	RHIBlendFactor_ConstantAlpha,
	RHIBlendFactor_OneMinusConstantAlpha,
	RHIBlendFactor_SrcAlphaSaturate,
	RHIBlendFactor_Src1Color,
	RHIBlendFactor_OneMinusSrc1Color,
	RHIBlendFactor_Src1Alpha,
	RHIBlendFactor_OneMinusSrc1Alpha
}RHIBlendFactor;

typedef enum RHIBlendOp
{
	RHIBlendOp_Add = 0,
	RHIBlendOp_Subrtact = 1,
	RHIBlendOp_ReverseSubtract = 2,
	RHIBlendOp_Min = 3,
	RHIBlendOp_Max = 4
}BlendOp;

typedef enum RHIColorMask
{
	RHIColorMask_Red = 1,
	RHIColorMask_Green = 2,
	RHIColorMask_Blue = 4,
	RHIColorMask_Alpha = 8,
	RHIColorMask_All = 0xF
}RHIColorMask;

typedef enum RHISamplerAddressMode
{
	RHISamplerAddressMode_ClampToEdge,
	RHISamplerAddressMode_Repeat,
	RHISamplerAddressMode_ClampToBorder,
	RHISamplerAddressMode_MirroredRepeat,
	RHISamplerAddressMode_MirrorClampToEdge
}RHISamplerAddressMode;

typedef enum RHIFilterMode
{
	RHIFilterMode_Linear,
	RHIFilterMode_Nearest
}RHIFilterMode;

typedef enum RHIBorderColor
{
	RHIBorderColor_FloatOpaqueBlack,
	RHIBorderColor_FloatOpaqueWhite,
	RHIBorderColor_FloatTransparentBlack
}RHIBorderColor;

typedef enum RHIBindingType
{
	RHIBindingType_None,
	RHIBindingType_SampledTexture, //SRV
	RHIBindingType_StorageTexture, //URV
	RHIBindingType_ReadOnlyStorageTexture,
	RHIBindingType_UniformBuffer, // CBV
	RHIBindingType_StorageBuffer, // UAV
	RHIBindingType_ReadOnlyStorageBuffer,
	RHIBindingType_Sampler,
	RHIBindingType_CombinedTextureSampler
}RHIBindingType;

typedef enum RHIShaderStage
{
	RHIShaderStage_None = (0 << 0),
	RHIShaderStage_Vertex = (1 << 0),
	RHIShaderStage_TessellationControl = (1 << 1),
	RHIShaderStage_TessellationEvaluation = (1 << 2),
	RHIShaderStage_Geometry = (1 << 3),
	RHIShaderStage_Fragment = (1 << 4),
	RHIShaderStage_Task = (1 << 5),
	RHIShaderStage_Mesh = (1 << 6),
	RHIShaderStage_Compute = (1 << 7),
	RHIShaderStage_AllGraphics = RHIShaderStage_Vertex | RHIShaderStage_TessellationControl | RHIShaderStage_TessellationEvaluation | RHIShaderStage_Geometry | RHIShaderStage_Fragment,
	RHIShaderStage_All = RHIShaderStage_Vertex | RHIShaderStage_TessellationControl | RHIShaderStage_TessellationEvaluation | RHIShaderStage_Geometry | RHIShaderStage_Fragment | RHIShaderStage_Task | RHIShaderStage_Mesh | RHIShaderStage_Compute
}RHIShaderStage;

typedef enum RHIFillMode
{
	RHIFillMode_Fill = 0,
	RHIFillMode_Line = 1,
	RHIFillMode_Point = 2
}RHIFillMode;

typedef enum RHICullMode
{
	RHICullMode_None,
	RHICullMode_Front,
	RHICullMode_Back
}RHICullMode;

typedef enum RHIFrontFace
{
	RHIFrontFace_FrontCounterClockwise,
	RHIFrontFace_FrontClockwise
}RHIFrontFace;

typedef enum RHICompareOp
{
	RHICompareOp_Never = 0,
	RHICompareOp_Less = 1,
	RHICompareOp_Equal = 2,
	RHICompareOp_LessOrEqual = 3,
	RHICompareOp_Greater = 4,
	RHICompareOp_NotEqual = 5,
	RHICompareOp_GreaterOrEqual = 6,
	RHICompareOp_Always = 7
}RHICompareOp;

typedef enum RHIStencilOp
{
	RHIStencilOp_Keep = 0,
	RHIStencilOp_Zero = 1,
	RHIStencilOp_Replace = 2,
	RHIStencilOp_IncrementAndClamp = 3,
	RHIStencilOp_DecrementAndClamp = 4,
	RHIStencilOp_Invert = 5,
	RHIStencilOp_IncrementAndWrap = 6,
	RHIStencilOp_DecrementAndWrap = 7
}RHIStencilOp;

typedef enum RHIPrimitiveType
{
	RHIPrimitiveType_PointList,
	RHIPrimitiveType_LineList,
	RHIPrimitiveType_LineStrip,
	RHIPrimitiveType_TriangleList,
	RHIPrimitiveType_TriangleStrip,
	RHIPrimitiveType_TriangleFan,
	RHIPrimitiveType_PatchList
}RHIPrimitiveType;

typedef enum RHIVertexInputRate
{
	RHIVertexInputRate_Vertex,
	RHIVertexInputRate_Instance
}RHIVertexInputRate;

typedef enum RHIQueueType
{
	RHIQueueType_Graphics,
	RHIQueueType_Compute,
	RHIQueueType_Transfer,
	RHIQueueType_Undefined
}RHIQueueType;

typedef enum RHILoadOp
{
	RHILoadOp_DontCare,
	RHILoadOp_Load,
	RHILoadOp_Clear
}RHILoadOp;

typedef enum RHIStoreOp
{
	RHIStoreOp_Store,
	RHIStoreOp_Discard
}RHIStoreOp;

typedef enum RHILoggingSeverity
{
	RHILoggingSeverity_Verbose,
	RHILoggingSeverity_Info,
	RHILoggingSeverity_Warning,
	RHILoggingSeverity_Error,
	RHILoggingSeverity_Fatal
}RHILoggingSeverity;

typedef enum RHIFeatureName
{
	RHIFeatureName_ShaderInt16,
	RHIFeatureName_ShaderInt64,
	RHIFeatureName_ShaderFloat64,
	RHIFeatureName_SampleRateShading,
	RHIFeatureName_SamplerAnisotropy,
	RHIFeatureName_TextureCompressionBC,
	RHIFeatureName_TextureCompressionETC2,
	RHIFeatureName_TextureCompressionASTC,
	RHIFeatureName_GeometryShader,
	RHIFeatureName_TessellationShader,
	RHIFeatureName_MultiViewport,
	RHIFeatureName_MultiDrawIndirect,
	RHIFeatureName_DepthBiasClamp,
	RHIFeatureName_DepthClamp,
	RHIFeatureName_R8UnormStorage
}RHIFeatureName;

typedef enum RHIBackendType
{
	RHIBackendType_Vulkan
}RHIBackendType;

typedef enum RHIAdapterType
{
	RHIAdapterType_DiscreteGPU,
	RHIAdapterType_IntegratedGPU,
	RHIAdapterType_VirtualGPU,
	RHIAdapterType_Cpu,
	RHIAdapterType_Unknown
}RHIAdapterType;

typedef enum RHIShareMode
{
	RHIShareMode_Exclusive,
	RHIShareMode_Concurrent
}RHIShareMode;

typedef enum RHIPresentMode
{
	RHIPresentMode_Fifo,
	RHIPresentMode_FifoRelaxed,
	RHIPresentMode_Immediate,
	RHIPresentMode_Mailbox
}RHIPresentMode;

typedef enum RHISurfaceAcquireNextTextureStatus
{
	RHISurfaceAcquireNextTextureStatus_Success,
	RHISurfaceAcquireNextTextureStatus_Timeout,
	RHISurfaceAcquireNextTextureStatus_Outdated,
	RHISurfaceAcquireNextTextureStatus_SurfaceLost,
	RHISurfaceAcquireNextTextureStatus_Error
}RHISurfaceAcquireNextTextureStatus;

typedef void (*RHIBufferMapCallback)(RHIBufferMapAsyncStatus status, void* mappedAdress, void* userdata);
typedef void(_stdcall* RHILoggingCallback) (RHILoggingSeverity severity, const char* msg, void* userData);

typedef struct RHIStringView
{
	const char* data;
	size_t length;
}RHIStringView;

typedef struct RHIOrigin3D
{
	uint32_t x;
	uint32_t y;
	uint32_t z;
}RHIOrigin3D;

typedef struct RHIExtent3D
{
	uint32_t width;
	uint32_t height;
	uint32_t depthOrArrayLayers;
}RHIExtent3D;

typedef struct RHIColor
{
	float r;
	float g;
	float b;
	float a;
}RHIColor;

typedef struct RHIViewport
{
	float    x;
	float    y;
	float    width;
	float    height;
	float    minDepth;
	float    maxDepth;
}RHIViewport;

typedef struct RHIRect
{
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
}RHIRect;

typedef struct RHITextureDataLayout
{
	uint64_t offset;
	uint32_t bytesPerRow;
	uint32_t rowsPerImage;
}RHITextureDataLayout;

typedef struct RHITextureSlice
{
	RHITexture texture;
	RHIOrigin3D origin;
	RHIExtent3D size;
	uint32_t mipLevel;
	RHITextureAspect aspect;
}RHITextureSlice;

typedef struct RHISpecializationConstant
{
	uint32_t constantID;
	union
	{
		uint32_t u;
		int32_t i;
		float f;
	} value;
}RHISpecializationConstant;


typedef struct RHIBindSetLayoutEntry
{
	RHIShaderStage visibleStages;
	RHIBindingType type;
	uint32_t binding;
	uint32_t arrayElementCount;
	bool hasDynamicOffset;
}RHIBindSetLayoutEntry;

typedef struct RHIBindSetEntry
{
	uint32_t binding;
	uint32_t arrayElementIndex;
	RHITextureView textureView;
	RHISampler sampler;
	RHIBuffer buffer;
	uint32_t bufferOffset;
	uint64_t bufferRange;  // if range is 0, use the range from offset to the end of the buffer.
}RHIBindSetEntry;

typedef struct RHIPushConstantRange
{
	RHIShaderStage visibility;
	uint32_t size = 0;
}RHIPushConstantRange;

typedef struct RHIShaderState
{
	RHIShaderModule shaderModule;
	RHISpecializationConstant const* constants;
	uint32_t constantCount;
}RHIShaderState;

typedef struct RHIVertexInputAttribute
{
	uint32_t bindingBufferSlot = 0;
	uint32_t location = 0;
	RHIVertexFormat format;
	RHIVertexInputRate rate;
	// If this value is set to UINT32_MAX (default value), the offset will
	// be computed automatically by placing the element right after the previous one.
	uint32_t offsetInElement;
	// Stride in bytes between two elements, for one buffer slot.
	// If this value is set to UINT32_MAX, the stride will be
	// computed automatically assuming that all elements in the same buffer slot that are
	// packed one by one. or must specify the same stride in the same buffer.
	uint32_t elementStride;
}RHIVertexInputAttribute;

typedef struct RHIColorAttachmentBlendState
{
	bool blendEnable;
	RHIBlendFactor srcColorBlend;
	RHIBlendFactor destColorBlend;
	RHIBlendOp     colorBlendOp;
	RHIBlendFactor srcAlphaBlend;
	RHIBlendFactor destAlphaBlend;
	RHIBlendOp     alphaBlendOp;
	RHIColorMask   colorWriteMask;
}RHIColorAttachmentBlendState;

typedef struct RHIBlendState
{
	bool alphaToCoverageEnable;
	RHIColorAttachmentBlendState colorAttachmentBlendStates[MAX_COLOR_ATTACHMENTS];
}RHIBlendState;

typedef struct RHIStencilOpState
{
	RHIStencilOp failOp;
	RHIStencilOp passOp;
	RHIStencilOp depthFailOp;
	RHICompareOp compareOp;
	uint8_t writeMask;
	uint8_t compareMak;
	uint32_t referenceValue;
}RHIStencilOpState;

typedef struct RHIDepthStencilState
{
	bool depthTestEnable;
	bool depthWriteEnable;
	RHICompareOp depthCompareOp;
	int32_t depthBias;
	float depthBiasSlopeScale;
	float depthBiasClamp;
	bool stencilTestEnable;
	uint8_t stencilReadMask;
	uint8_t stencilWriteMask;
	RHIStencilOpState frontFaceStencil;
	RHIStencilOpState backFaceStencil;
}RHIDepthStencilState;

typedef struct RHIRasterState
{
	RHIPrimitiveType primitiveType;
	RHIFillMode fillMode;
	RHICullMode cullMode;
	RHIFrontFace frontFace;
	bool depthClampEnable; //must be false if this feature is not enabled
	float lineWidth;
}RHIRasterState;

typedef struct RHISampleState
{
	uint32_t count;
	uint32_t quality;
	uint32_t mask;
}RHISampleState;

typedef struct RHIColorAttachment
{
	RHITextureView view;
	RHITextureView resolveView;
	RHILoadOp loadOp;
	RHIStoreOp storeOp;
	RHIColor clearValue;
}RHIColorAttachment;

typedef struct RHIDepthStencilAattachment
{
	RHITextureView view;
	RHILoadOp depthLoadOp;
	RHIStoreOp depthStoreOp;
	RHILoadOp stencilLoadOp;
	RHIStoreOp stencilStoreOp;
	float depthClearValue;
	uint32_t stencilClearValue;
}RHIDepthStencilAattachment;

typedef struct RHIAdapterInfo
{
	uint32_t apiVersion;
	uint32_t driverVersion;
	uint32_t vendorID;
	uint32_t deviceID;
	RHIAdapterType adapterType;
	RHIStringView deviceName;
}RHIAdapterInfo;

typedef struct RHILimits
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
}RHILimits;

typedef struct RHISurfaceConfiguration
{
	RHIDevice device;
	RHITextureFormat format;
	uint32_t width;
	uint32_t height;
	RHIPresentMode presentMode;
}RHISurfaceConfiguration;


typedef struct RHITextureSubresourceRange
{
	RHITextureAspect aspect;
	uint32_t baseMipLevel;
	uint32_t mipLevelCount;
	uint32_t baseArrayLayer;
	uint32_t arrayLayerCount;
}RHITextureSubresourceRange;

typedef struct RHITextureSubresources
{
	RHITexture texture;
	RHITextureSubresourceRange range;
}RHITextureSubresources;

typedef struct RHIResourceTransfer
{
	RHIQueue receivingQueue;
	RHIBuffer const* buffers;
	uint32_t bufferCount;
	RHITextureSubresources const* textureSubresources;
	uint32_t textureSubresourceCount;
}RHIResourceTransfer;

typedef struct RHIDrawIndirectCommand
{
	uint32_t    vertexCount;
	uint32_t    instanceCount;
	uint32_t    firstVertex;
	uint32_t    firstInstance;
}RHIDrawIndirectCommand;

typedef struct RHIDrawIndexedIndirectCommand
{
	uint32_t    indexCount;
	uint32_t    instanceCount;
	uint32_t    firstIndex;
	int32_t     vertexOffset;
	uint32_t    firstInstance;
}RHIDrawIndexedIndirectCommand;

typedef struct RHIDispatchIndirectCommand
{
	uint32_t    x;
	uint32_t    y;
	uint32_t    z;
}RHIDispatchIndirectCommand;

typedef struct RHIBindSetLayoutDesc
{
	RHIStringView name;
	uint32_t entryCount;
	RHIBindSetLayoutEntry const* entries;
}RHIBindSetLayoutDesc;

typedef struct RHIBindSetDesc
{
	RHIStringView name;
	RHIBindSetLayout layout;
	uint32_t entryCount;
	RHIBindSetEntry const* entries;
}RHIBindSetDesc;

typedef struct RHIBufferDesc
{
	size_t size;
	RHIStringView name;
	RHIBufferUsage usage;
	RHIShareMode shareMode;
}RHIBufferDesc;

typedef struct RHITextureDesc
{
	RHITextureDimension dimension;
	uint32_t width;
	uint32_t height;
	union
	{
		/// For a 1D array or 2D array, number of array slices
		uint32_t arraySize;
		/// For a 3D texture, number of depth slices
		uint32_t depth;
	};
	uint32_t sampleCount;
	uint32_t mipLevelCount;
	RHITextureFormat format;
	RHITextureUsage usage;

	RHIStringView name;
}RHITextureDesc;

typedef struct RHITextureViewDesc
{
	RHIStringView name;
	RHITextureFormat format;
	RHITextureDimension dimension;
	uint32_t baseMipLevel;
	uint32_t mipLevelCount;
	uint32_t baseArrayLayer;
	uint32_t arrayLayerCount;
	RHITextureAspect aspect;
	RHITextureUsage usage;
}RHITextureViewDesc;

typedef struct RHIShaderModuleDesc
{
	RHIShaderStage type;
	RHIStringView name;
	RHIStringView entry;
	RHIStringView code;
	RHISpecializationConstant const* specializationConstants;
	uint32_t specializationConstantCount;
}RHIShaderModuleDesc;

typedef struct RHISamplerDesc
{
	float maxAnisotropy;
	float mipLodBias;
	float minLod;
	float maxLod;

	RHICompareOp compareOp;
	RHIBorderColor borderColor;

	RHIFilterMode magFilter;
	RHIFilterMode minFilter;
	RHIFilterMode mipmapFilter;

	RHISamplerAddressMode addressModeU;
	RHISamplerAddressMode addressModeV;
	RHISamplerAddressMode addressModeW;

	RHIStringView name;
}RHISamplerDesc;

typedef struct RHIPipelineLayoutDesc
{
	RHIStringView name;
	RHIBindSetLayout const* bindSetLayouts;
	uint32_t bindSetLayoutCount;
	RHIPushConstantRange pushConstantRange;
}RHIPipelineLayoutDesc;

typedef struct RHIPipelineLayoutDesc2
{
	RHIStringView name;
	RHIShaderState const* shaders;
	uint32_t shaderCount;
}RHIPipelineLayoutDesc2;


typedef struct RHIRenderPipelineDesc
{
	RHIStringView name;
	RHIShaderState* vertexShader;
	RHIShaderState* fragmentShader;
	RHIShaderState* tessControlShader;
	RHIShaderState* tessEvaluationShader;
	RHIShaderState* geometryShader ;

	RHIPipelineLayout pipelineLayout;

	RHIVertexInputAttribute const* vertexAttributes;
	uint32_t vertexAttributeCount;

	RHIBlendState blendState;
	RHIRasterState rasterState;
	RHISampleState sampleState;
	RHIDepthStencilState depthStencilState;

	uint32_t viewportCount;
	uint32_t colorAttachmentCount;
	RHITextureFormat colorAttachmentFormats[MAX_COLOR_ATTACHMENTS];
	RHITextureFormat depthStencilFormat;

	uint32_t patchControlPoints;
}RHIRenderPipelineDesc;

typedef struct RHIComputePipelineDesc
{
	RHIStringView name;
	RHIShaderState* computeShader;

	RHIPipelineLayout pipelineLayout;
}RHIComputePipelineDesc;

typedef struct RHIRenderPassDesc
{
	uint32_t colorAttachmentCount = 0;
	RHIColorAttachment const* colorAttachments;
	RHIDepthStencilAattachment const* depthStencilAttachment;
}RHIRenderPassDesc;

typedef struct RHIInstanceDesc
{
	RHIBackendType backend;
	RHILoggingCallback loggingCallback;
	void* loggingCallbackUserData;
	bool enableDebugLayer;
}RHIInstanceDesc;

typedef struct RHIDeviceDesc
{
	RHIStringView name;
	uint32_t requiredFeatureCount = 0;
	RHIFeatureName const* requiredFeatures;
}RHIDeviceDesc;

RHIInstance rhiCreateInstance(const RHIInstanceDesc* desc);
// methods of Instance
void rhiInstanceEnumerateAdapters(RHIInstance instance, RHIAdapter* const pAdapters, uint32_t* adapterCount);
void rhiInstanceAddRef(RHIInstance instance);
void rhiInstanceRelease(RHIInstance instance);
// methods of Adapter
RHIDevice rhiAdapterCreateDevice(RHIAdapter adapter, const RHIDeviceDesc* desc);
void rhiAdapterGetInfo(RHIAdapter adapter, RHIAdapterInfo* info);
void rhiAdapterGetLimits(RHIAdapter adapter, RHILimits* limits);
RHIInstance rhiAdapterGetInstance(RHIAdapter adapter);
void rhiAdapterAddRef(RHIAdapter adapter);
void rhiAdapterRelease(RHIAdapter adapter);
// methods of Device
RHIAdapter rhiDeviceGetAdapter(RHIDevice device);
RHIQueue rhiDeviceGetQueue(RHIDevice device, RHIQueueType queueType);
RHIPipelineLayout rhiCreatePipelineLayout(RHIDevice device, const RHIPipelineLayoutDesc* desc);
RHIRenderPipeline rhiDeviceCreateRenderPipeline(RHIDevice device, const RHIRenderPipelineDesc* desc);
RHIComputePipeline rhiDeviceCreateComputePipeline(RHIDevice device, const RHIComputePipelineDesc* desc);
RHIBindSetLayout rhiDeviceCreateBindSetLayout(RHIDevice device, const RHIBindSetLayoutDesc* desc);
RHIBindSet rhiDeviceCreateBindSet(RHIDevice device, const RHIBindSetDesc* desc);
RHITexture rhiDeviceCreateTexture(RHIDevice device, const RHITextureDesc* desc);
RHIBuffer rhiDeviceCreateBuffer(RHIDevice device, const RHIBufferDesc* desc);
RHIShaderModule rhiDeviceCreateShader(RHIDevice device, const RHIShaderModuleDesc* desc);
RHISampler rhiDeviceCreateSampler(RHIDevice device, const RHISamplerDesc* desc);
RHICommandEncoder rhiDeviceCreateCommandEncoder(RHIDevice device);
void rhiDeviceTick(RHIDevice device);
void rhiDeviceAddRef(RHIDevice device);
void rhiDeviceRelease(RHIDevice device);
// methods of Queue
void rhiQueueWriteBuffer(RHIQueue queue, RHIBuffer buffer, const void* data, uint64_t dataSize, uint64_t offset);
void rhiQueueWriteTexture(RHIQueue queue, const RHITextureSlice* dstTexture, const void* data, size_t dataSize, const RHITextureDataLayout* dataLayout);
void rhiQueueWaitFor(RHIQueue queue, RHIQueue waitQueue, uint64_t submitSerial);
uint64_t rhiQueueSubmit(RHIQueue queue, RHICommandList const* commands, uint32_t commandListCount, RHIResourceTransfer const* transfers, uint32_t transferCount);
void rhiQueueAddRef(RHIQueue queue);
void rhiQueueRelease(RHIQueue queue);
// methods of Surface
RHISurface rhiCreateSurfaceFromWindowsHWND(RHIInstance instance, void* hwnd, void* hinstance);
void rhiSurfaceConfigure(RHISurface surface, const RHISurfaceConfiguration* config);
RHISurfaceAcquireNextTextureStatus rhiSurfaceAcquireNextTexture(RHISurface surface);
RHITexture rhiSurfaceGetCurrentTexture(RHISurface surface);
RHITextureView rhiSurfaceGetCurrentTextureView(RHISurface surface);
RHITextureFormat rhiSurfaceGetSwapChainFormat(RHISurface surface);
void rhiSurfacePresent(RHISurface surface);
void rhiSurfaceUnconfigure(RHISurface surface);
void rhiSurfaceAddRef(RHISurface surface);
void rhiSurfaceRelease(RHISurface surface);
// methods of CommandEncoder
void rhiCommandEncoderClearBuffer(RHICommandEncoder encoder, RHIBuffer buffer, uint32_t value, uint64_t offset, uint64_t size);
void rhiCommandEncoderCopyBufferToBuffer(RHICommandEncoder encoder, RHIBuffer srcBuffer, uint64_t srcOffset, RHIBuffer dstBuffer, uint64_t dstOffset, uint64_t dataSize);
void rhiCommandEncoderCopyBufferToTexture(RHICommandEncoder encoder, RHIBuffer srcBuffer, const RHITextureDataLayout* dataLayout, const RHITextureSlice* dstTextureSlice);
void rhiCommandEncoderCopyTextureToBuffer(RHICommandEncoder encoder, const RHITextureSlice* srcTextureSlice, RHIBuffer dstBuffer, const RHITextureDataLayout* dataLayout);
void rhiCommandEncoderCopyTextureToTexture(RHICommandEncoder encoder, const RHITextureSlice* srcTextureSlice, const RHITextureSlice* dstTextureSlice);
void rhiCommandEncoderMapBufferAsync(RHICommandEncoder encoder, RHIBuffer buffer, RHIMapMode usage, RHIBufferMapCallback callback, void* userData);
void rhiCommandEncoderBeginDebugLabel(RHICommandEncoder encoder, RHIStringView label, const RHIColor* color);
void rhiCommandEncoderEndDebugLabel(RHICommandEncoder encoder);
RHIRenderPassEncoder rhiCommandEncoderBeginRenderPass(RHICommandEncoder encoder, const RHIRenderPassDesc* desc);
RHIComputePassEncoder rhiCommandEncoderBeginComputePass(RHICommandEncoder encoder);
RHICommandList rhiCommandEncoderFinish(RHICommandEncoder encoder);
void rhiCommandEncoderAddRef(RHICommandEncoder encoder);
void rhiCommandEncoderRelease(RHICommandEncoder encoder);
// methods of RenderPassEncoder
void rhiRenderPassEncoderSetPipeline(RHIRenderPassEncoder encoder, RHIRenderPipeline pipeline);
void rhiRenderPassEncoderSetVertexBuffers(RHIRenderPassEncoder encoder, uint32_t firstSlot, uint32_t bufferCount, RHIBuffer const* buffers, uint64_t* offsets);
void rhiRenderPassEncoderSetIndexBuffer(RHIRenderPassEncoder encoder, RHIBuffer buffer, uint64_t offset, uint64_t size, RHIIndexFormat indexFormat);
void rhiRenderPassEncoderSetScissorRect(RHIRenderPassEncoder encoder, uint32_t firstScissor, const RHIRect* scissors, uint32_t scissorCount);
void rhiRenderPassEncoderSetStencilReference(RHIRenderPassEncoder encoder, uint32_t reference);
void rhiRenderPassEncoderSetBlendConstant(RHIRenderPassEncoder encoder, const RHIColor* blendConstants);
void rhiRenderPassEncoderSetViewport(RHIRenderPassEncoder encoder, uint32_t firstViewport, RHIViewport const* viewports, uint32_t viewportCount);
void rhiRenderPassEncoderSetBindSet(RHIRenderPassEncoder encoder, RHIBindSet set, uint32_t setIndex, uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets);
void rhiRenderPassEncoderDraw(RHIRenderPassEncoder encoder, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
void rhiRenderPassEncoderDrawIndexed(RHIRenderPassEncoder encoder, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t baseVertex, uint32_t firstInstance);
void rhiRenderPassEncoderDrawIndirect(RHIRenderPassEncoder encoder, RHIBuffer indirectBuffer, uint64_t indirectOffset);
void rhiRenderPassEncoderDrawIndexedIndirect(RHIRenderPassEncoder encoder, RHIBuffer indirectBuffer, uint64_t indirectOffset);
void rhiRenderPassEncoderMultiDrawIndirect(RHIRenderPassEncoder encoder, RHIBuffer indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, RHIBuffer drawCountBuffer, uint64_t drawCountBufferOffset);
void rhiRenderPassEncoderMultiDrawIndexedIndirect(RHIRenderPassEncoder encoder, RHIBuffer indirectBuffer, uint64_t indirectOffset, uint32_t maxDrawCount, RHIBuffer drawCountBuffer, uint64_t drawCountBufferOffset);
void rhiRenderPassEncoderSetPushConstant(RHIRenderPassEncoder encoder, const void* data, uint32_t size);
void rhiRenderPassEncoderBeginDebugLabel(RHIRenderPassEncoder encoder, RHIStringView label, const RHIColor* color);
void rhiRenderPassEncoderEndDebugLabel(RHIRenderPassEncoder encoder);
void rhiRenderPassEncoderEnd(RHIRenderPassEncoder encoder);
void rhiRenderPassEncoderAddRef(RHIRenderPassEncoder encoder);
void rhiRenderPassEncoderRelease(RHIRenderPassEncoder encoder);
// methods of ComputePassEncoder
void rhiComputePassEncoderSetPipeline(RHIComputePassEncoder encoder, RHIComputePipeline pipeline);
void rhiComputePassEncoderDispatch(RHIComputePassEncoder encoder, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
void rhiComputePassEncoderDispatchIndirect(RHIComputePassEncoder encoder, RHIBuffer indirectBuffer, uint64_t indirectOffset);
void rhiComputePassEncoderSetBindSet(RHIComputePassEncoder encoder, RHIBindSet set, uint32_t setIndex, uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets);
void rhiComputePassEncoderSetPushConstant(RHIComputePassEncoder encoder, const void* data, uint32_t size);
void rhiComputePassEncoderBeginDebugLabel(RHIComputePassEncoder encoder, RHIStringView label, const RHIColor* color);
void rhiComputePassEncoderEndDebugLabel(RHIComputePassEncoder encoder);
void rhiComputePassEncoderEnd(RHIComputePassEncoder encoder);
void rhiComputePassEncoderAddRef(RHIComputePassEncoder encoder);
void rhiComputePassEncoderRelease(RHIComputePassEncoder encoder);
// methods of ComputePipeline
void rhiComputePipelineAddRef(RHIComputePipeline pipeline);
void rhiComputePipelineRelease(RHIComputePipeline pipeline);
// methods of CommandList
void rhiCommandListAddRef(RHICommandList commandList);
void rhiCommandListRelease(RHICommandList commandList);
// methods of RenderPipeline
void rhiRenderPipelineAddRef(RHIRenderPipeline pipeline);
void rhiRenderPipelineRelease(RHIRenderPipeline pipeline);
// methods of PipelineLayout
RHIBindSetLayout rhiPipelineLayoutIGetBindSetLayout(RHIPipelineLayout pipelineLayout, uint32_t bindSetIndex);
void rhiPipelineLayoutAddRef(RHIPipelineLayout pipelineLayout);
void rhiPipelineLayoutRelease(RHIPipelineLayout pipelineLayout);
// methods of BindSetLayout
void rhiBindSetLayoutAddRef(RHIBindSetLayout bindSetLayout);
void rhiBindSetLayoutRelease(RHIBindSetLayout bindSetLayout);
// methods of BindSet
void rhiBindSetDestroy(RHIBindSet bindSet);
void rhiBindSetAddRef(RHIBindSet bindSet);
void rhiBindSetRelease(RHIBindSet bindSet);
// methods of Buffer
RHIBufferUsage rhiBufferGetUsage(RHIBuffer buffer);
uint64_t rhiBufferGetSize(RHIBuffer buffer);
void* rhiBufferGetMappedPointer(RHIBuffer buffer);
void rhiBufferDestroy(RHIBuffer buffer);
void rhiBufferAddRef(RHIBuffer buffer);
void rhiBufferRelease(RHIBuffer buffer);
// methods of Texture
uint32_t rhiTextureGetWidth(RHITexture texture);
uint32_t rhiTextureGetHeight(RHITexture texture);
uint32_t rhiTextureGetDepthOrArrayLayers(RHITexture texture);
uint32_t rhiTextureGetMipLevelCount(RHITexture texture);
uint32_t rhiTextureGetSampleCount(RHITexture texture);
RHITextureDimension rhiTextureGetDimension(RHITexture texture);
RHITextureFormat rhiTextureGetFormat(RHITexture texture);
RHITextureUsage rhiTextureGetUsage(RHITexture texture);
RHITextureView rhiTextureCreateView(RHITexture texture, const RHITextureViewDesc* desc);
void rhiTextureDestroy(RHITexture texture);
void rhiTextureAddRef(RHITexture texture);
void rhiTextureRelease(RHITexture texture);
// methods of TextureView
void rhiTextureViewAddRef(RHITextureView textureView);
void rhiTextureViewRelease(RHITextureView textureView);
// methods of Sampler
//void rhiSamplerDestroy(RHISampler sampler);
void rhiSamplerAddRef(RHISampler sampler);
void rhiSamplerRelease(RHISampler sampler);
// methods of ShaderModule
void rhiShaderModuleAddRef(RHIShaderModule shaderModule);
void rhiShaderModuleRelease(RHIShaderModule shaderModule);


