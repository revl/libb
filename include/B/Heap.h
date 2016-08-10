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
#endif // B_HEAP_SKIP_IMPLEMENTATION
#include "Array.h"

#ifndef B_HEAP_DEFINED
#define B_HEAP_DEFINED

B_BEGIN_NAMESPACE

template <class TYPE>
class Heap
{
// Attributes
public:
	static int GetParentIndex(int child);
	static int GetLeftChildIndex(int parent);
	static int GetRightChildIndex(int parent);

	int GetSize() const;
	bool IsEmpty() const;

	const TYPE& GetAt(int index) const;
	const TYPE& operator [](int index) const;

	Array<TYPE> GetData() const;

// Operations
public:
	static void Push(TYPE* data, int size);
	static void Pop(TYPE* data, int size);
	static void Sort(TYPE* data, int size);

	void Push(const TYPE& element);
	TYPE Pop();

// Implementation
protected:
	Array<TYPE> data;
};

template <class TYPE>
inline int Heap<TYPE>::GetSize() const
{
	return data.GetSize();
}

template <class TYPE>
inline bool Heap<TYPE>::IsEmpty() const
{
	return GetSize() == 0;
}

template <class TYPE>
inline int Heap<TYPE>::GetParentIndex(int child)
{
	return child >> 1;
}

template <class TYPE>
inline int Heap<TYPE>::GetLeftChildIndex(int parent)
{
	return parent << 1;
}

template <class TYPE>
inline int Heap<TYPE>::GetRightChildIndex(int parent)
{
	return (parent << 1) + 1;
}

template <class TYPE>
inline const TYPE& Heap<TYPE>::GetAt(int index) const
{
	return data[index];
}

template <class TYPE>
inline const TYPE& Heap<TYPE>::operator [](int index) const
{
	return data[index];
}

template <class TYPE>
inline Array<TYPE> Heap<TYPE>::GetData() const
{
	return data;
}

template <class TYPE>
inline void Heap<TYPE>::Push(const TYPE& element)
{
	data.Append(element);
	Push(data.LockBuffer(), data.GetSize());
	data.UnlockBuffer();
}

template <class TYPE>
inline TYPE Heap<TYPE>::Pop()
{
	int size = data.GetSize();

	Pop(data.LockBuffer(), size);
	data.UnlockBuffer();

	TYPE result(data[--size]);

	data.RemoveAt(size);

	return result;
}

B_END_NAMESPACE

#endif // B_HEAP_DEFINED

#include "HeapImpl.h"
