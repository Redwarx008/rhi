#pragma once

#include <absl/container/flat_hash_set.h>
#include <mutex>
#include <type_traits>
#include "Error.h"
#include "Ref.hpp"
#include "RefCounted.h"
#include "WeakRef.hpp"

namespace rhi::impl
{

    template <typename T>
    class CachedObjects;

    template <typename T>
    class Cached
    {
    public:
        // Functor necessary for the unordered_set<Cached*>-based cache.
        struct Hash
        {
            size_t operator()(const Cached* obj) const;
        };

        size_t GetContentHash() const;

        void SetContentHash(size_t contentHash);

        void Uncache();

    protected:
        ~Cached();

    private:
        friend class CachedObjects<T>;
        // Called by ObjectContentHasher
        virtual size_t ComputeContentHash() = 0;

        size_t mContentHash = 0;
        bool mIsContentHashInitialized = false;
        CachedObjects<T>* mCacheList = nullptr;
    };

    template <typename T>
    struct WeakRefAndHash
    {
        static_assert(std::is_base_of_v<RefCounted, T>, "Type must be refcounted.");
        static_assert(std::is_base_of_v<Cached<T>, T>, "Type must be Cached.");
        WeakRefAndHash(T* ptr) : weakRef(ptr), hash(typename T::Hash()(ptr))
        {}
        WeakRefAndHash(Ref<T> obj) : weakRef(obj), hash(typename T::Hash()(obj.Get()))
        {}

        WeakRef<T> weakRef;
        size_t hash;
    };

    // When erased, the reference count of WeakRef may be zero, causing the comparison to fail. Here we only care about
    // the pointer itself.
    template <typename T>
    struct ForErase
    {
        static_assert(std::is_base_of_v<RefCounted, T>, "Type must be refcounted.");
        static_assert(std::is_base_of_v<Cached<T>, T>, "Type must be Cached.");
        explicit ForErase(T* obj) : ptr(obj)
        {}
        T* ptr;
    };

    template <typename T>
    class CachedObjects
    {
        static_assert(std::is_base_of_v<RefCounted, T>, "Type must be refcounted.");
        static_assert(std::is_base_of_v<Cached<T>, T>, "Type must be Cached.");

    public:
        CachedObjects() : mCache(0, Hash(), Equal())
        {}
        ~CachedObjects()
        {
            ASSERT(mCache.empty());
        }
        std::pair<Ref<T>, bool> Insert(T* obj);
        // Inserts the object into the cache returning a pair where the first is a Ref to the
        // inserted or existing object, and the second is a bool that is true if we inserted
        // `object` and false otherwise.
        // std::pair<Ref<T>, bool> Insert(T* obj);
        // Returns a valid Ref<T> if we can Promote the underlying WeakRef. Returns nullptr otherwise.
        Ref<T> Find(T* key);

        void Erase(T* obj);

        bool Empty();

    private:
        struct Hash
        {
            using is_transparent = void;
            size_t operator()(const T* ptr) const;
            size_t operator()(const WeakRefAndHash<T>& obj) const;
            size_t operator()(const ForErase<T>& obj) const;
        };
        struct Equal
        {
            using is_transparent = void;
            bool operator()(const WeakRefAndHash<T>& a, const WeakRefAndHash<T>& b) const;
            bool operator()(const WeakRefAndHash<T>& a, const ForErase<T>& b) const;
        };

        absl::flat_hash_set<WeakRefAndHash<T>, Hash, Equal> mCache;
        std::mutex mMutex;
    };

    // Cached implementations
    template <typename T>
    size_t Cached<T>::Hash::operator()(const Cached* obj) const
    {
        return obj->GetContentHash();
    }

    template <typename T>
    size_t Cached<T>::GetContentHash() const
    {
        ASSERT(mIsContentHashInitialized);
        return mContentHash;
    }

    template <typename T>
    void Cached<T>::SetContentHash(size_t contentHash)
    {
        ASSERT(!mIsContentHashInitialized || contentHash == mContentHash);
        mContentHash = contentHash;
        mIsContentHashInitialized = true;
    }

    template <typename T>
    void Cached<T>::Uncache()
    {
        if (mCacheList != nullptr)
        {
            // Note that Erase sets mCache to nullptr. We do it in Erase instead of doing it here in
            // case users call Erase somewhere else before the Uncache call.
            mCacheList->Erase(static_cast<T*>(this));
        }
    }
    template <typename T>
    Cached<T>::~Cached()
    {
        ASSERT(mCacheList == nullptr);
    }

    // CachedObjects implementations

    template <typename T>
    std::pair<Ref<T>, bool> CachedObjects<T>::Insert(T* obj)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        auto [iter, inserted] = mCache.emplace(obj);
        if (inserted)
        {
            obj->mCacheList = this;
            return {Ref<T>(obj), inserted};
        }
        // Try to promote the found WeakRef to a Ref. If promotion fails, remove the old Key
        // and insert this one.
        Ref<T> ref = iter->weakRef.Promote();
        if (ref != nullptr)
        {
            return {ref, false};
        }
        else
        {
            mCache.erase(iter);
            auto result = mCache.emplace(obj);
            ASSERT(result.second);
            obj->mCacheList = this;
            return {Ref<T>(obj), true};
        }
    }

    template <typename T>
    Ref<T> CachedObjects<T>::Find(T* key)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        auto iter = mCache.find(key);
        if (iter != mCache.end())
        {
            return iter->weakRef.Promote();
        }
        return nullptr;
    }

    template <typename T>
    void CachedObjects<T>::Erase(T* obj)
    {
        size_t count;
        {
            std::lock_guard<std::mutex> lock(mMutex);
            count = mCache.erase(ForErase<T>(obj));
        }
        if (count == 0)
        {
            return;
        }
        obj->mCacheList = nullptr;
    }

    template <typename T>
    bool CachedObjects<T>::Empty()
    {
        std::lock_guard<std::mutex> lock(mMutex);
        return mCache.empty();
    }

    template <typename T>
    size_t CachedObjects<T>::Hash::operator()(const T* ptr) const
    {
        return typename T::Hash()(ptr);
    }
    template <typename T>
    size_t CachedObjects<T>::Hash::operator()(const WeakRefAndHash<T>& obj) const
    {
        return obj.hash;
    }
    template <typename T>
    size_t CachedObjects<T>::Hash::operator()(const ForErase<T>& obj) const
    {
        return typename T::Hash()(obj.ptr);
    }


    template <typename T>
    bool CachedObjects<T>::Equal::operator()(const WeakRefAndHash<T>& a, const WeakRefAndHash<T>& b) const
    {
        Ref<T> aRef = a.weakRef.Promote();
        Ref<T> bRef = b.weakRef.Promote();
        return aRef && bRef && typename T::Equal()(aRef.Get(), bRef.Get());
    }

    template <typename T>
    bool CachedObjects<T>::Equal::operator()(const WeakRefAndHash<T>& a, const ForErase<T>& b) const
    {
        return a.weakRef.UnsafeGet() == b.ptr;
    }
} // namespace rhi::impl
