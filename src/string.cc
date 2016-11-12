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

#include <stdint.h>

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
