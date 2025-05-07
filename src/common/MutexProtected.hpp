#pragma once

#include <mutex>
#include <functional>

namespace rhi::impl
{
    template<typename T>
    class Guard
    {
    public:
        Guard(T* obj, std::mutex& mutex) :mLock(mutex), mObj(obj) {}
        T* operator->() { return mObj; }
    private:
        std::unique_lock<std::mutex> mLock;
        T* mObj;
    };

    template<typename T>
    class MutexProtected
    {
    public:
        MutexProtected() {}
        template <typename... Args>
        MutexProtected(Args&&...args) : mObject(std::forward<Args>(args)...) {}
        Guard<T> operator->() { return Guard<T>(&mObject, mMutex); }

        template <typename Fn>
        auto Use(Fn&& fn) 
        {
            return fn(Guard<T>(&mObject, mMutex));
        }
    private:
        T mObject;
        std::mutex mMutex;
    };

}