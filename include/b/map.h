/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2020 Damon Revoe
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

#include "set.h"

B_BEGIN_NAMESPACE

// Map element type (a key-value pair).
template <class Key, class T>
struct kv_pair
{
	kv_pair(const Key& k, const T& v) : key(k), value(v)
	{
	}

	Key key;
	T value;
};

// Functor that returns the key for to the map element stored
// with the specified tree node.
template <class Key, class T>
struct map_key_op
{
	const Key& operator()(const binary_tree_node* node) const
	{
		B_ASSERT(node != NULL);

		return static_cast<const set_element<kv_pair<Key, T> >*>(
			node)->value.key;
	}
};

// Associative array container with unique keys.
template <class Key, class T>
class map : public set_base<kv_pair<Key, T>, map_key_op<Key, T> >
{
public:
	typedef set_base<kv_pair<Key, T>, map_key_op<Key, T> > base;

	// Finds the value that matches the specified key.
	// Returns NULL if there is no match.
	template <class Search_key>
	T* find(const Search_key& key) const;

	// Inserts a new map element after a failed search for it.
	// Returns a pointer to the newly insterted element.
	kv_pair<Key, T>* insert_new(const Key& key, const T& value,
			const typename base::search_result& sr);

	// Adds the specified key-value pair to this map.
	//
	// If an element with the same key already exists,
	// its value is overwritten with the specified value.
	//
	// The method returns a pointer to the stored map element.
	kv_pair<Key, T>* insert(const Key& key, const T& value);

	// Adds the specified key-value pair to this map.
	//
	// If an element with the same key already exists,
	// its value is overwritten with the specified value.
	//
	// The method returns a pointer to the stored map element
	// and sets 'new_element' to true or false, depending on
	// whether an insertion or a replacement has occurred.
	kv_pair<Key, T>* insert(const Key& key, const T& value,
			bool* new_inserted);
};

template <class Key, class T>
template <class Search_key>
T* map<Key, T>::find(const Search_key& key) const
{
	kv_pair<Key, T>* match = base::find(key);

	return match != NULL ? &match->value : NULL;
}

template <class Key, class T>
kv_pair<Key, T>* map<Key, T>::insert_new(const Key &key, const T &value,
		const typename map<Key, T>::base::search_result& sr)
{
	return base::insert_new(kv_pair<Key, T>(key, value), sr);
}

template <class Key, class T>
kv_pair<Key, T>* map<Key, T>::insert(const Key& key, const T& value)
{
	typename base::search_result sr = base::search(key);

	kv_pair<Key, T>* match = sr.match();

	if (match != NULL)
	{
		match->value = value;

		return match;
	}

	return base::insert_new(kv_pair<Key, T>(key, value), sr);
}

template <class Key, class T>
kv_pair<Key, T>* map<Key, T>::insert(const Key& key, const T& value,
		bool* new_inserted)
{
	typename base::search_result sr = base::search(key);

	kv_pair<Key, T>* match = sr.match();

	if (match != NULL)
	{
		match->value = value;

		*new_inserted = false;

		return match;
	}

	*new_inserted = true;

	return base::insert_new(kv_pair<Key, T>(key, value), sr);
}

B_END_NAMESPACE

#endif /* !defined(B_MAP_H) */
