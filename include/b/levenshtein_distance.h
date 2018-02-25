/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2018 Damon Revoe
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

#ifndef B_LEVENSHTEIN_DISTANCE_H
#define B_LEVENSHTEIN_DISTANCE_H

#include "array.h"

B_BEGIN_NAMESPACE

class levenshtein_distance
{
public:
	template <typename Iter>
	size_t operator ()(Iter string1, size_t length1,
		Iter string2, size_t length2);

private:
	// A row of the dynamic programming matrix
	array<size_t> row;
};

template <typename Iter>
size_t levenshtein_distance::operator ()(Iter string1,
	size_t length1, Iter string2, size_t length2)
{
	// Since we allocate a buffer of size length2 reverse
	// the argument order if the second string is shorter
	if (length1 < length2)
		return operator ()(string2, length2,
			string1, length1);

	// Ensure the buffer is big enough
	if (row.length() <= length2)
		row.append(length2 + 1 - row.length(), 0);

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

	size_t* first_element = row.lock();

	// Pointer to the matrix element over that
	// being calculated now
	size_t* upper = first_element + j;

	// Boundary conditions
	while ((*upper = j) > 0)
		--upper, --j;

	// For each symbol in the first string (the outer loop,
	// which cycles through all the rows of the matrix)
	for (; length1 > 0; --length1)
	{
		// Boundary conditions
		upper = first_element;

		diagonal = *upper;

		*upper++ = distance = diagonal + 1;

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

	row.unlock();

	return distance;
}

B_END_NAMESPACE

#endif /* !defined(B_LEVENSHTEIN_DISTANCE_H) */
