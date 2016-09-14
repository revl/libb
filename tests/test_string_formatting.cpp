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

#include <b/string.h>

#include "unit_test.h"

B_TEST_CASE(test_int_conversions)
{
	size_t len = b::format_string(NULL, "int: %d and int: %d", 0, 100);

	B_CHECK(len == 19);

	b::string s;

	s.format("verbatim %d verbatim", 123);
	printf("%s\n", s.c_str());

	B_CHECK(s == "verbatim 123 verbatim");
}
