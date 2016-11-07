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

B_STATIC_CONST_STRING(space, " ");
B_STATIC_CONST_STRING(zero, "0");

template <class T>
static b::string format(T value, const char* conversion,
	const char* flags = NULL, ssize_t width = 0, int precision = -1)
{
	b::string result('[', 1);

	b::string format_str = b::string::formatted("%%");
	if (flags != NULL)
		format_str.append_format("%s", flags);
	if (width > 0)
		format_str.append_format("%u", (unsigned) width);
	if (precision >= 0)
		format_str.append_format(".%d", precision);
	format_str.append_format("%s", conversion);
	format_str.append_format("]");

	result.append_format(format_str.data(), value);

	return result;
}

static b::string expect(unsigned leading_spaces, const char* prefix,
	unsigned zeros, unsigned long value, unsigned base = 10,
	unsigned trailing_spaces = 0)
{
	return b::string::formatted("[%s%s%s%s%s]",
		space.repeat(leading_spaces).data(),
		prefix,
		zero.repeat(zeros).data(),
		int_to_str((unsigned long) value, base).data(),
		space.repeat(trailing_spaces).data());
}

B_TEST_CASE(min_max_numbers)
{
	B_CHECK(format(ULONG_MAX, "lu") == expect(0, "", 0, ULONG_MAX));

	B_CHECK(format(LONG_MAX, "ld") ==
		expect(0, "", 0, (unsigned long) LONG_MAX));
	B_CHECK(format(LONG_MIN, "ld") ==
		expect(0, "-", 0, (unsigned long) LONG_MIN));

	B_CHECK(format(UINT_MAX, "u") == expect(0, "", 0, UINT_MAX));

	B_CHECK(format(INT_MAX, "d") == expect(0, "", 0, (unsigned) INT_MAX));
	B_CHECK(format(INT_MIN, "d") == expect(0, "-", 0, (unsigned) INT_MIN));

	B_CHECK(format(UINT_MAX, "u", "h") == "[65535]");

	B_CHECK(format(0x7FFF, "d", "h") == "[32767]");
	B_CHECK(format(0x8000, "d", "h") == "[-32768]");

	B_CHECK(format(ULONG_MAX, "lX", "#") ==
		expect(0, "0x", 0, ULONG_MAX, 16));
	B_CHECK(format(UINT_MAX, "X", "#") == expect(0, "0x", 0, UINT_MAX, 16));
	B_CHECK(format(0xFFFF, "hX", "#") == "[0xFFFF]");

	B_CHECK(format(ULONG_MAX, "lo", "#") == expect(0, "", 1, ULONG_MAX, 8));
	B_CHECK(format(UINT_MAX, "lo", "#") == expect(0, "", 1, UINT_MAX, 8));
	B_CHECK(format(0177777, "ho", "#") == "[0177777]");
}

B_TEST_CASE(decimal_conversions)
{
	const unsigned long billion = 1000 * 1000 * 1000;

	B_CHECK(format(billion, "ld", "'") == "[1,000,000,000]");
	B_CHECK(format(billion, "ld", "'+") == "[+1,000,000,000]");
	B_CHECK(format(billion, "ld", "+'") == "[+1,000,000,000]");

	const size_t digits_in_billion = 10;

	B_CHECK(format(billion, "ld", "", 0,
		digits_in_billion + 2) == "[001000000000]");

	B_CHECK(format(billion, "ld", "", digits_in_billion + 4,
		digits_in_billion + 2) == "[  001000000000]");

	const size_t width = B_DEC_BUF_LEN + 10;

	B_CHECK(format(billion, "ld", NULL, width, 4) ==
		expect(width - digits_in_billion, "", 0, billion));

	B_CHECK(format(billion, "ld", " ", width, 4) ==
		expect(width - digits_in_billion, "", 0, billion));

	B_CHECK(format(billion, "ld", "+", width, 4) ==
		expect(width - digits_in_billion - 1, "+", 0, billion));

	B_CHECK(format(billion, "ld", "+0", width, 4) ==
		expect(width - digits_in_billion - 1, "+", 0, billion));

	B_CHECK(format(1000, "d", "+", 100, 3) == expect(95, "+", 0, 1000));
	B_CHECK(format(1000, "d", " ", 100, 3) == expect(96, "", 0, 1000));
	B_CHECK(format(-1000, "d", "+", 100, 3) == expect(95, "-", 0, 1000));

	B_CHECK(format(-1000, "d", "", 234, 100) == expect(133, "-", 96, 1000));
	B_CHECK(format(1000, "d", "+", 234, 100) == expect(133, "+", 96, 1000));
	B_CHECK(format(1000, "d", " ", 234, 100) == expect(134, "", 96, 1000));
	B_CHECK(format(1000, "d", "+0", 234) == expect(0, "+", 229, 1000));
	B_CHECK(format(1000, "d", "+0", 234, 14) == expect(219, "+", 10, 1000));

	B_CHECK(format(1000, "u", "", 234, 100) == expect(134, "", 96, 1000));
	B_CHECK(format(1000, "u", "0", 234) == expect(0, "", 230, 1000));
}
