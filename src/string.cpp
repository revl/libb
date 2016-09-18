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

#define string_t wstring
#define char_t wchar_t
#define B_L_PREFIX(ch) L##ch
#include "string_impl.h"
#define string_formatting wstring_formatting
#include "string_formatting.h"
#undef string_formatting
#undef B_L_PREFIX
#undef char_t
#undef string_t

#define string_t string
#define char_t char
#define B_L_PREFIX(ch) ch
#include "string_impl.h"
#include "string_formatting.h"
#undef B_L_PREFIX
#undef char_t
#undef string_t
