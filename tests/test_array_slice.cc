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

#include <b/array_slice.h>

#include "unit_test.h"

static unsigned digits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

template class b::array_slice<unsigned>;

B_TEST_CASE(basic_checks)
{
	b::array_slice<unsigned> slice1;

	B_CHECK(slice1.is_empty());
	B_CHECK(slice1.length() == 0);

	slice1.assign(digits, 5);

	B_CHECK(slice1.length() == 5);
	B_CHECK(slice1.data() == digits);

	B_CHECK(slice1[3] == 3);

	b::array_slice<unsigned> slice2(digits, 10);

	B_CHECK(slice2.length() == 10);
	B_CHECK(slice2.data() == digits);

	B_CHECK(slice2[9] == 9);
}

B_TEST_CASE(subslices)
{
	b::array_slice<unsigned> slice(digits, 10);

	b::array_slice<unsigned> subslice = slice.slice(5, 2);

	B_CHECK(subslice.length() == 2);
	B_CHECK(subslice.data() == digits + 5);
	B_CHECK(subslice[0] == 5);
	B_CHECK(subslice[1] == 6);

	b::array_slice<unsigned> first_5 = slice.first_n(5);

	B_CHECK(first_5.length() == 5);
	B_CHECK(first_5.data() == digits);
	B_CHECK(first_5[4] == 4);

	b::array_slice<unsigned> last_5 = slice.last_n(5);

	B_CHECK(last_5.length() == 5);
	B_CHECK(last_5.data() == digits + 5);
	B_CHECK(last_5[4] == 9);
}
