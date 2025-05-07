#pragma once

#include <cassert>
#include <utility>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

namespace rhi::impl
{
	template<typename Serial, typename Value, typename Enable = std::enable_if_t<std::is_integral_v<Serial> || std::is_unsigned_v<Serial>>>
	class SerialQueue
	{
		using ValueType = std::pair<Serial, Value>;
		using SizeType = typename std::vector<ValueType>::size_type;
		using Storage = std::vector<ValueType>;
		using StorageIterator = typename Storage::iterator;
		using ConstStorageIterator = typename Storage::const_iterator;
	public:
		class Iterator
		{
		public:
			explicit Iterator(StorageIterator iter);
			Iterator& operator++();
			bool operator==(const Iterator& other) const;
			bool operator!=(const Iterator& other) const;
			Value& operator*() const;
		private:
			StorageIterator mStorageIterator;
		};

		class ConstIterator
		{
		public:
			explicit ConstIterator(ConstStorageIterator iter);
			ConstIterator& operator++();
			bool operator==(const ConstIterator& other) const;
			bool operator!=(const ConstIterator& other) const;
			const Value& operator*() const;

		private:
			ConstStorageIterator mStorageIterator;
		};

		class IteratorRange
		{
		public:
			IteratorRange(StorageIterator begin, StorageIterator end);
			Iterator begin() const;
			Iterator end() const;
		private:
			StorageIterator mBegin;
			StorageIterator mEnd;
		};

		class ConstIteratorRange
		{
		public:
			ConstIteratorRange(ConstStorageIterator begin, ConstStorageIterator end);
			ConstIterator begin() const;
			ConstIterator end() const;
		private:
			ConstStorageIterator mBegin;
			ConstStorageIterator mEnd;
		};

		// The serial must be given in (not strictly) increasing order.
		void Push(Serial serial, const Value& value);
		void Push(Serial serial, Value&& value);
		bool Empty() const;
		// smaller OR EQUAL to the given serial
		// Usage: for (const T& value : queue.IterateAll()) { stuff(T); }
		IteratorRange IterateAll();
		IteratorRange IterateUpTo(Serial serial);
		ConstIteratorRange CIterateAll() const;
		ConstIteratorRange CIterateUpTo(Serial serial) const;

		void Clear();
		void ClearUpTo(Serial serial);

		Serial FirstSerial() const;
		Serial LastSerial() const;

	private:
		std::vector<std::pair<Serial, Value>> mStorage;
	};

	// Iterator
	template <typename Serial, typename Value, typename Enable>
	SerialQueue<Serial, Value, Enable>::Iterator::Iterator(typename SerialQueue<Serial, Value, Enable>::StorageIterator iter)
		:mStorageIterator(iter) {
	}

	template <typename Serial, typename Value, typename Enable>
	typename SerialQueue<Serial, Value, Enable>::Iterator& SerialQueue<Serial, Value, Enable>::Iterator::operator++()
	{
		mStorageIterator++;
		return *this;
	}

	template <typename Serial, typename Value, typename Enable>
	bool SerialQueue<Serial, Value, Enable>::Iterator::operator==(const typename SerialQueue<Serial, Value, Enable>::Iterator& other) const
	{
		return other.mStorageIterator == mStorageIterator;
	}

	template <typename Serial, typename Value, typename Enable>
	bool SerialQueue<Serial, Value, Enable>::Iterator::operator!=(const typename SerialQueue<Serial, Value, Enable>::Iterator& other) const
	{
		return !(*this == other);
	}

	template <typename Serial, typename Value, typename Enable>
	Value& SerialQueue<Serial, Value, Enable>::Iterator::operator*() const
	{
		return mStorageIterator->second;
	}


	// ConstIterator
	template <typename Serial, typename Value, typename Enable>
	SerialQueue<Serial, Value, Enable>::ConstIterator::ConstIterator(typename SerialQueue<Serial, Value, Enable>::ConstStorageIterator iter)
		:mStorageIterator(iter) {
	}

	template <typename Serial, typename Value, typename Enable>
	typename SerialQueue<Serial, Value, Enable>::ConstIterator& SerialQueue<Serial, Value, Enable>::ConstIterator::operator++()
	{
		mStorageIterator++;
		return *this;
	}

	template <typename Serial, typename Value, typename Enable>
	bool SerialQueue<Serial, Value, Enable>::ConstIterator::operator==(const typename SerialQueue<Serial, Value, Enable>::ConstIterator& other) const
	{
		return other.mStorageIterator == mStorageIterator;
	}

	template <typename Serial, typename Value, typename Enable>
	bool SerialQueue<Serial, Value, Enable>::ConstIterator::operator!=(const typename SerialQueue<Serial, Value, Enable>::ConstIterator& other) const
	{
		return !(*this == other);
	}

	template <typename Serial, typename Value, typename Enable>
	const Value& SerialQueue<Serial, Value, Enable>::ConstIterator::operator*() const
	{
		return mStorageIterator->second;
	}

	// IteratorRange
	template <typename Serial, typename Value, typename Enable>
	SerialQueue<Serial, Value, Enable>::IteratorRange::IteratorRange(
		typename SerialQueue<Serial, Value, Enable>::StorageIterator begin,
		typename SerialQueue<Serial, Value, Enable>::StorageIterator end)
		:mBegin(begin), mEnd(end) {}

	template <typename Serial, typename Value, typename Enable>
	typename SerialQueue<Serial, Value, Enable>::Iterator SerialQueue<Serial, Value, Enable>::IteratorRange::begin() const
	{
		return SerialQueue<Serial, Value, Enable>::Iterator(mBegin);
	}

	template <typename Serial, typename Value, typename Enable>
	typename SerialQueue<Serial, Value, Enable>::Iterator SerialQueue<Serial, Value, Enable>::IteratorRange::end() const
	{
		return SerialQueue<Serial, Value, Enable>::Iterator(mEnd);
	}

	// ConstIteratorRange
	template <typename Serial, typename Value, typename Enable>
	SerialQueue<Serial, Value, Enable>::ConstIteratorRange::ConstIteratorRange(
		typename SerialQueue<Serial, Value, Enable>::ConstStorageIterator begin,
		typename SerialQueue<Serial, Value, Enable>::ConstStorageIterator end)
		:mBegin(begin), mEnd(end) {
	}

	template <typename Serial, typename Value, typename Enable>
	typename SerialQueue<Serial, Value, Enable>::ConstIterator SerialQueue<Serial, Value, Enable>::ConstIteratorRange::begin() const
	{
		return SerialQueue<Serial, Value, Enable>::ConstIterator(mBegin);
	}

	template <typename Serial, typename Value, typename Enable>
	typename SerialQueue<Serial, Value, Enable>::ConstIterator SerialQueue<Serial, Value, Enable>::ConstIteratorRange::end() const
	{
		return SerialQueue<Serial, Value, Enable>::ConstIterator(mEnd);
	}

	// methods
	template <typename Serial, typename Value, typename Enable>
	void SerialQueue<Serial, Value, Enable>::Push(Serial serial, const Value& value)
	{
		assert(Empty() || serial >= LastSerial());
		mStorage.push_back({ serial, value });
	}

	template <typename Serial, typename Value, typename Enable>
	void SerialQueue<Serial, Value, Enable>::Push(Serial serial, Value&& value)
	{
		assert(Empty() || serial >= LastSerial());
		mStorage.push_back({ serial, std::move(value)});
	}

	template <typename Serial, typename Value, typename Enable>
	bool SerialQueue<Serial, Value, Enable>::Empty() const
	{
		return mStorage.empty();
	}
	template <typename Serial, typename Value, typename Enable>
	typename SerialQueue<Serial, Value, Enable>::IteratorRange  SerialQueue<Serial, Value, Enable>::IterateAll()
	{
		return { mStorage.begin(), mStorage.end() };
	}

	template <typename Serial, typename Value, typename Enable>
	typename SerialQueue<Serial, Value, Enable>::IteratorRange  SerialQueue<Serial, Value, Enable>::IterateUpTo(Serial serial)
	{
		auto it = std::upper_bound(mStorage.begin(), mStorage.end(), serial,
			[](Serial s, const typename Storage::value_type& pair) { return s < pair.first; });
		return { mStorage.begin(), it };
	}

	template <typename Serial, typename Value, typename Enable>
	typename SerialQueue<Serial, Value, Enable>::ConstIteratorRange  SerialQueue<Serial, Value, Enable>::CIterateAll() const
	{
		return { mStorage.cbegin(), mStorage.cend() };
	}

	template <typename Serial, typename Value, typename Enable>
	typename SerialQueue<Serial, Value, Enable>::ConstIteratorRange  SerialQueue<Serial, Value, Enable>::CIterateUpTo(Serial serial) const
	{
		auto it = std::upper_bound(mStorage.cbegin(), mStorage.cend(), serial,
			[](Serial s, const typename Storage::value_type& pair) { return s < pair.first; });
		return { mStorage.cbegin(), it };
	}

	template <typename Serial, typename Value, typename Enable>
	void SerialQueue<Serial, Value, Enable>::Clear()
	{
		mStorage.clear();
	}

	template <typename Serial, typename Value, typename Enable>
	void SerialQueue<Serial, Value, Enable>::ClearUpTo(Serial serial)
	{
		auto it = std::upper_bound(mStorage.begin(), mStorage.end(), serial,
			[](Serial s, const ValueType& pair) { return s < pair.first; });
		mStorage.erase(mStorage.begin(), it);
	}

	template<typename Serial, typename Value, typename Enable>
	Serial SerialQueue<Serial, Value, Enable>::FirstSerial() const {
		if (mStorage.empty()) {
			throw std::out_of_range("SerialQueue is empty");
		}
		return mStorage.front().first;
	}

	template<typename Serial, typename Value, typename Enable>
	Serial SerialQueue<Serial, Value, Enable>::LastSerial() const {
		if (mStorage.empty()) {
			throw std::out_of_range("SerialQueue is empty");
		}
		return mStorage.back().first;
	}
}




