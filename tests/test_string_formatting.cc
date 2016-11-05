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

#include <limits.h>

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

B_TEST_CASE(zero_precision)
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

	B_CHECK(b::string::formatted("[%.0o]", 0) == "[]");
	B_CHECK(b::string::formatted("[%#.0o]", 0) == "[]");

	B_CHECK(b::string::formatted("[%.0X]", 0x0) == "[]");
	B_CHECK(b::string::formatted("[%#.0X]", 0x0) == "[]");
}

B_TEST_CASE(hexadecimal_conversion)
{
	B_CHECK(b::string::formatted("[%#5.4X]", 0xA) == "[0x000A]");
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

#define B_DEC_BUF_LEN (sizeof(long) * 3)

static b::string int_to_str(unsigned long n, unsigned base = 10)
{
	static const char hex_digits[17] = "0123456789ABCDEF";

	char buffer[B_DEC_BUF_LEN];

	char* const buffer_end = buffer + sizeof(buffer) / sizeof(*buffer);
	char* ptr = buffer_end;

	do
	{
		*--ptr = hex_digits[n % base];
		n /= base;
	}
	while (n > 0);

	return b::string(ptr, buffer_end - ptr);
}

B_STATIC_CONST_STRING(minus, "-");
B_STATIC_CONST_STRING(zero, "0");
B_STATIC_CONST_STRING(hex_prefix, "0x");

B_TEST_CASE(min_max_numbers)
{
	B_CHECK(b::string::formatted("%lu", ULONG_MAX) ==
		int_to_str(ULONG_MAX));

	B_CHECK(b::string::formatted("%ld", LONG_MAX) ==
		int_to_str((unsigned long) LONG_MAX));
	B_CHECK(b::string::formatted("%ld", LONG_MIN) == minus +
		int_to_str((unsigned long) -LONG_MIN));

	B_CHECK(b::string::formatted("%u", UINT_MAX) == int_to_str(UINT_MAX));

	B_CHECK(b::string::formatted("%d", INT_MAX) ==
		int_to_str((unsigned) INT_MAX));
	B_CHECK(b::string::formatted("%d", INT_MIN) == minus +
		int_to_str((unsigned) -INT_MIN));

	B_CHECK(b::string::formatted("%hu", UINT_MAX) == "65535");

	B_CHECK(b::string::formatted("%hd", 0x7FFF) == "32767");
	B_CHECK(b::string::formatted("%hd", 0x8000) == "-32768");

	B_CHECK(b::string::formatted("%#lX", ULONG_MAX) == hex_prefix +
		int_to_str(ULONG_MAX, 16));
	B_CHECK(b::string::formatted("%#X", UINT_MAX) == hex_prefix +
		int_to_str(UINT_MAX, 16));
	B_CHECK(b::string::formatted("%#hX", 0xFFFF) == "0xFFFF");

	B_CHECK(b::string::formatted("%#lo", ULONG_MAX) == zero +
		int_to_str(ULONG_MAX, 8));
	B_CHECK(b::string::formatted("%#o", UINT_MAX) == zero +
		int_to_str(UINT_MAX, 8));
	B_CHECK(b::string::formatted("%#ho", 0177777) == "0177777");
}

static b::string spaces(size_t len)
{
	return b::string_view(" ", 1).repeat(len);
}

static b::string zeros(size_t len)
{
	return b::string_view("0", 1).repeat(len);
}

B_STATIC_CONST_STRING(opening_bracket, "[");
B_STATIC_CONST_STRING(closing_bracket, "]");

template <class T>
static b::string format(const b::string& flags, unsigned width,
	unsigned precision, const b::string& conversion, T value)
{
	b::string result(opening_bracket);

	b::string format_str = b::string('%', 1) + flags +
		int_to_str(width) + '.' + int_to_str(precision) +
		conversion + closing_bracket;

	result.append_format(format_str.data(), value);

	return result;
}

B_STATIC_CONST_STRING(ld_conv, "ld");

B_STATIC_CONST_STRING(space, " ");
B_STATIC_CONST_STRING(plus, "+");

B_TEST_CASE(decimal_conversions)
{
	const unsigned width = B_DEC_BUF_LEN + 10;

	b::string formatted = format(b::string(), width, 4, ld_conv, LONG_MAX);

	b::string long_max_str = int_to_str(LONG_MAX);

	b::string expected = opening_bracket +
		spaces(width - long_max_str.length()) + long_max_str +
		closing_bracket;

	B_CHECK(formatted == expected);

	formatted = format(space, width, 4, ld_conv, LONG_MAX);

	B_CHECK(formatted == expected);

	formatted = format(plus, width, 4, ld_conv, LONG_MAX);

	expected = opening_bracket +
		spaces(width - long_max_str.length() - 1) + plus +
		long_max_str + closing_bracket;

	B_CHECK(formatted == expected);

	formatted = format(plus + zero, width, 4, ld_conv, LONG_MAX);

	expected = opening_bracket + plus +
		zeros(width - long_max_str.length() - 1) +
		long_max_str + closing_bracket;

	B_CHECK(formatted == expected);
}
