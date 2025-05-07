#pragma once

#include <cstdint>
#include <string_view>

namespace rhi::impl
{
	class AdapterBase;
	class BindSetBase;
	class BindSetLayoutBase;
	class BufferBase;
	class ComputePipelineBase;
	class CommandListBase;
	class CommandEncoder;
	class ComputePassEncoder;
	class SharedTextureMemoryBase;
	class InstanceBase;
	class PhysicalDeviceBase;
	class PipelineBase;
	class PipelineCacheBase;
	class PipelineLayoutBase;
	//class QuerySetBase;
	class QueueBase;
	class RenderPassEncoder;
	class RenderPipelineBase;
	class ResourceHeapBase;
	class SamplerBase;
	class ShaderModuleBase;
	class SwapChainBase;
	class TextureBase;
	class TextureViewBase;
	class DeviceBase;
	class SurfaceBase;

constexpr uint32_t CMaxColorAttachments = 8;
constexpr uint64_t CWholeSize = ~0ull;
constexpr uint32_t CAutoCompute = uint32_t(-1);
constexpr uint32_t CArraySizeUndefined = uint32_t(-1);
constexpr uint32_t CMipLevelCountUndefined = uint32_t(-1);


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
		Read,
		Write
	};

	enum class BufferMapAsyncStatus
	{
		Success,
		None,
		DeviceLost,
		DestroyedBeforeCallback,
	};

	// defualt VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
	enum class BufferUsage : uint32_t
	{
		None = 0 << 0,
		Vertex = 1 << 0,
		Index = 1 << 1,
		Indirect = 1 << 2,
		Uniform = 1 << 3,
		Storage = 1 << 4,
		MapRead = 1 << 5,
		MapWrite = 1 << 6,
		CopySrc = 1 << 7,
		CopyDst = 1 << 8,
		QueryResolve = 1 << 9
	};
	ENUM_CLASS_FLAG_OPERATORS(BufferUsage);

	enum class TextureDimension : uint32_t
	{
		Undefined,
		Texture1D,
		Texture1DArray,
		Texture2D,
		Texture2DArray,
		TextureCube,
		TextureCubeArray,
		Texture3D
	};

	enum class TextureFormat : uint32_t
	{
		Undefined,

		R8_UINT,
		R8_SINT,
		R8_UNORM,
		R8_SNORM,
		RG8_UINT,
		RG8_SINT,
		RG8_UNORM,
		RG8_SNORM,
		R16_UINT,
		R16_SINT,
		R16_UNORM,
		R16_SNORM,
		R16_FLOAT,
		BGRA4_UNORM,
		B5G6R5_UNORM,
		B5G5R5A1_UNORM,
		RGBA8_UINT,
		RGBA8_SINT,
		RGBA8_UNORM,
		RGBA8_SNORM,
		BGRA8_UNORM,
		RGBA8_SRGB,
		BGRA8_SRGB,
		R10G10B10A2_UNORM,
		R11G11B10_FLOAT,
		RG16_UINT,
		RG16_SINT,
		RG16_UNORM,
		RG16_SNORM,
		RG16_FLOAT,
		R32_UINT,
		R32_SINT,
		R32_FLOAT,
		RGBA16_UINT,
		RGBA16_SINT,
		RGBA16_FLOAT,
		RGBA16_UNORM,
		RGBA16_SNORM,
		RG32_UINT,
		RG32_SINT,
		RG32_FLOAT,
		RGB32_UINT,
		RGB32_SINT,
		RGB32_FLOAT,
		RGBA32_UINT,
		RGBA32_SINT,
		RGBA32_FLOAT,

		D16_UNORM,
		D24_UNORM_S8_UINT,
		D32_UNORM,
		D32_UNORM_S8_UINT,

		BC1_UNORM,
		BC1_UNORM_SRGB,
		BC2_UNORM,
		BC2_UNORM_SRGB,
		BC3_UNORM,
		BC3_UNORM_SRGB,
		BC4_UNORM,
		BC4_SNORM,
		BC5_UNORM,
		BC5_SNORM,
		BC6H_UFLOAT,
		BC6H_SFLOAT,
		BC7_UNORM,
		BC7_UNORM_SRGB,

		COUNT,
	};

	enum class VertexFormat : uint32_t
	{
		Uint8,
		Uint8x2,
		Uint8x4,
		Sint8,
		Sint8x2,
		Sint8x4,
		Unorm8,
		Unorm8x2,
		Unorm8x4,
		Snorm8,
		Snorm8x2,
		Snorm8x4,
		Uint16,
		Uint16x2,
		Uint16x4,
		Sint16,
		Sint16x2,
		Sint16x4,
		Unorm16,
		Unorm16x2,
		Unorm16x4,
		Snorm16,
		Snorm16x2,
		Snorm16x4,
		Float16,
		Float16x2,
		Float16x4,
		Float32,
		Float32x2,
		Float32x3,
		Float32x4,
		Uint32,
		Uint32x2,
		Uint32x3,
		Uint32x4,
		Sint32,
		Sint32x2,
		Sint32x3,
		Sint32x4,
		Unorm10_10_10_2,
	};

	enum class IndexFormat : uint32_t
	{
		Uint16,
		Uint32
	};

	enum class TextureUsage : uint32_t
	{
		None = 0 << 0,
		CopySrc = 1 << 0,
		CopyDst = 1 << 1,
		SampledBinding = 1 << 2,
		StorageBinding = 1 << 3,
		RenderAttachment = 1 << 4
	};
	ENUM_CLASS_FLAG_OPERATORS(TextureUsage);

	enum class TextureAspect : uint32_t
	{
		None = 0 << 0,
		All = 1 << 0,
		Depth = 1 << 1,
		Stencil = 1 << 2,
		Plane0 = 1 << 3,
		Plane1 = 1 << 4,
		Plane2 = 1 << 5
	};
	ENUM_CLASS_FLAG_OPERATORS(TextureAspect);

	enum class BlendFactor : uint32_t
	{
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha,
		SrcAlphaSaturate,
		Src1Color,
		OneMinusSrc1Color,
		Src1Alpha,
		OneMinusSrc1Alpha
	};

	enum class BlendOp : uint8_t
	{
		Add = 0,
		Subrtact = 1,
		ReverseSubtract = 2,
		Min = 3,
		Max = 4
	};

	enum class ColorMask : uint32_t
	{
		// These values are equal to their counterparts in DX11, DX12, and Vulkan.
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8,
		All = 0xF
	};
	ENUM_CLASS_FLAG_OPERATORS(ColorMask);

	enum class SamplerAddressMode : uint32_t
	{
		ClampToEdge,
		Repeat,
		ClampToBorder,
		MirroredRepeat,
		MirrorClampToEdge
	};

	enum class FilterMode : uint32_t
	{
		Linear,
		Nearest
	};

	enum class BorderColor : uint32_t
	{
		FloatOpaqueBlack,
		FloatOpaqueWhite,
		FloatTransparentBlack
	};

	enum class BindingType : uint32_t
	{
		None,
		SampledTexture, //SRV
		StorageTexture, //URV
		ReadOnlyStorageTexture,
		UniformBuffer, // CBV
		StorageBuffer, // UAV
		ReadOnlyStorageBuffer,
		//UniformBufferDynamic,
		//StorageBufferDynamic,

		Sampler,
		CombinedTextureSampler
	};

	enum class ShaderStage : uint32_t
	{
		None = (0 << 0),
		Vertex = (1 << 0),
		TessellationControl = (1 << 1),
		TessellationEvaluation = (1 << 2),
		Geometry = (1 << 3),
		Fragment = (1 << 4),
		Task = (1 << 5),
		Mesh = (1 << 6),
		Compute = (1 << 7),
		AllGraphics = Vertex | TessellationControl | TessellationEvaluation | Geometry | Fragment,
		All = Vertex | TessellationControl | TessellationEvaluation | Geometry | Fragment | Task | Mesh | Compute
	};
	ENUM_CLASS_FLAG_OPERATORS(ShaderStage);

	enum class FillMode : uint32_t
	{
		Fill = 0,
		Line = 1,
		Point = 2
	};

	enum class CullMode : uint32_t
	{
		None,
		Front,
		Back
	};

	enum class FrontFace : uint32_t
	{
		FrontCounterClockwise,
		FrontClockwise
	};

	enum class CompareOp : uint32_t
	{
		Never = 0,
		Less = 1,
		Equal = 2,
		LessOrEqual = 3,
		Greater = 4,
		NotEqual = 5,
		GreaterOrEqual = 6,
		Always = 7
	};

	enum class StencilOp : uint32_t
	{
		Keep = 0,
		Zero = 1,
		Replace = 2,
		IncrementAndClamp = 3,
		DecrementAndClamp = 4,
		Invert = 5,
		IncrementAndWrap = 6,
		DecrementAndWrap = 7
	};

	enum class PrimitiveType : uint32_t
	{
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		TriangleFan,
		PatchList
	};

	enum class VertexInputRate : uint32_t
	{
		Vertex,
		Instance
	};

	enum class QueueType
	{
		Graphics,
		Compute,
		Transfer,
		Undefined
	};

	enum class LoadOp : uint32_t
	{
		DontCare,
		Load,
		Clear
	};

	enum class StoreOp : uint32_t
	{
		Store,
		Discard
	};

	enum class LoggingSeverity : uint32_t
	{
		Verbose,
		Info,
		Warning,
		Error,
		Fatal
	};

	enum class FeatureName : uint32_t
	{
		ShaderInt16,
		ShaderInt64,
		ShaderFloat64,
		SampleRateShading,
		SamplerAnisotropy,
		TextureCompressionBC,
		TextureCompressionETC2,
		TextureCompressionASTC,
		GeometryShader,
		TessellationShader,
		MultiViewport,
		MultiDrawIndirect,
		DepthBiasClamp,
		DepthClamp,
		R8UnormStorage,
		Count
	};

	enum class BackendType : uint32_t
	{
		Vulkan
	};

	enum class AdapterType : uint32_t
	{
		DiscreteGPU,
		IntegratedGPU,
		VirtualGPU,
		Cpu,
		Unknown
	};

	enum class ShareMode : uint32_t
	{
		Exclusive,
		Concurrent
	};

	enum class PresentMode : uint32_t
	{
		Fifo,
		FifoRelaxed,
		Immediate,
		Mailbox
	};

	enum class SurfaceAcquireNextTextureStatus : uint32_t
	{
		Success,
		Suboptimal,
		Timeout,
		Outdated,
		SurfaceLost,
		Error
	};

	using BufferMapCallback = void (*)(BufferMapAsyncStatus status, void* mappedAdress, void* userdata);
	typedef void(_stdcall* LoggingCallback) (LoggingSeverity severity, const char* msg, void* userData);
	//using DebugMessageCallbackFunc = std::function<void(MessageSeverity severity, const char* msg)>;

	struct Region3D
	{
		uint32_t minX = 0;
		uint32_t maxX = 0;
		uint32_t minY = 0;
		uint32_t maxY = 0;
		uint32_t minZ = 0;
		uint32_t maxZ = 1;
		uint32_t getWidth() const { return maxX - minX; }
		uint32_t getHeight() const { return maxY - minY; }
		uint32_t getDepth() const { return maxZ - minZ; }
		bool isValid() const { return maxX > minX && maxY > minY && maxZ > minZ; }
	};

	struct Origin3D
	{
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t z = 0;
	};

	struct Extent3D
	{
		uint32_t width = 1;
		uint32_t height = 1;
		uint32_t depthOrArrayLayers = 1;
	};


	struct Color
	{
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 0.0f;
	};

	struct Viewport
	{
		float    x;
		float    y;
		float    width;
		float    height;
		float    minDepth;
		float    maxDepth;
	};

	struct Rect
	{
		uint32_t x;
		uint32_t y;
		uint32_t width;
		uint32_t height;
	};

	struct TextureDataLayout
	{
		uint64_t offset;
		uint32_t bytesPerRow;
		uint32_t rowsPerImage;
	};

	// describes a 2D section of a single mip level + single slice of a texture
	struct TextureSlice
	{
		TextureBase* texture;
		Origin3D origin;
		Extent3D size;
		uint32_t mipLevel = 0;
		TextureAspect aspect = TextureAspect::All;
	};

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

	struct SurfaceConfiguration
	{
		DeviceBase* device;
		TextureFormat format = TextureFormat::BGRA8_SRGB;
		uint32_t width;
		uint32_t height;
		PresentMode presentMode = PresentMode::Fifo;
	};

	struct ShaderState
	{
		ShaderModuleBase* shaderModule;
		SpecializationConstant const* constants;
		uint32_t constantCount;
	};

	struct VertexInputAttribute
	{
		uint32_t bindingBufferSlot = 0;
		uint32_t location = 0;
		VertexFormat format = VertexFormat::Float32x3;
		VertexInputRate rate = VertexInputRate::Vertex;
		// If this value is set to UINT32_MAX (default value), the offset will
		// be computed automatically by placing the element right after the previous one.
		uint32_t offsetInElement = CAutoCompute;
		// Stride in bytes between two elements, for one buffer slot.
		// If this value is set to UINT32_MAX, the stride will be
		// computed automatically assuming that all elements in the same buffer slot that are
		// packed one by one. or must specify the same stride in the same buffer.
		uint32_t elementStride = CAutoCompute;
	};

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

	struct BlendState
	{
		bool alphaToCoverageEnable = false;

		ColorAttachmentBlendState colorAttachmentBlendStates[CMaxColorAttachments];
		//std::array<ColorAttachmentBlendState, CMaxColorAttachments> colorAttachmentBlendStates;
	};

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


	struct RasterState
	{
		PrimitiveType primitiveType = PrimitiveType::TriangleList;
		FillMode fillMode = FillMode::Fill;
		CullMode cullMode = CullMode::Back;
		FrontFace frontFace = FrontFace::FrontCounterClockwise;
		bool depthClampEnable = false; //must be false if this feature is not enabled
		float lineWidth = 1.0f;
	};

	struct SampleState
	{
		uint32_t count = 1;
		uint32_t quality = 0;
		uint32_t mask = 0xFFFFFFFF;
	};

	struct ColorAttachment
	{
		TextureViewBase* view = nullptr;
		TextureViewBase* resolveView = nullptr;
		LoadOp loadOp = LoadOp::Clear;
		StoreOp storeOp = StoreOp::Store;
		Color clearValue = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	struct DepthStencilAattachment
	{
		TextureViewBase* view;
		LoadOp depthLoadOp = LoadOp::Clear;
		StoreOp depthStoreOp = StoreOp::Store;
		LoadOp stencilLoadOp = LoadOp::Clear;
		StoreOp stencilStoreOp = StoreOp::Store;
		float depthClearValue = 1.0f;
		uint32_t stencilClearValue = 0;
	};


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

	struct BindSetEntry
	{
		uint32_t binding = 0;
		uint32_t arrayElementIndex = 0;

		TextureViewBase* textureView = nullptr;
		SamplerBase* sampler = nullptr;
		BufferBase* buffer = nullptr;

		//for buffer
		uint32_t bufferOffset = 0;
		uint64_t bufferRange = CWholeSize;
	};

	struct PushConstantRange
	{
		ShaderStage visibility = ShaderStage::None;
		uint32_t size = 0;
	};

	struct TextureSubresourceRange
	{
		TextureAspect aspect = TextureAspect::All;
		uint32_t baseMipLevel = 0;
		uint32_t mipLevelCount;
		uint32_t baseArrayLayer = 0;
		uint32_t arrayLayerCount;
	};

	struct TextureSubresources
	{
		TextureBase* texture;
		TextureSubresourceRange range;
	};

	struct ResourceTransfer
	{
		QueueBase* receivingQueue;
		BufferBase* const* buffers;
		uint32_t bufferCount;
		TextureSubresources const* textureSubresources;
		uint32_t textureSubresourceCount;
	};

	struct BindSetLayoutDesc
	{
		std::string_view name;
		uint32_t entryCount;
		BindSetLayoutEntry const* entries;
	};

	struct BindSetDesc
	{
		std::string_view name;
		BindSetLayoutBase* layout;
		uint32_t entryCount;
		BindSetEntry const* entries;
	};

	struct BufferDesc
	{
		size_t size = 0;
		std::string_view name;
		BufferUsage usage = BufferUsage::None;
		ShareMode shareMode = ShareMode::Exclusive;
	};

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

	struct TextureViewDesc
	{
		std::string_view name;
		TextureFormat format = TextureFormat::Undefined;
		TextureDimension dimension = TextureDimension::Undefined;
		uint32_t baseMipLevel = 0;
		uint32_t mipLevelCount = CArraySizeUndefined;
		uint32_t baseArrayLayer = 0;
		uint32_t arrayLayerCount = CArraySizeUndefined;
		TextureAspect aspect = TextureAspect::All;
		TextureUsage usage = TextureUsage::None;
	};

	struct ShaderModuleDesc
	{
		ShaderStage type = ShaderStage::None;
		std::string_view name;
		std::string_view entry;
		std::string_view code;
		SpecializationConstant const * specializationConstants;
		uint32_t specializationConstantCount = 0;
	};

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

	struct PipelineLayoutDesc
	{
		std::string_view name;
		BindSetLayoutBase* const* bindSetLayouts;
		uint32_t bindSetLayoutCount;
		PushConstantRange pushConstantRange;
	};

	struct PipelineLayoutDesc2
	{
		std::string_view name;
		ShaderState const* shaders;
		uint32_t shaderCount;
	};

	struct RenderPipelineDesc
	{
		std::string_view name;
		ShaderState* vertexShader = nullptr;
		ShaderState* fragmentShader = nullptr;
		ShaderState* tessControlShader = nullptr;
		ShaderState* tessEvaluationShader = nullptr;
		ShaderState* geometryShader = nullptr;

		PipelineLayoutBase* pipelineLayout;

		VertexInputAttribute const * vertexAttributes;
		uint32_t vertexAttributeCount = 0;

		BlendState blendState;
		RasterState rasterState;
		SampleState sampleState;
		DepthStencilState depthStencilState;

		uint32_t viewportCount = 1;
		uint32_t colorAttachmentCount = 0;
		TextureFormat colorAttachmentFormats[CMaxColorAttachments];
		TextureFormat depthStencilFormat = TextureFormat::Undefined;

		uint32_t patchControlPoints = 0;
	};

	struct ComputePipelineDesc
	{
		std::string_view name;
		ShaderState* computeShader = nullptr;

		PipelineLayoutBase* pipelineLayout;
	};

	struct RenderPassDesc
	{
		uint32_t colorAttachmentCount = 0;
		ColorAttachment const * colorAttachments;
		DepthStencilAattachment const * depthStencilAttachment = nullptr;
	};
	// command list

	struct DrawIndirectCommand
	{
		uint32_t    vertexCount;
		uint32_t    instanceCount;
		uint32_t    firstVertex;
		uint32_t    firstInstance;
	};

	struct DrawIndexedIndirectCommand
	{
		uint32_t    indexCount;
		uint32_t    instanceCount;
		uint32_t    firstIndex;
		int32_t     vertexOffset;
		uint32_t    firstInstance;
	};

	struct DispatchIndirectCommand
	{
		uint32_t    x;
		uint32_t    y;
		uint32_t    z;
	};

	enum class ClearDepthStencilFlag : uint8_t
	{
		Depth = 0 << 0,
		Stencil = 1 << 0
	};
	ENUM_CLASS_FLAG_OPERATORS(ClearDepthStencilFlag);

	struct AdapterInfo
	{
		uint32_t apiVersion;
		uint32_t driverVersion;
		uint32_t vendorID;
		uint32_t deviceID;
		AdapterType adapterType;
		std::string_view deviceName;
	};

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

	struct InstanceDesc
	{
		BackendType backend = BackendType::Vulkan;
		LoggingCallback loggingCallback;
		void* loggingCallbackUserData;
		bool enableDebugLayer;
	};

	struct DeviceDesc
	{
		std::string_view name;
		uint32_t requiredFeatureCount = 0;
		FeatureName const* requiredFeatures;
	};
}