#include "TextureBase.h"

#include "DeviceBase.h"
#include "common/Constants.h"
#include "common/Error.h"

namespace rhi::impl
{
	TextureUsage AddInternalUsage(TextureUsage usage)
	{
		if ((usage & TextureUsage::StorageBinding) != 0)
		{
			usage |= cReadOnlyStorageTexture;
		}
		return usage;
	}

	TextureBase::TextureBase(DeviceBase* device, const TextureDesc& desc): 
		mDimension(desc.dimension),
		mFormat(desc.format),
		mWidth(desc.width),
		mHeight(desc.height),
		mDepth(desc.depth),
		mMipLevelCount(desc.mipLevelCount),
		mSampleCount(desc.sampleCount),
		mUsage(desc.usage),
		mInternalUsage(AddInternalUsage(desc.usage)),
		ResourceBase(device, desc.name)
	{

	}

	TextureBase::~TextureBase() {}

	void TextureBase::Initialize()
	{
		ResourceBase::Initialize();
	}

	ResourceType TextureBase::GetType() const
	{
		return ResourceType::Texture;
	}

	ResourceList* TextureBase::GetViewList() 
	{
		return &mTextureViews;
	}

	TextureUsage TextureBase::GetInternalUsage() const
	{
		return mInternalUsage;
	}

	SubresourceRange TextureBase::GetAllSubresources() const
	{
		return { GetAspectFromFormat(mFormat), 0, mDepth, 0, mMipLevelCount};
	}

	uint32_t TextureBase::APIGetWidth() const
	{
		return mWidth;
	}

	uint32_t TextureBase::APIGetHeight() const
	{
		return mHeight;
	}

	uint32_t TextureBase::APIGetDepthOrArrayLayers() const
	{
		return mDepth;
	}

	uint32_t TextureBase::APIGetMipLevelCount() const
	{
		return mMipLevelCount;
	}

	uint32_t TextureBase::APIGetSampleCount() const
	{
		return mSampleCount;
	}

	TextureDimension TextureBase::APIGetDimension() const
	{
		return mDimension;
	}

	TextureFormat TextureBase::APIGetFormat() const
	{
		return mFormat;
	}

	TextureUsage TextureBase::APIGetUsage() const
	{
		return mUsage;
	}

	TextureViewDesc FillWithDefualtTextureViewDesc(TextureBase* texture, const TextureViewDesc* desc)
	{
		TextureViewDesc result{};
		if (desc)
		{
			result = *desc;
		}
		if (result.dimension == TextureDimension::Undefined)
		{
			result.dimension = texture->APIGetDimension();
		}
		if (result.format == TextureFormat::Undefined)
		{
			result.format = texture->APIGetFormat();
		}
		if (result.arrayLayerCount == CArraySizeUndefined)
		{
			result.arrayLayerCount = texture->APIGetDepthOrArrayLayers() - result.baseArrayLayer;
		}
		if (result.mipLevelCount == CMipLevelCountUndefined)
		{
			result.mipLevelCount = texture->APIGetMipLevelCount() - result.baseMipLevel;
		}
		return result;
	}

	TextureViewBase* TextureBase::APICreateView(const TextureViewDesc* desc)
	{
		TextureViewDesc filledDesc = FillWithDefualtTextureViewDesc(this, desc);
		Ref<TextureViewBase> textureView = CreateView(filledDesc);
		return textureView.Detach();
	}

	void TextureBase::APIDestroy()
	{
		Destroy();
	}

	bool TextureBase::IsDestoryed() const
	{
		return mDestoryed;
	}


	TextureUsage GetTextureViewUsage(TextureUsage srcUsage, TextureUsage requestedViewUsage)
	{
		// If a view's requested usage is None, inherit usage from the source texture.
		if (requestedViewUsage == TextureUsage::None)
		{
			return srcUsage;
		}
		else
		{
			return requestedViewUsage;
		}
	}


	TextureViewBase::TextureViewBase(TextureBase* texture, const TextureViewDesc& desc) :
		mTexture(texture),
		mDimension(desc.dimension),
		mFormat(desc.format),
		mRange(ViewAspectConvert(desc.format, desc.aspect), desc.baseArrayLayer, desc.arrayLayerCount, desc.baseMipLevel, desc.mipLevelCount),
		mUsage(GetTextureViewUsage(texture->APIGetUsage(), desc.usage)),
		mInternalUsage(GetTextureViewUsage(texture->GetInternalUsage(), desc.usage)),
		ResourceBase(texture->GetDevice(), desc.name)
	{

	}

	TextureViewBase::~TextureViewBase() {}

	void TextureViewBase::Initialize()
	{
		ResourceBase::Initialize();
	}

	ResourceList* TextureViewBase::GetList() const
	{
		return mTexture->GetViewList();
	}

	ResourceType TextureViewBase::GetType() const
	{
		return ResourceType::TextureView;
	}

	TextureUsage TextureViewBase::GetInternalUsage() const
	{
		return mInternalUsage;
	}

	uint32_t TextureViewBase::GetBaseMipLevel() const
	{
		return mRange.baseMipLevel;
	}

	uint32_t TextureViewBase::GetLevelCount() const
	{
		return mRange.levelCount;
	}

	uint32_t TextureViewBase::GetBaseArrayLayer() const
	{
		return mRange.baseArrayLayer;
	}

	uint32_t TextureViewBase::GetLayerCount() const
	{
		return mRange.layerCount;
	}

	Aspect TextureViewBase::GetAspects() const
	{
		return mRange.aspects;
	}

	TextureUsage TextureViewBase::GetUsage() const
	{
		return mUsage;
	}

	TextureDimension TextureViewBase::GetDimension() const
	{
		return mDimension;
	}

	const SubresourceRange& TextureViewBase::GetSubresourceRange() const
	{
		return mRange;
	}

	TextureBase* TextureViewBase::GetTexture() const
	{
		return mTexture.Get();
	}

	// Format mapping table. The rows must be in the exactly same order as Format enum members are defined.
	static const FormatInfo c_FormatInfo[] = {
		//  format                   name           bytes blockSize   red   green   blue  alpha  depth  stencl type
	{ TextureFormat::Undefined,           "UNKNOWN",           0,   0,  false, false, false, false, false, false,  FormatComponentType::Undefined},
	{ TextureFormat::R8_UINT,           "R8_UINT",           1,   1,  true,  false, false, false, false, false,  FormatComponentType::Uint		},
	{ TextureFormat::R8_SINT,           "R8_SINT",           1,   1,  true,  false, false, false, false, false,  FormatComponentType::Sint		},
	{ TextureFormat::R8_UNORM,          "R8_UNORM",          1,   1,  true,  false, false, false, false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::R8_SNORM,          "R8_SNORM",          1,   1,  true,  false, false, false, false, false,  FormatComponentType::Snorm	},
	{ TextureFormat::RG8_UINT,          "RG8_UINT",          2,   1,  true,  true,  false, false, false, false,  FormatComponentType::Uint		},
	{ TextureFormat::RG8_SINT,          "RG8_SINT",          2,   1,  true,  true,  false, false, false, false,  FormatComponentType::Sint		},
	{ TextureFormat::RG8_UNORM,         "RG8_UNORM",         2,   1,  true,  true,  false, false, false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::RG8_SNORM,         "RG8_SNORM",         2,   1,  true,  true,  false, false, false, false,  FormatComponentType::Snorm	},
	{ TextureFormat::R16_UINT,          "R16_UINT",          2,   1,  true,  false, false, false, false, false,  FormatComponentType::Uint		},
	{ TextureFormat::R16_SINT,          "R16_SINT",          2,   1,  true,  false, false, false, false, false,  FormatComponentType::Sint		},
	{ TextureFormat::R16_UNORM,         "R16_UNORM",         2,   1,  true,  false, false, false, false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::R16_SNORM,         "R16_SNORM",         2,   1,  true,  false, false, false, false, false,  FormatComponentType::Snorm	},
	{ TextureFormat::R16_FLOAT,         "R16_FLOAT",         2,   1,  true,  false, false, false, false, false,  FormatComponentType::Float	},
	{ TextureFormat::BGRA4_UNORM,       "BGRA4_UNORM",       2,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::B5G6R5_UNORM,      "B5G6R5_UNORM",      2,   1,  true,  true,  true,  false, false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::B5G5R5A1_UNORM,    "B5G5R5A1_UNORM",    2,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::RGBA8_UINT,        "RGBA8_UINT",        4,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Uint		},
	{ TextureFormat::RGBA8_SINT,        "RGBA8_SINT",        4,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Sint		},
	{ TextureFormat::RGBA8_UNORM,       "RGBA8_UNORM",       4,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::RGBA8_SNORM,       "RGBA8_SNORM",       4,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Snorm	},
	{ TextureFormat::BGRA8_UNORM,       "BGRA8_UNORM",       4,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::RGBA8_SRGB,		 "RGBA8_SRGB",		  4,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::UnormSrgb},
	{ TextureFormat::BGRA8_SRGB,		 "BGRA8_SRGB",		  4,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::UnormSrgb},
	{ TextureFormat::R10G10B10A2_UNORM, "R10G10B10A2_UNORM", 4,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::R11G11B10_FLOAT,   "R11G11B10_FLOAT",   4,   1,  true,  true,  true,  false, false, false,  FormatComponentType::Float	},
	{ TextureFormat::RG16_UINT,         "RG16_UINT",         4,   1,  true,  true,  false, false, false, false,  FormatComponentType::Uint		},
	{ TextureFormat::RG16_SINT,         "RG16_SINT",         4,   1,  true,  true,  false, false, false, false,  FormatComponentType::Sint		},
	{ TextureFormat::RG16_UNORM,        "RG16_UNORM",        4,   1,  true,  true,  false, false, false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::RG16_SNORM,        "RG16_SNORM",        4,   1,  true,  true,  false, false, false, false,  FormatComponentType::Snorm	},
	{ TextureFormat::RG16_FLOAT,        "RG16_FLOAT",        4,   1,  true,  true,  false, false, false, false,  FormatComponentType::Float	},
	{ TextureFormat::R32_UINT,          "R32_UINT",          4,   1,  true,  false, false, false, false, false,  FormatComponentType::Uint		},
	{ TextureFormat::R32_SINT,          "R32_SINT",          4,   1,  true,  false, false, false, false, false,  FormatComponentType::Sint		},
	{ TextureFormat::R32_FLOAT,         "R32_FLOAT",         4,   1,  true,  false, false, false, false, false,  FormatComponentType::Float	},
	{ TextureFormat::RGBA16_UINT,       "RGBA16_UINT",       8,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Uint		},
	{ TextureFormat::RGBA16_SINT,       "RGBA16_SINT",       8,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Sint		},
	{ TextureFormat::RGBA16_FLOAT,      "RGBA16_FLOAT",      8,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Float	},
	{ TextureFormat::RGBA16_UNORM,      "RGBA16_UNORM",      8,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Unorm	},
	{ TextureFormat::RGBA16_SNORM,      "RGBA16_SNORM",      8,   1,  true,  true,  true,  true,  false, false,  FormatComponentType::Snorm	},
	{ TextureFormat::RG32_UINT,         "RG32_UINT",         8,   1,  true,  true,  false, false, false, false,  FormatComponentType::Uint		},
	{ TextureFormat::RG32_SINT,         "RG32_SINT",         8,   1,  true,  true,  false, false, false, false,  FormatComponentType::Sint		},
	{ TextureFormat::RG32_FLOAT,        "RG32_FLOAT",        8,   1,  true,  true,  false, false, false, false,  FormatComponentType::Float	},
	{ TextureFormat::RGB32_UINT,        "RGB32_UINT",        12,  1,  true,  true,  true,  false, false, false,  FormatComponentType::Uint		},
	{ TextureFormat::RGB32_SINT,        "RGB32_SINT",        12,  1,  true,  true,  true,  false, false, false,  FormatComponentType::Sint		},
	{ TextureFormat::RGB32_FLOAT,       "RGB32_FLOAT",       12,  1,  true,  true,  true,  false, false, false,  FormatComponentType::Float	},
	{ TextureFormat::RGBA32_UINT,       "RGBA32_UINT",       16,  1,  true,  true,  true,  true,  false, false,  FormatComponentType::Uint		},
	{ TextureFormat::RGBA32_SINT,       "RGBA32_SINT",       16,  1,  true,  true,  true,  true,  false, false,  FormatComponentType::Sint		},
	{ TextureFormat::RGBA32_FLOAT,      "RGBA32_FLOAT",      16,  1,  true,  true,  true,  true,  false, false,  FormatComponentType::Float	},
	{ TextureFormat::D16_UNORM,         "D16_UNORM",         2,   1,  false, false, false, false, true,  false,  FormatComponentType::Undefined},
	{ TextureFormat::D24_UNORM_S8_UINT, "D24_UNORM_S8_UINT", 4,   1,  false, false, false, false, true,  true,   FormatComponentType::Undefined},
	{ TextureFormat::D32_UNORM,         "D32_UNORM",         4,   1,  false, false, false, false, true,  false,  FormatComponentType::Undefined},
	{ TextureFormat::D32_UNORM_S8_UINT, "D32_UNORM_S8_UINT", 8,   1,  false, false, false, false, true,  true,   FormatComponentType::Undefined},
	{ TextureFormat::BC1_UNORM,         "BC1_UNORM",         8,   4,  true,  true,  true,  true,  false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC1_UNORM_SRGB,    "BC1_UNORM_SRGB",    8,   4,  true,  true,  true,  true,  false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC2_UNORM,         "BC2_UNORM",         16,  4,  true,  true,  true,  true,  false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC2_UNORM_SRGB,    "BC2_UNORM_SRGB",    16,  4,  true,  true,  true,  true,  false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC3_UNORM,         "BC3_UNORM",         16,  4,  true,  true,  true,  true,  false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC3_UNORM_SRGB,    "BC3_UNORM_SRGB",    16,  4,  true,  true,  true,  true,  false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC4_UNORM,         "BC4_UNORM",         8,   4,  true,  false, false, false, false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC4_SNORM,         "BC4_SNORM",         8,   4,  true,  false, false, false, false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC5_UNORM,         "BC5_UNORM",         16,  4,  true,  true,  false, false, false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC5_SNORM,         "BC5_SNORM",         16,  4,  true,  true,  false, false, false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC6H_UFLOAT,       "BC6H_UFLOAT",       16,  4,  true,  true,  true,  false, false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC6H_SFLOAT,       "BC6H_SFLOAT",       16,  4,  true,  true,  true,  false, false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC7_UNORM,         "BC7_UNORM",         16,  4,  true,  true,  true,  true,  false, false,  FormatComponentType::Undefined},
	{ TextureFormat::BC7_UNORM_SRGB,    "BC7_UNORM_SRGB",    16,  4,  true,  true,  true,  true,  false, false,  FormatComponentType::Undefined},
	};

	const FormatInfo& GetFormatInfo(TextureFormat format)
	{
		static_assert(sizeof(c_FormatInfo) / sizeof(FormatInfo) == size_t(TextureFormat::COUNT),
			"The format info table doesn't have the right number of elements");
		const FormatInfo& info = c_FormatInfo[uint32_t(format)];
		assert(info.format == format);
		return info;
	}
}