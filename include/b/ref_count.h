/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016, 2017 Damon Revoe
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

#ifndef B_REF_COUNT_H
#define B_REF_COUNT_H

#include "host.h"

#if !defined(B_MT)

#define B_ATOMIC_TYPE int

#elif defined(B_HAVE_STD_ATOMIC)

#include <atomic>
#define B_ATOMIC_TYPE std::atomic<int>

#elif defined(__GNUG__) && defined(__i386__)

#define B_ATOMIC_TYPE volatile int

#elif defined(B_HAVE_EXT_ATOMICITY_H)

#include <ext/atomicity.h>
#define B_ATOMIC_TYPE volatile _Atomic_word

#elif defined(B_HAVE_BITS_ATOMICITY_H)

#include <bits/atomicity.h>
#define B_ATOMIC_TYPE volatile _Atomic_word

#elif defined(B_HAVE_ASM_ATOMIC_H)

#include <asm/atomic.h>
#define B_ATOMIC_TYPE atomic_t

#elif defined(__DECCXX_VER) && defined(__ALPHA)

#include <machine/builtins.h>
#define B_ATOMIC_TYPE __int32

#elif defined(__APPLE__)

#include <libkern/OSAtomic.h>
#define B_ATOMIC_TYPE int32_t

#else
#error arithmetic on atomic data has to be implemented for this platform
#endif

#if defined(B_HAVE_STD_ATOMIC)
#define B_REFCOUNT_STATIC_INIT(i) {ATOMIC_VAR_INIT(i)}
#elif defined(B_HAVE_ASM_ATOMIC_H)
#define B_REFCOUNT_STATIC_INIT(i) {ATOMIC_INIT(i)}
#else
#define B_REFCOUNT_STATIC_INIT(i) {i}
#endif

B_BEGIN_NAMESPACE

// Portable thread-safe reference count class.
struct ref_count
{
	// Returns the current value of the counter.
	operator int() const;

	// Resets the counter with a new value.
	void operator =(int new_value);

	// Increments the counter by one.
	void operator ++();

	// Decrements the counter and returns false if it becomes zero.
	bool operator --();

	B_ATOMIC_TYPE value;
};

inline ref_count::operator int() const
{
#if defined(B_HAVE_ASM_ATOMIC_H)
	return atomic_read(&value)
#else
	return (int) value;
#endif
}

inline void ref_count::operator =(int new_value)
{
#if defined(B_HAVE_ASM_ATOMIC_H)
	atomic_set(&value, new_value);
#else
	value = new_value;
#endif
}

inline void ref_count::operator ++()
{
#if defined(__GNUG__) && defined(__i386__)
	__asm__ __volatile__("lock; incl %0" :"=m" (value) :"m" (value));
#elif defined(B_HAVE_EXT_ATOMICITY_H) || defined(B_HAVE_BITS_ATOMICITY_H)
	__gnu_cxx::__atomic_add(&value, 1);
#elif defined(B_HAVE_ASM_ATOMIC_H)
	atomic_inc(&value);
#elif defined(__DECCXX_VER) && defined(__ALPHA)
	__ATOMIC_INCREMENT_LONG(&value);
#elif defined(__APPLE__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	OSAtomicIncrement32Barrier(&value);
#pragma GCC diagnostic pop
#else
	++value;
#endif
}

inline bool ref_count::operator --()
{
#if defined(__GNUG__) && defined(__i386__)
	bool zero; __asm__ __volatile__("lock; decl %0; setne %1" \
	:"=m" (value), "=qm" (zero) :"m" (value) : "memory"); \
	return zero;
#elif defined(B_HAVE_EXT_ATOMICITY_H) || defined(B_HAVE_BITS_ATOMICITY_H)
	return __gnu_cxx::__exchange_and_add(&value, -1) != 1;
#elif defined(B_HAVE_ASM_ATOMIC_H)
	return !atomic_dec_and_test(&value);
#elif defined(__DECCXX_VER) && defined(__ALPHA)
	return __ATOMIC_DECREMENT_LONG(&value) != 1;
#elif defined(__APPLE__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	return OSAtomicDecrement32Barrier(&value) != 0;
#pragma GCC diagnostic pop
#else
	return --value != 0;
#endif
}

B_END_NAMESPACE

#endif /* !defined(B_REF_COUNT_H) */
