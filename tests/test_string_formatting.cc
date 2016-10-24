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

#include <b/custom_exception.h>

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

B_TEST_CASE(test_int_width_and_precision)
{
	B_CHECK(b::string::formatted("[%2.1d]", 100) == "[100]");
	B_CHECK(b::string::formatted("[%4.3d]", 100) == "[ 100]");
	B_CHECK(b::string::formatted("[%4.4d]", 100) == "[0100]");
	B_CHECK(b::string::formatted("[%5.4d]", 100) == "[ 0100]");
}

B_TEST_CASE(sign_is_required)
{
	B_CHECK(b::string::formatted("[% d]", 1) == "[ 1]");
	B_CHECK(b::string::formatted("[% 2d]", 1) == "[ 1]");
	B_CHECK(b::string::formatted("[% 3d]", 1) == "[  1]");

	B_CHECK(b::string::formatted("[%+d]", 1) == "[+1]");
	B_CHECK(b::string::formatted("[%+2d]", 1) == "[+1]");
	B_CHECK(b::string::formatted("[%+3d]", 1) == "[ +1]");

	B_CHECK(b::string::formatted("[%+d]", 0) == "[ 0]");
	B_CHECK(b::string::formatted("[%+2d]", 0) == "[ 0]");
	B_CHECK(b::string::formatted("[%+3d]", 0) == "[  0]");

	B_CHECK(b::string::formatted("[%+4.2d]", 0) == "[  00]");
	B_CHECK(b::string::formatted("[%+4.2d]", 1) == "[ +01]");
	B_CHECK(b::string::formatted("[% 4.2d]", 1) == "[  01]");
}

B_TEST_CASE(test_zero_precision)
{
	B_CHECK(b::string::formatted("[%3.0d]", -100) == "[-100]");
	B_CHECK(b::string::formatted("[%4.0d]", -100) == "[-100]");
	B_CHECK(b::string::formatted("[%.0d]", -100) == "[-100]");

	B_CHECK(b::string::formatted("[%3.0d]", 100) == "[100]");
	B_CHECK(b::string::formatted("[%4.0d]", 100) == "[ 100]");
	B_CHECK(b::string::formatted("[%.0d]", 100) == "[100]");

	B_CHECK(b::string::formatted("[%3.0u]", 100) == "[100]");
	B_CHECK(b::string::formatted("[%4.0u]", 100) == "[ 100]");
	B_CHECK(b::string::formatted("[%.0u]", 100) == "[100]");

	B_CHECK(b::string::formatted("[%.0d]", 0) == "[]");
	B_CHECK(b::string::formatted("[%.*d]", 0, 0) == "[]");

	B_CHECK(b::string::formatted("[%+.0d]", 0) == "[]");
	B_CHECK(b::string::formatted("[% .0d]", 0) == "[]");
	B_CHECK(b::string::formatted("[%+1.0d]", 0) == "[ ]");
	B_CHECK(b::string::formatted("[% 1.0d]", 0) == "[ ]");

	B_CHECK(b::string::formatted("[%.0u]", 0) == "[]");
	B_CHECK(b::string::formatted("[%.*u]", 0, 0) == "[]");
}

struct static_buffer_allocator : public b::allocator
{
	static_buffer_allocator(char* pre_allocated, size_t size) :
		buffer(pre_allocated),
		buffer_size(size)
	{
	}

	virtual void* allocate(size_t size);

	char* buffer;
	size_t buffer_size;
};

void* static_buffer_allocator::allocate(size_t size)
{
	if (size > buffer_size)
		throw b::custom_exception("Pre-allocated buffer "
			"is too small for the requested data size.");

	return buffer;
}

B_TEST_CASE(test_custom_allocator)
{
	static char buffer[1024];

	static_buffer_allocator custom_allocator(buffer, sizeof(buffer));

	b::string_view formatted = b::format_buffer(&custom_allocator,
		"an int (%d) and a string (%s)", 128, "test string");

	B_CHECK(formatted.data() == buffer);
	B_CHECK(formatted.length() == 39);
}

B_TEST_CASE(test_percent_sign_escaping)
{
	b::string s;

	s.format("%d%%", 99);

	B_CHECK(s == "99%");

	s.format("%d%% done", 100);

	B_CHECK(s == "100% done");
}
