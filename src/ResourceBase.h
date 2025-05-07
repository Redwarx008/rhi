#pragma once

#include <mutex>
#include <type_traits>
#include <string>
#include "common/Ref.hpp"
#include "common/LinkedList.h"
#include "common/RefCounted.h"
#include "common/MutexProtected.hpp"

namespace rhi::impl
{
	class ResourceBase;

	enum class ResourceType : uint32_t
	{
		Buffer,
		ComputePipeline,
		RenderPipeline,
		PipelineLayout,
		//QuerySet,
		BindSet,
		BindSetLayout,
		Sampler,
		ShaderModule,
		Texture,
		TextureView,
		Count
	};

	// Generic object list with a mutex for tracking for destruction.
	class ResourceList
	{
	public:
		void Track(ResourceBase* object);
		bool Untrack(ResourceBase* object);
		void Destroy();

		template <typename F>
		void ForEach(F fn) const
		{
			mObjects.Use([&fn](const auto lockedObjects)
				{
					for (const auto* node = lockedObjects->head(); node != lockedObjects->end();
						node = node->next())
					{
						fn(node->value());
					}
				});
		}
	private:
		MutexProtected<LinkedList<ResourceBase>> mObjects;
	};

	class DeviceBase;
	// Object which tracked by owner

	class ResourceBase : public RefCounted, public LinkNode<ResourceBase>
	{
	public:
		explicit ResourceBase(DeviceBase* device, std::string_view name);
		~ResourceBase();
		DeviceBase* GetDevice() const;
		virtual ResourceType GetType() const = 0;
		std::string_view GetName() const;
		void Destroy();
	protected:
		// will be called by the derived class to track the object
		void Initialize();
		void DeleteThis() override;
		virtual void DestroyImpl() = 0;
		virtual ResourceList* GetList() const;

		Ref<DeviceBase> mDevice;
	private:
		friend class ResourceList;
		std::string mName;
	};
}