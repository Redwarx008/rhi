 #pragma once

#include <cstddef>
#include <type_traits>

namespace rhi::impl
{
    // Just copy from dawn :D.
    template <typename T, typename Traits>
    class RefBase {
    public:
        // Default constructor and destructor.
        RefBase() : mValue(Traits::kNullValue) {}

        ~RefBase() noexcept { Release(mValue); }

        // Constructors from nullptr.
        // NOLINTNEXTLINE(runtime/explicit)
        constexpr RefBase(std::nullptr_t) : RefBase() {}

        RefBase<T, Traits>& operator=(std::nullptr_t) {
            Set(Traits::kNullValue);
            return *this;
        }

        // Constructors from a value T.
        // NOLINTNEXTLINE(runtime/explicit)
        RefBase(T value) : mValue(value) { AddRef(value); }

        RefBase<T, Traits>& operator=(const T& value) {
            Set(value);
            return *this;
        }

        // Constructors from a RefBase<T>
        RefBase(const RefBase<T, Traits>& other) : mValue(other.mValue) { AddRef(other.mValue); }

        RefBase<T, Traits>& operator=(const RefBase<T, Traits>& other) {
            Set(other.mValue);
            return *this;
        }

        RefBase(RefBase<T, Traits>&& other) { mValue = other.Detach(); }

        RefBase<T, Traits>& operator=(RefBase<T, Traits>&& other) {
            if (&other != this) {
                Release(mValue);
                mValue = other.Detach();
            }
            return *this;
        }

        // Constructors from a RefBase<U>. Note that in the *-assignment operators this cannot be the
        // same as `other` because overload resolution rules would have chosen the *-assignement
        // operators defined with `other` == RefBase<T, Traits>.
        template <typename U, typename UTraits, typename = typename std::is_convertible<U, T>::type>
        RefBase(const RefBase<U, UTraits>& other) :
            mValue(other.mValue) 
        {
            //mValue = other.mValue;
            AddRef(other.mValue);
        }

        template <typename U, typename UTraits, typename = typename std::is_convertible<U, T>::type>
        RefBase<T, Traits>& operator=(const RefBase<U, UTraits>& other) {
            Set(other.mValue);
            return *this;
        }

        template <typename U, typename UTraits, typename = typename std::is_convertible<U, T>::type>
        RefBase(RefBase<U, UTraits>&& other) {
            mValue = other.Detach();
        }

        template <typename U, typename UTraits, typename = typename std::is_convertible<U, T>::type>
        RefBase<T, Traits>& operator=(RefBase<U, UTraits>&& other) {
            Release(mValue);
            mValue = other.Detach();
            return *this;
        }

        explicit operator bool() const { return !!mValue; }

        // Comparison operators.
        bool operator==(const T& other) const { return mValue == other; }
        bool operator!=(const T& other) const { return mValue != other; }

        bool operator==(const RefBase<T, Traits>& other) const { return mValue == other.mValue; }
        bool operator!=(const RefBase<T, Traits>& other) const { return mValue != other.mValue; }

        const T operator->() const { return mValue; }
        T operator->() { return mValue; }

        bool operator<(const RefBase<T, Traits>& other) const { return mValue < other.mValue; }

        // Smart pointer methods.
        const T& Get() const { return mValue; }
        T& Get() { return mValue; }

        [[nodiscard]] T Detach() {
            T value{ std::move(mValue) };
            mValue = Traits::kNullValue;
            return value;
        }

        void Acquire(T value) {
            Release(mValue);
            mValue = value;
        }

        // Cast operator.
        template <typename Other>
        Other Cast()&& {
            Other other;
            CastImpl(this, &other);
            return other;
        }

    private:
        // Friend is needed so that instances of RefBase<U> can call AddRef and Release on
        // RefBase<T>.
        template <typename U, typename UTraits>
        friend class RefBase;

        template <typename U, typename UTraits, typename = typename std::is_convertible<U, T>::type>
        static void CastImpl(RefBase<T, Traits>* ref, RefBase<U, UTraits>* other) {
            other->Acquire(static_cast<U>(ref->Detach()));
        }

        static void AddRef(T value) {
            if (value != Traits::kNullValue) {
                Traits::AddRef(value);
            }
        }
        static void Release(T value) {
            if (value != Traits::kNullValue) {
                Traits::Release(value);
            }
        }

        void Set(T value) {
            if (mValue != value) {
                // Ensure that the new value is referenced before the old is released to prevent any
                // transitive frees that may affect the new value.
                AddRef(value);
                Release(mValue);
                mValue = value;
            }
        }

        T mValue;
    };

    template <typename T>
    struct RefCountedTraits {
        static constexpr T* kNullValue = nullptr;
        static void AddRef(T* value) { value->AddRef(); }
         static void Release(T* value) { value->Release(); }
    };

    template <typename T>
    class Ref : public RefBase<T*, RefCountedTraits<T>> {
    public:
        using RefBase<T*, RefCountedTraits<T>>::RefBase;
    };

    template <typename T>
    Ref<T> AcquireRef(T* pointee) {
        Ref<T> ref;
        ref.Acquire(pointee);
        return ref;
    }
}