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

#ifndef B_SET_H
#define B_SET_H

#include "binary_tree.h"

B_BEGIN_NAMESPACE

template <class T>
struct set_element : public binary_tree_node
{
	set_element(const T& v);

	T value;

	const set_element* next() const;

	set_element* next();

	const set_element* prev() const;

	set_element* prev();
};

template <class T>
inline set_element<T>::set_element(const T& v) : value(v)
{
}

template <class T>
inline const set_element<T>* set_element<T>::next() const
{
	return static_cast<const set_element<T>*>(binary_tree_node::next());
}

template <class T>
inline set_element<T>* set_element<T>::next()
{
	return static_cast<set_element<T>*>(binary_tree_node::next());
}

template <class T>
inline const set_element<T>* set_element<T>::prev() const
{
	return static_cast<const set_element<T>*>(binary_tree_node::prev());
}

template <class T>
inline set_element<T>* set_element<T>::prev()
{
	return static_cast<set_element<T>*>(binary_tree_node::prev());
}

// A sorted set of unique elements of type T. The class uses a
// binary search tree for searching and keeping track of the
// elements.
//
// The type T:
//
// 1) must be comparable (that is, 'operator <' must be defined
// for it) and
//
// 2) must support conversion to the argument type of 'search()'
// and 'find()'.
//
template <class T, class Key_op>
class set_base
{
public:
	// Initializes this object.
	set_base();

	// Returns true if this container is empty.
	bool is_empty() const;

	// Returns the number of elements in this container.
	size_t size() const;

	// Finds the element that matches the specified key.
	// Returns NULL if there is no match.
	template <class Search_key>
	T* find(const Search_key& key) const;

	// Structure returned by the search() method.
	struct search_result
	{
		// Returns the element that matches the search
		// key or NULL if there are no elements or there is
		// no match.
		T* match() const;

		// When 'value' is not NULL and 'cmp_result' is
		// zero, 'value' points to the matching element.
		// Otherwise, it provides a hint for one of the
		// 'insert()' methods below.
		T* value;

		// The result of comparison between '*value' and the
		// search key. Zero when the sought element is found.
		int cmp_result;
	};

	// Searches for the element that has the specified key.
	//
	// Returns a structure containing either the found
	// element or a hint for the subsequent insertion of
	// a new element with the same key.
	template <class Search_key>
	search_result search(const Search_key& key) const;

	// Inserts a new element after a failed search for it.
	//
	// Returns a pointer to the copy of 'value' that is stored
	// with the newly insterted element.
	T* insert(const T& value, const search_result& sr);

	// Returns a pointer to the first element or NULL if there are
	// no elements.
	const T* first() const;

	// Returns a pointer to the first element or NULL if there are
	// no elements.
	T* first();

	// Returns a pointer to the last element or NULL if there are
	// no elements.
	const T* last() const;

	// Returns a pointer to the last element or NULL if there are
	// no elements.
	T* last();

	// Returns a pointer to the immediate successor of 'value'
	// or NULL if 'value' is the last element.
	static const T* next(const T* value);

	// Returns a pointer to the immediate successor of 'value'
	// or NULL if 'value' is the last element.
	static T* next(T* value);

	// Returns a pointer to the immediate predecessor of 'value'
	// or NULL if 'value' is the first element.
	static const T* prev(const T* value);

	// Returns a pointer to the immediate predecessor of 'value'
	// or NULL if 'value' is the first element.
	static T* prev(T* value);

	// Bidirectional const iterator type.
	struct const_iterator;

	// Starts forward iteration.
	const_iterator begin() const;

	// Marks the end of forward iteration.
	const_iterator end() const;

protected:
	static const T* value_for_node(const binary_tree_node* node);

	static T* value_for_node(binary_tree_node* node);

	binary_search_tree<Key_op> tree;

public:
	~set_base();
};

template <class T, class Key_op>
inline set_base<T, Key_op>::set_base() : tree(Key_op())
{
}

template <class T, class Key_op>
inline bool set_base<T, Key_op>::is_empty() const
{
	return tree.root == NULL;
}

template <class T, class Key_op>
inline size_t set_base<T, Key_op>::size() const
{
	return tree.number_of_nodes;
}

template <class T, class Key_op>
template <class Search_key>
T* set_base<T, Key_op>::find(const Search_key& key) const
{
	return value_for_node(tree.find(key));
}

template <class T, class Key_op>
T* set_base<T, Key_op>::search_result::match() const
{
	return cmp_result == 0 ? value : NULL;
}

template <class T, class Key_op>
template <class Search_key>
typename set_base<T, Key_op>::search_result set_base<T, Key_op>::search(
		const Search_key& key) const
{
	search_result sr;

	sr.value = value_for_node(tree.search(key, &sr.cmp_result));

	return sr;
}

template <class T, class Key_op>
T* set_base<T, Key_op>::insert(const T& value, const set_base<T, Key_op>::search_result& sr)
{
	B_ASSERT(sr.match() == NULL);

	set_element<T>* new_element = new set_element<T>(value);

	tree.insert_after_search(new_element, sr.value != NULL ?
		B_OUTERSTRUCT(set_element<T>, value, sr.value) : NULL, sr.cmp_result);

	return &new_element->value;
}

template <class T, class Key_op>
const T* set_base<T, Key_op>::first() const
{
	return value_for_node(tree.leftmost);
}

template <class T, class Key_op>
T* set_base<T, Key_op>::first()
{
	return value_for_node(tree.leftmost);
}

template <class T, class Key_op>
const T* set_base<T, Key_op>::last() const
{
	return value_for_node(tree.rightmost);
}

template <class T, class Key_op>
T* set_base<T, Key_op>::last()
{
	return value_for_node(tree.rightmost);
}

template <class T, class Key_op>
struct set_base<T, Key_op>::const_iterator
{
	const T* value_addr;

	const_iterator(const T* va) : value_addr(va)
	{
	}

	const_iterator& operator ++()
	{
		B_ASSERT(value_addr != NULL);

		const set_element<T>* next_element = B_OUTERSTRUCT(
			set_element<T>, value, value_addr)->next();

		value_addr = next_element == NULL ? NULL : &next_element->value;

		return *this;
	}

	const_iterator& operator --()
	{
		B_ASSERT(value_addr != NULL);

		const set_element<T>* prev_element = B_OUTERSTRUCT(
			set_element<T>, value, value_addr)->prev();

		value_addr = prev_element == NULL ? NULL : &prev_element->value;

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

template <class T, class Key_op>
typename set_base<T, Key_op>::const_iterator set_base<T, Key_op>::begin() const
{
	return first();
}

template <class T, class Key_op>
inline typename set_base<T, Key_op>::const_iterator set_base<T, Key_op>::end() const
{
	return NULL;
}

template <class T, class Key_op>
const T* set_base<T, Key_op>::value_for_node(const binary_tree_node* node)
{
	return node != NULL ? &static_cast<const set_element<T>*>(node)->value : NULL;
}

template <class T, class Key_op>
T* set_base<T, Key_op>::value_for_node(binary_tree_node* node)
{
	return node != NULL ? &static_cast<set_element<T>*>(node)->value : NULL;
}

template <class T, class Key_op>
set_base<T, Key_op>::~set_base()
{
	for (set_element<T>* element_to_delete = static_cast<set_element<T>*>(tree.leftmost);
			element_to_delete != NULL; )
	{
		set_element<T>* next_element = element_to_delete->next();

		tree.remove(element_to_delete);

		delete element_to_delete;

		element_to_delete = next_element;
	}
}

// Functor that returns the set element stored with the specified tree node.
template <class T>
struct set_key_op
{
	const T& operator()(const binary_tree_node* node) const
	{
		B_ASSERT(node != NULL);

		return static_cast<const set_element<T>*>(node)->value;
	}
};

template <class T>
class set : public set_base<T, set_key_op<T> >
{
public:
	typedef set_base<T, set_key_op<T> > base;

	// Inserts the specified value into this set.
	//
	// If an element with the same key already exists,
	// its value is overwritten with the specified value.
	//
	// The method returns a pointer to the stored copy of 'value'.
	T* insert(const T& value);

	// Inserts the specified value into this set.
	//
	// If an element with the same key already exists,
	// its value is overwritten with the specified value.
	//
	// The method returns a pointer to the stored copy of
	// 'value' and sets 'new_element' to true or false,
	// depending on whether an insertion or a replacement has
	// occurred.
	T* insert(const T& value, bool* new_element);
};

template <class T>
T* set<T>::insert(const T& value)
{
	typename base::search_result sr = base::search(value);

	T* match = sr.match();

	if (match != NULL)
	{
		*match = value;

		return match;
	}

	return base::insert(value, sr);
}

template <class T>
T* set<T>::insert(const T& value, bool* new_inserted)
{
	typename base::search_result sr = base::search(value);

	T* match = sr.match();

	if (match != NULL)
	{
		*match = value;

		*new_inserted = false;

		return match;
	}

	*new_inserted = true;

	return base::insert(value, sr);
}

B_END_NAMESPACE

#endif /* !defined(B_SET_H) */
