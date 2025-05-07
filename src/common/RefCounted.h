#pragma once

#include <atomic>
#include <cstdint>

namespace rhi::impl
{
	class RefCounted
	{
	public:
		void AddRef()
		{
			mRefCount.fetch_add(1, std::memory_order_relaxed);
		}
	    void Release()
		{
			// See the explanation in the Boost documentation:
			// https://www.boost.org/doc/libs/1_55_0/doc/html/atomic/usage_examples.html
			uint64_t previousRefCount = mRefCount.fetch_sub(1, std::memory_order_release);

			if (previousRefCount < 2)
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				DeleteThis();
			}
		}
	protected:
		uint64_t GetRefCount() const
		{
			return mRefCount;
		}
		virtual ~RefCounted() {}
		virtual void DeleteThis()
		{
			delete this;
		}
	private:
		std::atomic<uint64_t> mRefCount = 1;
	};
}