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

#ifndef B_DOUBLYLINKEDLIST_H
#define B_DOUBLYLINKEDLIST_H

#include "LinkedList.h"

B_BEGIN_NAMESPACE

template <class TYPE>
class DoublyLinkedListNode : public LinkedListNode<TYPE>
{
// Type
public:
	typedef TYPE Element;

// Attributes
public:
	void SetPrev(Element* new_prev)
	{
		prev = new_prev;
	}

	Element* GetPrev()
	{
		return prev;
	}

	const Element* GetPrev() const
	{
		return prev;
	}

// Implementation
protected:
	Element* prev;
};

template <class SELECTOR>
class DoublyLinkedList : public LinkedList<SELECTOR>
{
// Types
public:
	typedef typename SELECTOR::Element Element;

	typedef typename SELECTOR::Node Node;

	typedef SELECTOR NodeSelector;

// Operations
public:
	static Element* GetPrev(Element* element)
	{
		return NodeSelector::GetNode(element)->GetPrev();
	}

	static const Element* GetPrev(const Element* element)
	{
		return NodeSelector::GetNode(element)->GetPrev();
	}

	void AddHead(Element* new_element)
	{
		B_ASSERT(new_element != NULL);

		Node* node = NodeSelector::GetNode(new_element);

		node->SetNext(this->head);
		node->SetPrev(NULL);

		if (this->head != NULL)
			NodeSelector::GetNode(this->head)->SetPrev(new_element);
		else
			this->tail = new_element;

		this->head = new_element;
	}

	void AddTail(Element* new_element)
	{
		B_ASSERT(new_element != NULL);

		Node* node = NodeSelector::GetNode(new_element);

		node->SetNext(NULL);
		node->SetPrev(this->tail);

		if (this->tail != NULL)
			NodeSelector::GetNode(this->tail)->SetNext(new_element);
		else
			this->head = new_element;

		this->tail = new_element;
	}

	void InsertAfter(Element* element, Element* new_element)
	{
		B_ASSERT(element != NULL && new_element != NULL);

		Node* node = NodeSelector::GetNode(element);

		Element* next = node->GetNext();

		node->SetNext(new_element);

		if (element == this->tail)
			this->tail = new_element;
		else
			NodeSelector::GetNode(next)->SetPrev(new_element);

		node = NodeSelector::GetNode(new_element);

		node->SetNext(next);
		node->SetPrev(element);
	}

	void InsertBefore(Element* element, Element* new_element)
	{
		B_ASSERT(element != NULL && new_element != NULL);

		Node* node = NodeSelector::GetNode(element);

		Element* prev = node->GetPrev();

		node->SetPrev(new_element);

		if (element == this->head)
			this->head = new_element;
		else
			NodeSelector::GetNode(prev)->SetNext(new_element);

		node = NodeSelector::GetNode(new_element);

		node->SetNext(element);
		node->SetPrev(prev);
	}

	void RemoveTail()
	{
		B_ASSERT(this->tail != NULL);

		if ((this->tail =
			NodeSelector::GetNode(this->tail)->GetPrev()) == NULL)
			this->head = NULL;
	}

	void Remove(Element* element)
	{
		B_ASSERT(this->head != NULL && element != NULL);

		Node* node = NodeSelector::GetNode(element);

		Element* prev = node->GetPrev();
		Element* next = node->GetNext();

		if (prev != NULL)
			NodeSelector::GetNode(prev)->SetNext(next);
		else
			this->head = next;

		if (next != NULL)
			NodeSelector::GetNode(next)->SetPrev(prev);
		else
			this->tail = prev;
	}

	void MoveToHead(Element* element)
	{
		B_ASSERT(this->head != NULL && element != NULL);

		Node* node = NodeSelector::GetNode(element);

		Element* prev = node->GetPrev();

		if (prev != NULL)
		{
			Element* next = node->GetNext();

			NodeSelector::GetNode(prev)->SetNext(next);

			if (next != NULL)
				NodeSelector::GetNode(next)->SetPrev(prev);
			else
				this->tail = prev;

			node->SetNext(this->head);
			node->SetPrev(NULL);

			NodeSelector::GetNode(this->head)->SetPrev(element);
			this->head = element;
		}
	}

	void MoveToTail(Element* element)
	{
		B_ASSERT(this->tail != NULL && element != NULL);

		Node* node = NodeSelector::GetNode(element);

		Element* next = node->GetNext();

		if (next != NULL)
		{
			Element* prev = node->GetPrev();

			NodeSelector::GetNode(next)->SetPrev(prev);

			if (prev != NULL)
				NodeSelector::GetNode(prev)->SetNext(next);
			else
				this->head = next;

			node->SetNext(NULL);
			node->SetPrev(this->tail);

			NodeSelector::GetNode(this->tail)->SetNext(element);
			this->tail = element;
		}
	}
};

B_END_NAMESPACE

#endif // B_DOUBLYLINKEDLIST_H
