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

#include <b/map.h>

#include "unit_test.h"

B_TEST_CASE(construction)
{
	typedef b::map<int, int> int_map;

	int_map m;

	B_CHECK(m.is_empty());
	B_CHECK(m.size() == 0);

	bool new_element;

	int_map::key_value_pair* w = m.insert(6, 6, &new_element);

	B_CHECK(new_element == true);
	B_CHECK(m.size() == 1);
	B_CHECK(w->key == 6);
	B_CHECK(w->value == 6);

	int cmp_result;

	w = m.search(6, &cmp_result);

	B_REQUIRE(w != NULL);
	B_CHECK(cmp_result == 0);
	B_CHECK(w->value == 6);

	w = m.insert(6, 7, &new_element);

	B_CHECK(new_element == false);
	B_CHECK(m.size() == 1);
	B_CHECK(w->key == 6);
	B_CHECK(w->value == 7);
}
