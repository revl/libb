/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2018 Damon Revoe
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

#include <b/set.h>

#include "unit_test.h"

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
