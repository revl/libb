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

#ifndef B_LINKED_LIST_CONTAINER_H
#define B_LINKED_LIST_CONTAINER_H

#include "linked_list.h"
#include "memory.h"

B_BEGIN_NAMESPACE

template <class T>
class linked_list_container
{
// Attributes
public:
	T* GetHead()
	{
		return wrappers.GetHead();
	}

	const T* GetHead() const
	{
		return wrappers.GetHead();
	}

	T* GetTail()
	{
		return wrappers.GetTail();
	}

	const T* GetTail() const
	{
		return wrappers.GetTail();
	}

	bool IsEmpty() const
	{
		return wrappers.IsEmpty();
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

	T* AddHead(const T& new_element)
	{
		T* copy = &(new element_wrapper(new_element))->element;

		wrappers.AddHead(copy);

		return copy;
	}

	T* AddTail(const T& new_element)
	{
		T* copy = &(new element_wrapper(new_element))->element;

		wrappers.AddTail(copy);

		return copy;
	}

	T* InsertAfter(T* element, const T& new_element)
	{
		T* copy = &(new element_wrapper(new_element))->element;

		wrappers.InsertAfter(element, copy);

		return copy;
	}

	void RemoveHead()
	{
		T* head = wrappers.GetHead();

		wrappers.RemoveHead();

		delete wrapper_node_access::node_for(head);
	}

	void Remove(T* element, T* prev)
	{
		wrappers.Remove(element, prev);

		delete wrapper_node_access::node_for(element);
	}

	void RemoveAll()
	{
		T* element = wrappers.GetHead();

		wrappers.RemoveAll();

		while (element != NULL)
		{
			element_wrapper* wrapper =
				wrapper_node_access::node_for(element);

			element = wrapper->next();

			delete wrapper;
		}
	}

	void MoveToHead(T* element, T* prev)
	{
		wrappers.MoveToHead(element, prev);
	}

	void MoveToTail(T* element, T* prev)
	{
		wrappers.MoveToTail(element, prev);
	}

// Implementation
private:
	struct element_wrapper : public linked_list_node<T>
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
	};

	linked_list<wrapper_node_access> wrappers;

public:
	linked_list_container() : wrappers(wrapper_node_access())
	{
	}
};

B_END_NAMESPACE

#endif /* !defined(B_LINKED_LIST_CONTAINER_H) */
