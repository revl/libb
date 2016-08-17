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

#include "Host.h"

B_BEGIN_NAMESPACE

// A simple pseudo-random number generator.
class Random
{
// Construction
public:
	typedef unsigned value_type;

	// Initializes this instance with the system time.
	Random();

	// Initializes this instance with the <seed> value.
	Random(value_type seed);

// Attributes
public:
	// Sets the seed of this generator to the <seed> value.
	void SetSeed(value_type seed);

// Operations
public:
	// Sets the seed of this generator to the system time.
	void Randomize();

	// Returns a pseudo-random number in range 0 to 0x7FFF.
	value_type GetNext();

	// Returns a pseudo-random number in range 0 to <limit>.
	value_type GetNext(value_type limit);

// Implementation
private:
	value_type seed;
};

inline Random::Random() :
	seed((Random::value_type) ::time(NULL))
{
}

inline Random::Random(value_type initial_seed) : seed(initial_seed)
{
}

inline void Random::SetSeed(value_type new_seed)
{
	seed = new_seed;
}

inline void Random::Randomize()
{
	seed = (value_type) ::time(NULL);
}

inline Random::value_type Random::GetNext()
{
	return ((seed = seed * 0x41C64E6D + 0x3039) >> 0x10) & 0x7FFF;
}

inline Random::value_type Random::GetNext(Random::value_type limit)
{
	return (GetNext() * limit) / 0x7FFF;
}

B_END_NAMESPACE

#endif // !defined(B_RANDOM_H)
