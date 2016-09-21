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

struct save_length_allocator : public b::allocator
{
	virtual void* allocate(size_t size);

	size_t saved_length;
};

void* save_length_allocator::allocate(size_t size)
{
	saved_length = size / sizeof(wchar_t);

	return NULL;
}

B_TEST_CASE(test_int_conversions)
{
	save_length_allocator length_saver;

	b::format_buffer(&length_saver, L"int: %d and int: %d", 0, 100);

	B_CHECK(length_saver.saved_length == 19);

	b::string s;

	s.format("verbatim %d verbatim", 123);

	B_CHECK(s == "verbatim 123 verbatim");
}

B_TEST_CASE(test_percent_output)
{
	b::string s;

	s.format("%d%%", 99);

	B_CHECK(s == "99%");

	s.format("%d%% done", 100);

	B_CHECK(s == "100% done");
}
