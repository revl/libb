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

#include <b/string_view.h>

#include "unit_test.h"

B_STATIC_CONST_STRING(static_const_string, "static_const_string");

B_STATIC_CONST_STRING(padded, "abcxxxcba");

B_TEST_CASE(test_string_view)
{
	b::string_view sv1;

	B_CHECK(sv1.is_empty());
	B_CHECK(sv1 == "");
	B_CHECK(sv1 == b::string_view());

	b::string_view sv2(static_const_string);

	B_CHECK(sv2.length() == static_const_string.length());
	B_CHECK(sv2 == static_const_string);

	sv1.assign("1234", 3);

	B_CHECK(sv1.length() == 3);
	B_CHECK(sv1 == "123");
	B_CHECK(sv1 < "1234");

	B_CHECK(b::string_view("123a", 3) == b::string_view("123b", 3));

	sv2 = b::string_view(padded);

	size_t b_index = sv2.find('b');

	B_REQUIRE(b_index == 1);
	B_CHECK(sv2[b_index] == 'b');

	b_index = sv2.rfind('b');

	B_REQUIRE(b_index == 7);
	B_CHECK(sv2[b_index] == 'b');

	sv2.trim("abc");

	B_CHECK(sv2 == "xxx");

	B_CHECK(sv2 > padded);
}
