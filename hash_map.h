/*
	2018. 08 .21
	전문가를 위한 C++에 존재하는
	HashMap 구현하기 코드를 사용하여
	내부적으로 HashMap이 어떻게 동작하는지
	직접 구현
*/

#pragma once

#include <cstddef>
#include <vector>
#include <list>
#include <string>
#include <functional>
#include <iterator>
#include <initializer_list>

namespace ProCpp {
	template <typename T>
	class hash
	{
	public:
		size_t operator()(const T& key) const;
	};
	
	template<>
	class hash<std::string>
	{
	public:
		size_t operator()(const std::string& key) const;
	};

	template <typename HashMap>
	class const_hash_map_iterator : public std::iterator<std::bidirectional_iterator_tag,
		typename HashMap::value_type>
	{
		friend HashMap;
	public:
		using value_type = typename HashMap::value_type;
		using list_iterator_type = typename HashMap::ListType::const_iterator;

		const_hash_map_iterator();
		const_hash_map_iterator(size_t bucket, list_iterator_type listIt, 
			const HashMap* inHashmap);

		const value_type& operator*() const;
		const value_type* operator->() const;
		const_hash_map_iterator<HashMap>& operator++();
		const_hash_map_iterator<HashMap> operator++(int);
		const_hash_map_iterator<HashMap>& operator--();
		const_hash_map_iterator<HashMap> operator--(int);
		bool operator==(const const_hash_map_iterator<HashMap>& rhs) const;
		bool operator!=(const const_hash_map_iterator<HashMap>& rhs) const;

	protected:
		size_t mBucketIndex;
		list_iterator_type mListIterator;
		const HashMap* mHashmap;

		void increment();
		void decrement();
		
	};

	template<typename HashMap>
	class hash_map_iterator : public const_hash_map_iterator<HashMap>
	{
		friend HashMap;
	public:
		using value_type = typename const_hash_map_iterator<HashMap>::value_type;
		using list_iterator_type = typename HashMap::ListType::iterator;

		hash_map_iterator();
		hash_map_iterator(size_t bucket, list_iterator_type listIt, HashMap* inHashmap);

		value_type& operator*();
		value_type* operator->();

		hash_map_iterator<HashMap>& operator++();
		hash_map_iterator<HashMap> operator++(int);
		hash_map_iterator<HashMap>& operator--();
		hash_map_iterator<HashMap> operator--(int);
	};

	template<typename Key, typename T, typename Compare = std::equal_to<Key>, 
		typename Hash = hash<Key>>
	class hash_map
	{
	public:
		using key_type = Key;
		using mapped_type = T;
		using value_type = std::pair<const Key, T>;
		using key_compare = Compare;
		using reference = std::pair<const Key, T>&;
		using const_reference = const std::pair<const Key, T>&;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using hash_map_type = hash_map<Key, T, Compare, Hash>;
		using iterator = hash_map_iterator<hash_map_type>;
		using const_iterator = const_hash_map_iterator<hash_map_type>;

		class value_compare : public std::binary_function<value_type, value_type, bool>
		{
		public:
			bool operator()(const value_type& x, const value_type& y) const
			{
				return comp(x.first, y.first);
			}

		protected:
			Compare comp;
			value_compare(Compare C) : comp(c) {}
		};

		friend class hash_map_iterator<hash_map_type>;
		friend class const_hash_map_iterator<hash_map_type>;

		virtual ~hash_map();

		explicit hash_map(const Compare& comp = Compare(), size_t numBuckets = 101,
			const Hash& hash = Hash());

		template<typename InputIterator>
		hash_map(InputIterator first, InputIterator last, const Compare& comp = Compare(),
			size_type numBuckets = 101, const Hash& hash = Hash());

		hash_map(const hash_map<Key, T, Compare, Hash>& src);
		hash_map(hash_map<Key, T, Compare, Hash>&& src) noexcept;

		explicit hash_map(std::initializer_list<value_type> il, const Compare& comp = Compare(),
			size_type numBuckets = 101, const Hash& hash = Hash());

		hash_map_type& operator=(std::initializer_list<value_type> il);
		hash_map<Key, T, Compare, Hash>& operator=(const hash_map<Key, T, Compare, Hash>& rhs);
		hash_map<Key, T, Compare, Hash>& operator=(hash_map<Key, T, Compare, Hash>&& rhs) noexcept;

		T& operator[] (const key_type& k);

		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;

		bool empty() const;
		size_type size() const;
		size_type max_size() const;

		std::pair<iterator, bool> insert(const value_type& x);
		iterator insert(iterator position, const value_type& x);

		template<typename InputIterator>
		void insert(InputIterator first, InputIterator last);

		void insert(std::initializer_list<value_type> il);
		//void insert(const value_type& x);

		std::pair<iterator, bool> emplace(value_type&& x);
		iterator emplace_hint(iterator hint, value_type&& x);

		size_type erase(const key_type& x);
		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);
		//void erase(const key_type& k);

		void swap(hash_map_type& hashIn);
		void clear() noexcept;

		key_compare key_comp() const;
		value_compare value_comp() const;

		iterator find(const key_type& x);
		const_iterator find(const key_type& x) const;

		std::pair<iterator, iterator> equal_range(const key_type& x);
		std::pair<const_iterator, const_iterator> equal_range(const key_type& x) const;

		size_type count(const key_type& x) const;

	private:
		using ListType = std::list<value_type>;

		typename ListType::iterator findElement(const key_type& k, size_t& bucket);
		void CopyFrom(const hash_map<Key, T, Compare, Hash>& src);

		std::vector<ListType> mBuckets;
		size_t mSize;
		Compare mComp;
		Hash mHash;
	};
}
#include "hash_map.inl"