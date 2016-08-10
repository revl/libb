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

#ifndef B_LEVENSHTEINDISTANCE
#define B_LEVENSHTEINDISTANCE

#include "Array.h"

#include <vector>

B_BEGIN_NAMESPACE

template <typename Iter>
class LevenshteinDistance
{
// Operations
public:
	size_t operator ()(Iter string1, size_t length1,
		Iter string2, size_t length2);

// Implementation
private:
	// A row of the dynamic programming matrix
	typedef std::vector<size_t> Row;

	Row row;
};

template <typename Iter>
size_t LevenshteinDistance<Iter>::operator ()(Iter string1,
	size_t length1, Iter string2, size_t length2)
{
	// Since we allocate a buffer of size length2 reverse
	// the argument order if the second string is shorter
	if (length1 < length2)
		return operator ()(string2, length2,
			string1, length1);

	// Ensure the buffer is big enough
	if (row.size() <= length2)
		row.resize(length2 + 1);

	// Pointer to the current character of the second string
	Iter current_char;

	// The value of the upper left matrix element
	// relatively to the current
	size_t diagonal;

	// Either the left element or the new value
	// of *upper
	size_t distance;

	// Loop controlling variable
	size_t j;

	j = distance = length2;

	// Pointer to the matrix element over that
	// being calculated now
	Row::iterator upper = row.begin() + j;

	// Boundary conditions
	while ((*upper = j) > 0)
		--upper, --j;

	// For each symbol in the first string (the outer loop,
	// which cycles through all the rows of the matrix)
	for (; length1 > 0; --length1)
	{
		// Boundary conditions
		upper = row.begin();

		*upper++ = distance = (diagonal = *upper) + 1;

		current_char = string2;

		// For each symbol in the second string
		for (j = length2; j > 0; --j)
		{
			// Find the minimal among the next three values:
			// - *upper (the upper element) + 1 (insertion)
			// - distance (the left element) + 1 (deletion)
			// - diagonal (the diagonal element) +
			//   (*string1 != *current_char) (change)
			// Store the result in distance

			if (distance > *upper)
				distance = *upper;

			if (*string1 != *current_char)
				++diagonal;

			if (++distance > diagonal)
				distance = diagonal;

			// The current upper becomes diagonal
			diagonal = *upper;

			// Store the newly calculated value
			// Shift the pointer to the next 'upper'
			*upper++ = distance;

			++current_char;
		}

		++string1;
	}

	return distance;
}

B_END_NAMESPACE

#endif // B_LEVENSHTEINDISTANCE
