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
template <class T>
class array
{
// Construction
public:
	// Creates an empty array.
	array();

	// Constructs a copy of an existing array object.
	array(const array<T>& source);

	// Constructs an array from a sequence of objects.
	array(const T* source, size_t count);

	// Creates an array of <count> copies of <element>.
	array(const T& element, size_t count = 1);

// Array Length
public:
	// Returns the number of elements for which memory has
	// been allocated.
	size_t capacity() const;

	// Returns the number of elements in the array.
	size_t size() const;

	// Checks if the array is empty.
	bool IsEmpty() const;

// Lock State
public:
	// Checks if the array is locked.
	bool IsLocked() const;

// Memory Management
public:
	// Allocates the exact amount of memory for the buffer.
	void AllocExactly(size_t new_capacity);

	// Reallocates the buffer preserving its contents.
	void ReallocExactly(size_t new_capacity);

	// Allocates enough memory to store <capacity> elements.
	void Alloc(size_t new_capacity);

	// Reallocates the buffer preserving its contents.
	void Realloc(size_t new_capacity);

	// Frees memory that is not used by the elements.
	void shrink_to_fit();

// Buffer Access
public:
	// Returns a constant pointer to the elements.
	const T* data() const;

	// Returns the same as data().
	operator const T*() const;

	// Fixes the buffer in memory and returns a modifiable pointer to it.
	T* LockBuffer();

	// Gives control over the buffer back to this object.
	void UnlockBuffer();

// Element Access
public:
	// Returns a constant reference to an element.
	const T& GetAt(size_t index) const;

	// Does the same thing as GetAt().
	const T& operator [](size_t index) const;

	// Returns a modifiable reference to an element.
	T& GetAt(size_t index);

	// Does the same thing as GetAt().
	T& operator [](size_t index);

	// Returns a constant reference to the first array element.
	const T& GetHead() const;

	// Returns a modifiable reference to the first array element.
	T& GetHead();

	// Returns a constant reference to the last array element.
	const T& GetTail() const;

	// Returns a modifiable reference to the last array element.
	T& GetTail();

// Assignment
public:
	// Assigns the contents of <source> to this object.
	void Assign(const array<T>& source);

	// Assigns a sequence of elements to this object.
	void Assign(const T* source, size_t count);

	// Creates an array with <count> copies of <element>.
	void Assign(const T& element, size_t count);

	// A shorter version of Assign(source).
	array<T>& operator =(const array<T>& source);

	// Overwrites array range with the contents of <source>.
	void SetAt(size_t index, const array<T>& source);

	// Overwrites array range with copies of elements
	// pointed to by <source>.
	void SetAt(size_t index, const T* source, size_t count);

	// Overwrites array range with <count> copies of <element>.
	void SetAt(size_t index, const T& element, size_t count = 1);

// Insertion
public:
	// Inserts the entire contents of <source> into this array
	// at the specified position.
	void InsertAt(size_t index, const array<T>& source);

	// Inserts elements pointed to by <source> at the specified position.
	void InsertAt(size_t index, const T* source, size_t count);

	// Inserts <count> copies of <element> at the specified position.
	void InsertAt(size_t index, const T& element, size_t count = 1);

// Appending
public:
	// Appends this array with copies of elements from <source>.
	void Append(const array<T>& source);

	// Appends this array with elements from <source>.
	void Append(const T* source, size_t count);

	// Appends this array with <count> copies of <element>.
	void Append(const T& element, size_t count = 1);

	// The same as Append(source).
	array<T>& operator +=(const array<T>& source);

	// The same as Append(element, 1).
	array<T>& operator +=(const T& element);

	// Concatenates this array with <source> and returns the
	// result as a new array object.
	array<T> operator +(const array<T>& source) const;

	// Appends <element> to the contents of this array and returns
	// the result as a new array object.
	array<T> operator +(const T& element) const;

// Removal
public:
	// Removes a number of elements at the specified position.
	void RemoveAt(size_t index, size_t count = 1);

	// Empties the array.
	void RemoveAll();

// Implementation
private:
	struct metadata
	{
		RefCount refs;
		size_t capacity;
		size_t size;
	};

	struct first_element_wrapper
	{
		T first;

		// The constructor is never used, because this structure
		// is never constructed explicitly.
		explicit first_element_wrapper(const T& value) : first(value)
		{
		}
	};

	struct buffer : public metadata, public first_element_wrapper
	{
		// The constructor is never used, because this structure
		// is never constructed explicitly.
		explicit buffer(const T& value) : first_element_wrapper(value)
		{
		}
	};

	T* data_ptr;

	static size_t Inc(size_t size);

	bool IsShared() const;

	static T* GetEmptyBuffer();

	static T* AllocBufferExactly(size_t capacity);

	static T* AllocBuffer(size_t capacity);

	static metadata* GetMetaData(const T* data_ptr);
	metadata* GetMetaData() const;

	void Release();

	void ReplaceBuffer(T* new_data);

	void CopyBeforeWrite();

public:
	~array()
		throw ();
};

template <class T>
inline array<T>::array() : data_ptr(GetEmptyBuffer())
{
}

template <class T>
inline array<T>::array(const array<T>& source) : data_ptr(GetEmptyBuffer())
{
	Assign(source);
}

template <class T>
inline array<T>::array(const T* source, size_t count) :
	data_ptr(GetEmptyBuffer())
{
	Assign(source, count);
}

template <class T>
inline array<T>::array(const T& element, size_t count) :
	data_ptr(GetEmptyBuffer())
{
	Assign(element, count);
}

template <class T>
inline size_t array<T>::capacity() const
{
	return GetMetaData()->capacity;
}

template <class T>
inline size_t array<T>::size() const
{
	return GetMetaData()->size;
}

template <class T>
inline bool array<T>::IsEmpty() const
{
	return size() == 0;
}

template <class T>
inline bool array<T>::IsLocked() const
{
	return GetMetaData()->refs < 0;
}

template <class T>
inline void array<T>::Alloc(size_t capacity)
{
	AllocExactly(Inc(capacity));
}

template <class T>
inline void array<T>::Realloc(size_t capacity)
{
	ReallocExactly(Inc(capacity));
}

template <class T>
inline void array<T>::shrink_to_fit()
{
	ReallocExactly(size());
}

template <class T>
inline const T* array<T>::data() const
{
	return data_ptr;
}

template <class T>
inline array<T>::operator const T*() const
{
	return data_ptr;
}

template <class T>
inline T* array<T>::LockBuffer()
{
	CopyBeforeWrite();

	--GetMetaData()->refs;

	return data_ptr;
}

template <class T>
inline void array<T>::UnlockBuffer()
{
	B_ASSERT(IsLocked());

	++GetMetaData()->refs;
}

template <class T>
inline const T& array<T>::GetAt(size_t index) const
{
	B_ASSERT(index < size());

	return data_ptr[index];
}

template <class T>
inline const T& array<T>::operator [](size_t index) const
{
	B_ASSERT(index < size());

	return data_ptr[index];
}

template <class T>
inline T& array<T>::GetAt(size_t index)
{
	B_ASSERT(index < size());

	CopyBeforeWrite();
	return data_ptr[index];
}

template <class T>
inline T& array<T>::operator [](size_t index)
{
	B_ASSERT(index < size());

	CopyBeforeWrite();
	return data_ptr[index];
}

template <class T>
inline const T& array<T>::GetHead() const
{
	B_ASSERT(!IsEmpty());

	return *data_ptr;
}

template <class T>
inline T& array<T>::GetHead()
{
	B_ASSERT(!IsEmpty());

	CopyBeforeWrite();
	return *data_ptr;
}

template <class T>
inline const T& array<T>::GetTail() const
{
	B_ASSERT(!IsEmpty());

	return data_ptr[size() - 1];
}

template <class T>
inline T& array<T>::GetTail()
{
	B_ASSERT(!IsEmpty());

	CopyBeforeWrite();
	return data_ptr[size() - 1];
}

template <class T>
inline array<T>& array<T>::operator =(const array<T>& source)
{
	Assign(source);
	return *this;
}

template <class T>
void array<T>::SetAt(size_t index, const array<T>& source)
{
	SetAt(index, source.data(), source.size());
}

template <class T>
inline void array<T>::InsertAt(size_t index, const array<T>& source)
{
	InsertAt(index, source.data(), source.size());
}

template <class T>
inline void array<T>::Append(const array<T>& source)
{
	Append(source.data_ptr, source.size());
}

template <class T>
inline array<T>& array<T>::operator +=(const array<T>& source)
{
	Append(source);
	return *this;
}

template <class T>
inline array<T>& array<T>::operator +=(const T& element)
{
	Append(element);
	return *this;
}

template <class T>
inline array<T> array<T>::operator +(const array<T>& source) const
{
	array<T> result(*this);
	return result += source;
}

template <class T>
inline array<T> array<T>::operator +(const T& element) const
{
	array<T> result(*this);
	return result += element;
}

template <class T>
inline size_t array<T>::Inc(size_t size)
{
	size_t increment;

	return size + ((increment = size >> 3) > 0x4 ?
		(increment <= 0x400 ? increment : 0x400) : 0x4);
}

template <class T>
inline bool array<T>::IsShared() const
{
	return GetMetaData()->refs > 0;
}

template <class T>
inline T* array<T>::AllocBuffer(size_t capacity)
{
	return AllocBufferExactly(Inc(capacity));
}

template <class T>
inline typename array<T>::metadata* array<T>::GetMetaData(const T* data)
{
	return static_cast<metadata*>(
		static_cast<buffer*>(
			reinterpret_cast<first_element_wrapper*>(
				const_cast<T*>(data))));
}

template <class T>
inline typename array<T>::metadata* array<T>::GetMetaData() const
{
	return GetMetaData(data_ptr);
}

template <class T>
inline void array<T>::ReplaceBuffer(T* new_data)
{
	Release();

	data_ptr = new_data;
}

template <class T>
inline void array<T>::CopyBeforeWrite()
{
	if (IsShared())
		Realloc(size());
}

template <class T>
inline array<T>::~array()
	throw ()
{
	Release();
}

B_END_NAMESPACE

#endif // B_ARRAY_DEFINED

#include "array_impl.h"
