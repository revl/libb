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

#ifndef B_LINKED_LIST_H
#define B_LINKED_LIST_H

#include "host.h"

B_BEGIN_NAMESPACE

template <class T>
class linked_list_node
{
// Type
public:
	typedef T element_type;

// Accessors
public:
	element_type* next()
	{
		return next_element;
	}

	const element_type* next() const
	{
		return next_element;
	}

	void set_next(element_type* new_next)
	{
		next_element = new_next;
	}

// Implementation
protected:
	element_type* next_element;
};

template <class Node_access>
class linked_list : public Node_access
{
// Types
public:
	typedef typename Node_access::element_type element_type;

	typedef typename Node_access::node_type node_type;

// Construction
public:
	linked_list(const Node_access& node_access_inst) :
		Node_access(node_access_inst), head(NULL), tail(NULL)
	{
	}

// Attributes
public:
	element_type* GetHead()
	{
		return head;
	}

	const element_type* GetHead() const
	{
		return head;
	}

	element_type* GetTail()
	{
		return tail;
	}

	const element_type* GetTail() const
	{
		return tail;
	}

	bool IsEmpty() const
	{
		return head == NULL;
	}

// Operations
public:
	static element_type* next(element_type* element)
	{
		return GetNode(element)->next();
	}

	static const element_type* next(const element_type* element)
	{
		return GetNode(element)->next();
	}

	void AddHead(element_type* new_element)
	{
		B_ASSERT(new_element != NULL);

		GetNode(new_element)->set_next(head);

		head = new_element;

		if (tail == NULL)
			tail = new_element;
	}

	void AddTail(element_type* new_element)
	{
		B_ASSERT(new_element != NULL);

		GetNode(new_element)->set_next(NULL);

		if (tail != NULL)
			GetNode(tail)->set_next(new_element);
		else
			head = new_element;

		tail = new_element;
	}

	void InsertAfter(element_type* element, element_type* new_element)
	{
		B_ASSERT(element != NULL && new_element != NULL);

		if (element == tail)
			tail = new_element;

		node_type* node = GetNode(element);

		GetNode(new_element)->set_next(node->next());

		node->set_next(new_element);
	}

	void RemoveHead()
	{
		B_ASSERT(head != NULL);

		if ((head = GetNode(head)->next()) == NULL)
			tail = NULL;
	}

	void Remove(element_type* element, element_type* prev)
	{
		B_ASSERT(head != NULL && element != NULL &&
			element == (prev == NULL ? head :
			GetNode(prev)->next()));

		element_type* next = GetNode(element)->next();

		if (prev != NULL)
			GetNode(prev)->set_next(next);
		else
			head = next;

		if (next == NULL)
			tail = prev;
	}

	void RemoveAll()
	{
		tail = head = NULL;
	}

	void MoveToHead(element_type* element, element_type* prev)
	{
		B_ASSERT(head != NULL && element != NULL &&
			element == (prev == NULL ? head :
			GetNode(prev)->next()));

		if (prev != NULL)
		{
			node_type* node = GetNode(element);

			element_type* next = node->next();

			GetNode(prev)->set_next(next);

			if (next == NULL)
				tail = prev;

			node->set_next(head);

			head = element;
		}
	}

	void MoveToTail(element_type* element, element_type* prev)
	{
		B_ASSERT(tail != NULL && element != NULL &&
			element == (prev == NULL ? head :
			GetNode(prev)->next()));

		node_type* node = GetNode(element);

		element_type* next = node->next();

		if (next != NULL)
		{
			if (prev != NULL)
				GetNode(prev)->set_next(next);
			else
				head = node->next();

			node->set_next(NULL);

			GetNode(tail)->set_next(element);
			tail = element;
		}
	}

// Implementation
public:
	~linked_list()
	{
		RemoveAll();
	}

protected:
	static node_type* GetNode(element_type* element)
	{
		return Node_access::node_for(element);
	}

	static const node_type* GetNode(const element_type* element)
	{
		return Node_access::node_for(
			const_cast<element_type*>(element));
	}

	element_type* head;
	element_type* tail;
};

B_END_NAMESPACE

#endif /* !defined(B_LINKED_LIST_H) */
