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

#ifndef B_PSEUDORANDOM_H
#define B_PSEUDORANDOM_H

#include "host.h"

B_BEGIN_NAMESPACE

// A linear congruential generator that uses the same parameters
// as the glibc version of rand(3).
class pseudorandom
{
public:
	// Initializes this instance with the system time.
	pseudorandom();

	// Initializes this instance with the <seed> value.
	pseudorandom(size_t seed);

	// Sets the seed of this generator to the <seed> value.
	void set_seed(size_t seed);

	// Sets the seed of this generator to the system time.
	void randomize();

	// Returns the maximum value the next() method can return.
	// This implementation always returns 0x7FFFFFFF.
	static size_t max();

	// Returns a pseudorandom number in the range from zero
	// to max() (inclusively).
	size_t next();

	// Returns a pseudorandom number in the range from zero
	// up to (but not including) 'limit'.
	size_t next(size_t limit);

private:
	size_t seed;
};

inline pseudorandom::pseudorandom() :
	seed(((size_t) ::time(NULL)) & 0x7FFFFFFFU)
{
}

inline pseudorandom::pseudorandom(size_t initial_seed) : seed(initial_seed)
{
}

inline void pseudorandom::set_seed(size_t new_seed)
{
	seed = new_seed;
}

inline void pseudorandom::randomize()
{
	seed = ((size_t) ::time(NULL)) & 0x7FFFFFFFU;
}

inline size_t pseudorandom::max()
{
	return 0x7FFFFFFFU;
}

inline size_t pseudorandom::next()
{
	return (seed = seed * 1103515245U + 12345U) & 0x7FFFFFFFU;
}

inline size_t pseudorandom::next(size_t limit)
{
	B_ASSERT(limit > 0);

	return next() % limit;
}

B_END_NAMESPACE

#endif /* !defined(B_PSEUDORANDOM_H) */
