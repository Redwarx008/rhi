#pragma once

#include "Ref.hpp"
#include "RefCounted.h"
#include <mutex>


namespace rhi::impl
{
    template<typename RefCountedT>
    class WeakRef
    {
        static_assert(std::is_base_of_v<RefCounted, RefCountedT>);
    public:
        WeakRef() {}
        constexpr WeakRef(std::nullptr_t) : WeakRef() {}

        // Constructors from a WeakRef<U>, where U can also equal T.
        template <typename U, typename = typename std::enable_if<std::is_base_of_v<RefCountedT, U>>::type>
        WeakRef(const WeakRef<U>& other) : mValue(other.mValue) {}
        template <typename U, typename = typename std::enable_if<std::is_base_of_v<RefCountedT, U>>::type>
        WeakRef<RefCountedT>& operator=(const WeakRef<U>& other)
        {
            mValue = other.mValue;
            return *this;
        }
        template <typename U, typename = typename std::enable_if<std::is_base_of_v<RefCountedT, U>>::type>
        WeakRef(WeakRef<U>&& other) : mValue(std::move(other.mValue)) {}
        template <typename U, typename = typename std::enable_if<std::is_base_of_v<RefCountedT, U>>::type>
        WeakRef<RefCountedT>& operator=(WeakRef<U>&& other)
        {
            if (&other != this) {
                mValue = std::move(other.mValue);
            }
            return *this;
        }
        WeakRef(const Ref<RefCountedT>& object) : mValue(object.Get()) {}
        WeakRef(RefCountedT* ptr) : mValue(ptr) {}
        Ref<RefCountedT> Promote() const
        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (mValue && mValue->TryIncrement())
            {
                return AcquireRef(mValue);
            }
            return nullptr;
        }
        bool Expired() const
        {
            std::lock_guard<std::mutex> lock(mMutex);
            return mValue && mValue->GetRefCount() > 0;
        }
        RefCountedT* UnsafeGet() const
        {
            if (mValue)
            {
                return mValue;
            }
            return nullptr;
        }
    private:
        mutable std::mutex mMutex;
        RefCountedT* mValue = nullptr;
    };
}
