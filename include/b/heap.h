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
// Attributes
public:
	int size() const;
	bool is_empty() const;

// Operations
public:
	static void push(T* data, int size);
	static void pop(T* data, int size);

	// Sorts the array pointed to by 'data' using heapsort.
	// Complexity: O(n * log(n))
	static void sort(T* data, int size);

	void push(const T& element);
	T pop();

// Implementation
protected:
	array<T> data;
};

template <class T>
int heap<T>::size() const
{
	return data.size();
}

template <class T>
bool heap<T>::is_empty() const
{
	return data.is_empty();
}

template <class T>
void heap<T>::push(const T& element)
{
	data.Append(element);
	push(data.LockBuffer(), data.size());
	data.UnlockBuffer();
}

template <class T>
T heap<T>::pop()
{
	int size = data.size();

	pop(data.LockBuffer(), size);
	data.UnlockBuffer();

	T result(data[--size]);

	data.RemoveAt(size);

	return result;
}

B_END_NAMESPACE

#endif /* !defined(B_HEAP_DEFINED) */

#include "heap_impl.h"
