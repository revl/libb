/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016, 2017 Damon Revoe
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef B_MAP_H
#define B_MAP_H

#include "binary_tree.h"

B_BEGIN_NAMESPACE

template <class Key, class T>
class map
{
public:
	struct key_value_pair : public binary_tree_node
	{
		key_value_pair(const Key& k, const T& v);

		Key key;
		T value;

		const key_value_pair* next() const;

		key_value_pair* next();
	};

	map();

	bool is_empty() const;

	size_t size() const;

	template <class Search_key>
	T* find(const Search_key& key) const;

	struct search_result
	{
		key_value_pair* parent_or_match;
		int cmp_result;

		bool found() const;
	};

	template <class Search_key>
	search_result search(const Search_key& key) const;

	// Inserts a new value after a failed search for it.
	key_value_pair* insert(const Key &key, const T &value,
			const search_result& sr);

	key_value_pair* insert(const Key& key, const T& value);

	key_value_pair* insert(const Key& key, const T& value,
			bool* new_inserted);

// Iteration over key-value pairs
public:
	const key_value_pair* first() const;

	key_value_pair* first();

	const key_value_pair* last() const;

	key_value_pair* last();

// Iteration over values and C++11 compatibility
public:
	struct const_iterator;

	const_iterator begin() const;

	const_iterator end() const;

// Implementation
private:
	struct key_for_node
	{
		const Key& operator()(const binary_tree_node* node) const
		{
			return static_cast<const key_value_pair*>(node)->key;
		}
	};

	binary_search_tree<key_for_node> tree;

public:
	~map();
};

template <class Key, class T>
inline map<Key, T>::key_value_pair::key_value_pair(const Key& k, const T& v) :
	key(k), value(v)
{
}

template <class Key, class T>
inline const typename map<Key, T>::key_value_pair*
		map<Key, T>::key_value_pair::next() const
{
	return static_cast<const key_value_pair*>(binary_tree_node::next());
}

template <class Key, class T>
inline typename map<Key, T>::key_value_pair*
		map<Key, T>::key_value_pair::next()
{
	return static_cast<key_value_pair*>(binary_tree_node::next());
}

template <class Key, class T>
inline map<Key, T>::map() : tree(key_for_node())
{
}

template <class Key, class T>
inline bool map<Key, T>::is_empty() const
{
	return tree.root == NULL;
}

template <class Key, class T>
inline size_t map<Key, T>::size() const
{
	return tree.number_of_nodes;
}

template <class Key, class T>
template <class Search_key>
T* map<Key, T>::find(const Search_key& key) const
{
	binary_tree_node* match = tree.find(key);

	if (match == NULL)
		return NULL;

	return &static_cast<key_value_pair*>(match)->value;
}

template <class Key, class T>
bool map<Key, T>::search_result::found() const
{
	return parent_or_match != NULL && cmp_result == 0;
}

template <class Key, class T>
template <class Search_key>
typename map<Key, T>::search_result map<Key, T>::search(
		const Search_key& key) const
{
	search_result sr;

	sr.parent_or_match = static_cast<key_value_pair*>(
			tree.search(key, &sr.cmp_result));

	return sr;
}

template <class Key, class T>
typename map<Key, T>::key_value_pair* map<Key, T>::insert(
		const Key &key, const T &value,
		const map<Key, T>::search_result& sr)
{
	B_ASSERT(!sr.found());

	key_value_pair* new_wrapper = new key_value_pair(key, value);

	tree.insert_after_search(new_wrapper,
			sr.parent_or_match, sr.cmp_result);

	return new_wrapper;
}

template <class Key, class T>
typename map<Key, T>::key_value_pair* map<Key, T>::insert(
		const Key& key, const T& value)
{
	search_result sr = search(key);

	if (sr.found())
	{
		sr.parent_or_match->value = value;

		return sr.parent_or_match;
	}

	return insert(key, value, sr);
}

template <class Key, class T>
typename map<Key, T>::key_value_pair* map<Key, T>::insert(
		const Key& key, const T& value, bool* new_inserted)
{
	search_result sr = search(key);

	if (sr.found())
	{
		sr.parent_or_match->value = value;

		*new_inserted = false;

		return sr.parent_or_match;
	}

	*new_inserted = true;

	return insert(key, value, sr);
}

template <class Key, class T>
const typename map<Key, T>::key_value_pair* map<Key, T>::first() const
{
	return static_cast<const key_value_pair*>(tree.leftmost);
}

template <class Key, class T>
typename map<Key, T>::key_value_pair* map<Key, T>::first()
{
	return static_cast<key_value_pair*>(tree.leftmost);
}

template <class Key, class T>
const typename map<Key, T>::key_value_pair* map<Key, T>::last() const
{
	return static_cast<const key_value_pair*>(tree.rightmost);
}

template <class Key, class T>
typename map<Key, T>::key_value_pair* map<Key, T>::last()
{
	return static_cast<key_value_pair*>(tree.rightmost);
}

template <class Key, class T>
struct map<Key, T>::const_iterator
{
	const T* value_addr;

	const_iterator(const T* va) : value_addr(va)
	{
	}

	const_iterator& operator ++()
	{
		B_ASSERT(value_addr != NULL);

		const key_value_pair* next_kv_pair =
			B_OUTERSTRUCT(key_value_pair, value, value_addr);

		value_addr = next_kv_pair == NULL ? NULL : &next_kv_pair->value;

		return *this;
	}

	bool operator ==(const const_iterator& rhs) const
	{
		return value_addr == rhs.value_addr;
	}

	bool operator !=(const const_iterator& rhs) const
	{
		return value_addr != rhs.value_addr;
	}

	const T* operator ->() const
	{
		return value_addr;
	}

	const T& operator *() const
	{
		return *value_addr;
	}
};

template <class Key, class T>
typename map<Key, T>::const_iterator map<Key, T>::begin() const
{
	const key_value_pair* first_kv_pair = first();

	return first_kv_pair == NULL ? NULL : &first_kv_pair->value;
}

template <class Key, class T>
inline typename map<Key, T>::const_iterator map<Key, T>::end() const
{
	return NULL;
}

template <class Key, class T>
map<Key, T>::~map()
{
	for (key_value_pair* kv_pair = first(); kv_pair != NULL; )
	{
		key_value_pair* next_kv_pair = kv_pair->next();

		delete kv_pair;

		kv_pair = next_kv_pair;
	}
}

B_END_NAMESPACE

#endif /* !defined(B_MAP_H) */