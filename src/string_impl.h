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

B_BEGIN_NAMESPACE

void string::AllocExactly(size_t capacity)
{
	B_ASSERT(!IsLocked());

	if (GetCapacity() != capacity || IsShared())
	{
		// Release previous buffer before allocating a new one.
		ReplaceBuffer(GetEmptyBuffer());

		if (capacity > 0)
			// No need to call ReplaceBuffer again:
			// "ReplaceBuffer(AllocBufferExactly(capacity));"
			// It does the same thing as the next line:
			buffer = AllocBufferExactly(capacity);
	}
	else
	{
		GetData()->length = 0;
		*buffer = 0;
	}
}

void string::ReallocExactly(size_t capacity)
{
	B_ASSERT(!IsLocked());

	// Even if the array already has the capacity requested,
	// if the buffer is shared, it must be reallocated.
	// This behavior is used by other functions.
	if (GetCapacity() != capacity || IsShared())
	{
		size_t length = GetLength() < capacity ? GetLength() : capacity;

		if (capacity > 0)
		{
			char* new_buffer = AllocBufferExactly(capacity);

			Copy(new_buffer, buffer, (GetData(
				new_buffer)->length = length) + 1);

			ReplaceBuffer(new_buffer);
		}
		else
			ReplaceBuffer(GetEmptyBuffer());
	}
}

void string::Assign(const string& source)
{
	if (!IsLocked() && !source.IsLocked())
	{
		if (source.buffer != GetEmptyBuffer())
			++source.GetData()->refs;

		ReplaceBuffer(source.buffer);
	}
	else
		if (buffer != source.buffer)
			Assign(source.GetBuffer(), source.GetLength());
}

void string::Assign(const char* source, size_t count)
{
	B_ASSERT(!IsLocked());

	if (count > 0)
	{
		if (IsShared() || count > GetCapacity())
			Alloc(count);
		Copy(buffer, source, count);
		buffer[GetData()->length = count] = 0;
	}
	else
		Empty();
}

void string::Assign(char source, size_t count)
{
	B_ASSERT(!IsLocked());

	if (count > 0)
	{
		if (IsShared() || count > GetCapacity())
			Alloc(count);
		Copy(buffer, source, count);
		buffer[GetData()->length = count] = 0;
	}
	else
		Empty();
}

/*
void string::Assign(const char* source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Copy(Allocate(count), source, count);
		UnlockBuffer();
	}
	else
		Empty();
}

void string::Assign(char source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Copy(Allocate(count), source, count);
		UnlockBuffer();
	}
	else
		Empty();
}

void string::Replace(size_t index, const char* source, size_t count)
{
	B_ASSERT(index >= 0 && index <= GetLength() && count >= 0);

	size_t unaffected = index + count;

	Copy((unaffected <= GetLength() ? LockBuffer() :
		LockBuffer(unaffected)) + index, source, count);

	UnlockBuffer();
}

void string::Replace(size_t index, char source, size_t count)
{
	B_ASSERT(index >= 0 && index <= GetLength() && count >= 0);

	size_t unaffected = index + count;

	Copy((unaffected <= GetLength() ? LockBuffer() :
		LockBuffer(unaffected)) + index, source, count);

	UnlockBuffer();
}
*/

void string::Insert(size_t index, const char* source, size_t count)
{
	B_ASSERT(index <= GetLength());
	// source must not be a part of this array
	B_ASSERT(source >= buffer + GetCapacity() || source + count < buffer);

	if (count > 0)
	{
		char* tail = buffer + index;
		size_t tail_length = GetLength() - index;
		size_t new_length = GetLength() + count;

		if (new_length > GetCapacity() || IsShared())
		{
			char* new_buffer = AllocBuffer(new_length);

			Copy(new_buffer, buffer, index);
			Copy(new_buffer + index, source, count);
			Copy(new_buffer + index + count,
				tail, tail_length);

			ReplaceBuffer(new_buffer);
		}
		else
		{
			if (count < tail_length)
			{
				Copy(tail + tail_length,
					tail + tail_length - count, count);

				ReverseCopy(tail + count,
					tail, tail_length - count);

				Copy(tail, source, count);
			}
			else
			{
				Copy(tail + tail_length, source + tail_length,
					count - tail_length);

				Copy(tail + count, tail, tail_length);

				Copy(tail, source, tail_length);
			}

			GetData()->length = new_length;
		}
	}
}

void string::Insert(size_t index, char source, size_t count)
{
	B_ASSERT(index <= GetLength());

	if (count > 0)
	{
		char* tail = buffer + index;
		size_t tail_length = GetLength() - index;
		size_t new_length = GetLength() + count;

		if (new_length > GetCapacity() || IsShared())
		{
			char* new_buffer = AllocBuffer(new_length);

			Copy(new_buffer, buffer, index);
			Copy(new_buffer + index, source, count);
			Copy(new_buffer + index + count,
				tail, tail_length);

			ReplaceBuffer(new_buffer);
		}
		else
		{
			if (count < tail_length)
			{
				Copy(tail + tail_length,
					tail + tail_length - count, count);

				ReverseCopy(tail + count,
					tail, tail_length - count);

				Copy(tail, source, count);
			}
			else
			{
				Copy(tail + tail_length,
					source, count - tail_length);

				Copy(tail + count, tail, tail_length);

				Copy(tail, source, tail_length);
			}

			GetData()->length = new_length;
		}
	}
}

void string::Append(const char* source, size_t count)
{
	B_ASSERT(!IsLocked());

	if (count > 0)
	{
		if (IsShared() || count + GetLength() > GetCapacity())
			Realloc(GetLength() + count);
		Copy(buffer + GetLength(), source, count);
		buffer[GetData()->length += count] = 0;
	}
}

void string::Append(char source, size_t count)
{
	B_ASSERT(!IsLocked());

	if (count > 0)
	{
		if (IsShared() || count + GetLength() > GetCapacity())
			Realloc(GetLength() + count);
		Copy(buffer + GetLength(), source, count);
		buffer[GetData()->length += count] = 0;
	}
}

/*
void string::Append(const char* source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Lock(GetLength() + count);
		Copy(buffer + GetLength(), source, count);
		Unlock();
	}
}

void string::Append(char source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Lock(GetLength() + count);
		Copy(buffer + GetLength(), source, count);
		UnlockBuffer();
	}
}
*/

string string::operator +(const string& source) const
{
	string result(*this);
	return result += source;
}

void string::Delete(size_t index, size_t count)
{
	if (index + count > GetLength())
		count = GetLength() - index;

	if (count > 0)
	{
		size_t new_length = GetLength() - count;

		if (!IsShared())
		{
			Copy(buffer + index, buffer + index + count,
				new_length - index + 1);

			GetData()->length = new_length;
		}
		else
		{
			char* new_buffer = AllocBuffer(new_length);

			Copy(new_buffer, buffer, index);

			Copy(new_buffer + index, buffer + index + count,
				new_length - index + 1);

			ReplaceBuffer(new_buffer);
		}
	}
}

void string::Empty()
{
	if (!IsShared())
	{
		GetData()->length = 0;
		*buffer = 0;
	}
	else
		ReplaceBuffer(GetEmptyBuffer());
}

void string::AppendFormatV(const char* format, va_list arguments)
{
	ReallocExactly(GetLength() + 8 * 1024);

	GetData()->length += FormatString(buffer + GetLength(),
		format, arguments);
}

void string::AppendFormat(const char* format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	AppendFormatV(format, arguments);
	va_end(arguments);
}

void string::Format(const char* format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	FormatV(format, arguments);
	va_end(arguments);
}

size_t string::Find(char c) const
{
	size_t counter = GetLength();

	const char* pos = buffer;

	while (counter-- > 0)
	{
		if (*pos == c)
			return (size_t) (pos - buffer);

		++pos;
	}

	return (size_t) -1;
}

size_t string::ReverseFind(char c) const
{
	size_t index = GetLength();

	const char* pos = buffer + index;

	while (index-- > 0)
		if (*--pos == c)
			return (size_t) (pos - buffer);

	return (size_t) -1;
}

void string::TrimRight(const char* samples)
{
	char* end = buffer + GetLength();

	while (--end >= buffer &&
		FindCharInString(samples, *end) != NULL)
		;

	size_t new_length = ++end - buffer;

	if (new_length < GetLength())
	{
		if (!IsShared())
			*end = 0;
		else
		{
			char* new_buffer = AllocBuffer(new_length);

			Copy(new_buffer, buffer, new_length);

			ReplaceBuffer(new_buffer);

			new_buffer[new_length] = 0;
		}

		GetData()->length = new_length;
	}
}

void string::TrimLeft(const char* samples)
{
	char* start = buffer;

	while (*start && FindCharInString(samples, *start) != NULL)
		++start;

	if (start > buffer)
	{
		size_t new_length = buffer + GetLength() - start;

		if (!IsShared())
			ReverseCopy(buffer, start, new_length);
		else
		{
			char* new_buffer = AllocBuffer(new_length);

			Copy(new_buffer, start, new_length);

			ReplaceBuffer(new_buffer);
		}

		buffer[GetData()->length = new_length] = 0;
	}
}

char* string::GetEmptyBuffer()
{
	static const Data empty_data =
	{
		/* refs         */ B_REFCOUNT_STATIC_INIT(2),
		/* capacity     */ 0,
		/* length       */ 0,
		/* buffer       */ {0}
	};

	return const_cast<char*>(empty_data.buffer);
}

char* string::AllocBufferExactly(size_t capacity)
{
	Data* new_data = (Data*) Memory::Alloc(sizeof(Data) +
		capacity * sizeof(char));

	new_data->refs = 1;
	new_data->capacity = capacity;
	new_data->length = 0;

	return new_data->buffer;
}

void string::ReplaceBuffer(char* new_buffer)
{
	B_ASSERT(!IsLocked());

	if (buffer != GetEmptyBuffer() && !--GetData()->refs)
		Memory::Free(GetData());

	buffer = new_buffer;
}

string::~string()
	throw ()
{
	if (buffer != GetEmptyBuffer() && (IsLocked() || !--GetData()->refs))
		Memory::Free(GetData());
}

char* FindCharInString(const char* string, char c)
{
	while (*string != 0)
		if (*string++ == c)
			return const_cast<char*>(string);

	return NULL;
}

char* FindLastCharInString(const char* string, char c)
{
	size_t index = CalcLength(string);

	string += index;

	while (index-- > 0)
		if (*--string == c)
			return const_cast<char*>(string);

	return NULL;
}

B_END_NAMESPACE
