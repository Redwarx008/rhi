#pragma once

#include <atomic>
#include <cstdint>
#include <cassert>
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
            assert(mRefCount != 0);
            // See the explanation in the Boost documentation:
            // https://www.boost.org/doc/libs/1_55_0/doc/html/atomic/usage_examples.html
            uint64_t previousRefCount = mRefCount.fetch_sub(1, std::memory_order_release);

            if (previousRefCount < 2)
            {
                std::atomic_thread_fence(std::memory_order_acquire);
                DeleteThis();
            }
        }
        uint64_t GetRefCount() const
        {
            return mRefCount.load(std::memory_order_relaxed);
        }
        bool TryIncrement() 
        {
            uint64_t current = mRefCount.load(std::memory_order_relaxed);
            bool success = false;
            do 
            {
                if (current == 0u) 
                {
                    return false;
                }
                // The relaxed ordering guarantees only the atomicity of the update. This is fine because:
                //   - If another thread's decrement happens before this increment, the increment should
                //     fail.
                //   - If another thread's decrement happens after this increment, the decrement shouldn't
                //     delete the object, because the ref count > 0.
                // See Boost library for reference:
                //   https://github.com/boostorg/smart_ptr/blob/develop/include/boost/smart_ptr/detail/sp_counted_base_std_atomic.hpp#L62
                success = mRefCount.compare_exchange_weak(current, current + 1,
                    std::memory_order_relaxed);
            } while (!success);
            return true;
        }

    protected:
        virtual ~RefCounted()
        {}

        virtual void DeleteThis()
        {
            delete this;
        }

    private:
        std::atomic<uint64_t> mRefCount = 1;
    };
} // namespace rhi::impl
