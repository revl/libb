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

#include "Array.h"

B_BEGIN_NAMESPACE

template <class TYPE>
void Array<TYPE>::AllocExactly(int capacity)
{
	B_ASSERT(capacity >= 0);
	B_ASSERT(!IsLocked());

	if (GetCapacity() != capacity || IsShared())
	{
		Release();

		buffer = capacity > 0 ?
			AllocBufferExactly(capacity) :
			GetEmptyBuffer();
	}
	else
	{
		// It makes no sense to allocate memory block
		// of the same size. However, all elements are
		// destroyed to simulate usual behavior.
		Destroy(buffer, GetSize());
		GetMetaData()->size = 0;
	}
}

template <class TYPE>
void Array<TYPE>::ReallocExactly(int capacity)
{
	B_ASSERT(capacity >= 0);
	B_ASSERT(!IsLocked());

	// Even if the array already has the capacity requested,
	// if the buffer is shared, it must be reallocated.
	if (GetCapacity() != capacity || IsShared())
	{
		int size = GetSize() < capacity ? GetSize() : capacity;

		if (capacity > 0)
		{
			TYPE* new_buffer = AllocBufferExactly(capacity);

			Construct(new_buffer, buffer,
				GetMetaData(new_buffer)->size = size);

			ReplaceBuffer(new_buffer);
		}
		else
			ReplaceBuffer(GetEmptyBuffer());
	}
}

template <class TYPE>
void Array<TYPE>::Assign(const Array<TYPE>& source)
{
	if (!IsLocked() && !source.IsLocked())
	{
		if (source.buffer != GetEmptyBuffer())
			++source.GetMetaData()->refs;

		ReplaceBuffer(source.buffer);
	}
	else
		if (buffer != source.buffer)
			Assign(source.GetBuffer(), source.GetSize());
}

template <class TYPE>
void Array<TYPE>::Assign(const TYPE* source, int count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		if (!IsShared() && count <= GetCapacity())
			if (count > GetSize())
			{
				Copy(buffer, source, GetSize());
				Construct(buffer + GetSize(),
					source + GetSize(),
					count - GetSize());
			}
			else
			{
				Copy(buffer, source, count);
				Destroy(buffer + count,
					GetSize() - count);
			}
		else
		{
			Alloc(count);
			Construct(buffer, source, count);
		}

		GetMetaData()->size = count;
	}
	else
		RemoveAll();
}

template <class TYPE>
void Array<TYPE>::Assign(const TYPE& element, int count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		if (!IsShared() && count <= GetCapacity())
			if (count > GetSize())
			{
				Copy(buffer, element, GetSize());
				Construct(buffer + GetSize(),
					element, count - GetSize());
			}
			else
			{
				Copy(buffer, element, count);
				Destroy(buffer + count,
					GetSize() - count);
			}
		else
		{
			Alloc(count);
			Construct(buffer, element, count);
		}

		GetMetaData()->size = count;
	}
	else
		RemoveAll();
}

template <class TYPE>
void Array<TYPE>::SetAt(int index, const TYPE* source, int count)
{
	B_ASSERT(index >= 0 && index <= GetSize() && count >= 0);
	// The source buffer cannot be a part of this array
	B_ASSERT(source >= buffer + GetCapacity() ||
		source + count < buffer);

	if (count > 0)
	{
		int tail_index = index + count;

		if (!IsShared())
			if (tail_index <= GetSize())
			{
				// This is the most frequent case,
				// that's why it is processed first.

				Copy(buffer + index, source, count);

				return;
			}
			else
				if (tail_index <= GetCapacity())
				{
					Copy(buffer + index, source,
						GetSize() - index);
					Construct(buffer + GetSize(),
						source + GetSize() - index,
						tail_index - GetSize());

					GetMetaData()->size = tail_index;

					return;
				}

		// Either the buffer is shared among instances or
		// index of the last element exceeds array boundaries.

		// The array must not be locked, because its buffer
		// is to be reallocated.
		B_ASSERT(!IsLocked());

		TYPE* new_buffer;

		if (tail_index < GetSize())
		{
			new_buffer = AllocBuffer(GetSize());

			Construct(new_buffer + tail_index, buffer + tail_index,
				(GetMetaData(new_buffer)->size =
					GetSize()) - tail_index);
		}
		else
		{
			new_buffer = AllocBuffer(tail_index);

			GetMetaData(new_buffer)->size = tail_index;
		}

		Construct(new_buffer, buffer, index);
		Construct(new_buffer + index, source, count);

		ReplaceBuffer(new_buffer);
	}
}

template <class TYPE>
void Array<TYPE>::SetAt(int index, const TYPE& element, int count)
{
	B_ASSERT(index >= 0 && index <= GetSize() && count >= 0);

	if (count > 0)
	{
		int tail_index = index + count;

		if (!IsShared())
			if (tail_index <= GetSize())
			{
				Copy(buffer + index, element, count);

				return;
			}
			else
				if (tail_index <= GetCapacity())
				{
					Copy(buffer + index, element,
						GetSize() - index);

					Construct(buffer + GetSize(), element,
						tail_index - GetSize());

					GetMetaData()->size = tail_index;

					return;
				}

		B_ASSERT(!IsLocked());

		TYPE* new_buffer;

		if (tail_index < GetSize())
		{
			new_buffer = AllocBuffer(GetSize());

			Construct(new_buffer + tail_index, buffer + tail_index,
				(GetMetaData(new_buffer)->size =
					GetSize()) - tail_index);
		}
		else
		{
			new_buffer = AllocBuffer(tail_index);

			GetMetaData(new_buffer)->size = tail_index;
		}

		Construct(new_buffer, buffer, index);
		Construct(new_buffer + index, element, count);

		ReplaceBuffer(new_buffer);
	}
}

template <class TYPE>
void Array<TYPE>::InsertAt(int index, const TYPE* source, int count)
{
	B_ASSERT(index >= 0 && index <= GetSize() && count >= 0);
	// <source> must not be a part of this array
	B_ASSERT(source >= buffer + GetCapacity() ||
		source + count < buffer);

	if (count > 0)
	{
		TYPE* tail = buffer + index;
		int tail_size = GetSize() - index;
		int new_size = GetSize() + count;

		if (new_size > GetCapacity() || IsShared())
		{
			TYPE* new_buffer = AllocBuffer(new_size);

			Construct(new_buffer, buffer, index);
			Construct(new_buffer + index, source, count);
			Construct(new_buffer + index + count,
				tail, tail_size);

			ReplaceBuffer(new_buffer);
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

template <class TYPE>
void Array<TYPE>::InsertAt(int index, const TYPE& element, int count)
{
	B_ASSERT(index >= 0 && index <= GetSize() && count >= 0);

	if (count > 0)
	{
		TYPE* tail = buffer + index;
		int tail_size = GetSize() - index;
		int new_size = GetSize() + count;

		if (new_size > GetCapacity() || IsShared())
		{
			TYPE* new_buffer = AllocBuffer(new_size);

			Construct(new_buffer, buffer, index);
			Construct(new_buffer + index, element, count);
			Construct(new_buffer + index + count, tail, tail_size);

			ReplaceBuffer(new_buffer);
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

template <class TYPE>
void Array<TYPE>::Append(const TYPE* source, int count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		if (IsShared() || GetSize() + count > GetCapacity())
			Realloc(GetSize() + count);

		Construct(buffer + GetSize(), source, count);
		GetMetaData()->size += count;
	}
}

template <class TYPE>
void Array<TYPE>::Append(const TYPE& element, int count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		if (IsShared() || GetSize() + count > GetCapacity())
			Realloc(GetSize() + count);

		Construct(buffer + GetSize(), element, count);
		GetMetaData()->size += count;
	}
}

template <class TYPE>
void Array<TYPE>::RemoveAt(int index, int count)
{
	B_ASSERT(index >= 0 && count >= 0);

	if (index + count > GetSize())
		count = GetSize() - index;

	if (count > 0)
	{
		int new_size = GetSize() - count;

		if (!IsShared())
		{
			Copy(buffer + index, buffer + index + count,
				new_size - index);

			Destroy(buffer + new_size, count);
		}
		else
		{
			TYPE* new_buffer = AllocBuffer(new_size);

			Construct(new_buffer, buffer, index);

			Construct(new_buffer + index, buffer + index + count,
				new_size - index);

			ReplaceBuffer(new_buffer);
		}

		GetMetaData()->size = new_size;
	}
}

template <class TYPE>
void Array<TYPE>::RemoveAll()
{
	if (!IsShared())
	{
		Destroy(buffer, GetSize());
		GetMetaData()->size = 0;
	}
	else
		ReplaceBuffer(GetEmptyBuffer());
}

template <class TYPE>
TYPE* Array<TYPE>::GetEmptyBuffer()
{
	static const MetaData empty =
	{
		/* refs         */ B_REFCOUNT_STATIC_INIT(2),
		/* capacity     */ 0,
		/* size         */ 0
	};

	return &static_cast<Buffer&>(const_cast<MetaData&>(empty)).first;
}

template <class TYPE>
TYPE* Array<TYPE>::AllocBufferExactly(int capacity)
{
	Buffer* new_buffer = (Buffer*) Memory::Alloc((size_t)
		&((Buffer*) (sizeof(TYPE) * capacity))->first);

	new_buffer->capacity = capacity;
	new_buffer->refs = new_buffer->size = 0;

	return &new_buffer->first;
}

template <class TYPE>
void Array<TYPE>::Release()
{
	B_ASSERT(!IsLocked());

	if (buffer != GetEmptyBuffer() && !--GetMetaData()->refs)
	{
		Destroy(buffer, GetSize());
		Memory::Free(GetMetaData());
	}
}

B_END_NAMESPACE

#endif // B_ARRAY_IMPLEMENTED

#else
#undef B_ARRAY_SKIP_IMPLEMENTATION
#endif // B_ARRAY_SKIP_IMPLEMENTATION
