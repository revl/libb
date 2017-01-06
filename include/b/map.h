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
	key_value_pair* search(const Search_key& key, int* cmp_result) const;

	// Inserts a new value after a failed search for it.
	key_value_pair* insert_new(const Key &key, const T &value,
			key_value_pair *search_result, int cmp_result);

	key_value_pair* insert(const Key& key, const T& value);

	key_value_pair* insert(const Key& key, const T& value,
		bool* new_inserted);

	~map();

// C++11 compatibility
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
	return static_cast<const key_value_pair*>(
			binary_tree_node::next());
}

template <class Key, class T>
inline typename map<Key, T>::key_value_pair*
		map<Key, T>::key_value_pair::next()
{
	return static_cast<key_value_pair*>(
			binary_tree_node::next());
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
typename map<Key, T>::key_value_pair* map<Key, T>::search(
		const Search_key& key, int* cmp_result) const
{
	binary_tree_node* search_result = tree.search(key, cmp_result);

	if (search_result == NULL)
		return NULL;

	return static_cast<key_value_pair*>(search_result);
}

template <class Key, class T>
typename map<Key, T>::key_value_pair* map<Key, T>::insert_new(
		const Key &key, const T &value,
		key_value_pair *search_result, int cmp_result)
{
	B_ASSERT(search_result == NULL || cmp_result != 0);

	key_value_pair* new_wrapper = new key_value_pair(key, value);

	tree.insert_after_search(new_wrapper,
		search_result, cmp_result);

	return new_wrapper;
}

template <class Key, class T>
typename map<Key, T>::key_value_pair* map<Key, T>::insert(
		const Key& key, const T& value)
{
	int cmp_result;

	key_value_pair* search_result = search(key, &cmp_result);

	if (search_result != NULL && cmp_result == 0)
	{
		search_result->value = value;

		return search_result;
	}

	return insert_new(key, value, search_result, cmp_result);
}

template <class Key, class T>
typename map<Key, T>::key_value_pair* map<Key, T>::insert(
		const Key& key, const T& value, bool* new_inserted)
{
	int cmp_result;

	key_value_pair* search_result = search(key, &cmp_result);

	if (search_result != NULL && cmp_result == 0)
	{
		search_result->value = value;

		*new_inserted = false;

		return search_result;
	}

	*new_inserted = true;

	return insert_new(key, value, search_result, cmp_result);
}

template <class Key, class T>
map<Key, T>::~map()
{
	binary_tree_node* node = tree.leftmost;

	while (node != NULL)
	{
		binary_tree_node* next_node = node->next();

		delete static_cast<key_value_pair*>(node);

		node = next_node;
	}
}

template <class Key, class T>
struct map<Key, T>::const_iterator
{
	const key_value_pair* kv_pair;

	const_iterator& operator ++()
	{
		kv_pair = kv_pair->next();
		return *this;
	}

	bool operator ==(const const_iterator& rhs) const
	{
		return kv_pair == rhs.kv_pair;
	}

	bool operator !=(const const_iterator& rhs) const
	{
		return kv_pair != rhs.kv_pair;
	}

	const key_value_pair* operator ->() const
	{
		return kv_pair;
	}

	const key_value_pair& operator *() const
	{
		return *kv_pair;
	}
};

template <class Key, class T>
typename map<Key, T>::const_iterator map<Key, T>::begin() const
{
	const_iterator leftmost_iter =
	{
		tree.leftmost
	};

	return leftmost_iter;
}

template <class Key, class T>
inline typename map<Key, T>::const_iterator map<Key, T>::end() const
{
	return NULL;
}

B_END_NAMESPACE

#endif /* !defined(B_MAP_H) */
