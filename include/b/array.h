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

#include "ref_count.h"
#include "helpers.h"

#ifndef B_ARRAY_DEFINED
#define B_ARRAY_DEFINED

B_BEGIN_NAMESPACE

// Linear array template class.
template <class TYPE>
class Array
{
// Construction
public:
	// Creates an empty array.
	Array();

	// Constructs a copy of an existing array object.
	Array(const Array<TYPE>& source);

	// Constructs an array from a sequence of objects.
	Array(const TYPE* source, size_t count);

	// Creates an array of <count> copies of <element>.
	Array(const TYPE& element, size_t count = 1);

// Array Length
public:
	// Returns the number of elements for which memory has
	// been allocated.
	size_t GetCapacity() const;

	// Returns the number of elements in the array.
	size_t GetSize() const;

	// Checks if the array is empty.
	bool IsEmpty() const;

// Lock State
public:
	// Checks if the array is locked.
	bool IsLocked() const;

// Memory Management
public:
	// Allocates the exact amount of memory for the buffer.
	void AllocExactly(size_t capacity);

	// Reallocates the buffer preserving its contents.
	void ReallocExactly(size_t capacity);

	// Allocates enough memory to store <capacity> elements.
	void Alloc(size_t capacity);

	// Reallocates the buffer preserving its contents.
	void Realloc(size_t capacity);

	// Frees memory that is not used by the elements.
	void FreeExtra();

// Buffer Access
public:
	// Returns a constant pointer to the elements.
	const TYPE* GetBuffer() const;

	// Returns the same as GetBuffer().
	operator const TYPE*() const;

	// Fixes the buffer in memory and returns a modifiable pointer to it.
	TYPE* LockBuffer();

	// Gives control over the buffer back to this object.
	void UnlockBuffer();

// Element Access
public:
	// Returns a constant reference to an element.
	const TYPE& GetAt(size_t index) const;

	// Does the same thing as GetAt().
	const TYPE& operator [](size_t index) const;

	// Returns a modifiable reference to an element.
	TYPE& GetAt(size_t index);

	// Does the same thing as GetAt().
	TYPE& operator [](size_t index);

	// Returns a constant reference to the first array element.
	const TYPE& GetHead() const;

	// Returns a modifiable reference to the first array element.
	TYPE& GetHead();

	// Returns a constant reference to the last array element.
	const TYPE& GetTail() const;

	// Returns a modifiable reference to the last array element.
	TYPE& GetTail();

// Assignment
public:
	// Assigns the contents of <source> to this object.
	void Assign(const Array<TYPE>& source);

	// Assigns a sequence of elements to this object.
	void Assign(const TYPE* source, size_t count);

	// Creates an array with <count> copies of <element>.
	void Assign(const TYPE& element, size_t count = 1);

	// A shorter version of Assign(source).
	Array<TYPE>& operator =(const Array<TYPE>& source);

	// A shorter version of Assign(element, 1).
	Array<TYPE>& operator =(const TYPE& element);

	// Overwrites array range with the contents of <source>.
	void SetAt(size_t index, const Array<TYPE>& source);

	// Overwrites array range with copies of elements
	// pointed to by <source>.
	void SetAt(size_t index, const TYPE* source, size_t count);

	// Overwrites array range with <count> copies of <element>.
	void SetAt(size_t index, const TYPE& element, size_t count = 1);

// Insertion
public:
	// Inserts the entire contents of <source> into this array
	// at the specified position.
	void InsertAt(size_t index, const Array<TYPE>& source);

	// Inserts elements pointed to by <source> at the specified position.
	void InsertAt(size_t index, const TYPE* source, size_t count);

	// Inserts <count> copies of <element> at the specified position.
	void InsertAt(size_t index, const TYPE& element, size_t count = 1);

// Appending
public:
	// Appends this array with copies of elements from <source>.
	void Append(const Array<TYPE>& source);

	// Appends this array with elements from <source>.
	void Append(const TYPE* source, size_t count);

	// Appends this array with <count> copies of <element>.
	void Append(const TYPE& element, size_t count = 1);

	// The same as Append(source).
	Array<TYPE>& operator +=(const Array<TYPE>& source);

	// The same as Append(element, 1).
	Array<TYPE>& operator +=(const TYPE& element);

	// Concatenates this array with <source> and returns the
	// result as a new array object.
	Array<TYPE> operator +(const Array<TYPE>& source) const;

	// Appends <element> to the contents of this array and returns
	// the result as a new array object.
	Array<TYPE> operator +(const TYPE& element) const;

// Removal
public:
	// Removes a number of elements at the specified position.
	void RemoveAt(size_t index, size_t count = 1);

	// Empties the array.
	void RemoveAll();

// Implementation
private:
	struct MetaData
	{
		RefCount refs;
		size_t capacity;
		size_t size;
	};

	struct Wrapper
	{
		TYPE first;

		// Unused dummy constructor.
		explicit Wrapper(const TYPE& value) : first(value)
		{
		}
	};

	struct Buffer : public MetaData, public Wrapper
	{
		// Unused dummy constructor.
		explicit Buffer(const TYPE& value) : Wrapper(value)
		{
		}
	};

	TYPE* buffer;

	static size_t Inc(size_t size);

	bool IsShared() const;

	static TYPE* GetEmptyBuffer();

	static TYPE* AllocBufferExactly(size_t capacity);

	static TYPE* AllocBuffer(size_t capacity);

	static MetaData* GetMetaData(const TYPE* buffer);
	MetaData* GetMetaData() const;

	void Release();

	void ReplaceBuffer(TYPE* new_buffer);

	void CopyBeforeWrite();

public:
	~Array()
		throw ();
};

template <class TYPE>
inline Array<TYPE>::Array()
	: buffer(GetEmptyBuffer())
{
}

template <class TYPE>
inline Array<TYPE>::Array(const Array<TYPE>& source)
	: buffer(GetEmptyBuffer())
{
	Assign(source);
}

template <class TYPE>
inline Array<TYPE>::Array(const TYPE* source, size_t count)
	: buffer(GetEmptyBuffer())
{
	Assign(source, count);
}

template <class TYPE>
inline Array<TYPE>::Array(const TYPE& element, size_t count)
	: buffer(GetEmptyBuffer())
{
	Assign(element, count);
}

template <class TYPE>
inline size_t Array<TYPE>::GetCapacity() const
{
	return GetMetaData()->capacity;
}

template <class TYPE>
inline size_t Array<TYPE>::GetSize() const
{
	return GetMetaData()->size;
}

template <class TYPE>
inline bool Array<TYPE>::IsEmpty() const
{
	return GetSize() == 0;
}

template <class TYPE>
inline bool Array<TYPE>::IsLocked() const
{
	return GetMetaData()->refs < 0;
}

template <class TYPE>
inline void Array<TYPE>::Alloc(size_t capacity)
{
	AllocExactly(Inc(capacity));
}

template <class TYPE>
inline void Array<TYPE>::Realloc(size_t capacity)
{
	ReallocExactly(Inc(capacity));
}

template <class TYPE>
inline void Array<TYPE>::FreeExtra()
{
	ReallocExactly(GetSize());
}

template <class TYPE>
inline const TYPE* Array<TYPE>::GetBuffer() const
{
	return buffer;
}

template <class TYPE>
inline Array<TYPE>::operator const TYPE*() const
{
	return buffer;
}

template <class TYPE>
inline TYPE* Array<TYPE>::LockBuffer()
{
	CopyBeforeWrite();

	--GetMetaData()->refs;

	return buffer;
}

template <class TYPE>
inline void Array<TYPE>::UnlockBuffer()
{
	B_ASSERT(IsLocked());

	++GetMetaData()->refs;
}

template <class TYPE>
inline const TYPE& Array<TYPE>::GetAt(size_t index) const
{
	B_ASSERT(index >= 0 && index < GetSize());

	return buffer[index];
}

template <class TYPE>
inline const TYPE& Array<TYPE>::operator [](size_t index) const
{
	B_ASSERT(index >= 0 && index < GetSize());

	return buffer[index];
}

template <class TYPE>
inline TYPE& Array<TYPE>::GetAt(size_t index)
{
	B_ASSERT(index >= 0 && index < GetSize());

	CopyBeforeWrite();
	return buffer[index];
}

template <class TYPE>
inline TYPE& Array<TYPE>::operator [](size_t index)
{
	B_ASSERT(index >= 0 && index < GetSize());

	CopyBeforeWrite();
	return buffer[index];
}

template <class TYPE>
inline const TYPE& Array<TYPE>::GetHead() const
{
	B_ASSERT(!IsEmpty());

	return *buffer;
}

template <class TYPE>
inline TYPE& Array<TYPE>::GetHead()
{
	B_ASSERT(!IsEmpty());

	CopyBeforeWrite();
	return *buffer;
}

template <class TYPE>
inline const TYPE& Array<TYPE>::GetTail() const
{
	B_ASSERT(!IsEmpty());

	return buffer[GetSize() - 1];
}

template <class TYPE>
inline TYPE& Array<TYPE>::GetTail()
{
	B_ASSERT(!IsEmpty());

	CopyBeforeWrite();
	return buffer[GetSize() - 1];
}

template <class TYPE>
inline Array<TYPE>& Array<TYPE>::operator =(const Array<TYPE>& source)
{
	Assign(source);
	return *this;
}

template <class TYPE>
inline Array<TYPE>& Array<TYPE>::operator =(const TYPE& element)
{
	Assign(element);
	return *this;
}

template <class TYPE>
void Array<TYPE>::SetAt(size_t index, const Array<TYPE>& source)
{
	SetAt(index, source.GetBuffer(), source.GetSize());
}

template <class TYPE>
inline void Array<TYPE>::InsertAt(size_t index, const Array<TYPE>& source)
{
	InsertAt(index, source.GetBuffer(), source.GetSize());
}

template <class TYPE>
inline void Array<TYPE>::Append(const Array<TYPE>& source)
{
	Append(source.buffer, source.GetSize());
}

template <class TYPE>
inline Array<TYPE>& Array<TYPE>::operator +=(const Array<TYPE>& source)
{
	Append(source);
	return *this;
}

template <class TYPE>
inline Array<TYPE>& Array<TYPE>::operator +=(const TYPE& element)
{
	Append(element);
	return *this;
}

template <class TYPE>
inline Array<TYPE> Array<TYPE>::
	operator +(const Array<TYPE>& source) const
{
	Array<TYPE> result(*this);
	return result += source;
}

template <class TYPE>
inline Array<TYPE> Array<TYPE>::operator +(const TYPE& element) const
{
	Array<TYPE> result(*this);
	return result += element;
}

template <class TYPE>
inline size_t Array<TYPE>::Inc(size_t size)
{
	B_ASSERT(size >= 0);

	size_t increment;

	return size + ((increment = size >> 3) > 0x4 ?
		(increment <= 0x400 ? increment : 0x400) : 0x4);
}

template <class TYPE>
inline bool Array<TYPE>::IsShared() const
{
	return GetMetaData()->refs > 0;
}

template <class TYPE>
inline TYPE* Array<TYPE>::AllocBuffer(size_t capacity)
{
	return AllocBufferExactly(Inc(capacity));
}

template <class TYPE>
inline typename Array<TYPE>::MetaData*
	Array<TYPE>::GetMetaData(const TYPE* buffer)
{
	return static_cast<MetaData*>(static_cast<Buffer*>(
		reinterpret_cast<Wrapper*>(const_cast<TYPE*>(buffer))));
}

template <class TYPE>
inline typename Array<TYPE>::MetaData* Array<TYPE>::GetMetaData() const
{
	return GetMetaData(buffer);
}

template <class TYPE>
inline void Array<TYPE>::ReplaceBuffer(TYPE* new_buffer)
{
	Release();

	buffer = new_buffer;
}

template <class TYPE>
inline void Array<TYPE>::CopyBeforeWrite()
{
	if (IsShared())
		Realloc(GetSize());
}

template <class TYPE>
inline Array<TYPE>::~Array()
	throw ()
{
	Release();
}

B_END_NAMESPACE

#endif // B_ARRAY_DEFINED

#include "array_impl.h"
