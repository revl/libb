/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2020 Damon Revoe
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

#ifndef B_HOST_H
#define B_HOST_H

#ifdef B_NO_NAMESPACE

#undef b
#define b

#define B_BEGIN_NAMESPACE
#define B_END_NAMESPACE

#else

#define B_BEGIN_NAMESPACE namespace b {
#define B_END_NAMESPACE }

#endif /* defined(B_NO_NAMESPACE) */

// Most compilers support placement form of operator delete.
#ifndef USE_PLACEMENT_DELETE
#ifdef __GNUG__
#define USE_PLACEMENT_DELETE
#endif /* defined(__GNUG__) */
#endif /* !defined(USE_PLACEMENT_DELETE) */

#if defined(__DECCXX_VER)
// Disable Digital C++ warnings:
// inlinefunnoinst: inline function cannot be explicitly instantiated
#pragma message disable inlinefunnoinst
// For older Digital C++ compilers, also disable
// basclsnondto: destructor for base class "Base" is not virtual
#if __DECCXX_VER < 60200000
#pragma message disable basclsnondto
#endif
#elif defined(__INTEL_COMPILER)
// Disable Intel C++ warnings:
// 530: inline function cannot be explicitly instantiated
// 981: operands are evaluated in unspecified order
#pragma warning (disable: 530 981)
#endif

// Macro to enable syntax checking of the format strings supplied
// as arguments to the printf-style functions.
#if defined(__GNUG__)
#define B_PRINTF_STYLE(fmt_index, arg_index) \
	__attribute__ ((format (printf, fmt_index, arg_index)))
#else
#define B_PRINTF_STYLE(fmt_index, arg_index)
#endif /* defined(__GNUG__) */

#include <b/config.h>

#include <errno.h>

#include <stdio.h>
#include <sys/types.h>
#include <wchar.h>

#include <time.h>

#include <stdlib.h>
#include <stdarg.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <stddef.h>
#include <new>
#include <string.h>

#define B_OFFSETOF(s, m) ((char*) (void*) &((s*) 0x1000)->m - (char*) 0x1000)

#define B_OUTERSTRUCT(s, m, a) const_cast<s*>((const s*) \
	((const char*) (const void*) (a) - B_OFFSETOF(s, m)))

#define B_COUNTOF(array) (sizeof(array) / sizeof(*(array)))

#ifdef B_DEBUG
#include <assert.h>
#define B_ASSERT(expr) assert(expr)
#else
#define B_ASSERT(expr)
#endif /* defined(B_DEBUG) */

#ifdef B_USE_STL
#include <exception>
#include <string>
#endif /* defined(B_USE_STL) */

#define B_PATH_SEPARATOR '/'
#define B_PATH_SEPARATOR_SZ "/"

B_BEGIN_NAMESPACE

class wstring;
class string;

B_END_NAMESPACE

#endif /* !defined(B_HOST_H) */
