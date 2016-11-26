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

#include <b/random.h>

#include "unit_test.h"

B_TEST_CASE(test_seed_1000)
{
	b::random seed_1000(1000);

	static const b::random::value_type values[] =
	{
		28322, 7943, 4653, 8133, 31660,
		22230, 1785, 18381, 6181, 10155
	};

	const size_t number_of_values = sizeof(values) / sizeof(*values);

	for (size_t i = 0; i < number_of_values; ++i)
		B_CHECK(values[i] == seed_1000.next());
}
