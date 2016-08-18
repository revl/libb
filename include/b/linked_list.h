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

#ifndef B_LINKEDLIST_H
#define B_LINKEDLIST_H

#include "helpers.h"

B_BEGIN_NAMESPACE

template <class TYPE>
class LinkedListNode
{
// Type
public:
	typedef TYPE Element;

// Attributes
public:
	void SetNext(Element* new_next)
	{
		next = new_next;
	}

	Element* GetNext()
	{
		return next;
	}

	const Element* GetNext() const
	{
		return next;
	}

// Implementation
protected:
	Element* next;
};

template <class SELECTOR>
class LinkedList
{
// Types
public:
	typedef typename SELECTOR::Element Element;

	typedef typename SELECTOR::Node Node;

	typedef SELECTOR NodeSelector;

// Construction
public:
	LinkedList() : head(NULL), tail(NULL)
	{
	}

// Attributes
public:
	Element* GetHead()
	{
		return head;
	}

	const Element* GetHead() const
	{
		return head;
	}

	Element* GetTail()
	{
		return tail;
	}

	const Element* GetTail() const
	{
		return tail;
	}

	bool IsEmpty() const
	{
		return head == NULL;
	}

// Operations
public:
	static Element* GetNext(Element* element)
	{
		return NodeSelector::GetNode(element)->GetNext();
	}

	static const Element* GetNext(const Element* element)
	{
		return NodeSelector::GetNode(element)->GetNext();
	}

	void AddHead(Element* new_element)
	{
		B_ASSERT(new_element != NULL);

		NodeSelector::GetNode(new_element)->SetNext(head);

		head = new_element;

		if (tail == NULL)
			tail = new_element;
	}

	void AddTail(Element* new_element)
	{
		B_ASSERT(new_element != NULL);

		NodeSelector::GetNode(new_element)->SetNext(NULL);

		if (tail != NULL)
			NodeSelector::GetNode(tail)->SetNext(new_element);
		else
			head = new_element;

		tail = new_element;
	}

	void InsertAfter(Element* element, Element* new_element)
	{
		B_ASSERT(element != NULL && new_element != NULL);

		if (element == tail)
			tail = new_element;

		Node* node = NodeSelector::GetNode(element);

		NodeSelector::GetNode(new_element)->SetNext(node->GetNext());

		node->SetNext(new_element);
	}

	void RemoveHead()
	{
		B_ASSERT(head != NULL);

		if ((head = NodeSelector::GetNode(head)->GetNext()) == NULL)
			tail = NULL;
	}

	void Remove(Element* element, Element* prev)
	{
		B_ASSERT(head != NULL && element != NULL &&
			element == (prev == NULL ? head :
			NodeSelector::GetNode(prev)->GetNext()));

		Element* next = NodeSelector::GetNode(element)->GetNext();

		if (prev != NULL)
			NodeSelector::GetNode(prev)->SetNext(next);
		else
			head = next;

		if (next == NULL)
			tail = prev;
	}

	void RemoveAll()
	{
		tail = head = NULL;
	}

	void MoveToHead(Element* element, Element* prev)
	{
		B_ASSERT(head != NULL && element != NULL &&
			element == (prev == NULL ? head :
			NodeSelector::GetNode(prev)->GetNext()));

		if (prev != NULL)
		{
			Node* node = NodeSelector::GetNode(element);

			Element* next = node->GetNext();

			NodeSelector::GetNode(prev)->SetNext(next);

			if (next == NULL)
				tail = prev;

			node->SetNext(head);

			head = element;
		}
	}

	void MoveToTail(Element* element, Element* prev)
	{
		B_ASSERT(tail != NULL && element != NULL &&
			element == (prev == NULL ? head :
			NodeSelector::GetNode(prev)->GetNext()));

		Node* node = NodeSelector::GetNode(element);

		Element* next = node->GetNext();

		if (next != NULL)
		{
			if (prev != NULL)
				NodeSelector::GetNode(prev)->SetNext(next);
			else
				head = node->GetNext();

			node->SetNext(NULL);

			NodeSelector::GetNode(tail)->SetNext(element);
			tail = element;
		}
	}

// Implementation
public:
	~LinkedList()
		throw ()
	{
		RemoveAll();
	}

protected:
	Element* head;
	Element* tail;
};

B_END_NAMESPACE

#endif // B_LINKEDLIST_H
