// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/set.h>

#include "test_case.h"

template class b::set<int>;

typedef b::set<int> int_set;

B_TEST_CASE(construction)
{
	int_set s;

	B_CHECK(s.is_empty());
	B_CHECK(s.size() == 0);

	bool new_element;

	int* v = s.insert(6, &new_element);

	B_CHECK(new_element == true);
	B_CHECK(s.size() == 1);
	B_CHECK(*v == 6);

	int_set::search_result sr = s.search(6);

	v = sr.match();

	B_REQUIRE(v != NULL);
	B_CHECK(*v == 6);

	v = s.find(6);

	B_REQUIRE(v != NULL);
	B_CHECK(*v == 6);

	v = s.insert(6, &new_element);

	B_CHECK(new_element == false);
	B_CHECK(s.size() == 1);
	B_CHECK(*v == 6);

	sr = s.search(10);

	B_REQUIRE(sr.match() == NULL);
	B_CHECK(*s.insert_new(10, sr) == 10);

	B_CHECK(s.remove(10));
	B_CHECK(!s.remove(10));
	B_CHECK(!s.remove(11));
}

B_TEST_CASE(iteration)
{
	int_set s;

	B_CHECK(*s.insert(20) == 20);
	B_CHECK(*s.insert(40) == 40);
	B_CHECK(*s.insert(20) == 20);
	B_CHECK(*s.insert(30) == 30);
	B_CHECK(*s.insert(10) == 10);

	int* element = s.first();

	for (int v = 10; v <= 40; v += 10)
	{
		B_CHECK(*element == v);

		element = int_set::next(element);
	}

	B_CHECK(element == NULL);

	element = s.last();

	for (int v = 40; v >= 10; v -= 10)
	{
		B_CHECK(*element == v);

		element = int_set::prev(element);
	}

	B_CHECK(element == NULL);
}
