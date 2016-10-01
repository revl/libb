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
		Node_access(node_access_inst),
		first_element(NULL),
		last_element(NULL)
	{
	}

// Accessors
public:
	element_type* first()
	{
		return first_element;
	}

	const element_type* first() const
	{
		return first_element;
	}

	element_type* last()
	{
		return last_element;
	}

	const element_type* last() const
	{
		return last_element;
	}

	bool is_empty() const
	{
		return first_element == NULL;
	}

// Operations
public:
	static element_type* next(element_type* element)
	{
		return Node_access::node_for(element)->next();
	}

	static const element_type* next(const element_type* element)
	{
		return Node_access::node_for(element)->next();
	}

	void insert_first(element_type* new_element)
	{
		B_ASSERT(new_element != NULL);

		Node_access::node_for(new_element)->set_next(first_element);

		first_element = new_element;

		if (last_element == NULL)
			last_element = new_element;
	}

	void append(element_type* new_element)
	{
		B_ASSERT(new_element != NULL);

		Node_access::node_for(new_element)->set_next(NULL);

		if (last_element != NULL)
			Node_access::node_for(
				last_element)->set_next(new_element);
		else
			first_element = new_element;

		last_element = new_element;
	}

	void insert_after(element_type* element, element_type* new_element)
	{
		B_ASSERT(element != NULL && new_element != NULL);

		if (element == last_element)
			last_element = new_element;

		node_type* node = Node_access::node_for(element);

		Node_access::node_for(new_element)->set_next(node->next());

		node->set_next(new_element);
	}

	void remove_first()
	{
		B_ASSERT(first_element != NULL);

		if ((first_element = Node_access::node_for(
				first_element)->next()) == NULL)
			last_element = NULL;
	}

	void remove(element_type* element, element_type* prev_element)
	{
		B_ASSERT(first_element != NULL && element != NULL &&
			element == (prev_element == NULL ? first_element :
			Node_access::node_for(prev_element)->next()));

		element_type* next_element =
			Node_access::node_for(element)->next();

		if (prev_element != NULL)
			Node_access::node_for(
				prev_element)->set_next(next_element);
		else
			first_element = next_element;

		if (next_element == NULL)
			last_element = prev_element;
	}

	void remove_all()
	{
		last_element = first_element = NULL;
	}

	void move_to_front(element_type* element, element_type* prev_element)
	{
		B_ASSERT(first_element != NULL && element != NULL &&
			element == (prev_element == NULL ? first_element :
			Node_access::node_for(prev_element)->next()));

		if (prev_element != NULL)
		{
			node_type* node = Node_access::node_for(element);

			element_type* next_element = node->next();

			Node_access::node_for(
				prev_element)->set_next(next_element);

			if (next_element == NULL)
				last_element = prev_element;

			node->set_next(first_element);

			first_element = element;
		}
	}

	void move_to_back(element_type* element, element_type* prev_element)
	{
		B_ASSERT(last_element != NULL && element != NULL &&
			element == (prev_element == NULL ? first_element :
			Node_access::node_for(prev_element)->next()));

		node_type* node = Node_access::node_for(element);

		element_type* next_element = node->next();

		if (next_element != NULL)
		{
			if (prev_element != NULL)
				Node_access::node_for(
					prev_element)->set_next(next_element);
			else
				first_element = node->next();

			node->set_next(NULL);

			Node_access::node_for(last_element)->set_next(element);
			last_element = element;
		}
	}

// Implementation
public:
	~linked_list()
	{
		remove_all();
	}

protected:
	element_type* first_element;
	element_type* last_element;
};

B_END_NAMESPACE

#endif /* !defined(B_LINKED_LIST_H) */
