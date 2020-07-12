// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/map.h>

#include "test_case.h"

template class b::map<int, int>;

B_TEST_CASE(construction)
{
	typedef b::map<int, int> int_map;

	int_map m;

	B_CHECK(m.is_empty());
	B_CHECK(m.size() == 0);

	bool new_element;

	b::kv_pair<int, int>* kv = m.insert(6, 6, &new_element);

	B_CHECK(new_element == true);
	B_CHECK(m.size() == 1);
	B_CHECK(kv->key == 6);
	B_CHECK(kv->value == 6);

	int_map::search_result sr = m.search(6);

	B_REQUIRE(sr.match() != NULL);
	B_CHECK(sr.match()->value == 6);

	int* v = m.find(6);

	B_REQUIRE(v != NULL);
	B_CHECK(*v == 6);

	kv = m.insert(6, 7, &new_element);

	B_CHECK(new_element == false);
	B_CHECK(m.size() == 1);
	B_CHECK(kv->key == 6);
	B_CHECK(kv->value == 7);

	kv->value = 8;
	B_CHECK(*m.find(6) == 8);

	B_CHECK(m.remove(6));
	B_CHECK(!m.remove(6));
	B_CHECK(!m.remove(10));
}
