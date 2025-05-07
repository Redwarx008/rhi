#pragma once

#include <sstream>
#include <cassert>
#include <type_traits>
#include "Ref.hpp"

namespace rhi::impl
{
	inline void combineStrSS(std::stringstream& ss)
	{
	}

	template <typename ArgType>
	void combineStrSS(std::stringstream& ss, const ArgType& Arg)
	{
		ss << Arg;
	}

	template <typename FirstArgType, typename... RemainArgsType>
	void combineStrSS(std::stringstream& ss, const FirstArgType& firstArg, const RemainArgsType&... remainArgs)
	{
		combineStrSS(ss, firstArg);
		combineStrSS(ss, remainArgs...); // recursive call using pack expansion syntax
	}

	template <typename... ArgsType>
	std::string combineString(const ArgsType&... remainArgs)
	{
		std::stringstream ss;
		combineStrSS(ss, remainArgs...);
		return ss.str();
	}

	// A type cast that is safer than static_cast in debug builds, and is a simple static_cast in release builds.
	// Used for downcasting various ISomething* pointers to their implementation classes in the backends.
	template <typename T, typename S>
	T* checked_cast(S* source)
	{
		static_assert(!std::is_pointer_v<T> && !std::is_pointer_v<S>);
		if (!source)
		{
			return nullptr;
		}

#if !defined(NDEBUG)
		if constexpr (std::is_polymorphic_v<S> && std::is_polymorphic_v<T>) 
		{
			auto temp = dynamic_cast<T*>(source);
			assert(temp != nullptr && "checked_cast failed: pointer type mismatch");
			return temp;
		}
		else 
		{
			static_assert(
				std::is_base_of_v<T, S> || std::is_base_of_v<S, T>,
				"checked_cast on non-polymorphic types requires inheritance"
				);
			return static_cast<T*>(source);
		}
#else
		// 发布模式直接 static_cast
		return static_cast<T*>(source);
#endif
	}

	template <typename T, typename S>
	Ref<T> checked_cast(Ref<S> source)
	{
		static_assert(!std::is_pointer_v<T> && !std::is_pointer_v<S>);
		if (!source)
		{
			return nullptr;
		}

#if !defined(NDEBUG)
		if constexpr (std::is_polymorphic_v<S> && std::is_polymorphic_v<T>)
		{

			auto temp = dynamic_cast<T*>(source.Detach());
			assert(temp != nullptr && "checked_cast failed: pointer type mismatch");
			return AcquireRef(temp);
		}
		else 
		{
			static_assert(
				std::is_base_of_v<T, S> || std::is_base_of_v<S, T>,
				"checked_cast on non-polymorphic types requires inheritance"
				);
			return AcquireRef(static_cast<T*>(source.Detach()));
		}
#else
		// 发布模式直接 static_cast
		return AcquireRef(static_cast<T*>(source.Detach()));
#endif
	}

	template <typename T>
	bool IsPowerOfTwo(T val) noexcept
	{
		return val > 0 && (val & (val - 1)) == 0;
	}

	template <typename T>
	int PopCount(T x) noexcept
	{
		static_assert(std::is_unsigned<T>::value, "T must be unsigned");
		static_assert(sizeof(x) <= sizeof(uint64_t), "T is too large");

#if defined (__GNUC__) || defined(__clang__)
		if constexpr (sizeof(x) <= sizeof(uint32_t))
		{
			return __builtin_popcount(x);
		}
		else
		{
			return __builtin_popcountll(x);
		}
#else
		int count = 0;
		while (x) {
			x &= x - 1;
			count++;
		}
		return count;
#endif 
	}

	template <typename T1, typename T2>
	inline typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type AlignUp(T1 val, T2 alignment)
	{
		static_assert(std::is_unsigned<T1>::value == std::is_unsigned<T2>::value, "both types must be either signed or unsigned");
		static_assert(!std::is_pointer<T1>::value && !std::is_pointer<T2>::value, "types must not be pointers");
		assert(IsPowerOfTwo(alignment));

		using ResultType = typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type;
		return (static_cast<ResultType>(val) + static_cast<ResultType>(alignment - 1)) & ~static_cast<ResultType>(alignment - 1);
	}


	template<typename T> [[nodiscard]] bool arraysAreDifferent(const T* a, uint32_t a_len, const T* b, uint32_t b_len)
	{
		if (a_len != b_len)
		{
			return true;
		}

		for (uint32_t i = 0; i < a_len; i++)
		{
			if (a[i] != b[i])
				return true;
		}

		return false;
	}

	template <typename T1,
		typename T2,
		typename Enable = typename std::enable_if<sizeof(T1) == sizeof(T2)>::type>
	constexpr bool IsSubset(T1 subset, T2 set)
	{
		T2 bitsAlsoInSet = subset & set;
		return bitsAlsoInSet == subset;
	}

	template <typename T1,
		typename T2,
		typename Enable = typename std::enable_if<sizeof(T1) == sizeof(T2)>::type>
	constexpr bool HasFlag(T1 set, T2 flag)
	{
		return (flag & set) != 0;
	}

	template <typename Enum>
	bool HasOneFlag(Enum value)
	{
		using Underlying = std::underlying_type_t<Enum>;

		Underlying underlyingValue = static_cast<Underlying>(value);

		return (underlyingValue & (underlyingValue - 1)) == 0;
	}

	template <typename T>
	T* AlignPtr(T* ptr, size_t alignment) {
		assert(IsPowerOfTwo(alignment));
		return reinterpret_cast<T*>((reinterpret_cast<size_t>(ptr) + (alignment - 1)) &
			~(alignment - 1));
	}

	inline bool IsPtrAligned(const void* ptr, size_t alignment) {
		assert(IsPowerOfTwo(alignment));
		assert(alignment != 0);
		return (reinterpret_cast<size_t>(ptr) & (alignment - 1)) == 0;
	}

	template <typename LHS, typename RHS = LHS, typename T = void>
	struct HasEqualityOperator {
		static constexpr const bool value = false;
	};

	template <typename LHS, typename RHS>
	struct HasEqualityOperator<
		LHS,
		RHS,
		std::enable_if_t<
		std::is_same<decltype(std::declval<LHS>() == std::declval<RHS>()), bool>::value>> {
		static constexpr const bool value = true;
	};

}