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

#ifndef B_RANDOM_H
#define B_RANDOM_H

#include "host.h"

B_BEGIN_NAMESPACE

// A simple pseudo-random number generator.
class random
{
// Construction
public:
	typedef unsigned value_type;

	// Initializes this instance with the system time.
	random();

	// Initializes this instance with the <seed> value.
	random(value_type seed);

// Attributes
public:
	// Sets the seed of this generator to the <seed> value.
	void set_seed(value_type seed);

// Operations
public:
	// Sets the seed of this generator to the system time.
	void randomize();

	// Returns a pseudo-random number in range 0 to 0x7FFF.
	value_type next();

	// Returns a pseudo-random number in range 0 to <limit>.
	value_type next(value_type limit);

// Implementation
private:
	value_type seed;
};

inline random::random() :
	seed((random::value_type) ::time(NULL))
{
}

inline random::random(value_type initial_seed) : seed(initial_seed)
{
}

inline void random::set_seed(value_type new_seed)
{
	seed = new_seed;
}

inline void random::randomize()
{
	seed = (value_type) ::time(NULL);
}

inline random::value_type random::next()
{
	return ((seed = seed * 0x41C64E6D + 0x3039) >> 0x10) & 0x7FFF;
}

inline random::value_type random::next(random::value_type limit)
{
	return (next() * limit) / 0x7FFF;
}

B_END_NAMESPACE

#endif /* !defined(B_RANDOM_H) */
