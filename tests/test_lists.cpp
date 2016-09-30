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

#include <b/doubly_linked_list_container.h>
#include <b/node_access_via_cast.h>

#include <b/object.h>

#include "unit_test.h"

class test_element;

typedef b::linked_list_node<test_element> test_element_list_node;

class asc_list_node : public test_element_list_node
{
};

class desc_list_node : public test_element_list_node
{
};

static int object_count = 0;

class test_element : public b::Object,
	public asc_list_node,
	public desc_list_node
{
public:
	test_element(int number);

	const int value;

	virtual ~test_element();
};

inline test_element::test_element(int number) : value(number)
{
	++object_count;
}

test_element::~test_element()
{
	--object_count;
}

typedef b::node_access_via_cast<asc_list_node> asc_list_node_access;
typedef b::node_access_via_cast<desc_list_node> desc_list_node_access;

typedef b::linked_list<asc_list_node_access> AscList;
typedef b::linked_list<desc_list_node_access> DescList;

B_TEST_CASE(test_object_lists)
{
	AscList asc_list = asc_list_node_access();
	DescList desc_list = desc_list_node_access();

	test_element* element = new test_element(2);

	asc_list.AddHead(element);
	desc_list.AddTail(element);
	element->AddRef();

	element = new test_element(3);

	asc_list.AddTail(element);
	desc_list.AddHead(element);
	element->AddRef();

	element = new test_element(1);

	asc_list.AddHead(element);
	desc_list.AddTail(element);
	element->AddRef();

	int i = 0;

	element = asc_list.GetHead();

	do
		B_CHECK(element->value == ++i);
	while ((element = asc_list.next(element)) != NULL);

	B_CHECK((element = desc_list.GetHead())->value == i);

	do
		B_CHECK((element = desc_list.next(element))->value == --i);
	while (element != desc_list.GetTail());

	desc_list.MoveToTail(desc_list.GetHead(), NULL);
	desc_list.MoveToHead(desc_list.next(desc_list.GetHead()),
		desc_list.GetHead());

	element = desc_list.GetHead();

	while (!asc_list.IsEmpty())
	{
		B_CHECK(asc_list.GetHead()->value == element->value);

		asc_list.RemoveHead();

		test_element* next = desc_list.next(element);
		element->Release();
		element = next;
	}

	desc_list.RemoveAll();

	B_CHECK(object_count == 0);
}

typedef b::doubly_linked_list_container<int> IntegerList;

B_TEST_CASE(test_integer_lists)
{
	IntegerList integer_list;

	int* element = integer_list.AddHead(2);
	integer_list.InsertAfter(element, 3);
	integer_list.AddHead(1);

	element = integer_list.GetHead();

	int i = 0;

	do
		B_CHECK(*element == ++i);
	while ((element = integer_list.next(element)) != NULL);
}
