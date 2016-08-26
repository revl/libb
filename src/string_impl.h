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
		ReplaceBuffer(empty_string());

		if (capacity > 0)
			// No need to call ReplaceBuffer again:
			// "ReplaceBuffer(AllocBufferExactly(capacity));"
			// It does the same thing as the next line:
			chars = AllocBufferExactly(capacity);
	}
	else
	{
		metadata()->length = 0;
		*chars = 0;
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
			char* new_buffer_chars = AllocBufferExactly(capacity);

			Copy(new_buffer_chars, chars, (metadata(
				new_buffer_chars)->length = length) + 1);

			ReplaceBuffer(new_buffer_chars);
		}
		else
			ReplaceBuffer(empty_string());
	}
}

void string::assign(const string& source)
{
	if (!IsLocked() && !source.IsLocked())
	{
		if (source.chars != empty_string())
			++source.metadata()->refs;

		ReplaceBuffer(source.chars);
	}
	else
		if (chars != source.chars)
			assign(source.GetBuffer(), source.GetLength());
}

void string::assign(const char* source, size_t count)
{
	B_ASSERT(!IsLocked());

	if (count > 0)
	{
		if (IsShared() || count > GetCapacity())
			Alloc(count);
		Copy(chars, source, count);
		chars[metadata()->length = count] = 0;
	}
	else
		clear();
}

void string::assign(char source, size_t count)
{
	B_ASSERT(!IsLocked());

	if (count > 0)
	{
		if (IsShared() || count > GetCapacity())
			Alloc(count);
		Copy(chars, source, count);
		chars[metadata()->length = count] = 0;
	}
	else
		clear();
}

/*
void string::assign(const char* source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Copy(Allocate(count), source, count);
		UnlockBuffer();
	}
	else
		clear();
}

void string::assign(char source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Copy(Allocate(count), source, count);
		UnlockBuffer();
	}
	else
		clear();
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
	B_ASSERT(source >= chars + GetCapacity() || source + count < chars);

	if (count > 0)
	{
		char* tail = chars + index;
		size_t tail_length = GetLength() - index;
		size_t new_length = GetLength() + count;

		if (new_length > GetCapacity() || IsShared())
		{
			char* new_buffer_chars = AllocBuffer(new_length);

			Copy(new_buffer_chars, chars, index);
			Copy(new_buffer_chars + index, source, count);
			Copy(new_buffer_chars + index + count,
				tail, tail_length);

			ReplaceBuffer(new_buffer_chars);
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

			metadata()->length = new_length;
		}
	}
}

void string::Insert(size_t index, char source, size_t count)
{
	B_ASSERT(index <= GetLength());

	if (count > 0)
	{
		char* tail = chars + index;
		size_t tail_length = GetLength() - index;
		size_t new_length = GetLength() + count;

		if (new_length > GetCapacity() || IsShared())
		{
			char* new_buffer_chars = AllocBuffer(new_length);

			Copy(new_buffer_chars, chars, index);
			Copy(new_buffer_chars + index, source, count);
			Copy(new_buffer_chars + index + count,
				tail, tail_length);

			ReplaceBuffer(new_buffer_chars);
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

			metadata()->length = new_length;
		}
	}
}

void string::append(const char* source, size_t count)
{
	B_ASSERT(!IsLocked());

	if (count > 0)
	{
		if (IsShared() || count + GetLength() > GetCapacity())
			Realloc(GetLength() + count);
		Copy(chars + GetLength(), source, count);
		chars[metadata()->length += count] = 0;
	}
}

void string::append(char source, size_t count)
{
	B_ASSERT(!IsLocked());

	if (count > 0)
	{
		if (IsShared() || count + GetLength() > GetCapacity())
			Realloc(GetLength() + count);
		Copy(chars + GetLength(), source, count);
		chars[metadata()->length += count] = 0;
	}
}

/*
void string::append(const char* source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Lock(GetLength() + count);
		Copy(chars + GetLength(), source, count);
		Unlock();
	}
}

void string::append(char source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Lock(GetLength() + count);
		Copy(chars + GetLength(), source, count);
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
			Copy(chars + index, chars + index + count,
				new_length - index + 1);

			metadata()->length = new_length;
		}
		else
		{
			char* new_buffer_chars = AllocBuffer(new_length);

			Copy(new_buffer_chars, chars, index);

			Copy(new_buffer_chars + index, chars + index + count,
				new_length - index + 1);

			ReplaceBuffer(new_buffer_chars);
		}
	}
}

void string::clear()
{
	if (!IsShared())
	{
		metadata()->length = 0;
		*chars = 0;
	}
	else
		ReplaceBuffer(empty_string());
}

void string::appendfv(const char* format, va_list arguments)
{
	ReallocExactly(GetLength() + 8 * 1024);

	metadata()->length += FormatString(chars + GetLength(),
		format, arguments);
}

void string::appendf(const char* format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	appendfv(format, arguments);
	va_end(arguments);
}

void string::format(const char* format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	formatv(format, arguments);
	va_end(arguments);
}

size_t string::Find(char c) const
{
	size_t counter = GetLength();

	const char* pos = chars;

	while (counter-- > 0)
	{
		if (*pos == c)
			return (size_t) (pos - chars);

		++pos;
	}

	return (size_t) -1;
}

size_t string::ReverseFind(char c) const
{
	size_t index = GetLength();

	const char* pos = chars + index;

	while (index-- > 0)
		if (*--pos == c)
			return (size_t) (pos - chars);

	return (size_t) -1;
}

void string::TrimRight(const char* samples)
{
	char* end = chars + GetLength();

	while (--end >= chars &&
		FindCharInString(samples, *end) != NULL)
		;

	size_t new_length = ++end - chars;

	if (new_length < GetLength())
	{
		if (!IsShared())
			*end = 0;
		else
		{
			char* new_buffer_chars = AllocBuffer(new_length);

			Copy(new_buffer_chars, chars, new_length);

			ReplaceBuffer(new_buffer_chars);

			new_buffer_chars[new_length] = 0;
		}

		metadata()->length = new_length;
	}
}

void string::TrimLeft(const char* samples)
{
	char* start = chars;

	while (*start && FindCharInString(samples, *start) != NULL)
		++start;

	if (start > chars)
	{
		size_t new_length = chars + GetLength() - start;

		if (!IsShared())
			ReverseCopy(chars, start, new_length);
		else
		{
			char* new_buffer_chars = AllocBuffer(new_length);

			Copy(new_buffer_chars, start, new_length);

			ReplaceBuffer(new_buffer_chars);
		}

		chars[metadata()->length = new_length] = 0;
	}
}

char* string::empty_string()
{
	static const buffer empty_string_buffer =
	{
		/* refs         */ B_REFCOUNT_STATIC_INIT(2),
		/* capacity     */ 0,
		/* length       */ 0,
		/* first_char   */ {0}
	};

	return const_cast<char*>(empty_string_buffer.first_char);
}

char* string::AllocBufferExactly(size_t capacity)
{
	buffer* new_buffer = (buffer*) Memory::Alloc(sizeof(buffer) +
		capacity * sizeof(char));

	new_buffer->refs = 1;
	new_buffer->capacity = capacity;
	new_buffer->length = 0;

	return new_buffer->first_char;
}

void string::ReplaceBuffer(char* new_buffer_chars)
{
	B_ASSERT(!IsLocked());

	if (chars != empty_string() && !--metadata()->refs)
		Memory::Free(metadata());

	chars = new_buffer_chars;
}

string::~string()
	throw ()
{
	if (chars != empty_string() && (IsLocked() || !--metadata()->refs))
		Memory::Free(metadata());
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
