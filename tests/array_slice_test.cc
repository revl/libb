// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/array_slice.h>

#include "test_case.h"

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

	b::array_slice<unsigned> first_3 = first_5.without_last_n(2);

	B_CHECK(first_3.length() == 3);
	B_CHECK(first_3.data() == digits);
	B_CHECK(first_3[2] == 2);

	b::array_slice<unsigned> last_5 = slice.last_n(5);

	B_CHECK(last_5.length() == 5);
	B_CHECK(last_5.data() == digits + 5);
	B_CHECK(last_5[4] == 9);

	b::array_slice<unsigned> last_3 = last_5.without_first_n(2);

	B_CHECK(last_3.length() == 3);
	B_CHECK(last_3.data() == digits + 7);
	B_CHECK(last_3[2] == 9);
}

B_TEST_CASE(join)
{
	B_STRING_LITERAL(one, "one");
	B_STRING_LITERAL(two, "two");
	B_STRING_LITERAL(three, "three");

	const b::string one_two_three[] = {one, two, three};

	b::array_slice<b::string> slice(one_two_three, 3);

	B_CHECK(slice.join('+') == "one+two+three");
}
