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

#ifndef B_REF_COUNT_H
#define B_REF_COUNT_H

#include "host.h"

#if !defined(B_MT)

#define B_REFCOUNT_TYPE int
#define B_REFCOUNT_INC(counter) ++counter
#define B_REFCOUNT_DEC(counter) return --counter != 0

#elif defined(__GNUG__) && defined(__i386__)

#define B_REFCOUNT_TYPE volatile int
#define B_REFCOUNT_INC(counter) \
	__asm__ __volatile__("lock; incl %0" :"=m" (counter) :"m" (counter));
#define B_REFCOUNT_DEC(counter) \
	bool zero; __asm__ __volatile__("lock; decl %0; setne %1" \
	:"=m" (counter), "=qm" (zero) :"m" (counter) : "memory"); \
	return zero;

#elif defined(B_HAVE_EXT_ATOMICITY_H) || defined(B_HAVE_BITS_ATOMICITY_H)

#if defined(B_HAVE_EXT_ATOMICITY_H)
#include <ext/atomicity.h>
#else
#include <bits/atomicity.h>
#endif
#define B_REFCOUNT_TYPE volatile _Atomic_word
#define B_REFCOUNT_INC(counter) __gnu_cxx::__atomic_add(&counter, 1)
#define B_REFCOUNT_DEC(counter) \
	return __gnu_cxx::__exchange_and_add(&counter, -1) != 1

#elif defined(B_HAVE_ASM_ATOMIC_H)

#include <asm/atomic.h>
#define B_REFCOUNT_TYPE atomic_t
#define B_REFCOUNT_GET(counter) return atomic_read(&counter)
#define B_REFCOUNT_SET(counter, value) atomic_set(&counter, value)
#define B_REFCOUNT_INC(counter) atomic_inc(&counter)
#define B_REFCOUNT_DEC(counter) return !atomic_dec_and_test(&counter)
#define B_REFCOUNT_STATIC_INIT(i) {ATOMIC_INIT(i)}

#elif defined(__DECCXX_VER) && defined(__ALPHA)

#include <machine/builtins.h>
#define B_REFCOUNT_TYPE __int32
#define B_REFCOUNT_INC(counter) __ATOMIC_INCREMENT_LONG(&counter)
#define B_REFCOUNT_DEC(counter) return __ATOMIC_DECREMENT_LONG(&counter) != 1

#else
#error arithmetic on atomic data has to be implemented for this platform
#endif

#ifndef B_REFCOUNT_GET
#define B_REFCOUNT_GET(counter) return (int) counter
#endif

#ifndef B_REFCOUNT_SET
#define B_REFCOUNT_SET(counter, value) counter = value
#endif

#ifndef B_REFCOUNT_STATIC_INIT
#define B_REFCOUNT_STATIC_INIT(i) {i}
#endif

B_BEGIN_NAMESPACE

// Portable thread-safe reference count class.
struct RefCount
{
// Attributes
public:
	operator int() const;

// Operations
public:
	void operator =(int new_value);
	void operator ++();
	bool operator --();

// Implementation
public:
	B_REFCOUNT_TYPE value;
};

inline RefCount::operator int() const
{
	B_REFCOUNT_GET(value);
}

inline void RefCount::operator =(int new_value)
{
	B_REFCOUNT_SET(value, new_value);
}

inline void RefCount::operator ++()
{
	B_REFCOUNT_INC(value);
}

inline bool RefCount::operator --()
{
	B_REFCOUNT_DEC(value);
}

B_END_NAMESPACE

#endif /* !defined(B_REF_COUNT_H) */
