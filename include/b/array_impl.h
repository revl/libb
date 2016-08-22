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

#ifndef B_ARRAY_SKIP_IMPLEMENTATION

#ifndef B_ARRAY_IMPLEMENTED
#define B_ARRAY_IMPLEMENTED

#include "array.h"

B_BEGIN_NAMESPACE

template <class T>
void array<T>::AllocExactly(size_t new_capacity)
{
	B_ASSERT(!IsLocked());

	if (capacity() != new_capacity || IsShared())
	{
		Release();

		data_ptr = new_capacity > 0 ?
			AllocBufferExactly(new_capacity) :
			empty_array();
	}
	else
	{
		// It makes no sense to allocate memory block
		// of the same size. However, all elements are
		// destroyed to simulate usual behavior.
		Destroy(data_ptr, size());
		GetMetaData()->size = 0;
	}
}

template <class T>
void array<T>::ReallocExactly(size_t new_capacity)
{
	B_ASSERT(!IsLocked());

	// Even if the array already has the capacity requested,
	// if the buffer is shared, it must be reallocated.
	if (capacity() != new_capacity || IsShared())
	{
		if (new_capacity > 0)
		{
			T* new_data = AllocBufferExactly(new_capacity);

			size_t new_array_size =
				size() < new_capacity ? size() : new_capacity;

			Construct(new_data, data_ptr,
				GetMetaData(new_data)->size = new_array_size);

			ReplaceBuffer(new_data);
		}
		else
			ReplaceBuffer(empty_array());
	}
}

template <class T>
void array<T>::Assign(const array<T>& source)
{
	if (!IsLocked() && !source.IsLocked())
	{
		if (source.data_ptr != empty_array())
			++source.GetMetaData()->refs;

		ReplaceBuffer(source.data_ptr);
	}
	else
		if (data_ptr != source.data_ptr)
			Assign(source.data(), source.size());
}

template <class T>
void array<T>::Assign(const T* source, size_t count)
{
	if (count > 0)
	{
		if (!IsShared() && count <= capacity())
			if (count > size())
			{
				Copy(data_ptr, source, size());
				Construct(data_ptr + size(),
					source + size(),
					count - size());
			}
			else
			{
				Copy(data_ptr, source, count);
				Destroy(data_ptr + count,
					size() - count);
			}
		else
		{
			Alloc(count);
			Construct(data_ptr, source, count);
		}

		GetMetaData()->size = count;
	}
	else
		clear();
}

template <class T>
void array<T>::Assign(const T& element, size_t count)
{
	if (count > 0)
	{
		if (!IsShared() && count <= capacity())
			if (count > size())
			{
				Copy(data_ptr, element, size());
				Construct(data_ptr + size(),
					element, count - size());
			}
			else
			{
				Copy(data_ptr, element, count);
				Destroy(data_ptr + count,
					size() - count);
			}
		else
		{
			Alloc(count);
			Construct(data_ptr, element, count);
		}

		GetMetaData()->size = count;
	}
	else
		clear();
}

template <class T>
void array<T>::SetAt(size_t index, const T* source, size_t count)
{
	B_ASSERT(index <= size());
	// The source array cannot be a part of this array
	B_ASSERT(source >= data_ptr + capacity() ||
		source + count < data_ptr);

	if (count > 0)
	{
		size_t tail_index = index + count;

		if (!IsShared())
		{
			if (tail_index <= size())
			{
				// This is the most frequent case,
				// that's why it is processed first.

				Copy(data_ptr + index, source, count);

				return;
			}
			else
				if (tail_index <= capacity())
				{
					Copy(data_ptr + index, source,
						size() - index);
					Construct(data_ptr + size(),
						source + size() - index,
						tail_index - size());

					GetMetaData()->size = tail_index;

					return;
				}
		}

		// Either the buffer is shared among instances or
		// index of the last element exceeds array boundaries.

		// The array must not be locked, because its buffer
		// is to be reallocated.
		B_ASSERT(!IsLocked());

		T* new_data;

		if (tail_index < size())
		{
			new_data = AllocBuffer(size());

			Construct(new_data + tail_index, data_ptr + tail_index,
				(GetMetaData(new_data)->size =
					size()) - tail_index);
		}
		else
		{
			new_data = AllocBuffer(tail_index);

			GetMetaData(new_data)->size = tail_index;
		}

		Construct(new_data, data_ptr, index);
		Construct(new_data + index, source, count);

		ReplaceBuffer(new_data);
	}
}

template <class T>
void array<T>::SetAt(size_t index, const T& element, size_t count)
{
	B_ASSERT(index <= size());

	if (count > 0)
	{
		size_t tail_index = index + count;

		if (!IsShared())
		{
			if (tail_index <= size())
			{
				Copy(data_ptr + index, element, count);

				return;
			}
			else
				if (tail_index <= capacity())
				{
					Copy(data_ptr + index, element,
						size() - index);

					Construct(data_ptr + size(), element,
						tail_index - size());

					GetMetaData()->size = tail_index;

					return;
				}
		}

		B_ASSERT(!IsLocked());

		T* new_data;

		if (tail_index < size())
		{
			new_data = AllocBuffer(size());

			Construct(new_data + tail_index, data_ptr + tail_index,
				(GetMetaData(new_data)->size =
					size()) - tail_index);
		}
		else
		{
			new_data = AllocBuffer(tail_index);

			GetMetaData(new_data)->size = tail_index;
		}

		Construct(new_data, data_ptr, index);
		Construct(new_data + index, element, count);

		ReplaceBuffer(new_data);
	}
}

template <class T>
void array<T>::InsertAt(size_t index, const T* source, size_t count)
{
	B_ASSERT(index <= size());
	// <source> must not be a part of this array
	B_ASSERT(source >= data_ptr + capacity() ||
		source + count < data_ptr);

	if (count > 0)
	{
		T* tail = data_ptr + index;
		size_t tail_size = size() - index;
		size_t new_size = size() + count;

		if (new_size > capacity() || IsShared())
		{
			T* new_data = AllocBuffer(new_size);

			Construct(new_data, data_ptr, index);
			Construct(new_data + index, source, count);
			Construct(new_data + index + count,
				tail, tail_size);

			ReplaceBuffer(new_data);
		}
		else
			if (count < tail_size)
			{
				Construct(tail + tail_size,
					tail + tail_size - count, count);

				ReverseCopy(tail + count,
					tail, tail_size - count);

				Copy(tail, source, count);
			}
			else
			{
				Construct(tail + tail_size, source + tail_size,
					count - tail_size);

				Construct(tail + count, tail, tail_size);

				Copy(tail, source, tail_size);
			}

		GetMetaData()->size = new_size;
	}
}

template <class T>
void array<T>::InsertAt(size_t index, const T& element, size_t count)
{
	B_ASSERT(index <= size());

	if (count > 0)
	{
		T* tail = data_ptr + index;
		size_t tail_size = size() - index;
		size_t new_size = size() + count;

		if (new_size > capacity() || IsShared())
		{
			T* new_data = AllocBuffer(new_size);

			Construct(new_data, data_ptr, index);
			Construct(new_data + index, element, count);
			Construct(new_data + index + count, tail, tail_size);

			ReplaceBuffer(new_data);
		}
		else
			if (count < tail_size)
			{
				Construct(tail + tail_size,
					tail + tail_size - count, count);

				ReverseCopy(tail + count,
					tail, tail_size - count);

				Copy(tail, element, count);
			}
			else
			{
				Construct(tail + tail_size,
					element, count - tail_size);

				Construct(tail + count, tail, tail_size);

				Copy(tail, element, tail_size);
			}

		GetMetaData()->size = new_size;
	}
}

template <class T>
void array<T>::Append(const T* source, size_t count)
{
	if (count > 0)
	{
		if (IsShared() || size() + count > capacity())
			Realloc(size() + count);

		Construct(data_ptr + size(), source, count);
		GetMetaData()->size += count;
	}
}

template <class T>
void array<T>::Append(const T& element, size_t count)
{
	if (count > 0)
	{
		if (IsShared() || size() + count > capacity())
			Realloc(size() + count);

		Construct(data_ptr + size(), element, count);
		GetMetaData()->size += count;
	}
}

template <class T>
void array<T>::RemoveAt(size_t index, size_t count)
{
	if (index + count > size())
		count = size() - index;

	if (count > 0)
	{
		size_t new_size = size() - count;

		if (!IsShared())
		{
			Copy(data_ptr + index, data_ptr + index + count,
				new_size - index);

			Destroy(data_ptr + new_size, count);
		}
		else
		{
			T* new_data = AllocBuffer(new_size);

			Construct(new_data, data_ptr, index);

			Construct(new_data + index, data_ptr + index + count,
				new_size - index);

			ReplaceBuffer(new_data);
		}

		GetMetaData()->size = new_size;
	}
}

template <class T>
void array<T>::clear()
{
	if (!IsShared())
	{
		Destroy(data_ptr, size());
		GetMetaData()->size = 0;
	}
	else
		ReplaceBuffer(empty_array());
}

template <class T>
T* array<T>::empty_array()
{
	static const metadata empty =
	{
		/* refs         */ B_REFCOUNT_STATIC_INIT(2),
		/* capacity     */ 0,
		/* size         */ 0
	};

	return &static_cast<buffer&>(const_cast<metadata&>(empty)).first;
}

template <class T>
T* array<T>::AllocBufferExactly(size_t capacity)
{
	buffer* new_buffer = (buffer*) Memory::Alloc((size_t)
		&((buffer*) (sizeof(T) * capacity))->first);

	new_buffer->refs = 0;
	new_buffer->capacity = capacity;
	new_buffer->size = 0;

	return &new_buffer->first;
}

template <class T>
void array<T>::Release()
{
	B_ASSERT(!IsLocked());

	if (data_ptr != empty_array() && !--GetMetaData()->refs)
	{
		Destroy(data_ptr, size());
		Memory::Free(GetMetaData());
	}
}

B_END_NAMESPACE

#endif // B_ARRAY_IMPLEMENTED

#else
#undef B_ARRAY_SKIP_IMPLEMENTATION
#endif // B_ARRAY_SKIP_IMPLEMENTATION
