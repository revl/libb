// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/string.h>

#include <stdint.h>

namespace
{
	class conversion_exception : public b::runtime_exception
	{
		virtual b::string message() const;
	};

	b::string conversion_exception::message() const
	{
		B_STRING_LITERAL(error_message,
			"multibyte to/from wchar conversion error");

		return error_message;
	}
}

#if defined(va_copy)
#define B_VA_COPY(dest_list, src_list) va_copy(dest_list, src_list)
#define B_VA_COPY_END(list) va_end(list)
#elif defined(__va_copy)
#define B_VA_COPY(dest_list, src_list) __va_copy(dest_list, src_list)
#define B_VA_COPY_END(list) va_end(list)
#else
#error Must implement B_VA_COPY for this platform
#endif

// Maximum number of characters required to print an unsigned integer.
// A binary integer type of size S bytes takes up to
// L = ceil(S * log10(256)) digits when represented in the
// decimal numeral system. A convenient approximation of
// log10(256) is 2.5: L = ceil((S * 5) / 2).
// The result is multiplied by 4/3 to accommodate for
// the thousand dividers.
#define MAX_UNSIGNED_BUF_LEN(type) ((((sizeof(type) * 5 + 1) / 2)) * 4 / 3)

// Maximum number of characters required to print
// an integer of the specified type.
// An additional byte is reserved for the sign.
#define MAX_DECIMAL_BUF_LEN(type) (MAX_UNSIGNED_BUF_LEN(type) + 1)

// Maximum number of characters required to represent an integer
// in the octal numeral system.
#define MAX_OCTAL_BUF_LEN(type) ((sizeof(type) * 8 + 2) / 3)

// Maximum number of characters required to represent an integer
// in the hexadecimal numeral system.
#define MAX_HEX_BUF_LEN(type) (sizeof(type) * 2)

// The number of bits in an integer type.
#define MAX_BINARY_BUF_LEN(type) (sizeof(type) * 8)

#define string wstring
#define char_t wchar_t
#define B_L_PREFIX(ch) L##ch
#define string_view wstring_view
#define string_allocator wstring_allocator
#include "string_impl.h"
#include "string_view_impl.h"
#undef string_allocator
#define string_formatting wstring_formatting
#include "string_formatting.h"
#undef string_formatting
#undef string_view
#undef B_L_PREFIX
#undef char_t
#undef string

#define char_t char
#define B_L_PREFIX(ch) ch
#include "string_impl.h"
#include "string_view_impl.h"
#include "string_formatting.h"
#undef B_L_PREFIX
#undef char_t

B_BEGIN_NAMESPACE

string string::from_wstring(const wstring& wstr)
{
	string mbstr;

	size_t cap = extra_capacity(::wcstombs(NULL, wstr.data(), 0) + 1);

	mbstr.discard_and_alloc(cap);

	size_t result_len = ::wcstombs(mbstr.lock(), wstr.data(), cap);

	if (result_len == (size_t) -1)
		throw conversion_exception();

	mbstr.unlock(result_len);

	return mbstr;
}

wstring string::to_wstring() const
{
	wstring wstr;

	size_t cap = extra_capacity(::mbstowcs(NULL, data(), 0) + 1);

	wstr.discard_and_alloc(cap);

	size_t result_len = ::mbstowcs(wstr.lock(), data(), cap);

	if (result_len == (size_t) -1)
		throw conversion_exception();

	wstr.unlock(result_len);

	return wstr;
}

B_END_NAMESPACE
