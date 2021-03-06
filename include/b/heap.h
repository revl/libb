// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

// Heap-based priority queue and heapsort implementation

#ifndef B_HEAP_H
#define B_HEAP_H

#include "fn.h"

B_BEGIN_NAMESPACE

// Adds a value to a binary heap. The last element of the
// 'data' array (the one with the index size-1) must contain
// the value to be inserted and the rest of the elements must
// contain a properly formed heap.
template <class T>
void push_into_heap(T* data, size_t size)
{
	B_ASSERT(size > 0);

	// Algorithm:
	//
	//   child = size - 1
	//
	//   while child > 0 and A[parent = child / 2] < A[child]
	//       A[parent] <=> A[child]
	//       child = parent

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

// Given a valid heap in the 'data' array, moves the
// element with the greatest value to the end of the array
// and then restores the heap property among the remaining
// elements.
template <class T>
void pop_from_heap(T* data, size_t size)
{
	B_ASSERT(size > 0);

	// Algorithm:
	//
	//   A[size - 1] <=> A[0]
	//
	//   i = 0
	//
	//   while i has child(ren) and A[i] < A[j = maximum child of i]
	//       A[i] <=> A[j]
	//       i = j

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
				// find the maximum one (use 'operator <' only)
				if (child < size &&
					data[child] < data[child + 1])
					// The maximum child is the right child
					// (or the children are equal)
					++child;

			// Continue if the parent is less then the maximum child
		}
		while (data[parent] < data[child]);
	}
}

// Sorts the array pointed to by 'data' using heapsort.
// Complexity: O(n * log(n))
template <class T>
void heapsort(T* data, size_t size)
{
	if (size > 1)
	{
		size_t heap_size = 1;

		while (heap_size < size)
			push_into_heap(data, ++heap_size);

		while (heap_size > 1)
			pop_from_heap(data, heap_size--);
	}
}

B_END_NAMESPACE

#endif /* !defined(B_HEAP_H) */
