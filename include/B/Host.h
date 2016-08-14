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

#ifndef B_HOST_H
#define B_HOST_H

// Both DEBUG and _DEBUG macros enable various runtime checks.
#if (defined(DEBUG) || defined(_DEBUG)) && !defined(B_DEBUG)
#define B_DEBUG
#endif

// _MT indicates that a multithreaded version is to be built.
#if defined(_MT) && !defined(B_MT)
#define B_MT
#endif

#ifdef B_NO_NAMESPACE

#undef B
#define B

#define B_BEGIN_NAMESPACE
#define B_END_NAMESPACE

#else

#define B_BEGIN_NAMESPACE namespace B {

#if !defined(__WATCOM_CPLUSPLUS__) || __WATCOM_CPLUSPLUS__ >= 1220
#define B_END_NAMESPACE }
#else
// Get rid of the WATCOM C++ parser error
#define B_END_NAMESPACE extern int unused; }
#endif // __WATCOM_CPLUSPLUS__

#endif // B_NO_NAMESPACE

#if (defined(WIN32) || defined(_WIN32)) && !defined(B_WIN32)
#define B_WIN32
#endif

#if defined(B_WIN32) && !defined(__GNUG__) && !defined(B_USE_WIN32_API)
#define B_USE_WIN32_API
#endif

// Most compilers support placement form of operator delete.
#if !defined(USE_PLACEMENT_DELETE)
#if defined(__GNUG__)
#define USE_PLACEMENT_DELETE
#endif // defined(__GNUG__)
#endif // !defined(USE_PLACEMENT_DELETE)

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

#ifdef B_USE_WIN32_API

#if (defined(UNICODE) || defined(_UNICODE)) && !defined(B_UNICODE)
#define B_UNICODE 1
#elif defined(B_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif // B_UNICODE

#ifndef STRICT
#define STRICT 1
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <Windows.h>

#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

#else

#ifdef B_UNINSTALLED
#include <B/config.h>
#else
#ifdef B_MT
#ifdef B_DEBUG
#include <B/configd_r.h>
#else
#include <B/config_r.h>
#endif // defined(B_DEBUG)
#else
#ifdef B_DEBUG
#include <B/configd.h>
#else
#include <B/config.h>
#endif // defined(B_DEBUG)
#endif // defined(B_MT)
#endif // defined(B_UNINSTALLED)

#include <errno.h>

#include <stdio.h>
#include <sys/types.h>
#include <wchar.h>

#include <time.h>

#include <stdlib.h>
#include <stdarg.h>

#include <fcntl.h>
#include <sys/stat.h>

#endif // B_USE_WIN32_API

#include <stddef.h>
#include <new>
#include <string.h>

#define B_OFFSETOF(s, m) ((char*) &((s*) 0x1000)->m - (char*) 0x1000)

#define B_OUTERSTRUCT(s, m, a) const_cast<s*>((const s*) \
	((const char*) (a) - B_OFFSETOF(s, m)))

#if defined(B_DEBUG)
#include <assert.h>
#define B_ASSERT(expr) assert(expr)
#else
#define B_ASSERT(expr)
#endif // defined(B_DEBUG)

#ifdef B_USE_STL
#include <exception>
#endif // B_USE_STL

#ifndef B_TEXT
#ifdef B_UNICODE
#define B_TEXT(x) L##x
#else
#define B_TEXT(x) x
#endif // B_UNICODE
#endif // TEXT

#ifndef B_CHAR
#ifdef B_UNICODE
#define B_CHAR wchar_t
#else
#define B_CHAR char
#endif // B_UNICODE
#endif // B_CHAR

#ifndef B_WIN32
#define B_PATH_SEPARATOR B_TEXT('/')
#define B_PATH_SEPARATOR_SZ B_TEXT("/")
#else
#define B_PATH_SEPARATOR B_TEXT('\\')
#define B_PATH_SEPARATOR_SZ B_TEXT("\\")
#endif // !B_WIN32

B_BEGIN_NAMESPACE

class StringW;
class StringA;

#ifdef B_UNICODE
typedef StringW String;
#else
typedef StringA String;
#endif // B_UNICODE

B_END_NAMESPACE

#endif // !B_HOST_H
