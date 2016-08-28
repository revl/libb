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

#include "ref_count.h"
#include "helpers.h"

#undef B_STRING_H

#ifdef B_DECLARE_STRING

B_BEGIN_NAMESPACE

// String class template
class string
{
// Construction
public:
	// Creates an empty string.
	string();

	// Constructs a copy of an existing string.
	string(const string& source);

	// Constructs a string from a linear character array.
	string(const char* source, size_t count);

	// Creates a string of <count> copies of source.
	explicit string(char source, size_t count = 1);

// String Length
public:
	// Returns the size (in characters) of the allocated buffer.
	size_t GetCapacity() const;

	// Returns the string length in characters.
	size_t GetLength() const;

	// Returns true if the string is empty.
	bool empty() const;

// Lock State
public:
	// Checks if the string is locked.
	bool IsLocked() const;

// Memory Management
public:
	// Discards the string contents and allocates the exact
	// amount of memory for the buffer.
	void discard_and_alloc(size_t capacity);

	// Reallocates the buffer preserving the string contents.
	void alloc_and_copy(size_t capacity);

	// Does the same as discard_and_alloc(), but also allocates some
	// extra characters for future string expansion.
	void Alloc(size_t capacity);

	// The same as alloc_and_copy(), but also allocates some
	// extra characters for future string expansion.
	void Realloc(size_t capacity);

	// Allocates enough memory to store 'capacity' characters.
	// In case if the buffer gets reallocated, the contents is
	// not preserved.
	void reserve(size_t capacity);

	// Frees memory that is not used by the string.
	void shrink_to_fit();

// Buffer Access
public:
	// Returns a constant pointer to the array of characters.
	const char* GetBuffer() const;

	// Converts to a const char pointer.
	operator const char*() const;

	// Fixes the buffer in memory disabling the memory reallocation.
	char* LockBuffer();

	// Gives control over the buffer back to this object.
	void UnlockBuffer();

	// Unlocks the buffer and sets the new string length.
	void UnlockBuffer(size_t new_length);

// Single Character Access
public:
	// Returns the character with the specified index.
	char GetAt(size_t index) const;

	// Returns the character with the specified index.
	char operator [](size_t index) const;

	// Overwrites character in position specified by <index>.
	void SetAt(size_t index, char value);

	// Returns a reference to the specified character.
	char& operator [](size_t index);

	// Returns the first character of the string.
	char GetHead() const;

	// Returns a reference to the first character of the string.
	char& GetHead();

	// Returns the last character of the string.
	char GetTail() const;

	// Returns a reference to the last character of the string.
	char& GetTail();

// Assignment
public:
	// Assigns the contents of <source> to this string.
	void assign(const string& source);

	// Replaces the string contents with a copy of character array.
	void assign(const char* source, size_t count);

	// Replaces the string contents with <count> copies of <source>.
	void assign(char source, size_t count = 1);

	// Assigns the contents of one string object to another.
	string& operator =(const string& source);

	// Replaces the contents of this string with a character.
	string& operator =(char source);

// Replacement
public:
	// Replaces a part of this string with a character sequence.
	void Replace(size_t index, const char* source, size_t count);

	// Replaces a part of this string with a fill character.
	void Replace(size_t index, char source, size_t count = 1);

// Insertion
public:
	// Inserts a character array into this string.
	void Insert(size_t index, const char* source, size_t count);

	// Inserts the contents of <source> into this string.
	void Insert(size_t index, const string& source);

	// Inserts <count> copies of <source> into this string.
	void Insert(size_t index, char source, size_t count = 1);

// Concatenation
public:
	// Appends characters from <source> to this string.
	void append(const char* source, size_t count);

	// Appends the contents of <source> at the end of this string.
	void append(const string& source);

	// Appends <count> copies of <source> to this string.
	void append(char source, size_t count = 1);

	// Operator version of append(source).
	string& operator +=(const string& source);

	// Operator version of append(source).
	string& operator +=(char source);

	// Constructs a new string from the concatenation of
	// this one and <source>.
	string operator +(const string& source) const;

	// Constructs a new string from the concatenation of
	// this one and character <source>.
	string operator +(char source) const;

// Deletion
public:
	// Deletes a range of characters.
	void erase(size_t index, size_t count = 1);

	// Empties the string.
	void clear();

// Comparison
public:
	// Compares this string against <right_side> lexicographically.
	int compare(const char* right_side) const;

	// Returns true if this string and <right_side> are equal.
	bool operator ==(const string& right_side) const;

	// Checks this string and result for equality.
	bool operator ==(const char* right_side) const;

	// Returns true if this string and <right_side> differ.
	bool operator !=(const string& right_side) const;

	// Returns true if this string and <right_side> differ.
	bool operator !=(const char* right_side) const;

	// Returns true if this string is less than <right_side>.
	bool operator <(const string& right_side) const;

	// Returns true if this string is less than <right_side>.
	bool operator <(const char* right_side) const;

	// Returns true if this string is greater than <right_side>.
	bool operator >(const string& right_side) const;

	// Returns true if this string is greater than <right_side>.
	bool operator >(const char* right_side) const;

	// Returns true if this string is less or equal than <right_side>.
	bool operator <=(const string& right_side) const;

	// Returns true if this string is less or equal than <right_side>.
	bool operator <=(const char* right_side) const;

	// Returns true if this string is greater or equal than <right_side>.
	bool operator >=(const string& right_side) const;

	// Returns true if this string is greater or equal than <right_side>.
	bool operator >=(const char* right_side) const;

// Formatting
public:
	// Appends formatted data to this string.
	void appendfv(const char* format, va_list arguments);

	// Formats this string like vsprintf does.
	void formatv(const char* format, va_list arguments);

	// Appends formatted data to this string.
	void appendf(const char* format, ...);

	// Formats this string like sprintf does.
	void format(const char* format, ...);

// Searching
public:
	// Finds a character in this string.
	size_t find(char c) const;

	// Finds the last occurrence of a character in this string.
	size_t rfind(char c) const;

// Character Removal Operations
public:
	// Removes all occurrences of characters from <samples>
	// from the end of this string.
	void TrimRight(const char* samples);

	// Removes all occurrences of characters from <samples>
	// found at the beginning of this string.
	void TrimLeft(const char* samples);

	// Removes all characters from <samples> from both ends of this string.
	void Trim(const char* samples);

// Implementation
protected:
	struct buffer
	{
		RefCount refs;
		size_t capacity;
		size_t length;
		char first_char[1];
	};

	char* chars;

	bool IsShared() const;

	static char* empty_string();

	static char* AllocBufferExactly(size_t capacity, size_t length);

	static char* AllocBuffer(size_t length);

	static buffer* metadata(const char* chars);
	buffer* metadata() const;

	void ReplaceBuffer(char* new_buffer_chars);

	// Make sure that the buffer is not shared with other strings.
	// Reallocate the buffer if it's shared; keep the original
	// contents.
	void isolate();

public:
	~string()
		throw ();
};

inline string::string() : chars(empty_string())
{
}

inline string::string(const string& source) : chars(empty_string())
{
	assign(source);
}

inline string::string(const char* source, size_t count) :
	chars(empty_string())
{
	assign(source, count);
}

inline string::string(char source, size_t count) : chars(empty_string())
{
	assign(source, count);
}

inline string::buffer* string::metadata(const char* chars)
{
	return B_OUTERSTRUCT(buffer, first_char[0], chars);
}

inline string::buffer* string::metadata() const
{
	return metadata(chars);
}

inline bool string::IsLocked() const
{
	return metadata()->refs <= 0;
}

inline size_t string::GetCapacity() const
{
	return metadata()->capacity;
}

inline size_t string::GetLength() const
{
	return metadata()->length;
}

inline bool string::empty() const
{
	return GetLength() == 0;
}

inline void string::Alloc(size_t capacity)
{
	discard_and_alloc(extra_capacity(capacity));
}

inline void string::Realloc(size_t capacity)
{
	alloc_and_copy(extra_capacity(capacity));
}

inline bool string::IsShared() const
{
	return metadata()->refs > 1;
}

inline void string::reserve(size_t capacity)
{
	if (GetCapacity() < capacity || IsShared())
		Realloc(capacity);
}

inline void string::shrink_to_fit()
{
	size_t length = GetLength();

	if (!IsShared() && length != GetCapacity())
	{
		char* new_buffer_chars = AllocBufferExactly(length, length);

		Copy(new_buffer_chars, chars, length + 1);

		ReplaceBuffer(new_buffer_chars);
	}
}

inline const char* string::GetBuffer() const
{
	return chars;
}

inline string::operator const char*() const
{
	return chars;
}

inline char* string::AllocBuffer(size_t length)
{
	char* new_buffer_chars =
		AllocBufferExactly(extra_capacity(length), length);
	new_buffer_chars[length] = 0;

	return new_buffer_chars;
}

inline void string::isolate()
{
	if (IsShared())
	{
		char* new_buffer_chars = AllocBuffer(GetLength());

		Copy(new_buffer_chars, chars, GetLength());

		ReplaceBuffer(new_buffer_chars);
	}
}

inline char* string::LockBuffer()
{
	isolate();

	--metadata()->refs;

	return chars;
}

inline void string::UnlockBuffer()
{
	B_ASSERT(IsLocked());

	++metadata()->refs;
}

inline void string::UnlockBuffer(size_t new_length)
{
	B_ASSERT(IsLocked() && new_length <= GetCapacity());

	chars[metadata()->length = new_length] = 0;
	++metadata()->refs;
}

inline char string::GetAt(size_t index) const
{
	B_ASSERT(index < (IsLocked() ? GetCapacity() : GetLength()));

	return chars[index];
}

inline char string::operator [](size_t index) const
{
	B_ASSERT(index < (IsLocked() ? GetCapacity() : GetLength()));

	return chars[index];
}

inline void string::SetAt(size_t index, char value)
{
	B_ASSERT(index < (IsLocked() ? GetCapacity() : GetLength()));

	isolate();
	chars[index] = value;
}

inline char& string::operator [](size_t index)
{
	B_ASSERT(index < (IsLocked() ? GetCapacity() : GetLength()));

	isolate();
	return chars[index];
}

inline char string::GetHead() const
{
	B_ASSERT(!empty());

	return *chars;
}

inline char& string::GetHead()
{
	B_ASSERT(!empty());

	isolate();
	return *chars;
}

inline char string::GetTail() const
{
	B_ASSERT(!empty());

	return chars[GetLength() - 1];
}

inline char& string::GetTail()
{
	B_ASSERT(!empty());

	isolate();
	return chars[GetLength() - 1];
}

inline string& string::operator =(const string& source)
{
	assign(source);
	return *this;
}

inline string& string::operator =(char source)
{
	assign(source);
	return *this;
}

inline void string::Insert(size_t index, const string& source)
{
	Insert(index, source.GetBuffer(), source.GetLength());
}

inline void string::append(const string& source)
{
	append(source.GetBuffer(), source.GetLength());
}

inline string& string::operator +=(const string& source)
{
	append(source.GetBuffer(), source.GetLength());
	return *this;
}

inline string& string::operator +=(char source)
{
	append(source);
	return *this;
}

inline string string::operator +(char source) const
{
	string result(*this);
	return result += source;
}

inline int string::compare(const char* right_side) const
{
	return CompareStrings(chars, right_side);
}

inline bool string::operator ==(const string& right_side) const
{
	return compare(right_side) == 0;
}

inline bool string::operator ==(const char* right_side) const
{
	return compare(right_side) == 0;
}

inline bool string::operator !=(const string& right_side) const
{
	return compare(right_side) != 0;
}

inline bool string::operator !=(const char* right_side) const
{
	return compare(right_side) != 0;
}

inline bool string::operator <(const string& right_side) const
{
	return compare(right_side) < 0;
}

inline bool string::operator <(const char* right_side) const
{
	return compare(right_side) < 0;
}

inline bool string::operator >(const string& right_side) const
{
	return compare(right_side) > 0;
}

inline bool string::operator >(const char* right_side) const
{
	return compare(right_side) > 0;
}

inline bool string::operator <=(const string& right_side) const
{
	return compare(right_side) <= 0;
}

inline bool string::operator <=(const char* right_side) const
{
	return compare(right_side) <= 0;
}

inline bool string::operator >=(const string& right_side) const
{
	return compare(right_side) >= 0;
}

inline bool string::operator >=(const char* right_side) const
{
	return compare(right_side) >= 0;
}

inline void string::formatv(const char* format, va_list arguments)
{
	clear();
	appendfv(format, arguments);
}

inline void string::Trim(const char* samples)
{
	TrimRight(samples);
	TrimLeft(samples);
}

inline string operator +(char left_side, const string& right_side)
{
	string result;
	size_t length = right_side.GetLength();

	result.Alloc(length + 1);
	char* chars = result.LockBuffer();

	*chars = left_side;
	Copy(chars + 1, right_side.GetBuffer(), length);

	result.UnlockBuffer(length + 1);
	return result;
}

inline bool operator ==(const char* left_side, const string& right_side)
{
	return right_side.compare(left_side) == 0;
}

inline bool operator !=(const char* left_side, const string& right_side)
{
	return right_side.compare(left_side) != 0;
}

inline bool operator <(const char* left_side, const string& right_side)
{
	return right_side.compare(left_side) > 0;
}

inline bool operator >(const char* left_side, const string& right_side)
{
	return right_side.compare(left_side) < 0;
}

inline bool operator <=(const char* left_side, const string& right_side)
{
	return right_side.compare(left_side) >= 0;
}

inline bool operator >=(const char* left_side, const string& right_side)
{
	return right_side.compare(left_side) <= 0;
}

// Finds the first occurrence of character <c> in <string>.
char* FindCharInString(const char* string, char c);

// Finds the last occurrence of character <c> in <string>.
char* FindLastCharInString(const char* string, char c);

B_END_NAMESPACE

#else

#define B_DECLARE_STRING

#define char wchar_t
#define string wstring
#include "string.h"
#undef string
#undef char

#include "string.h"

#define B_DEFINE_STATIC_STRING_T(name, value) \
	static struct \
	{ \
		b::RefCount refs; \
		size_t capacity; \
		size_t length; \
		B_CHAR chars[sizeof(value)]; \
	} \
	const name##buffer = \
	{ \
		B_REFCOUNT_STATIC_INIT(-1), \
		sizeof(value) / sizeof(B_CHAR) - 1, \
		sizeof(value) / sizeof(B_CHAR) - 1, \
		value \
	}; \
	static const B_CHAR* name##Buffer = name##buffer.chars

#define B_DEFINE_STATIC_STRING(name, value) \
	B_DEFINE_STATIC_STRING_T(name, B_TEXT(value))

#define B_STATIC_STRING(name) (*(const b::string*) &name##Buffer)

#define B_STRING_H

#endif // B_DECLARE_STRING

#endif // !defined(B_STRING_H)
