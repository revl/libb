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

#ifndef B_ATOMIC_H
#define B_ATOMIC_H

#include "host.h"

#if defined(B_HAVE_STD_ATOMIC) && __cplusplus >= 201103L

#include <atomic>

B_BEGIN_NAMESPACE

typedef std::atomic<int> atomic;

B_END_NAMESPACE

#define B_ATOMIC_INIT(i) ATOMIC_VAR_INIT(i)

#else

B_BEGIN_NAMESPACE

#if defined(B_HAVE_EXT_ATOMICITY_H)

#include <ext/atomicity.h>
typedef volatile _Atomic_word platform_atomic_type;

#elif defined(B_HAVE_BITS_ATOMICITY_H)

#include <bits/atomicity.h>
typedef volatile _Atomic_word platform_atomic_type;

#elif defined(B_HAVE_ASM_ATOMIC_H)

#include <asm/atomic.h>
typedef atomic_t platform_atomic_type;

#elif defined(__DECCXX_VER) && defined(__ALPHA)

#include <machine/builtins.h>
typedef __int32 platform_atomic_type;

#elif defined(B_HAVE_ATOMIC_SYNC)

typedef int platform_atomic_type;

#elif defined(__APPLE__)

#include <libkern/OSAtomic.h>
typedef int32_t platform_atomic_type;
#define B_USE_ATOMIC_INC_DEC_BARRIER

#else
#error arithmetic on atomic data has to be implemented for this platform
#endif

// Portable thread-safe reference count class.
struct atomic
{
	// Returns the current value of the counter.
	operator int() const;

	// Resets the counter with a new value.
	void operator =(int new_value);

	// Increments the counter by one.
	void operator ++();

	// Decrements the counter and returns false if it becomes zero.
	bool operator --();

	platform_atomic_type value;
};

inline atomic::operator int() const
{
#if defined(B_HAVE_ASM_ATOMIC_H)
	return atomic_read(&value)
#else
	return (int) value;
#endif
}

inline void atomic::operator =(int new_value)
{
#if defined(B_HAVE_ASM_ATOMIC_H)
	atomic_set(&value, new_value);
#else
	value = new_value;
#endif
}

inline void atomic::operator ++()
{
#if defined(B_HAVE_EXT_ATOMICITY_H) || defined(B_HAVE_BITS_ATOMICITY_H)
	__gnu_cxx::__atomic_add(&value, 1);
#elif defined(B_HAVE_ASM_ATOMIC_H)
	atomic_inc(&value);
#elif defined(__DECCXX_VER) && defined(__ALPHA)
	__ATOMIC_INCREMENT_LONG(&value);
#elif defined(B_HAVE_ATOMIC_SYNC)
	__sync_add_and_fetch(&value, 1);
#elif defined(B_USE_ATOMIC_INC_DEC_BARRIER)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	OSAtomicIncrement32Barrier(&value);
#pragma GCC diagnostic pop
#else
	++value;
#endif
}

inline bool atomic::operator --()
{
#if defined(B_HAVE_EXT_ATOMICITY_H) || defined(B_HAVE_BITS_ATOMICITY_H)
	return __gnu_cxx::__exchange_and_add(&value, -1) != 1;
#elif defined(B_HAVE_ASM_ATOMIC_H)
	return !atomic_dec_and_test(&value);
#elif defined(__DECCXX_VER) && defined(__ALPHA)
	return __ATOMIC_DECREMENT_LONG(&value) != 1;
#elif defined(B_HAVE_ATOMIC_SYNC)
	return __sync_add_and_fetch(&value, -1) != 0;
#elif defined(B_USE_ATOMIC_INC_DEC_BARRIER)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	return OSAtomicDecrement32Barrier(&value) != 0;
#pragma GCC diagnostic pop
#else
	return --value != 0;
#endif
}

B_END_NAMESPACE

#if defined(B_HAVE_ASM_ATOMIC_H)
#define B_ATOMIC_INIT(i) {ATOMIC_INIT(i)}
#else
#define B_ATOMIC_INIT(i) {i}
#endif

#endif /* !defined(B_HAVE_STD_ATOMIC) */

#endif /* !defined(B_ATOMIC_H) */
