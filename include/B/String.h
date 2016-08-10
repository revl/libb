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

#ifndef B_STRING_H
#define B_STRING_H

#include "RefCount.h"
#include "Helpers.h"

#undef B_STRING_H

#ifdef B_DECLARE_STRING

B_BEGIN_NAMESPACE

// String class template
class B_API String
{
// Construction
public:
	// Creates an empty string.
	String();

	// Constructs a copy of an existing string.
	String(const String& source);

	// Constructs a string from a linear character array.
	String(const Char* source, int count);

	// Creates a string of <count> copies of source.
	explicit String(Char source, int count = 1);

// String Length
public:
	// Returns the size (in characters) of the allocated buffer.
	int GetCapacity() const;

	// Returns the string length in characters.
	int GetLength() const;

	// Returns true if the string is empty.
	bool IsEmpty() const;

// Lock State
public:
	// Checks if the string is locked.
	bool IsLocked() const;

// Memory Management
public:
	// Discards the string contents and allocates the exact
	// amount of memory for the buffer.
	void AllocExactly(int capacity);

	// Reallocates the buffer preserving the string contents.
	void ReallocExactly(int capacity);

	// Does the same as AllocExactly(), but also allocates some
	// extra characters for future string expansion.
	void Alloc(int capacity);

	// The same as ReallocExactly(), but also allocates some
	// extra characters for future string expansion.
	void Realloc(int capacity);

	// Allocates enough memory to store <capacity> characters.
	// In case if the buffer gets reallocated, the contents is
	// not preserved.
	void Reserve(int capacity);

	// Frees memory that is not used by the string.
	void FreeExtra();

// Buffer Access
public:
	// Returns a constant pointer to the array of characters.
	const Char* GetBuffer() const;

	// Converts to a const char pointer.
	operator const Char*() const;

	// Fixes the buffer in memory disabling the memory reallocation.
	Char* LockBuffer();

	// Gives control over the buffer back to this object.
	void UnlockBuffer();

	// Unlocks the buffer and sets the new string length.
	void UnlockBuffer(int new_length);

// Single Character Access
public:
	// Returns the character with the specified index.
	Char GetAt(int index) const;

	// Returns the character with the specified index.
	Char operator [](int index) const;

	// Overwrites character in position specified by <index>.
	void SetAt(int index, Char value);

	// Returns a reference to the specified character.
	Char& operator [](int index);

	// Returns the first character of the string.
	Char GetHead() const;

	// Returns a reference to the first character of the string.
	Char& GetHead();

	// Returns the last character of the string.
	Char GetTail() const;

	// Returns a reference to the last character of the string.
	Char& GetTail();

// Assignment
public:
	// Assigns the contents of <source> to this string.
	void Assign(const String& source);

	// Replaces the string contents with a copy of character array.
	void Assign(const Char* source, int count);

	// Replaces the string contents with <count> copies of <source>.
	void Assign(Char source, int count = 1);

	// Assigns the contents of one string object to another.
	String& operator =(const String& source);

	// Replaces the contents of this string with a character.
	String& operator =(Char source);

// Replacement
public:
	// Replaces a part of this string with a character sequence.
	void Replace(int index, const Char* source, int count);

	// Replaces a part of this string with a fill character.
	void Replace(int index, Char source, int count = 1);

// Insertion
public:
	// Inserts a character array into this string.
	void Insert(int index, const Char* source, int count);

	// Inserts the contents of <source> into this string.
	void Insert(int index, const String& source);

	// Inserts <count> copies of <source> into this string.
	void Insert(int index, Char source, int count = 1);

// Concatenation
public:
	// Appends characters from <source> to this string.
	void Append(const Char* source, int count);

	// Appends the contents of <source> at the end of this string.
	void Append(const String& source);

	// Appends <count> copies of <source> to this string.
	void Append(Char source, int count = 1);

	// Operator version of Append(source).
	String& operator +=(const String& source);

	// Operator version of Append(source).
	String& operator +=(Char source);

	// Constructs a new string from the concatenation of
	// this one and <source>.
	String operator +(const String& source) const;

	// Constructs a new string from the concatenation of
	// this one and character <source>.
	String operator +(Char source) const;

// Deletion
public:
	// Deletes a range of characters.
	void Delete(int index, int count = 1);

	// Empties the string.
	void Empty();

// Comparison
public:
	// Compares this string against <right_side> lexicographically.
	int Compare(const Char* right_side) const;

	// Returns true if this string and <right_side> are equal.
	bool operator ==(const String& right_side) const;

	// Checks this string and result for equality.
	bool operator ==(const Char* right_side) const;

	// Returns true if this string and <right_side> differ.
	bool operator !=(const String& right_side) const;

	// Returns true if this string and <right_side> differ.
	bool operator !=(const Char* right_side) const;

	// Returns true if this string is less than <right_side>.
	bool operator <(const String& right_side) const;

	// Returns true if this string is less than <right_side>.
	bool operator <(const Char* right_side) const;

	// Returns true if this string is greater than <right_side>.
	bool operator >(const String& right_side) const;

	// Returns true if this string is greater than <right_side>.
	bool operator >(const Char* right_side) const;

	// Returns true if this string is less or equal than <right_side>.
	bool operator <=(const String& right_side) const;

	// Returns true if this string is less or equal than <right_side>.
	bool operator <=(const Char* right_side) const;

	// Returns true if this string is greater or equal than <right_side>.
	bool operator >=(const String& right_side) const;

	// Returns true if this string is greater or equal than <right_side>.
	bool operator >=(const Char* right_side) const;

// Formatting
public:
	// Appends formatted data to this string.
	void AppendFormatV(const Char* format, va_list arguments);

	// Formats this string like vsprintf does.
	void FormatV(const Char* format, va_list arguments);

	// Appends formatted data to this string.
	void AppendFormat(const Char* format, ...);

	// Formats this string like sprintf does.
	void Format(const Char* format, ...);

// Searching
public:
	// Finds a character in this string.
	int Find(Char c) const;

	// Finds the last occurrence of a character in this string.
	int ReverseFind(Char c) const;

// Character Removal Operations
public:
	// Removes all occurrences of characters from <samples>
	// from the end of this string.
	void TrimRight(const Char* samples);

	// Removes all occurrences of characters from <samples>
	// found at the beginning of this string.
	void TrimLeft(const Char* samples);

	// Removes all characters from <samples> from both ends of this string.
	void Trim(const Char* samples);

// Implementation
protected:
	struct Data
	{
		RefCount refs;
		int capacity;
		int length;
		Char buffer[1];
	};

	Char* buffer;

	static int Inc(int length);

	bool IsShared() const;

	static Char* GetEmptyBuffer();

	static Char* AllocBufferExactly(int capacity);

	static Char* AllocBuffer(int length);

	static Data* GetData(const Char* buffer);
	Data* GetData() const;

	void ReplaceBuffer(Char* new_buffer);

	void CopyBeforeWrite();

public:
	~String()
		throw ();
};

inline String::String() : buffer(GetEmptyBuffer())
{
}

inline String::String(const String& source) : buffer(GetEmptyBuffer())
{
	Assign(source);
}

inline String::String(const Char* source, int count) : buffer(GetEmptyBuffer())
{
	Assign(source, count);
}

inline String::String(Char source, int count) : buffer(GetEmptyBuffer())
{
	Assign(source, count);
}

inline String::Data* String::GetData(const Char* buffer)
{
	return B_OUTERSTRUCT(Data, buffer[0], buffer);
}

inline String::Data* String::GetData() const
{
	return GetData(buffer);
}

inline bool String::IsLocked() const
{
	return GetData()->refs <= 0;
}

inline int String::GetCapacity() const
{
	return GetData()->capacity;
}

inline int String::GetLength() const
{
	return GetData()->length;
}

inline bool String::IsEmpty() const
{
	return GetLength() == 0;
}

inline int String::Inc(int length)
{
	B_ASSERT(length >= 0);

	int increment;

	return length + ((increment = length >> 3) > 0x4 ?
		(increment <= 0x400 ? increment : 0x400) : 0x4);
}

inline void String::Alloc(int capacity)
{
	AllocExactly(Inc(capacity));
}

inline void String::Realloc(int capacity)
{
	ReallocExactly(Inc(capacity));
}

inline bool String::IsShared() const
{
	return GetData()->refs > 1;
}

inline void String::Reserve(int capacity)
{
	if (GetCapacity() < capacity || IsShared())
		Realloc(capacity);
}

inline void String::FreeExtra()
{
	int length = GetLength();

	if (!IsShared() && length != GetCapacity())
	{
		Char* new_buffer = AllocBufferExactly(length);

		Copy(new_buffer, buffer, length);

		ReplaceBuffer(new_buffer);
	}
}

inline const Char* String::GetBuffer() const
{
	return buffer;
}

inline String::operator const Char*() const
{
	return buffer;
}

inline Char* String::AllocBuffer(int length)
{
	Char* new_buffer = AllocBufferExactly(Inc(length));
	new_buffer[GetData(new_buffer)->length = length] = 0;

	return new_buffer;
}

inline void String::CopyBeforeWrite()
{
	if (IsShared())
	{
		Char* new_buffer = AllocBuffer(GetLength());

		Copy(new_buffer, buffer, GetLength());

		ReplaceBuffer(new_buffer);
	}
}

inline Char* String::LockBuffer()
{
	CopyBeforeWrite();

	--GetData()->refs;

	return buffer;
}

inline void String::UnlockBuffer()
{
	B_ASSERT(IsLocked());

	++GetData()->refs;
}

inline void String::UnlockBuffer(int new_length)
{
	B_ASSERT(IsLocked() && new_length <= GetCapacity());

	buffer[GetData()->length = new_length] = 0;
	++GetData()->refs;
}

inline Char String::GetAt(int index) const
{
	B_ASSERT(index >= 0 &&
		index < (IsLocked() ? GetCapacity() : GetLength()));

	return buffer[index];
}

inline Char String::operator [](int index) const
{
	B_ASSERT(index >= 0 &&
		index < (IsLocked() ? GetCapacity() : GetLength()));

	return buffer[index];
}

inline void String::SetAt(int index, Char value)
{
	B_ASSERT(index >= 0 &&
		index < (IsLocked() ? GetCapacity() : GetLength()));

	CopyBeforeWrite();
	buffer[index] = value;
}

inline Char& String::operator [](int index)
{
	B_ASSERT(index >= 0 &&
		index < (IsLocked() ? GetCapacity() : GetLength()));

	CopyBeforeWrite();
	return buffer[index];
}

inline Char String::GetHead() const
{
	B_ASSERT(!IsEmpty());

	return *buffer;
}

inline Char& String::GetHead()
{
	B_ASSERT(!IsEmpty());

	CopyBeforeWrite();
	return *buffer;
}

inline Char String::GetTail() const
{
	B_ASSERT(!IsEmpty());

	return buffer[GetLength() - 1];
}

inline Char& String::GetTail()
{
	B_ASSERT(!IsEmpty());

	CopyBeforeWrite();
	return buffer[GetLength() - 1];
}

inline String& String::operator =(const String& source)
{
	Assign(source);
	return *this;
}

inline String& String::operator =(Char source)
{
	Assign(source);
	return *this;
}

inline void String::Insert(int index, const String& source)
{
	Insert(index, source.GetBuffer(), source.GetLength());
}

inline void String::Append(const String& source)
{
	Append(source.GetBuffer(), source.GetLength());
}

inline String& String::operator +=(const String& source)
{
	Append(source.GetBuffer(), source.GetLength());
	return *this;
}

inline String& String::operator +=(Char source)
{
	Append(source);
	return *this;
}

inline String String::operator +(const String& source) const
{
	String result(*this);
	return result += source;
}

inline String String::operator +(Char source) const
{
	String result(*this);
	return result += source;
}

inline int String::Compare(const Char* right_side) const
{
	return CompareStrings(buffer, right_side);
}

inline bool String::operator ==(const String& right_side) const
{
	return Compare(right_side) == 0;
}

inline bool String::operator ==(const Char* right_side) const
{
	return Compare(right_side) == 0;
}

inline bool String::operator !=(const String& right_side) const
{
	return Compare(right_side) != 0;
}

inline bool String::operator !=(const Char* right_side) const
{
	return Compare(right_side) != 0;
}

inline bool String::operator <(const String& right_side) const
{
	return Compare(right_side) < 0;
}

inline bool String::operator <(const Char* right_side) const
{
	return Compare(right_side) < 0;
}

inline bool String::operator >(const String& right_side) const
{
	return Compare(right_side) > 0;
}

inline bool String::operator >(const Char* right_side) const
{
	return Compare(right_side) > 0;
}

inline bool String::operator <=(const String& right_side) const
{
	return Compare(right_side) <= 0;
}

inline bool String::operator <=(const Char* right_side) const
{
	return Compare(right_side) <= 0;
}

inline bool String::operator >=(const String& right_side) const
{
	return Compare(right_side) >= 0;
}

inline bool String::operator >=(const Char* right_side) const
{
	return Compare(right_side) >= 0;
}

inline void String::FormatV(const Char* format, va_list arguments)
{
	Empty();
	AppendFormatV(format, arguments);
}

inline void String::Trim(const Char* samples)
{
	TrimRight(samples);
	TrimLeft(samples);
}

inline String operator +(Char left_side, const String& right_side)
{
	String result;
	int length = right_side.GetLength();

	result.Alloc(length + 1);
	Char* buffer = result.LockBuffer();

	*buffer = left_side;
	Copy(buffer + 1, right_side.GetBuffer(), length);

	result.UnlockBuffer(length + 1);
	return result;
}

inline bool operator ==(const Char* left_side, const String& right_side)
{
	return right_side.Compare(left_side) == 0;
}

inline bool operator !=(const Char* left_side, const String& right_side)
{
	return right_side.Compare(left_side) != 0;
}

inline bool operator <(const Char* left_side, const String& right_side)
{
	return right_side.Compare(left_side) > 0;
}

inline bool operator >(const Char* left_side, const String& right_side)
{
	return right_side.Compare(left_side) < 0;
}

inline bool operator <=(const Char* left_side, const String& right_side)
{
	return right_side.Compare(left_side) >= 0;
}

inline bool operator >=(const Char* left_side, const String& right_side)
{
	return right_side.Compare(left_side) <= 0;
}

// Finds the first occurrence of character <c> in <string>.
B_API Char* FindCharInString(const Char* string, Char c);

// Finds the last occurrence of character <c> in <string>.
B_API Char* FindLastCharInString(const Char* string, Char c);

B_END_NAMESPACE

#else

#define B_DECLARE_STRING

#define Char wchar_t
#define String StringW
#include "String.h"
#undef String
#undef Char

#define Char char
#define String StringA
#include "String.h"
#undef String
#undef Char

#define B_DEFINE_STATIC_STRING_T(name, value) \
	static struct \
	{ \
		B::RefCount refs; \
		int capacity; \
		int length; \
		B_CHAR buffer[sizeof(value)]; \
	} \
	const name##Data = \
	{ \
		B_REFCOUNT_STATIC_INIT(-1), \
		sizeof(value) / sizeof(B_CHAR) - 1, \
		sizeof(value) / sizeof(B_CHAR) - 1, \
		value \
	}; \
	static const B_CHAR* name##Buffer = name##Data.buffer

#define B_DEFINE_STATIC_STRING(name, value) \
	B_DEFINE_STATIC_STRING_T(name, B_TEXT(value))

#define B_STATIC_STRING(name) (*(const B::String*) &name##Buffer)

#define B_STRING_H

#endif // B_DECLARE_STRING

#endif // !defined(B_STRING_H)
