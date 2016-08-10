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

// Modern versions of Microsoft C++ support placement form of operator delete.
#if _MSC_VER >= 1200
#define USE_PLACEMENT_DELETE
#endif

// Disable Microsoft C++ warnings:
// 4290: C++ Exception Specification ignored
// 4514: unreferenced inline/local function has been removed
// 4710: 'function' : function not inlined
#pragma warning (disable: 4290 4514 4710)

#if defined(B_BUILD_STATIC)
#define B_API
#elif defined(B_BUILD)
#define B_API __declspec (dllexport)
#elif defined(B_STATIC)
#define B_API
#ifdef B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1mad.lib")
#else // B_DEBUG
#pragma comment (lib, "b1ma.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1mabd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1mab.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#else // B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1masd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1mas.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1masbd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1masb.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#endif // B_MT
#else
#define B_API __declspec (dllimport)
#ifdef B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1md.lib")
#else // B_DEBUG
#pragma comment (lib, "b1m.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1mbd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1mb.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#else // B_MT
#error B: Single-threaded DLL for Microsoft C++ is not available.
#endif // B_MT
#endif
