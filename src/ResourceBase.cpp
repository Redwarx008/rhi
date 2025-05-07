#include "ResourceBase.h"

#include "DeviceBase.h"
#include <string_view>

namespace rhi::impl
{
	ResourceBase::ResourceBase(DeviceBase* device, std::string_view name) :
		mDevice(device),
		mName(name)
	{}

	ResourceBase::~ResourceBase() {}

	DeviceBase* ResourceBase::GetDevice() const
	{
		return mDevice.Get();
	}

	void ResourceBase::Destroy()
	{
		ResourceList* list = GetList();
		assert(list);
		if (list->Untrack(this))
		{
			DestroyImpl();
		}
	}

	void ResourceBase::DeleteThis()
	{
		Destroy();
		RefCounted::DeleteThis();
	};

	ResourceList* ResourceBase::GetList() const
	{
		return mDevice->GetTrackedObjectList(GetType());
	}

	std::string_view ResourceBase::GetName() const
	{
		return mName;
	}

	void ResourceBase::Initialize()
	{
		ResourceList* list = GetList();
		assert(list);
		list->Track(this);
	}

	void ResourceList::Destroy()
	{
		LinkedList<ResourceBase> objects;
		{
			mObjects->MoveInto(&objects);
		}

		while (!objects.empty())
		{
			auto* head = objects.head();
			bool removed = head->RemoveFromList();
			assert(removed);
			head->value()->DestroyImpl();
		}
	}

	void ResourceList::Track(ResourceBase* object)
	{
		mObjects.Use([&object](auto lockedObjects) { lockedObjects->Prepend(object); });
	}

	bool ResourceList::Untrack(ResourceBase* object)
	{
		return mObjects.Use([&object](auto lockedObjects) { return object->RemoveFromList(); });
	}
}