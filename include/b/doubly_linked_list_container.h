// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#ifndef B_DOUBLY_LINKED_LIST_CONTAINER_H
#define B_DOUBLY_LINKED_LIST_CONTAINER_H

#include "doubly_linked_list.h"
#include "memory.h"

B_BEGIN_NAMESPACE

// Bidirectional linked list container.
template <class T>
class doubly_linked_list_container
{
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

	void empty()
	{
		T* current_element = wrappers.first();

		wrappers.empty();

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

private:
	struct element_wrapper : public doubly_linked_list_node<T>
	{
		static void* operator new(size_t size)
		{
			return memory::alloc(size);
		}

		static void operator delete(void* address)
		{
			memory::free(address);
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
