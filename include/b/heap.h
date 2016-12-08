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

#ifdef B_HEAP_SKIP_IMPLEMENTATION
#undef B_ARRAY_SKIP_IMPLEMENTATION
#define B_ARRAY_SKIP_IMPLEMENTATION
#endif /* defined(B_HEAP_SKIP_IMPLEMENTATION) */
#include "array.h"

#ifndef B_HEAP_DEFINED
#define B_HEAP_DEFINED

B_BEGIN_NAMESPACE

template <class T>
class heap
{
// Methods on the
public:
	// Adds a value to this heap.
	void push(const T& element);

	// Removes the element with the greatest value from the heap
	// and returns it.
	T pop();

	// Returns the number of elements in this heap.
	size_t size() const;

	// Returns true if the heap is empty.
	bool is_empty() const;

// Operations on the external array
public:
	// Adds a value to a heap. The last element of the 'data'
	// array (the one with the index size-1) must contain the
	// value to be inserted and the rest of the elements must
	// contain a properly formed heap.
	static void push(T* data, size_t size);

	// Given a valid heap in the 'data' array, moves the
	// element with the greatest value to the end of the array
	// and then restores the heap property among the remaining
	// elements.
	static void pop(T* data, size_t size);

	// Sorts the array pointed to by 'data' using heapsort.
	// Complexity: O(n * log(n))
	static void sort(T* data, size_t size);

protected:
	array<T> data;
};

template <class T>
void heap<T>::push(const T& element)
{
	data.append(1, element);
	push(data.lock(), data.size());
	data.unlock();
}

template <class T>
T heap<T>::pop()
{
	size_t data_size = data.size();

	pop(data.lock(), data_size);
	data.unlock();

	T result(data[--data_size]);

	data.remove(data_size);

	return result;
}

template <class T>
size_t heap<T>::size() const
{
	return data.size();
}

template <class T>
bool heap<T>::is_empty() const
{
	return data.is_empty();
}

B_END_NAMESPACE

#endif /* !defined(B_HEAP_DEFINED) */

#include "heap_impl.h"
