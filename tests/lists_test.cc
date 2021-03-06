// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/doubly_linked_list_container.h>
#include <b/node_access_via_cast.h>

#include <b/object.h>

#include "test_case.h"

class test_element;

typedef b::linked_list_node<test_element> list_node;

class asc_list_node : public list_node
{
};

class desc_list_node : public list_node
{
};

static int object_count = 0;

class test_element : public b::object,
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

typedef b::linked_list<asc_list_node_access> asc_test_element_list;
typedef b::linked_list<desc_list_node_access> desc_test_element_list;

B_TEST_CASE(object_lists)
{
	asc_test_element_list asc_list = asc_list_node_access();
	desc_test_element_list desc_list = desc_list_node_access();

	test_element* element = new test_element(2);

	asc_list.insert_first(element);
	desc_list.append(element);
	element->add_ref();

	element = new test_element(3);

	asc_list.append(element);
	desc_list.insert_first(element);
	element->add_ref();

	element = new test_element(1);

	asc_list.insert_first(element);
	desc_list.append(element);
	element->add_ref();

	int i = 0;

	element = asc_list.first();

	do
		B_CHECK(element->value == ++i);
	while ((element = asc_list.next(element)) != NULL);

	B_CHECK((element = desc_list.first())->value == i);

	do
		B_CHECK((element = desc_list.next(element))->value == --i);
	while (element != desc_list.last());

	desc_list.move_to_back(desc_list.first(), NULL);
	desc_list.move_to_front(desc_list.next(desc_list.first()),
		desc_list.first());

	element = desc_list.first();

	while (!asc_list.is_empty())
	{
		B_CHECK(asc_list.first()->value == element->value);

		asc_list.remove_first();

		test_element* next = desc_list.next(element);
		element->release();
		element = next;
	}

	desc_list.empty();

	B_CHECK(object_count == 0);
}

typedef b::doubly_linked_list_container<int> integer_list;

B_TEST_CASE(integer_lists)
{
	integer_list list;

	int* element = list.insert_first(2);
	list.insert_after(element, 3);
	list.insert_first(1);

	element = list.first();

	int i = 0;

	do
		B_CHECK(*element == ++i);
	while ((element = list.next(element)) != NULL);
}
