/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016 Damon Revoe
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

#ifndef B_DOUBLY_LINKED_LIST_CONTAINER_H
#define B_DOUBLY_LINKED_LIST_CONTAINER_H

#include "doubly_linked_list.h"
#include "memory.h"

B_BEGIN_NAMESPACE

template <class T>
class doubly_linked_list_container
{
// Attributes
public:
	T* first()
	{
		return wrappers.first();
	}

	const T* first() const
	{
		return wrappers.first();
	}

	T* last()
	{
		return wrappers.last();
	}

	const T* last() const
	{
		return wrappers.last();
	}

	bool is_empty() const
	{
		return wrappers.is_empty();
	}

// Operations
public:
	static T* next(T* element)
	{
		B_ASSERT(element != NULL);

		return wrapper_node_access::node_for(element)->next();
	}

	static const T* next(const T* element)
	{
		B_ASSERT(element != NULL);

		return wrapper_node_access::node_for(element)->next();
	}

	static T* prev(T* element)
	{
		B_ASSERT(element != NULL);

		return wrapper_node_access::node_for(element)->prev();
	}

	static const T* prev(const T* element)
	{
		B_ASSERT(element != NULL);

		return wrapper_node_access::node_for(element)->prev();
	}

	T* insert_first(const T& new_element)
	{
		T* copy = &(new element_wrapper(new_element))->element;

		wrappers.insert_first(copy);

		return copy;
	}

	T* append(const T& new_element)
	{
		T* copy = &(new element_wrapper(new_element))->element;

		wrappers.append(copy);

		return copy;
	}

	T* insert_after(T* element, const T& new_element)
	{
		T* copy = &(new element_wrapper(new_element))->element;

		wrappers.insert_after(element, copy);

		return copy;
	}

	T* insert_before(T* element, const T& new_element)
	{
		T* copy = &(new element_wrapper(new_element))->element;

		wrappers.insert_before(element, copy);

		return copy;
	}

	void remove_first()
	{
		T* element = wrappers.first();

		wrappers.remove_first();

		delete wrapper_node_access::node_for(element);
	}

	void remove_last()
	{
		T* element = wrappers.last();

		wrappers.remove_last();

		delete wrapper_node_access::node_for(element);
	}

	void remove(T* element)
	{
		wrappers.remove(element);

		delete wrapper_node_access::node_for(element);
	}

	void remove_all()
	{
		T* current_element = wrappers.first();

		wrappers.remove_all();

		while (current_element != NULL)
		{
			element_wrapper* wrapper =
				wrapper_node_access::node_for(current_element);

			current_element = wrapper->next();

			delete wrapper;
		}
	}

	void move_to_front(T* element)
	{
		wrappers.move_to_front(element);
	}

	void move_to_back(T* element)
	{
		wrappers.move_to_back(element);
	}

// Implementation
private:
	struct element_wrapper : public doubly_linked_list_node<T>
	{
		static void* operator new(size_t size)
		{
			return Memory::Alloc(size);
		}

		static void operator delete(void* address)
		{
			Memory::Free(address);
		}

		element_wrapper(const T& source) : element(source)
		{
		}

		T element;
	};

	struct wrapper_node_access
	{
		typedef T element_type;
		typedef element_wrapper node_type;

		static node_type* node_for(T* element)
		{
			return B_OUTERSTRUCT(element_wrapper, element, element);
		}

		static const node_type* node_for(const T* element)
		{
			return B_OUTERSTRUCT(element_wrapper, element, element);
		}
	};

	doubly_linked_list<wrapper_node_access> wrappers;

public:
	doubly_linked_list_container() : wrappers(wrapper_node_access())
	{
	}
};

B_END_NAMESPACE

#endif /* !defined(B_DOUBLY_LINKED_LIST_CONTAINER_H) */
