#pragma once

#include "../common/RefCounted.h"
#include <functional>

namespace rhi::impl::vulkan
{
	class Device;
	template<typename Handle>
	class RefCountedHandle : public RefCounted
	{
	public:
		RefCountedHandle(Device* device, Handle handle, std::function<void(Device*, Handle)> deleteMethod) :
			mDevice(device),
			mHandle(handle),
			mDeleteMethod(deleteMethod)
		{

		}
		~RefCountedHandle()
		{
			mDeleteMethod(mDevice, mHandle);
		}

		Handle GetHandle() const
		{
			return mHandle;
		}
	private:
		Device* mDevice;
		Handle mHandle;
		std::function<void(Device*, Handle)> mDeleteMethod;
	};
}