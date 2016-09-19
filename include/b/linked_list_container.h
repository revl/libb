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

B_BEGIN_NAMESPACE

template <class TYPE>
class LinkedListContainer
{
// Attributes
public:
	TYPE* GetHead()
	{
		return list.GetHead();
	}

	const TYPE* GetHead() const
	{
		return list.GetHead();
	}

	TYPE* GetTail()
	{
		return list.GetTail();
	}

	const TYPE* GetTail() const
	{
		return list.GetTail();
	}

	bool IsEmpty() const
	{
		return list.IsEmpty();
	}

// Operations
public:
	static TYPE* GetNext(TYPE* element)
	{
		B_ASSERT(element != NULL);

		return Wrapper::GetNode(element)->GetNext();
	}

	static const TYPE* GetNext(const TYPE* element)
	{
		B_ASSERT(element != NULL);

		return Wrapper::GetNode(element)->GetNext();
	}

	TYPE* AddHead(const TYPE& new_element)
	{
		TYPE* copy = &(new Wrapper(new_element))->element;

		list.AddHead(copy);

		return copy;
	}

	TYPE* AddTail(const TYPE& new_element)
	{
		TYPE* copy = &(new Wrapper(new_element))->element;

		list.AddTail(copy);

		return copy;
	}

	TYPE* InsertAfter(TYPE* element, const TYPE& new_element)
	{
		TYPE* copy = &(new Wrapper(new_element))->element;

		list.InsertAfter(element, copy);

		return copy;
	}

	void RemoveHead()
	{
		TYPE* head = list.GetHead();

		list.RemoveHead();

		delete Wrapper::GetNode(head);
	}

	void Remove(TYPE* element, TYPE* prev)
	{
		list.Remove(element, prev);

		delete Wrapper::GetNode(element);
	}

	void RemoveAll()
	{
		TYPE* element = list.GetHead();

		list.RemoveAll();

		while (element != NULL)
		{
			Wrapper* wrapper = Wrapper::GetNode(element);

			element = wrapper->GetNext();

			delete wrapper;
		}
	}

	void MoveToHead(TYPE* element, TYPE* prev)
	{
		list.MoveToHead(element, prev);
	}

	void MoveToTail(TYPE* element, TYPE* prev)
	{
		list.MoveToTail(element, prev);
	}

// Implementation
private:
	struct Wrapper : public LinkedListNode<TYPE>
	{
		typedef Wrapper Node;

		static void* operator new(size_t size)
		{
			return Memory::Alloc(size);
		}

		static void operator delete(void* address)
		{
			Memory::Free(address);
		}

		Wrapper(const TYPE& source) : element(source)
		{
		}

		static Node* GetNode(TYPE* element)
		{
			return B_OUTERSTRUCT(Wrapper, element, element);
		}

		static const Node* GetNode(const TYPE* element)
		{
			return B_OUTERSTRUCT(Wrapper, element, element);
		}

		TYPE element;
	};

	LinkedList<Wrapper> list;
};

B_END_NAMESPACE

#endif /* !defined(B_LINKED_LIST_CONTAINER_H) */
