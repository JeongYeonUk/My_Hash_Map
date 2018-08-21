#include <stdexcept>
#include <algorithm>
#include <functional>

namespace ProCpp {
	template<typename T>
	inline size_t hash<T>::operator()(const T & key) const
	{
		size_t bytes = sizeof(key);
		size_t res = 0;
		for (size_t i = 0; i < bytes; ++i)
		{
			unsigned char b = *((unsigned char*)&key + i);
			res += b;
		}
		return res;
	}
	inline size_t hash<std::string>::operator()(const std::string & key) const
	{
		size_t sum = 0;
		for (size_t i = 0; i < key.size(); ++i)
		{
			sum += (unsigned char)key[i];
		}
		return sum;
	}
	template<typename HashMap>
	inline const_hash_map_iterator<HashMap>::const_hash_map_iterator()
		: mBucketIndex(0)
		, mListIterator(list_iterator_type())
		, mHashmap(nullptr)
	{
	}
	template<typename HashMap>
	inline const_hash_map_iterator<HashMap>::const_hash_map_iterator(size_t bucket, list_iterator_type listIt, const HashMap* inHashmap)
		: mBucketIndex(bucket)
		, mListIterator(listIt)
		, mHashmap(inHashmap)
	{
	}
	template<typename HashMap>
	inline const typename const_hash_map_iterator<HashMap>::value_type& const_hash_map_iterator<HashMap>::operator*() const
	{
		return *mListIterator;
	}
	template<typename HashMap>
	inline const typename const_hash_map_iterator<HashMap>::value_type* const_hash_map_iterator<HashMap>::operator->() const
	{
		return &(*mListIterator);
	}
	template<typename HashMap>
	inline const_hash_map_iterator<HashMap>& const_hash_map_iterator<HashMap>::operator++()
	{
		increment();
		return *this;
	}
	template<typename HashMap>
	inline const_hash_map_iterator<HashMap> const_hash_map_iterator<HashMap>::operator++(int)
	{
		auto oldit = *this;
		increment();
		return oldIt;
	}
	template<typename HashMap>
	inline const_hash_map_iterator<HashMap>& const_hash_map_iterator<HashMap>::operator--()
	{
		decrement();
		return *this;
	}
	template<typename HashMap>
	inline const_hash_map_iterator<HashMap> const_hash_map_iterator<HashMap>::operator--(int)
	{
		auto oldIt = *this;
		decrement();
		return oldIt;
	}
	template<typename HashMap>
	inline bool const_hash_map_iterator<HashMap>::operator==(const const_hash_map_iterator<HashMap>& rhs) const
	{
		return (mHashmap == rhs.mHashmap && mBucketIndex == rhs.mBucketIndex && mListIterator == rhs.mListIterator);
	}
	template<typename HashMap>
	inline bool const_hash_map_iterator<HashMap>::operator!=(const const_hash_map_iterator<HashMap>& rhs) const
	{
		return !(*this == rhs);
	}
	template<typename HashMap>
	inline void const_hash_map_iterator<HashMap>::increment()
	{
		++mListIterator;

		auto& buckets = mHashmap->mBuckets;
		if (mListIterator == std::end(buckets[mBucketIndex]))
		{
			for (size_t i = mBucketIndex + 1; i < buckets.size(); ++i)
			{
				if (!buckets[i].empty())
				{
					mListIterator = std::begin(buckets[i]);
					mBucketIndex = i;
					return;
				}
			}
			mBucketIndex = buckets.size() - 1;
			mListIterator = std::end(buckets[mBucketIndex]);
		}

	}
	template<typename HashMap>
	inline void const_hash_map_iterator<HashMap>::decrement()
	{
		auto& buckets = mHashmap->mBuckets;
		if (mListIterator == std::begin(buckets[mBucketIndex]))
		{
			for (int i = mBucketIndex - 1; i >= 0; --i)
			{
				if (!buckets[i].empty())
				{
					mListIterator = std::end(buckets[i]);
					--mListIterator;
					mBucketIndex = i;
					return;
				}
			}
			mBucketIndex = buckets.size() - 1;
			mListIterator = std::end(buckets[mBucketIndex]);
		}
		else
		{
			--mListIterator;
		}
	}
	template<typename HashMap>
	inline hash_map_iterator<HashMap>::hash_map_iterator() 
		: const_hash_map_iterator<HashMap>()
	{
	}
	template<typename HashMap>
	inline hash_map_iterator<HashMap>::hash_map_iterator(size_t bucket, list_iterator_type listIt, HashMap* inHashmap)
		: const_hash_map_iterator<HashMap>(bucket, listIt, inHashmap)
	{
	}
	template<typename HashMap>
	inline typename hash_map_iterator<HashMap>::value_type& hash_map_iterator<HashMap>::operator*()
	{
		return const_cast<value_type&>(*this->mListIterator);
	}
	template<typename HashMap>
	inline typename hash_map_iterator<HashMap>::value_type* hash_map_iterator<HashMap>::operator->()
	{
		return const_cast<value_type*>(&(*this->mListIterator));
	}
	template<typename HashMap>
	inline hash_map_iterator<HashMap>& hash_map_iterator<HashMap>::operator++()
	{
		this->increment();
		return *this;
	}
	template<typename HashMap>
	inline hash_map_iterator<HashMap> hash_map_iterator<HashMap>::operator++(int)
	{
		auto oldIt = *this;
		this->increment();
		return *oldIt;
	}
	template<typename HashMap>
	inline hash_map_iterator<HashMap>& hash_map_iterator<HashMap>::operator--()
	{
		this->decrement();
		return *this;
	}
	template<typename HashMap>
	inline hash_map_iterator<HashMap> hash_map_iterator<HashMap>::operator--(int)
	{
		auto oldIt = *this;
		this->decrement();
		return *oldIt;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline hash_map<Key, T, Compare, Hash>::~hash_map()
	{
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline hash_map<Key, T, Compare, Hash>::hash_map(const Compare & comp, 
		size_t numBuckets, const Hash & hash)
		: mSize(0)
		, mComp(comp)
		, mHash(hash)
	{
		if (numBuckets == 0)
		{
			throw std::invalid_argument("Number of buckets must be positive");
		}
		mBuckets.resize(numBuckets);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	template<typename InputIterator>
	inline hash_map<Key, T, Compare, Hash>::hash_map(InputIterator first, InputIterator last,
		const Compare& comp, size_type numBuckets, const Hash& hash)
		: mSize(0)
		, mComp(comp)
		, mHash(hash)
	{
		if (numBuckets == 0)
			throw std::invalid_argument("Number of buckets must be positive");

		mBuckets.resize(numBuckets);
		insert(first, last);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline void hash_map<Key, T, Compare, Hash>::CopyFrom(const hash_map<Key, T, Compare, Hash>& src)
	{
		mComp = src.mComp;
		mHash = src.mHash;
		mBuckets.resize(src.mBuckets.size());

		for (auto& bucket : src.mBuckets)
		{
			for (auto& element : bucket)
			{
				insert(element);
			}
		}
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline hash_map<Key, T, Compare, Hash>::hash_map(const hash_map<Key, T, Compare, Hash>& src)
	{
		CopyFrom(src);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline hash_map<Key, T, Compare, Hash>::hash_map(hash_map<Key, T, Compare, Hash>&& src) noexcept
	{
		mHash = std::move(src.mHash);
		mComp = std::move(src.mComp);
		mSize = std::move(src.mSize);
		mBuckets = std::move(src.mBuckets);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline hash_map<Key, T, Compare, Hash>& 
		hash_map<Key, T, Compare, Hash>::operator=(const hash_map<Key, T, Compare, Hash>& rhs)
	{
		// TODO: 여기에 반환 구문을 삽입합니다.
		if (&rhs == this) {
			return *this;
		}

		mBuckets.clear();
		mSize = 0;

		CopyFrom(rhs);

		return *this;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline hash_map<Key, T, Compare, Hash>& 
		hash_map<Key, T, Compare, Hash>::operator=(hash_map<Key, T, Compare, Hash>&& rhs) noexcept
	{
		// TODO: 여기에 반환 구문을 삽입합니다.
		mHash = std::move(rhs.mHash);
		mComp = std::move(rhs.mComp);
		mSize = std::move(rhs.mSize);
		mBuckets = std::move(rhs.mBuckets);

		return *this;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline hash_map<Key, T, Compare, Hash>::hash_map(std::initializer_list<value_type> il,
		const Compare& comp, size_type numBuckets, const Hash& hash)
		: mSize(0)
		, mComp(comp)
		, mHash(hash)
	{
		if (numBuckets == 0)
			throw std::invalid_argument("Number of buckets must be positive");

		mBuckets.resize(numBuckets);
		insert(std::begin(il), std::end(il));
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline hash_map<Key, T, Compare, Hash>& hash_map<Key, T, Compare, Hash>::operator=(std::initializer_list<value_type> il)
	{
		clear();
		insert(std::begin(il), std::end(il));
		return *this;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::ListType::iterator
		hash_map<Key, T, Compare, Hash>::findElement(const key_type & k, size_t & bucket)
	{
		bucket = mHash(k) % mBuckets.size();

		auto endIter = std::end(mBuckets[bucket]);
		for (auto it = std::begin(mBuckets[bucket]); it != endIter; ++it)
		{
			if (mComp(it->first, k))
				return it;
		}
		return endIter;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::iterator 
		hash_map<Key, T, Compare, Hash>::find(const key_type& x)
	{
		size_t bucket;
		auto it = findElement(x, bucket);
		if (it == std::end(mBuckets[bucket]))
			return end();

		return hash_map_iterator<hash_map_type>(bucket, it, this);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::const_iterator 
		hash_map<Key, T, Compare, Hash>::find(const key_type& x) const
	{
		size_t bucket;
		auto it = const_cast<hash_map_type*>(this)->findElement(x, bucket);
		if (it == std::end(mBuckets[bucket]))
			return end();

		return const_hash_map_iterator<hash_map_type>(bucket, it, this);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline std::pair<typename hash_map<Key,T,Compare,Hash>::iterator, typename hash_map<Key, T, Compare, Hash>::iterator>
		hash_map<Key, T, Compare, Hash>::equal_range(const key_type& x)
	{
		auto it = find(x);
		return std::make_pair(it, it);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline std::pair<typename hash_map<Key, T, Compare, Hash>::const_iterator, typename hash_map<Key,T,Compare,Hash>::const_iterator>
		hash_map<Key, T, Compare, Hash>::equal_range(const key_type& x) const
	{
		auto it = find(x);
		return std::make_pair(it, it);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::size_type
		hash_map<Key, T, Compare, Hash>::count(const key_type& x) const
	{
		if (find(x) == end())
			return 0;
		else
			return 1;
	}
	template <typename Key, typename T, typename Compare, typename Hash>
	inline T& hash_map<Key, T, Compare, Hash>::operator[](const key_type& x)
	{
		return ((insert(std::make_pair(x, T()))).first)->second;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline std::pair<typename hash_map<Key, T, Compare, Hash>::iterator, bool>
		hash_map<Key, T, Compare, Hash>::insert(const value_type& x)
	{
		size_t bucket;
		auto it = findElement(x.first, bucket);
		bool inserted = false;
		if (it == std::end(mBuckets[bucket]))
		{
			it = mBuckets[bucket].insert(std::end(mBuckets[bucket]), x);

			inserted = true;
			mSize++;
		}
		return std::make_pair(hash_map_iterator<hash_map_type>(bucket, it, this), inserted);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::iterator 
		hash_map<Key, T, Compare, Hash>::insert(iterator /*position*/, const value_type& x)
	{
		return insert(x).first;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	template<typename InputIterator>
	inline void hash_map<Key, T, Compare, Hash>::insert(InputIterator first, InputIterator last)
	{
		std::insert_iterator<hash_map_type> inserter(*this, begin());
		std::copy(first, last, inserter);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline void hash_map<Key, T, Compare, Hash>::insert(std::initializer_list<value_type> il)
	{
		insert(std::begin(il), std::end(il));
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline std::pair<typename hash_map<Key, T, Compare, Hash>::iterator, bool>
		hash_map<Key, T, Compare, Hash>::emplace(value_type&& x)
	{
		size_t bucket;
		auto it = findElement(x.first, bucket);
		bool inserted = false;
		if (it == std::end(mBuckets[bucket]))
		{
			it = mBuckets[bucket].emplace(std::end(mBuckets[bucket]), x);
			inserted = true;
			mSize++;
		}
		return std::make_pair(hash_map_iterator<hash_map_type>(bucket, it, this), inserted);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::iterator
		hash_map<Key, T, Compare, Hash>::emplace_hint(iterator /*hint*/, value_type&& x)
	{
		return emplace(std::forward<value_type>(x)).first;
	}
	template <typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::size_type
		hash_map<Key, T, Compare, Hash>::erase(const key_type& x)
	{
		size_t bucket;
		auto it = findElement(x, bucket);

		if (it != std::end(mBuckets[bucket]))
		{
			mBuckets[bucket].erase(it);
			mSize--;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::iterator
		hash_map<Key, T, Compare, Hash>::erase(iterator position)
	{
		iterator next = position;
		++next;

		mBuckets[position.mBucketIndex].erase(position.mListIterator);
		mSize--;
		
		return next;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::iterator
		hash_map<Key, T, Compare, Hash>::erase(iterator first, iterator last)
	{
		for (iterator next = first; next != last;)
		{
			next = erase(next);
		}
		return last;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline void hash_map<Key, T, Compare, Hash>::clear() noexcept
	{
		for (auto& bucket : mBuckets)
		{
			bucket.clear();
		}
		mSize = 0;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline bool hash_map<Key, T, Compare, Hash>::empty() const
	{
		return mSize == 0;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::size_type
		hash_map<Key, T, Compare, Hash>::size() const
	{
		return mSize;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::size_type
		hash_map<Key, T, Compare, Hash>::max_size() const
	{
		return mBuckets[0].max_size();
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline void hash_map<Key, T, Compare, Hash>::swap(hash_map_type& hashIn)
	{
		std::swap(*this, hashIn);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::iterator
		hash_map<Key, T, Compare, Hash>::begin()
	{
		if (mSize == 0)
			return end();

		for (size_t i = 0; i < mBuckets.size(); ++i)
		{
			if (!mBuckets[i].empty())
				return hash_map_iterator<hash_map_type>(i, std::begin(mBuckets[i]), this);
		}
		return end();
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::const_iterator
		hash_map<Key, T, Compare, Hash>::begin() const
	{
		return const_cast<hash_map_type*>(this)->begin();
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::const_iterator
		hash_map<Key, T, Compare, Hash>::cbegin() const
	{
		return begin();
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::iterator
		hash_map<Key, T, Compare, Hash>::end()
	{
		size_t bucket = mBuckets.size() - 1;
		return hash_map_iterator<hash_map_type>(bucket, std::end(mBuckets[bucket]), this);
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::const_iterator
		hash_map<Key, T, Compare, Hash>::end() const
	{
		return const_cast<hash_map_type*>(this)->end();
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::const_iterator
		hash_map<Key, T, Compare, Hash>::cend() const
	{
		return end();
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::key_compare
		hash_map<Key, T, Compare, Hash>::key_comp() const
	{
		return mComp;
	}
	template<typename Key, typename T, typename Compare, typename Hash>
	inline typename hash_map<Key, T, Compare, Hash>::value_compare
		hash_map<Key, T, Compare, Hash>::value_comp() const
	{
		return value_compare(mComp);
	}
}