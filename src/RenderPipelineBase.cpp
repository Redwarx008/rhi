#include "RenderPipelinebase.h"
#include "DeviceBase.h"
#include "TextureBase.h"

namespace rhi::impl
{
	TextureFormat ToTextureFormat(VertexFormat format)
	{
		switch (format)
		{
		case VertexFormat::Uint8:
			return TextureFormat::R8_UINT;
		case VertexFormat::Uint8x2:
			return TextureFormat::RG8_UINT;
		case VertexFormat::Uint8x4:
			return TextureFormat::RGBA8_UINT;
		case VertexFormat::Sint8:
			return TextureFormat::R8_SINT;
		case VertexFormat::Sint8x2:
			return TextureFormat::RG8_SINT;
		case VertexFormat::Sint8x4:
			return TextureFormat::RGBA8_SINT;
		case VertexFormat::Unorm8:
			return TextureFormat::R8_UNORM;
		case VertexFormat::Unorm8x2:
			return TextureFormat::RG8_UNORM;
		case VertexFormat::Unorm8x4:
			return TextureFormat::RGBA8_UNORM;
		case VertexFormat::Snorm8:
			return TextureFormat::R8_SNORM;
		case VertexFormat::Snorm8x2:
			return TextureFormat::RG8_SNORM;
		case VertexFormat::Snorm8x4:
			return TextureFormat::RGBA8_SNORM;
		case VertexFormat::Uint16:
			return TextureFormat::R16_UINT;
		case VertexFormat::Uint16x2:
			return TextureFormat::RG16_UINT;
		case VertexFormat::Uint16x4:
			return TextureFormat::RGBA16_UINT;
		case VertexFormat::Sint16:
			return TextureFormat::R16_SINT;
		case VertexFormat::Sint16x2:
			return TextureFormat::RG16_SINT;
		case VertexFormat::Sint16x4:
			return TextureFormat::RGBA16_SINT;
		case VertexFormat::Unorm16:
			return TextureFormat::R16_UNORM;
		case VertexFormat::Unorm16x2:
			return TextureFormat::RG16_UNORM;
		case VertexFormat::Unorm16x4:
			return TextureFormat::RGBA16_UNORM;
		case VertexFormat::Snorm16:
			return TextureFormat::R16_SNORM;
		case VertexFormat::Snorm16x2:
			return TextureFormat::RG16_SNORM;
		case VertexFormat::Snorm16x4:
			return TextureFormat::RGBA16_SNORM;
		case VertexFormat::Float16:
			return TextureFormat::R16_FLOAT;
		case VertexFormat::Float16x2:
			return TextureFormat::RG16_FLOAT;
		case VertexFormat::Float16x4:
			return TextureFormat::RGBA16_FLOAT;
		case VertexFormat::Float32:
			return TextureFormat::R32_FLOAT;
		case VertexFormat::Float32x2:
			return TextureFormat::RG32_FLOAT;
		case VertexFormat::Float32x3:
			return TextureFormat::RGB32_FLOAT;
		case VertexFormat::Float32x4:
			return TextureFormat::RGBA32_FLOAT;
		case VertexFormat::Uint32:
			return TextureFormat::R32_UINT;
		case VertexFormat::Uint32x2:
			return TextureFormat::RG32_UINT;
		case VertexFormat::Uint32x3:
			return TextureFormat::RGB32_UINT;
		case VertexFormat::Uint32x4:
			return TextureFormat::RGBA32_UINT;
		case VertexFormat::Sint32:
			return TextureFormat::R32_SINT;
		case VertexFormat::Sint32x2:
			return TextureFormat::RG32_SINT;
		case VertexFormat::Sint32x3:
			return TextureFormat::RGB32_SINT;
		case VertexFormat::Sint32x4:
			return TextureFormat::RGBA32_SINT;
		case VertexFormat::Unorm10_10_10_2:
			return TextureFormat::R10G10B10A2_UNORM;
		default:
			ASSERT(!"Unreachable");
			break;
		}
	}

	RenderPipelineBase::RenderPipelineBase(DeviceBase* device, const RenderPipelineDesc& desc) :
		PipelineBase(device, desc),
		mRasterState(desc.rasterState),
		mBlendState(desc.blendState),
		mSampleState(desc.sampleState),
		mDepthStencilState(desc.depthStencilState),
		mViewportCount(desc.viewportCount),
		mPatchControlPoints(desc.patchControlPoints),
		mDepthStencilFormat(desc.depthStencilFormat),
		mVertexInputAttributes(desc.vertexAttributeCount),
		mColorAttachmentFormats(desc.colorAttachmentCount)

	{
		for (uint32_t i = 0; i < desc.vertexAttributeCount; ++i)
		{
			mVertexInputAttributes[i] = desc.vertexAttributes[i];
		}
		for (uint32_t i = 0; i < desc.colorAttachmentCount; ++i)
		{
			mColorAttachmentFormats[i] = desc.colorAttachmentFormats[i];
		}

		ResolveVertexInputOffsetAndStride();
	}

	void RenderPipelineBase::ResolveVertexInputOffsetAndStride()
	{
				uint32_t bufferSlotUsed = 0;
		for (uint32_t i = 0; i < mVertexInputAttributes.size(); ++i)
		{
			bufferSlotUsed = (std::max)(bufferSlotUsed, mVertexInputAttributes[i].bindingBufferSlot + 1);
		}
		std::vector<uint32_t> autoSetStrides(bufferSlotUsed, 0);
		std::vector<uint32_t> originStrides(bufferSlotUsed, CAutoCompute);

		for (uint32_t i = 0; i < mVertexInputAttributes.size(); ++i)
		{
			uint32_t bufferSlot = mVertexInputAttributes[i].bindingBufferSlot;
			auto& currentAutoSetStride = autoSetStrides[bufferSlot];

			auto& formatInfo = GetFormatInfo(ToTextureFormat(mVertexInputAttributes[i].format));

			if (mVertexInputAttributes[i].offsetInElement == CAutoCompute)
			{
				mVertexInputAttributes[i].offsetInElement = currentAutoSetStride;
			}

			if (mVertexInputAttributes[i].elementStride != CAutoCompute)
			{
				if (originStrides[bufferSlot] != CAutoCompute &&
					mVertexInputAttributes[i].elementStride != originStrides[bufferSlot])
				{
					LOG_ERROR("stride between elements is not consistent in the same buffer slot.");
				}
				originStrides[bufferSlot] = mVertexInputAttributes[i].elementStride;
			}

			currentAutoSetStride = (std::max)(currentAutoSetStride, mVertexInputAttributes[i].offsetInElement + formatInfo.bytesPerTexel);
		}

		for (uint32_t i = 0; i < mVertexInputAttributes.size(); ++i)
		{
			uint32_t bufferSlot = mVertexInputAttributes[i].bindingBufferSlot;
			if (originStrides[bufferSlot] != CAutoCompute && originStrides[bufferSlot] >= autoSetStrides[bufferSlot])
			{
				LOG_ERROR("stride is too small");
			}

			if (mVertexInputAttributes[i].elementStride == CAutoCompute)
			{
				mVertexInputAttributes[i].elementStride = autoSetStrides[bufferSlot];
			}
		}
	}

	ResourceType RenderPipelineBase::GetType() const
	{
		return ResourceType::RenderPipeline;
	}

	RenderPipelineBase::~RenderPipelineBase() {}
}