// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#ifndef B_DOUBLY_LINKED_LIST_H
#define B_DOUBLY_LINKED_LIST_H

#include "linked_list.h"

B_BEGIN_NAMESPACE

// A structure to implant into the linked objects themselves
// to string them together in a bidirectional list.
template <class T>
class doubly_linked_list_node : public linked_list_node<T>
{
public:
	typedef T element_type;

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

protected:
	element_type* prev_element;
};

// Bidirectional linked list.
template <class Node_access>
class doubly_linked_list : public linked_list<Node_access>
{
public:
	typedef typename Node_access::element_type element_type;
	typedef typename Node_access::node_type node_type;

	doubly_linked_list(const Node_access& node_access_inst) :
		linked_list<Node_access>(node_access_inst)
	{
	}

	static element_type* prev(element_type* element)
	{
		return Node_access::node_for(element)->prev();
	}

	static const element_type* prev(const element_type* element)
	{
		return Node_access::node_for(element)->prev();
	}

	void insert_first(element_type* new_element)
	{
		B_ASSERT(new_element != NULL);

		node_type* node = Node_access::node_for(new_element);

		node->set_next(this->first_element);
		node->set_prev(NULL);

		if (this->first_element != NULL)
			Node_access::node_for(
				this->first_element)->set_prev(new_element);
		else
			this->last_element = new_element;

		this->first_element = new_element;
	}

	void append(element_type* new_element)
	{
		B_ASSERT(new_element != NULL);

		node_type* node = Node_access::node_for(new_element);

		node->set_next(NULL);
		node->set_prev(this->last_element);

		if (this->last_element != NULL)
			Node_access::node_for(
				this->last_element)->set_next(new_element);
		else
			this->first_element = new_element;

		this->last_element = new_element;
	}

	void insert_after(element_type* element, element_type* new_element)
	{
		B_ASSERT(element != NULL && new_element != NULL);

		node_type* node = Node_access::node_for(element);

		element_type* next_element = node->next();

		node->set_next(new_element);

		if (element == this->last_element)
			this->last_element = new_element;
		else
			Node_access::node_for(
				next_element)->set_prev(new_element);

		node = Node_access::node_for(new_element);

		node->set_next(next_element);
		node->set_prev(element);
	}

	void insert_before(element_type* element, element_type* new_element)
	{
		B_ASSERT(element != NULL && new_element != NULL);

		node_type* node = Node_access::node_for(element);

		element_type* prev_element = node->prev();

		node->set_prev(new_element);

		if (element == this->first_element)
			this->first_element = new_element;
		else
			Node_access::node_for(
				prev_element)->set_next(new_element);

		node = Node_access::node_for(new_element);

		node->set_next(element);
		node->set_prev(prev_element);
	}

	void remove_last()
	{
		B_ASSERT(this->last_element != NULL);

		if ((this->last_element = Node_access::node_for(
				this->last_element)->prev()) == NULL)
			this->first_element = NULL;
	}

	void remove(element_type* element)
	{
		B_ASSERT(this->first_element != NULL && element != NULL);

		node_type* node = Node_access::node_for(element);

		element_type* prev_element = node->prev();
		element_type* next_element = node->next();

		if (prev_element != NULL)
			Node_access::node_for(
				prev_element)->set_next(next_element);
		else
			this->first_element = next_element;

		if (next_element != NULL)
			Node_access::node_for(
				next_element)->set_prev(prev_element);
		else
			this->last_element = prev_element;
	}

	void move_to_front(element_type* element)
	{
		B_ASSERT(this->first_element != NULL && element != NULL);

		node_type* node = Node_access::node_for(element);

		element_type* prev_element = node->prev();

		if (prev_element != NULL)
		{
			element_type* next_element = node->next();

			Node_access::node_for(
				prev_element)->set_next(next_element);

			if (next_element != NULL)
				Node_access::node_for(next_element)->
					set_prev(prev_element);
			else
				this->last_element = prev_element;

			node->set_next(this->first_element);
			node->set_prev(NULL);

			Node_access::node_for(
				this->first_element)->set_prev(element);
			this->first_element = element;
		}
	}

	void move_to_back(element_type* element)
	{
		B_ASSERT(this->last_element != NULL && element != NULL);

		node_type* node = Node_access::node_for(element);

		element_type* next_element = node->next();

		if (next_element != NULL)
		{
			element_type* prev_element = node->prev();

			Node_access::node_for(
				next_element)->set_prev(prev_element);

			if (prev_element != NULL)
				Node_access::node_for(prev_element)->
					set_next(next_element);
			else
				this->first_element = next_element;

			node->set_next(NULL);
			node->set_prev(this->last_element);

			Node_access::node_for(
				this->last_element)->set_next(element);
			this->last_element = element;
		}
	}
};

B_END_NAMESPACE

#endif /* !defined(B_DOUBLY_LINKED_LIST_H) */
