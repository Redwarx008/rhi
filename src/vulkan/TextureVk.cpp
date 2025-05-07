#include "TextureVk.h"
#include "DeviceVk.h"
#include "QueueVk.h"
#include "ErrorsVk.h"
#include "VulkanUtils.h"
#include "RefCountedHandle.h"
#include "../common/EnumFlagIterator.hpp"
#include "../common/Constants.h"
#include "../PassResourceUsage.h"

namespace rhi::impl::vulkan
{
	constexpr TextureUsage cShaderTextureUsages = TextureUsage::SampledBinding | TextureUsage::StorageBinding | cReadOnlyStorageTexture;

	constexpr TextureUsage cReadOnlyTextureUsages = TextureUsage::CopySrc | TextureUsage::SampledBinding | cReadOnlyStorageTexture;

	struct TexFmtToVkFmtMapping
	{
		TextureFormat rhiFormat;
		VkFormat vkFormat;
	};

	static const std::array<TexFmtToVkFmtMapping, size_t(TextureFormat::COUNT)> _texFmtToVkFmtMap = { {
		{ TextureFormat::Undefined,           VK_FORMAT_UNDEFINED                },
		{ TextureFormat::R8_UINT,           VK_FORMAT_R8_UINT                  },
		{ TextureFormat::R8_SINT,           VK_FORMAT_R8_SINT                  },
		{ TextureFormat::R8_UNORM,          VK_FORMAT_R8_UNORM                 },
		{ TextureFormat::R8_SNORM,          VK_FORMAT_R8_SNORM                 },
		{ TextureFormat::RG8_UINT,          VK_FORMAT_R8G8_UINT                },
		{ TextureFormat::RG8_SINT,          VK_FORMAT_R8G8_SINT                },
		{ TextureFormat::RG8_UNORM,         VK_FORMAT_R8G8_UNORM               },
		{ TextureFormat::RG8_SNORM,         VK_FORMAT_R8G8_SNORM               },
		{ TextureFormat::R16_UINT,          VK_FORMAT_R16_UINT                 },
		{ TextureFormat::R16_SINT,          VK_FORMAT_R16_SINT                 },
		{ TextureFormat::R16_UNORM,         VK_FORMAT_R16_UNORM                },
		{ TextureFormat::R16_SNORM,         VK_FORMAT_R16_SNORM                },
		{ TextureFormat::R16_FLOAT,         VK_FORMAT_R16_SFLOAT               },
		{ TextureFormat::BGRA4_UNORM,       VK_FORMAT_B4G4R4A4_UNORM_PACK16    },
		{ TextureFormat::B5G6R5_UNORM,      VK_FORMAT_B5G6R5_UNORM_PACK16      },
		{ TextureFormat::B5G5R5A1_UNORM,    VK_FORMAT_B5G5R5A1_UNORM_PACK16    },
		{ TextureFormat::RGBA8_UINT,        VK_FORMAT_R8G8B8A8_UINT            },
		{ TextureFormat::RGBA8_SINT,        VK_FORMAT_R8G8B8A8_SINT            },
		{ TextureFormat::RGBA8_UNORM,       VK_FORMAT_R8G8B8A8_UNORM           },
		{ TextureFormat::RGBA8_SNORM,       VK_FORMAT_R8G8B8A8_SNORM           },
		{ TextureFormat::BGRA8_UNORM,       VK_FORMAT_B8G8R8A8_UNORM           },
		{ TextureFormat::RGBA8_SRGB,        VK_FORMAT_R8G8B8A8_SRGB            },
		{ TextureFormat::BGRA8_SRGB,		VK_FORMAT_B8G8R8A8_SRGB            },
		{ TextureFormat::R10G10B10A2_UNORM, VK_FORMAT_A2B10G10R10_UNORM_PACK32 },
		{ TextureFormat::R11G11B10_FLOAT,   VK_FORMAT_B10G11R11_UFLOAT_PACK32  },
		{ TextureFormat::RG16_UINT,         VK_FORMAT_R16G16_UINT              },
		{ TextureFormat::RG16_SINT,         VK_FORMAT_R16G16_SINT              },
		{ TextureFormat::RG16_UNORM,        VK_FORMAT_R16G16_UNORM             },
		{ TextureFormat::RG16_SNORM,        VK_FORMAT_R16G16_SNORM             },
		{ TextureFormat::RG16_FLOAT,        VK_FORMAT_R16G16_SFLOAT            },
		{ TextureFormat::R32_UINT,          VK_FORMAT_R32_UINT                 },
		{ TextureFormat::R32_SINT,          VK_FORMAT_R32_SINT                 },
		{ TextureFormat::R32_FLOAT,         VK_FORMAT_R32_SFLOAT               },
		{ TextureFormat::RGBA16_UINT,       VK_FORMAT_R16G16B16A16_UINT        },
		{ TextureFormat::RGBA16_SINT,       VK_FORMAT_R16G16B16A16_SINT        },
		{ TextureFormat::RGBA16_FLOAT,      VK_FORMAT_R16G16B16A16_SFLOAT      },
		{ TextureFormat::RGBA16_UNORM,      VK_FORMAT_R16G16B16A16_UNORM       },
		{ TextureFormat::RGBA16_SNORM,      VK_FORMAT_R16G16B16A16_SNORM       },
		{ TextureFormat::RG32_UINT,         VK_FORMAT_R32G32_UINT              },
		{ TextureFormat::RG32_SINT,         VK_FORMAT_R32G32_SINT              },
		{ TextureFormat::RG32_FLOAT,        VK_FORMAT_R32G32_SFLOAT            },
		{ TextureFormat::RGB32_UINT,        VK_FORMAT_R32G32B32_UINT           },
		{ TextureFormat::RGB32_SINT,        VK_FORMAT_R32G32B32_SINT           },
		{ TextureFormat::RGB32_FLOAT,       VK_FORMAT_R32G32B32_SFLOAT         },
		{ TextureFormat::RGBA32_UINT,       VK_FORMAT_R32G32B32A32_UINT        },
		{ TextureFormat::RGBA32_SINT,       VK_FORMAT_R32G32B32A32_SINT        },
		{ TextureFormat::RGBA32_FLOAT,      VK_FORMAT_R32G32B32A32_SFLOAT      },
		{ TextureFormat::D16_UNORM,         VK_FORMAT_D16_UNORM                },
		{ TextureFormat::D24_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT        },
		{ TextureFormat::D32_UNORM,         VK_FORMAT_D32_SFLOAT               },
		{ TextureFormat::D32_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT       },
		{ TextureFormat::BC1_UNORM,         VK_FORMAT_BC1_RGBA_UNORM_BLOCK     },
		{ TextureFormat::BC1_UNORM_SRGB,    VK_FORMAT_BC1_RGBA_SRGB_BLOCK      },
		{ TextureFormat::BC2_UNORM,         VK_FORMAT_BC2_UNORM_BLOCK          },
		{ TextureFormat::BC2_UNORM_SRGB,    VK_FORMAT_BC2_SRGB_BLOCK           },
		{ TextureFormat::BC3_UNORM,         VK_FORMAT_BC3_UNORM_BLOCK          },
		{ TextureFormat::BC3_UNORM_SRGB,    VK_FORMAT_BC3_SRGB_BLOCK           },
		{ TextureFormat::BC4_UNORM,         VK_FORMAT_BC4_UNORM_BLOCK          },
		{ TextureFormat::BC4_SNORM,         VK_FORMAT_BC4_SNORM_BLOCK          },
		{ TextureFormat::BC5_UNORM,         VK_FORMAT_BC5_UNORM_BLOCK          },
		{ TextureFormat::BC5_SNORM,         VK_FORMAT_BC5_SNORM_BLOCK          },
		{ TextureFormat::BC6H_UFLOAT,       VK_FORMAT_BC6H_UFLOAT_BLOCK        },
		{ TextureFormat::BC6H_SFLOAT,       VK_FORMAT_BC6H_SFLOAT_BLOCK        },
		{ TextureFormat::BC7_UNORM,         VK_FORMAT_BC7_UNORM_BLOCK          },
		{ TextureFormat::BC7_UNORM_SRGB,    VK_FORMAT_BC7_SRGB_BLOCK           },

	} };

	static const std::unordered_map<VkFormat, TextureFormat> _VkFmtToTexFmtMap =
	{
		{ VK_FORMAT_UNDEFINED                ,TextureFormat::Undefined          },
		{ VK_FORMAT_R8_UINT                  ,TextureFormat::R8_UINT          },
		{ VK_FORMAT_R8_SINT                  ,TextureFormat::R8_SINT          },
		{ VK_FORMAT_R8_UNORM                 ,TextureFormat::R8_UNORM         },
		{ VK_FORMAT_R8_SNORM                 ,TextureFormat::R8_SNORM         },
		{ VK_FORMAT_R8G8_UINT                ,TextureFormat::RG8_UINT         },
		{ VK_FORMAT_R8G8_SINT                ,TextureFormat::RG8_SINT         },
		{ VK_FORMAT_R8G8_UNORM               ,TextureFormat::RG8_UNORM        },
		{ VK_FORMAT_R8G8_SNORM               ,TextureFormat::RG8_SNORM        },
		{ VK_FORMAT_R16_UINT                 ,TextureFormat::R16_UINT         },
		{ VK_FORMAT_R16_SINT                 ,TextureFormat::R16_SINT         },
		{ VK_FORMAT_R16_UNORM                ,TextureFormat::R16_UNORM        },
		{ VK_FORMAT_R16_SNORM                ,TextureFormat::R16_SNORM        },
		{ VK_FORMAT_R16_SFLOAT               ,TextureFormat::R16_FLOAT        },
		{ VK_FORMAT_B4G4R4A4_UNORM_PACK16    ,TextureFormat::BGRA4_UNORM      },
		{ VK_FORMAT_B5G6R5_UNORM_PACK16      ,TextureFormat::B5G6R5_UNORM     },
		{ VK_FORMAT_B5G5R5A1_UNORM_PACK16    ,TextureFormat::B5G5R5A1_UNORM   },
		{ VK_FORMAT_R8G8B8A8_UINT            ,TextureFormat::RGBA8_UINT       },
		{ VK_FORMAT_R8G8B8A8_SINT            ,TextureFormat::RGBA8_SINT       },
		{ VK_FORMAT_R8G8B8A8_UNORM           ,TextureFormat::RGBA8_UNORM      },
		{ VK_FORMAT_R8G8B8A8_SNORM           ,TextureFormat::RGBA8_SNORM      },
		{ VK_FORMAT_B8G8R8A8_UNORM           ,TextureFormat::BGRA8_UNORM      },
		{ VK_FORMAT_R8G8B8A8_SRGB            ,TextureFormat::RGBA8_SRGB       },
		{ VK_FORMAT_B8G8R8A8_SRGB            ,TextureFormat::BGRA8_SRGB	   },
		{ VK_FORMAT_A2B10G10R10_UNORM_PACK32 ,TextureFormat::R10G10B10A2_UNORM},
		{ VK_FORMAT_B10G11R11_UFLOAT_PACK32  ,TextureFormat::R11G11B10_FLOAT  },
		{ VK_FORMAT_R16G16_UINT              ,TextureFormat::RG16_UINT        },
		{ VK_FORMAT_R16G16_SINT              ,TextureFormat::RG16_SINT        },
		{ VK_FORMAT_R16G16_UNORM             ,TextureFormat::RG16_UNORM       },
		{ VK_FORMAT_R16G16_SNORM             ,TextureFormat::RG16_SNORM       },
		{ VK_FORMAT_R16G16_SFLOAT            ,TextureFormat::RG16_FLOAT       },
		{ VK_FORMAT_R32_UINT                 ,TextureFormat::R32_UINT         },
		{ VK_FORMAT_R32_SINT                 ,TextureFormat::R32_SINT         },
		{ VK_FORMAT_R32_SFLOAT               ,TextureFormat::R32_FLOAT        },
		{ VK_FORMAT_R16G16B16A16_UINT        ,TextureFormat::RGBA16_UINT      },
		{ VK_FORMAT_R16G16B16A16_SINT        ,TextureFormat::RGBA16_SINT      },
		{ VK_FORMAT_R16G16B16A16_SFLOAT      ,TextureFormat::RGBA16_FLOAT     },
		{ VK_FORMAT_R16G16B16A16_UNORM       ,TextureFormat::RGBA16_UNORM     },
		{ VK_FORMAT_R16G16B16A16_SNORM       ,TextureFormat::RGBA16_SNORM     },
		{ VK_FORMAT_R32G32_UINT              ,TextureFormat::RG32_UINT        },
		{ VK_FORMAT_R32G32_SINT              ,TextureFormat::RG32_SINT        },
		{ VK_FORMAT_R32G32_SFLOAT            ,TextureFormat::RG32_FLOAT       },
		{ VK_FORMAT_R32G32B32_UINT           ,TextureFormat::RGB32_UINT       },
		{ VK_FORMAT_R32G32B32_SINT           ,TextureFormat::RGB32_SINT       },
		{ VK_FORMAT_R32G32B32_SFLOAT         ,TextureFormat::RGB32_FLOAT      },
		{ VK_FORMAT_R32G32B32A32_UINT        ,TextureFormat::RGBA32_UINT      },
		{ VK_FORMAT_R32G32B32A32_SINT        ,TextureFormat::RGBA32_SINT      },
		{ VK_FORMAT_R32G32B32A32_SFLOAT      ,TextureFormat::RGBA32_FLOAT     },
		{ VK_FORMAT_D16_UNORM                ,TextureFormat::D16_UNORM        },
		{ VK_FORMAT_D24_UNORM_S8_UINT        ,TextureFormat::D24_UNORM_S8_UINT},
		{ VK_FORMAT_D32_SFLOAT               ,TextureFormat::D32_UNORM        },
		{ VK_FORMAT_D32_SFLOAT_S8_UINT       ,TextureFormat::D32_UNORM_S8_UINT},
		{ VK_FORMAT_BC1_RGBA_UNORM_BLOCK     ,TextureFormat::BC1_UNORM        },
		{ VK_FORMAT_BC1_RGBA_SRGB_BLOCK      ,TextureFormat::BC1_UNORM_SRGB   },
		{ VK_FORMAT_BC2_UNORM_BLOCK          ,TextureFormat::BC2_UNORM        },
		{ VK_FORMAT_BC2_SRGB_BLOCK           ,TextureFormat::BC2_UNORM_SRGB   },
		{ VK_FORMAT_BC3_UNORM_BLOCK          ,TextureFormat::BC3_UNORM        },
		{ VK_FORMAT_BC3_SRGB_BLOCK           ,TextureFormat::BC3_UNORM_SRGB   },
		{ VK_FORMAT_BC4_UNORM_BLOCK          ,TextureFormat::BC4_UNORM        },
		{ VK_FORMAT_BC4_SNORM_BLOCK          ,TextureFormat::BC4_SNORM        },
		{ VK_FORMAT_BC5_UNORM_BLOCK          ,TextureFormat::BC5_UNORM        },
		{ VK_FORMAT_BC5_SNORM_BLOCK          ,TextureFormat::BC5_SNORM        },
		{ VK_FORMAT_BC6H_UFLOAT_BLOCK        ,TextureFormat::BC6H_UFLOAT      },
		{ VK_FORMAT_BC6H_SFLOAT_BLOCK        ,TextureFormat::BC6H_SFLOAT      },
		{ VK_FORMAT_BC7_UNORM_BLOCK          ,TextureFormat::BC7_UNORM        },
		{ VK_FORMAT_BC7_SRGB_BLOCK           ,TextureFormat::BC7_UNORM_SRGB   },

	};

	VkImageUsageFlags GetVkImageUsageFlags(TextureUsage usage, TextureFormat format)
	{
		const FormatInfo& formatInfo = GetFormatInfo(format);

		VkImageUsageFlags flags{};

		if ((usage & TextureUsage::CopySrc) != 0)
		{
			flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		if ((usage & TextureUsage::CopyDst) != 0)
		{
			flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		if ((usage & TextureUsage::SampledBinding) != 0)
		{
			flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
			if (formatInfo.hasDepth || formatInfo.hasStencil)
			{
				flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}
		}

		if ((usage & TextureUsage::StorageBinding) != 0)
		{
			flags |= VK_IMAGE_USAGE_STORAGE_BIT;
		}

		if ((usage & TextureUsage::RenderAttachment) != 0)
		{
			if (formatInfo.hasDepth || formatInfo.hasStencil)
			{
				flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}
			else
			{
				flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}
		}

		return flags;
	}

	VkSampleCountFlagBits SampleCountConvert(uint32_t sampleCount)
	{
		switch (sampleCount)
		{
		case 1:
			return VK_SAMPLE_COUNT_1_BIT;

		case 2:
			return VK_SAMPLE_COUNT_2_BIT;

		case 4:
			return VK_SAMPLE_COUNT_4_BIT;

		case 8:
			return VK_SAMPLE_COUNT_8_BIT;

		case 16:
			return VK_SAMPLE_COUNT_16_BIT;

		case 32:
			return VK_SAMPLE_COUNT_32_BIT;

		case 64:
			return VK_SAMPLE_COUNT_64_BIT;

		default:
			assert(!"Invalid Enumeration Value");
			return VK_SAMPLE_COUNT_1_BIT;
		}
	}

	VkImageType GetVkImageType(TextureDimension dimension)
	{
		switch (dimension)
		{
		case TextureDimension::Texture1D:
		case TextureDimension::Texture1DArray:
			return VK_IMAGE_TYPE_1D;

		case TextureDimension::Texture2D:
		case TextureDimension::Texture2DArray:
		case TextureDimension::TextureCube:
		case TextureDimension::TextureCubeArray:
			return VK_IMAGE_TYPE_2D;

		case TextureDimension::Texture3D:
			return VK_IMAGE_TYPE_3D;

		case TextureDimension::Undefined:
		default:
			assert(!"Invalid Enumeration Value");
			return VK_IMAGE_TYPE_2D;
		}
	}

	VkImageViewType GetVkImageViewType(TextureDimension dimension)
	{
		switch (dimension)
		{
		case TextureDimension::Texture1D:
			return VK_IMAGE_VIEW_TYPE_1D;

		case TextureDimension::Texture1DArray:
			return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

		case TextureDimension::Texture2D:
			return VK_IMAGE_VIEW_TYPE_2D;

		case TextureDimension::Texture2DArray:
			return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

		case TextureDimension::TextureCube:
			return VK_IMAGE_VIEW_TYPE_CUBE;

		case TextureDimension::TextureCubeArray:
			return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

		case TextureDimension::Texture3D:
			return VK_IMAGE_VIEW_TYPE_3D;

		case TextureDimension::Undefined:
		default:
			assert(!"Invalid Enumeration Value");
			return VK_IMAGE_VIEW_TYPE_2D;
		}
	}

	VkImageAspectFlags GetVkAspectMask(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_S8_UINT:
			return VK_IMAGE_ASPECT_STENCIL_BIT;
			break;
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_D32_SFLOAT:
			return VK_IMAGE_ASPECT_DEPTH_BIT;
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		default:
			return VK_IMAGE_ASPECT_COLOR_BIT;
		}
	}

	VkFormat ToVkFormat(TextureFormat format)
	{
		assert(format < TextureFormat::COUNT);
		assert(_texFmtToVkFmtMap[uint32_t(format)].rhiFormat == format);

		return _texFmtToVkFmtMap[uint32_t(format)].vkFormat;
	}

	TextureFormat ToTextureFormat(VkFormat format)
	{
		auto res = _VkFmtToTexFmtMap.find(format);
		if (res != _VkFmtToTexFmtMap.end())
		{
			return res->second;
		}
		else
		{
			return TextureFormat::Undefined;
		}
	}

	//bool IsSampleCountSupported(Device* device, const VkImageCreateInfo& imageCreateInfo)
	//{
	//	VkPhysicalDevice physicalDevice = device->GetVkPhysicalDevice();
	//	VkImageFormatProperties properties;
	//	VkResult res = vkGetPhysicalDeviceImageFormatProperties(physicalDevice, imageCreateInfo.format, 
	//		imageCreateInfo.imageType, imageCreateInfo.tiling,
	//		imageCreateInfo.usage, imageCreateInfo.flags,
	//		&properties);
	//	CHECK_VK_RESULT(res);

	//	return properties.sampleCounts & imageCreateInfo.samples;
	//}

	VkPipelineStageFlags2 PiplineStageConvert(TextureUsage usage, ShaderStage shaderStage, TextureFormat format)
	{
		VkPipelineStageFlags2 flags = 0;

		if (usage == TextureUsage::None)
		{
			return VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT; //todo: use VK_PIPELINE_STAGE_2_NONE instead.
		}

		if ((usage & (TextureUsage::CopySrc | TextureUsage::CopyDst)) != 0)
		{
			flags |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		}

		if ((usage & cShaderTextureUsages) != 0)
		{
			if (HasFlag(shaderStage, ShaderStage::Vertex))
			{
				flags |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
			}

			if (HasFlag(shaderStage, ShaderStage::Fragment))
			{
				flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			}

			if (HasFlag(shaderStage, ShaderStage::Compute))
			{
				flags |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			}

			if (HasFlag(shaderStage, ShaderStage::TessellationControl))
			{
				flags |= VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT;
			}

			if (HasFlag(shaderStage, ShaderStage::TessellationEvaluation))
			{
				flags |= VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT;
			}

			if (HasFlag(shaderStage, ShaderStage::Geometry))
			{
				flags |= VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT;
			}
		}

		if (HasFlag(usage, TextureUsage::RenderAttachment))
		{
			if (GetFormatInfo(format).IsDeepStencil())
			{
				flags |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
			}
			else
			{
				flags |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
			}
		}

		if (HasFlag(usage, cSwapChainImageAcquireUsage))
		{
			// The usage is only used internally by the swapchain and is never used in
			// combination with other usages.
			assert(usage == cSwapChainImageAcquireUsage);
			flags |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		}

		if (HasFlag(usage, cSwapChainImagePresentUsage))
		{
			assert(usage == cSwapChainImagePresentUsage);
			flags |= VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT; // todo: use VK_PIPELINE_STAGE_2_NONE instead.
		}

		return flags;
	}

	VkAccessFlags2 AccessFlagsConvert(TextureUsage usage, TextureFormat format)
	{
		assert(HasOneFlag(usage));

		VkAccessFlags2 flags = 0;

		if (HasFlag(usage, TextureUsage::CopySrc))
		{
			flags |= VK_ACCESS_2_TRANSFER_READ_BIT;
		}

		if (HasFlag(usage, TextureUsage::CopyDst))
		{
			flags |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
		}

		if (HasFlag(usage, TextureUsage::SampledBinding | cReadOnlyStorageTexture))
		{
			flags |= VK_ACCESS_2_SHADER_READ_BIT;
		}

		if (HasFlag(usage, TextureUsage::StorageBinding))
		{
			flags |= VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT;
		}

		if (HasFlag(usage, TextureUsage::RenderAttachment))
		{
			if (GetFormatInfo(format).IsDeepStencil())
			{
				flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			}
			else
			{
				flags |= VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
			}
		}

		if (HasFlag(usage, cSwapChainImageAcquireUsage) || HasFlag(usage, cSwapChainImagePresentUsage))
		{
			assert(usage == cSwapChainImageAcquireUsage || usage == cSwapChainImagePresentUsage);
			flags |= 0;
		}

		return flags;
	}

	VkImageLayout ImageLayoutConvert(TextureUsage usage, TextureFormat format)
	{
		assert(HasOneFlag(usage));

		switch (usage)
		{
		case TextureUsage::None:
			return VK_IMAGE_LAYOUT_UNDEFINED;
		case TextureUsage::CopySrc:
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		case TextureUsage::CopyDst:
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		case TextureUsage::SampledBinding:
			if (GetFormatInfo(format).IsDeepStencil())
			{
				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			}
			else
			{
				return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
		case TextureUsage::StorageBinding:
		case cReadOnlyStorageTexture:
			return VK_IMAGE_LAYOUT_GENERAL;
		case TextureUsage::RenderAttachment:
		{
			if (GetFormatInfo(format).IsDeepStencil())
			{
				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
			else
			{
				return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
		}
		case cSwapChainImageAcquireUsage:
			return VK_IMAGE_LAYOUT_UNDEFINED;
		case cSwapChainImagePresentUsage:
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		default:
			assert(!"Invalid Enumeration Value");
		}
	}

	Ref<Texture> Texture::Create(Device* device, const TextureDesc& desc)
	{
		Ref<Texture> texture = AcquireRef(new Texture(device, desc));
		if (!texture->Initialize())
		{
			return nullptr;
		}
		return texture;
	}

	Ref<TextureViewBase> Texture::CreateView(const TextureViewDesc& desc)
	{
		return TextureView::Create(this, desc);
	}

	Texture::Texture(Device* device, const TextureDesc& desc) :
		TextureBase(device, desc),
		mVkFormat(ToVkFormat(mFormat)),
		mSubresourceLastSyncInfos(GetAspectFromFormat(mFormat), mArraySize, mMipLevelCount)
	{
	}

	Texture::~Texture() {}

	bool Texture::Initialize()
	{
		TextureBase::Initialize();
		// If this triggers, it means it's time to add tests and implement support for readonly
		// depth-stencil attachments that are also used as readonly storage bindings in the pass.
		// Have fun! :)
		assert(!(GetFormatInfo(mFormat).IsDeepStencil() && (mUsage & TextureUsage::StorageBinding) != 0));

		Device* device = checked_cast<Device>(mDevice.Get());

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = GetVkImageType(mDimension);
		imageCreateInfo.extent = { mWidth, mHeight, mDepth };
		imageCreateInfo.mipLevels = mMipLevelCount;
		imageCreateInfo.arrayLayers = mArraySize;
		imageCreateInfo.format = mVkFormat;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = GetVkImageUsageFlags(mInternalUsage, mFormat);
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.samples = SampleCountConvert(mSampleCount);
		if (mArraySize >= 6 && mWidth == mHeight)
		{
			imageCreateInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		}
		if (imageCreateInfo.imageType == VK_IMAGE_TYPE_3D &&
			imageCreateInfo.usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		{
			imageCreateInfo.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
		}
		// We always set VK_IMAGE_USAGE_TRANSFER_DST_BIT unconditionally because the Vulkan images
		// that are used in vkCmdClearColorImage() must have been created with this flag
		imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		// Let the library select the optimal memory type, which will likely have VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT.
		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		allocCreateInfo.priority = 1.0f;
		VkResult err = vmaCreateImage(device->GetMemoryAllocator(), &imageCreateInfo, &allocCreateInfo, &mHandle, &mAllocation, nullptr);
		CHECK_VK_RESULT_FALSE(err, "Could not to create vkImage");

		SetDebugName(device, mHandle, "Texture", GetName());

		return true;
	}

	inline bool CanReuseWithoutBarrier(TextureUsage lastUsage, TextureUsage usage, ShaderStage lastShaderStage, ShaderStage shaderStage)
	{
		bool isLastReadOnly = IsSubset(lastUsage, cReadOnlyTextureUsages);
		return isLastReadOnly && lastUsage == usage && IsSubset(shaderStage, lastShaderStage);
	}

	void Texture::TransitionUsageAndGetResourceBarrier(Queue* queue,
													TextureUsage usage,
													ShaderStage shaderStages,
													const SubresourceRange& range)
	{
		if (shaderStages == ShaderStage::None)
		{
			// If the image isn't used in any shader stages, ignore shader usages. Eg. ignore a texture
			// binding that isn't actually sampled in any shader.
			usage &= ~cShaderTextureUsages;
		}

		mSubresourceLastSyncInfos.Update(range, [&](const SubresourceRange& range, TextureSyncInfo& lastSyncInfo)
			{

				bool needTransferOwnership = lastSyncInfo.queue != QueueType::Undefined && lastSyncInfo.queue != queue->GetType();

				if (!needTransferOwnership && CanReuseWithoutBarrier(lastSyncInfo.usage, usage, lastSyncInfo.shaderStages, shaderStages))
				{
					return;
				}

				VkImageMemoryBarrier2 barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
				barrier.pNext = nullptr;
				barrier.srcAccessMask = AccessFlagsConvert(lastSyncInfo.usage, mFormat); //todo: use 0 when we need transfer ownership.
				barrier.srcStageMask = PiplineStageConvert(lastSyncInfo.usage, lastSyncInfo.shaderStages, mFormat);
				barrier.dstAccessMask = AccessFlagsConvert(usage, mFormat);
				barrier.dstStageMask = PiplineStageConvert(usage, shaderStages, mFormat);
				barrier.oldLayout = ImageLayoutConvert(lastSyncInfo.usage, mFormat);
				barrier.newLayout = ImageLayoutConvert(usage, mFormat);
				barrier.image = mHandle;
				barrier.subresourceRange.aspectMask = ImageAspectFlagsConvert(range.aspects);
				barrier.subresourceRange.baseMipLevel = range.baseMipLevel;
				barrier.subresourceRange.levelCount = range.levelCount;
				barrier.subresourceRange.baseArrayLayer = range.baseArrayLayer;
				barrier.subresourceRange.layerCount = range.layerCount;

				if (needTransferOwnership)
				{
					barrier.srcQueueFamilyIndex = checked_cast<Queue>(checked_cast<Device>(mDevice.Get())->GetQueue(lastSyncInfo.queue))->GetQueueFamilyIndex();
					barrier.dstQueueFamilyIndex = queue->GetQueueFamilyIndex();;
				}

				queue->GetPendingRecordingContext()->AddTextureBarrier(barrier);

				if (lastSyncInfo.usage == usage && IsSubset(usage, cReadOnlyTextureUsages)) {
					// Read only usage and no layout transition. We can keep previous shader stages so
					// future uses in those stages don't insert barriers.
					lastSyncInfo.shaderStages |= shaderStages;
				}
				else {
					// Image was altered by write or layout transition. We need to clear previous shader
					// stages so future uses in those stages will insert barriers.
					lastSyncInfo.shaderStages = shaderStages;
				}
				lastSyncInfo.usage = usage;
				lastSyncInfo.queue = queue->GetType();
			});

	}

	void Texture::TransitionUsageForMultiRange(Queue* queue, const SubresourceStorage<TextureSyncInfo>& syncInfos)
	{
		mSubresourceLastSyncInfos.Merge(syncInfos, [&](const SubresourceRange& range, TextureSyncInfo& lastSyncInfo, const TextureSyncInfo& newSyncInfo)
			{
				TextureUsage newUsage = newSyncInfo.usage;
				if (newSyncInfo.shaderStages == ShaderStage::None)
				{
					// If the image isn't used in any shader stages, ignore shader usages. Eg. ignore a
					// texture binding that isn't actually sampled in any shader.
					newUsage &= ~cShaderTextureUsages;
				}

				bool needTransferOwnership = lastSyncInfo.queue != QueueType::Undefined && lastSyncInfo.queue != queue->GetType();

				if (newUsage == TextureUsage::None || 
					(!needTransferOwnership && CanReuseWithoutBarrier(lastSyncInfo.usage, newUsage, lastSyncInfo.shaderStages, newSyncInfo.shaderStages)))
				{
					return;
				}

				VkImageMemoryBarrier2 barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
				barrier.pNext = nullptr;
				barrier.srcAccessMask = AccessFlagsConvert(lastSyncInfo.usage, mFormat); //todo: use 0 when we need transfer ownership.
				barrier.srcStageMask = PiplineStageConvert(lastSyncInfo.usage, lastSyncInfo.shaderStages, mFormat);
				barrier.dstAccessMask = AccessFlagsConvert(newUsage, mFormat);
				barrier.dstStageMask = PiplineStageConvert(newUsage, newSyncInfo.shaderStages, mFormat);
				barrier.oldLayout = ImageLayoutConvert(lastSyncInfo.usage, mFormat);
				barrier.newLayout = ImageLayoutConvert(newUsage, mFormat);
				barrier.image = mHandle;
				barrier.subresourceRange.aspectMask = ImageAspectFlagsConvert(range.aspects);
				barrier.subresourceRange.baseMipLevel = range.baseMipLevel;
				barrier.subresourceRange.levelCount = range.levelCount;
				barrier.subresourceRange.baseArrayLayer = range.baseArrayLayer;
				barrier.subresourceRange.layerCount = range.layerCount;

				if (needTransferOwnership)
				{
					barrier.srcQueueFamilyIndex = checked_cast<Queue>(checked_cast<Device>(mDevice.Get())->GetQueue(lastSyncInfo.queue))->GetQueueFamilyIndex();
					barrier.dstQueueFamilyIndex = queue->GetQueueFamilyIndex();;
				}

				queue->GetPendingRecordingContext()->AddTextureBarrier(barrier);

				if (lastSyncInfo.usage == newUsage && IsSubset(newUsage, cReadOnlyTextureUsages)) {
					// Read only usage and no layout transition. We can keep previous shader stages so
					// future uses in those stages don't insert barriers.
					lastSyncInfo.shaderStages |= newSyncInfo.shaderStages;
				}
				else {
					// Image was altered by write or layout transition. We need to clear previous shader
					// stages so future uses in those stages will insert barriers.
					lastSyncInfo.shaderStages = newSyncInfo.shaderStages;
				}
				lastSyncInfo.usage = newUsage;
				lastSyncInfo.queue = queue->GetType();
			});
	}

	void Texture::TransitionOwnership(Queue* queue, const SubresourceRange& range, Queue* recevingQueue)
	{
		mSubresourceLastSyncInfos.Update(range, [&](const SubresourceRange& syncRange, const TextureSyncInfo& lastSyncInfo)
			{
				assert(lastSyncInfo.queue == queue->GetType());

				VkImageMemoryBarrier2 barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
				barrier.pNext = nullptr;
				barrier.srcAccessMask = AccessFlagsConvert(lastSyncInfo.usage, mFormat); //todo: use 0 when we need transfer ownership.
				barrier.srcStageMask = PiplineStageConvert(lastSyncInfo.usage, lastSyncInfo.shaderStages, mFormat);
				barrier.image = mHandle;
				barrier.subresourceRange.aspectMask = ImageAspectFlagsConvert(syncRange.aspects);
				barrier.subresourceRange.baseMipLevel = syncRange.baseMipLevel;
				barrier.subresourceRange.levelCount = syncRange.levelCount;
				barrier.subresourceRange.baseArrayLayer = syncRange.baseArrayLayer;
				barrier.subresourceRange.layerCount = syncRange.layerCount;
				barrier.srcQueueFamilyIndex = checked_cast<Queue>(checked_cast<Device>(mDevice.Get())->GetQueue(lastSyncInfo.queue))->GetQueueFamilyIndex();
				barrier.dstQueueFamilyIndex = queue->GetQueueFamilyIndex();;

				queue->GetPendingRecordingContext()->AddTextureBarrier(barrier);
			});
	}

	void Texture::TransitionUsageNow(Queue* queue, TextureUsage usage, const SubresourceRange& range, ShaderStage shaderStages)
	{
		TransitionUsageAndGetResourceBarrier(queue, usage, shaderStages, range);
		queue->GetPendingRecordingContext()->EmitBarriers();
	}

	VkImage Texture::GetHandle() const
	{
		return mHandle;
	}

	void Texture::DestroyImpl()
	{
		mTextureViews.Destroy();

		std::array<bool, 3> isUsedInQueue{};
		
		mSubresourceLastSyncInfos.Iterate([&isUsedInQueue](const SubresourceRange& syncRange, const TextureSyncInfo& lastSyncInfo)
			{
				isUsedInQueue[static_cast<uint32_t>(lastSyncInfo.queue)] = true;
			});

		Device* device = checked_cast<Device>(mDevice.Get());

		//Ref<RefCountedHandle<ImageAllocation>> imageAllocation = AcquireRef(new RefCountedHandle<ImageAllocation>(device, { mHandle, mAllocation },
		//	[](Device* device, ImageAllocation handle)
		//	{
		//		vmaDestroyImage(device->GetMemoryAllocator(), handle.image, handle.allocation);
		//	}
		//));

		for (uint32_t i = 0; i < isUsedInQueue.size(); ++i)
		{
			if (isUsedInQueue[i])
			{
				//checked_cast<Queue>(device->GetQueue(static_cast<QueueType>(i)))->GetDeleter()->DeleteWhenUnused(imageAllocation);
			}
		}

		mHandle = VK_NULL_HANDLE;
		mAllocation = VK_NULL_HANDLE;
		mDestoryed = true;
	}

	Ref<SwapChainTexture> SwapChainTexture::Create(Device* device, const TextureDesc& desc, VkImage nativeImage)
	{
		Ref<SwapChainTexture> texture = AcquireRef(new SwapChainTexture(device, desc));
		texture->Initialize(nativeImage);
		return texture;
	}

	SwapChainTexture::SwapChainTexture(Device* device, const TextureDesc& desc) :
		Texture(device, desc)
	{

	}

	SwapChainTexture::~SwapChainTexture() {}

	void SwapChainTexture::Initialize(VkImage nativeImage)
	{
		TextureBase::Initialize();

		mHandle = nativeImage;
		mSubresourceLastSyncInfos.Fill({ cSwapChainImageAcquireUsage, ShaderStage::None });

		Device* device = checked_cast<Device>(mDevice.Get());

		SetDebugName(device, mHandle, "Texture", GetName());
	}

	void SwapChainTexture::DestroyImpl()
	{
		mTextureViews.Destroy();
		mHandle = VK_NULL_HANDLE;
		mDestoryed = true;
	}

	Ref<TextureView> TextureView::Create(TextureBase* texture, const TextureViewDesc& desc)
	{
		Ref<TextureView> textureView = AcquireRef(new TextureView(texture, desc));
		if (!textureView->Initialize())
		{
			return nullptr;
		}

		return textureView;
	}

	TextureView::TextureView(TextureBase* texture, const TextureViewDesc& desc) :
		TextureViewBase(texture, desc)
	{

	}

	bool TextureView::Initialize()
	{
		TextureViewBase::Initialize();
		
		if ((mInternalUsage & ~(TextureUsage::CopySrc | TextureUsage::CopyDst)) == 0)
		{
			// If the texture view has no other usage than CopySrc and CopyDst, then it can't
			// actually be used as a render pass attachment or sampled/storage texture. The Vulkan
			// validation errors warn if you create such a vkImageView, so return early.
			return false;
		}

		if (mTexture->IsDestoryed())
		{
			return false;
		}

		Device* device = checked_cast<Device>(mTexture->GetDevice());

		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = checked_cast<Texture>(mTexture)->GetHandle();
		createInfo.viewType = GetVkImageViewType(mDimension);
		createInfo.format = ToVkFormat(mFormat);
		createInfo.components = VkComponentMapping{ VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
										   VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		createInfo.subresourceRange.aspectMask = ImageAspectFlagsConvert(mRange.aspects);
		createInfo.subresourceRange.baseArrayLayer = mRange.baseArrayLayer;
		createInfo.subresourceRange.layerCount = mRange.layerCount;
		createInfo.subresourceRange.baseMipLevel = mRange.baseMipLevel;
		createInfo.subresourceRange.levelCount = mRange.levelCount;

		VkImageViewUsageCreateInfo usageInfo{};
		usageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO;
		usageInfo.usage = GetVkImageUsageFlags(mInternalUsage, mFormat);

		createInfo.pNext = &usageInfo;

		VkResult err = vkCreateImageView(device->GetHandle(), &createInfo, nullptr, &mHandle);
		CHECK_VK_RESULT_FALSE(err, "Could not to create vkImageView");

		SetDebugName(device, mHandle, "TextureView", GetName());

		return true;
	}

	VkImageView TextureView::GetHandle() const
	{
		return mHandle;
	}

	void TextureView::DestroyImpl()
	{
		Device* device = checked_cast<Device>(mDevice.Get());

		// SubresourceStorage<T>::Get does not accept combined enum, separate it.
		for (Aspect aspect : IterateEnumFlags(mRange.aspects))
		{
			QueueType lastQueue = checked_cast<Texture>(mTexture.Get())->mSubresourceLastSyncInfos.Get(aspect, mRange.baseArrayLayer, mRange.baseMipLevel).queue;
			assert(lastQueue != QueueType::Undefined);

			checked_cast<Queue>(device->GetQueue(lastQueue))->GetDeleter()->DeleteWhenUnused(mHandle);

			// cannot be used by multiple queues at the same time.
			break;
		}
	}
}