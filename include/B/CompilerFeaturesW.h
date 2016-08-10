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

// Older versions of WATCOM C++ did not support the typename keyword.
#if __WATCOM_CPLUSPLUS__ < 1240
#define typename
#endif

#if defined(B_BUILD_STATIC) || defined(B_STATIC)
#define B_API
#elif defined(B_BUILD)
#define B_API __declspec (dllexport)
#else
#define B_API __declspec (dllimport)
#endif

#if __WATCOM_CPLUSPLUS__ >= 1200

#if defined(B_STATIC)
#ifdef B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1owad.lib")
#else // B_DEBUG
#pragma comment (lib, "b1owa.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1owabd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1owab.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#else // B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1owasd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1owas.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1owasbd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1owasb.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#endif // B_MT
#elif !(defined(B_BUILD) || defined(B_BUILD_STATIC))
#ifdef B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1owd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1ow.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1owbd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1owb.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#else // B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1owsd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1ows.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1owsbd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1owsb.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#endif // B_MT
#endif

#else // __WATCOM_CPLUSPLUS__

#if defined(B_STATIC)
#ifdef B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1wad.lib")
#else // B_DEBUG
#pragma comment (lib, "b1wa.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1wabd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1wab.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#else // B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1wasd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1was.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1wasbd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1wasb.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#endif // B_MT
#elif !(defined(B_BUILD) || defined(B_BUILD_STATIC))
#ifdef B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1wd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1w.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1wbd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1wb.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#else // B_MT
#ifdef B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1wsd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1ws.lib")
#endif // B_DEBUG
#else // B_UNICODE
#ifdef B_DEBUG
#pragma comment (lib, "b1wsbd.lib")
#else // B_DEBUG
#pragma comment (lib, "b1wsb.lib")
#endif // B_DEBUG
#endif // B_UNICODE
#endif // B_MT
#endif

#endif // __WATCOM_CPLUSPLUS__
