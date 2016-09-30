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

#ifndef B_DOUBLY_LINKED_LIST_H
#define B_DOUBLY_LINKED_LIST_H

#include "linked_list.h"

B_BEGIN_NAMESPACE

template <class T>
class doubly_linked_list_node : public linked_list_node<T>
{
// Type
public:
	typedef T element_type;

// Accessors
public:
	element_type* prev()
	{
		return prev_element;
	}

	const element_type* prev() const
	{
		return prev_element;
	}

	void set_prev(element_type* new_prev)
	{
		prev_element = new_prev;
	}

// Implementation
protected:
	element_type* prev_element;
};

template <class Node_access>
class doubly_linked_list : public linked_list<Node_access>
{
// Types
public:
	typedef typename Node_access::element_type element_type;

	typedef typename Node_access::node_type node_type;

	typedef linked_list<Node_access> base;

// Operations
public:
	doubly_linked_list(const Node_access& node_access_inst) :
		linked_list<Node_access>(node_access_inst)
	{
	}

	static element_type* prev(element_type* element)
	{
		return base::GetNode(element)->prev();
	}

	static const element_type* prev(const element_type* element)
	{
		return base::GetNode(element)->prev();
	}

	void AddHead(element_type* new_element)
	{
		B_ASSERT(new_element != NULL);

		node_type* node = base::GetNode(new_element);

		node->set_next(this->head);
		node->set_prev(NULL);

		if (this->head != NULL)
			base::GetNode(this->head)->set_prev(new_element);
		else
			this->tail = new_element;

		this->head = new_element;
	}

	void AddTail(element_type* new_element)
	{
		B_ASSERT(new_element != NULL);

		node_type* node = base::GetNode(new_element);

		node->set_next(NULL);
		node->set_prev(this->tail);

		if (this->tail != NULL)
			base::GetNode(this->tail)->set_next(new_element);
		else
			this->head = new_element;

		this->tail = new_element;
	}

	void InsertAfter(element_type* element, element_type* new_element)
	{
		B_ASSERT(element != NULL && new_element != NULL);

		node_type* node = base::GetNode(element);

		element_type* next_element = node->next();

		node->set_next(new_element);

		if (element == this->tail)
			this->tail = new_element;
		else
			base::GetNode(next_element)->set_prev(new_element);

		node = base::GetNode(new_element);

		node->set_next(next_element);
		node->set_prev(element);
	}

	void InsertBefore(element_type* element, element_type* new_element)
	{
		B_ASSERT(element != NULL && new_element != NULL);

		node_type* node = base::GetNode(element);

		element_type* prev_element = node->prev();

		node->set_prev(new_element);

		if (element == this->head)
			this->head = new_element;
		else
			base::GetNode(prev_element)->set_next(new_element);

		node = base::GetNode(new_element);

		node->set_next(element);
		node->set_prev(prev_element);
	}

	void RemoveTail()
	{
		B_ASSERT(this->tail != NULL);

		if ((this->tail = base::GetNode(this->tail)->prev()) == NULL)
			this->head = NULL;
	}

	void Remove(element_type* element)
	{
		B_ASSERT(this->head != NULL && element != NULL);

		node_type* node = base::GetNode(element);

		element_type* prev_element = node->prev();
		element_type* next_element = node->next();

		if (prev_element != NULL)
			base::GetNode(prev_element)->set_next(next_element);
		else
			this->head = next_element;

		if (next_element != NULL)
			base::GetNode(next_element)->set_prev(prev_element);
		else
			this->tail = prev_element;
	}

	void MoveToHead(element_type* element)
	{
		B_ASSERT(this->head != NULL && element != NULL);

		node_type* node = base::GetNode(element);

		element_type* prev_element = node->prev();

		if (prev_element != NULL)
		{
			element_type* next_element = node->next();

			base::GetNode(prev_element)->set_next(next_element);

			if (next_element != NULL)
				base::GetNode(next_element)->
					set_prev(prev_element);
			else
				this->tail = prev_element;

			node->set_next(this->head);
			node->set_prev(NULL);

			base::GetNode(this->head)->set_prev(element);
			this->head = element;
		}
	}

	void MoveToTail(element_type* element)
	{
		B_ASSERT(this->tail != NULL && element != NULL);

		node_type* node = base::GetNode(element);

		element_type* next_element = node->next();

		if (next_element != NULL)
		{
			element_type* prev_element = node->prev();

			base::GetNode(next_element)->set_prev(prev_element);

			if (prev_element != NULL)
				base::GetNode(prev_element)->
					set_next(next_element);
			else
				this->head = next_element;

			node->set_next(NULL);
			node->set_prev(this->tail);

			base::GetNode(this->tail)->set_next(element);
			this->tail = element;
		}
	}
};

B_END_NAMESPACE

#endif /* !defined(B_DOUBLY_LINKED_LIST_H) */
