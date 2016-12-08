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

#ifndef B_HEAP_SKIP_IMPLEMENTATION

#ifndef B_HEAP_IMPLEMENTED
#define B_HEAP_IMPLEMENTED

#include "heap.h"

B_BEGIN_NAMESPACE

template <class T>
void heap<T>::push(T* data, size_t size)
{
	B_ASSERT(size > 0);

	/*
	 * Algorithm:
	 *
	 *   child = size - 1
	 *
	 *   while child > 0 and A[parent = child / 2] < A[child]
	 *       A[parent] <=> A[child]
	 *       child = parent
	 *
	 */

	size_t parent;

	--size; // Use size as the current child index

	while (size > 0 && data[parent = size >> 1] < data[size])
	{
		// Restore the heap property
		swap(data[parent], data[size]);

		// Move up
		size = parent;
	}
}

template <class T>
void heap<T>::pop(T* data, size_t size)
{
	B_ASSERT(size > 0);

	/*
	 * Algorithm:
	 *
	 *   A[size - 1] <=> A[0]
	 *
	 *   i = 0
	 *
	 *   while i has child(ren) and A[i] < A[j = maximal child of i]
	 *       A[i] <=> A[j]
	 *       i = j
	 *
	 */

	if (size > 1)
	{
		// The first call to swap() function will exchange the
		// first and the last elements of the array

		size_t child = 0; // Index of the first element
		size_t parent = size - 1; // Index of the last element

		// New index of the last element (after the pop operation)
		size = parent - 1;

		do
		{
			// Restore the heap property
			swap(data[parent], data[child]);

			// Continue with subtree rooted at child
			parent = child;

			// Set child to the index of the left child
			child <<= 1;

			if (child > size)
				// The parent is a leaf or the tree is empty
				// (It cannot be empty because of the first
				// 'if (size > 1)')
				// heap is in proper order, exit loop
				break;
			else
				// The parent has at least one child
				// If the parent has two children then
				// find the maximal one (use 'operator <' only)
				if (child < size &&
					data[child] < data[child + 1])
					// The maximal child is the right child
					// (or the children are equal)
					++child;

			// Continue if the parent is less then the maximal child
		}
		while (data[parent] < data[child]);
	}
}

template <class T>
void heap<T>::sort(T* data, size_t size)
{
	B_ASSERT(size >= 0);

	if (size > 1)
	{
		size_t heap_size = 1;

		while (heap_size < size)
			push(data, ++heap_size);

		while (heap_size > 1)
			pop(data, heap_size--);
	}
}

B_END_NAMESPACE

#endif /* !defined(B_HEAP_IMPLEMENTED) */

#else
#undef B_HEAP_SKIP_IMPLEMENTATION
#endif /* !defined(B_HEAP_SKIP_IMPLEMENTATION) */
