#pragma once

#include "ResourceBase.h"
#include "RHIStruct.h"
#include "Subresource.h"

namespace rhi::impl
{
	class DeviceBase;
	class TextureViewBase;

	class TextureBase : public ResourceBase
	{
	public:
		uint32_t APIGetWidth() const;
		uint32_t APIGetHeight() const;
		uint32_t APIGetDepthOrArrayLayers() const;
		uint32_t APIGetMipLevelCount() const;
		uint32_t APIGetSampleCount() const;
		TextureDimension APIGetDimension() const;
		TextureFormat APIGetFormat() const;
		TextureUsage APIGetUsage() const;
		TextureViewBase* APICreateView(const TextureViewDesc* desc = nullptr);
		void APIDestroy();
		virtual Ref<TextureViewBase> CreateView(const TextureViewDesc& desc) = 0;
		// internal 
		ResourceType GetType() const override;
		ResourceList* GetViewList();
		TextureUsage GetInternalUsage() const;
		SubresourceRange GetAllSubresources() const;
		bool IsDestoryed() const;
	protected:
		explicit TextureBase(DeviceBase* device, const TextureDesc& desc);
		~TextureBase();
		void Initialize();

		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mMipLevelCount;
		uint32_t mSampleCount;
		TextureDimension mDimension;
		const TextureUsage mUsage;
		const TextureUsage mInternalUsage;
		TextureFormat mFormat;
		union
		{
			uint32_t mDepth;
			uint32_t mArraySize;
		};
		bool mDestoryed = false;

		ResourceList mTextureViews;
	};

	class TextureViewBase : public ResourceBase
	{
	public:
		uint32_t GetBaseMipLevel() const;
		uint32_t GetLevelCount() const;
		uint32_t GetBaseArrayLayer() const;
		uint32_t GetLayerCount() const;
		Aspect GetAspects() const;
		TextureUsage GetUsage() const;
		TextureDimension GetDimension() const;
		TextureBase* GetTexture() const;
		// internal
		const SubresourceRange& GetSubresourceRange() const;
		ResourceType GetType() const override;
		TextureUsage GetInternalUsage() const;
	protected:
		explicit TextureViewBase(TextureBase* texture, const TextureViewDesc& desc);
		~TextureViewBase();
		void Initialize();
		ResourceList* GetList() const override;

		Ref<TextureBase> mTexture;
		TextureDimension mDimension;
		SubresourceRange mRange;
		TextureFormat mFormat;
		const TextureUsage mUsage;
		const TextureUsage mInternalUsage;
	};

	enum class FormatComponentType : uint8_t
	{
		Undefined,
		Float,
		Snorm,
		Unorm,
		UnormSrgb,
		Sint,
		Uint,
	};

	struct FormatInfo
	{
		TextureFormat format;
		const char* name;
		union
		{
			uint8_t bytesPerTexel;
			uint8_t bytesPerBlock;
		};
		uint8_t blockSize;
		bool hasRed : 1;
		bool hasGreen : 1;
		bool hasBlue : 1;
		bool hasAlpha : 1;
		bool hasDepth : 1;
		bool hasStencil : 1;
		FormatComponentType componentType;
		bool IsCompressed() const { return blockSize != 1; }
		bool IsDeepStencil() const { return hasDepth && hasStencil; }
	};


	const FormatInfo& GetFormatInfo(TextureFormat format);
}