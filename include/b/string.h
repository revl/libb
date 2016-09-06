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
#include "misc.h"

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
	size_t capacity() const;

	// Returns the string length in characters.
	size_t length() const;

	// Returns true if the string is empty.
	bool is_empty() const;

// Lock State
public:
	// Checks if the string is locked.
	bool is_locked() const;

// Memory Management
public:
	// Discards the string contents and allocates the exact
	// amount of memory for the buffer.
	void discard_and_alloc(size_t new_capacity);

	// Reallocates the buffer preserving the string contents.
	void alloc_and_copy(size_t new_capacity);

	// Allocates enough memory to store 'new_capacity' characters.
	// In case if the buffer gets reallocated, the contents is
	// not preserved.
	void reserve(size_t new_capacity);

	// Frees memory that is not used by the string.
	void shrink_to_fit();

// Buffer Access
public:
	// Returns a constant pointer to the null-terminated
	// array of characters.
	const char* c_str() const;

	// Converts to a const char pointer.
	operator const char*() const;

	// Fixes the buffer in memory disabling the memory reallocation.
	char* lock();

	// Gives control over the buffer back to this object.
	void unlock();

	// Unlocks the buffer and sets the new string length.
	void unlock(size_t new_length);

// Single Character Access
public:
	// Returns the character with the specified index.
	char at(size_t index) const;

	// Returns a reference to the character at the specified position.
	char& at(size_t index);

	// Returns the character with the specified index.
	char operator [](size_t index) const;

	// Returns a reference to the specified character.
	char& operator [](size_t index);

	// Returns the first character of the string.
	char front() const;

	// Returns a reference to the first character of the string.
	char& front();

	// Returns the last character of the string.
	char back() const;

	// Returns a reference to the last character of the string.
	char& back();

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
	void replace(size_t index, const char* source, size_t count);

	// Replaces a part of this string with a fill character.
	void replace(size_t index, char source, size_t count = 1);

// Insertion
public:
	// Inserts a character array into this string.
	void insert(size_t index, const char* source, size_t count);

	// Inserts the contents of <source> into this string.
	void insert(size_t index, const string& source);

	// Inserts <count> copies of <source> into this string.
	void insert(size_t index, char source, size_t count = 1);

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
	// Compares this string against 'rhs' lexicographically.
	int compare(const char* rhs) const;

	// Returns true if this string and 'rhs' are equal.
	bool operator ==(const string& rhs) const;

	// Checks this string and result for equality.
	bool operator ==(const char* rhs) const;

	// Returns true if this string and 'rhs' differ.
	bool operator !=(const string& rhs) const;

	// Returns true if this string and 'rhs' differ.
	bool operator !=(const char* rhs) const;

	// Returns true if this string is less than 'rhs'.
	bool operator <(const string& rhs) const;

	// Returns true if this string is less than 'rhs'.
	bool operator <(const char* rhs) const;

	// Returns true if this string is greater than 'rhs'.
	bool operator >(const string& rhs) const;

	// Returns true if this string is greater than 'rhs'.
	bool operator >(const char* rhs) const;

	// Returns true if this string is less or equal than 'rhs'.
	bool operator <=(const string& rhs) const;

	// Returns true if this string is less or equal than 'rhs'.
	bool operator <=(const char* rhs) const;

	// Returns true if this string is greater or equal than 'rhs'.
	bool operator >=(const string& rhs) const;

	// Returns true if this string is greater or equal than 'rhs'.
	bool operator >=(const char* rhs) const;

// Formatting
public:
	// Appends formatted data to this string.
	void appendfv(const char* format, va_list arguments);

	// Appends formatted data to this string.
	void appendf(const char* format, ...);

	// Formats this string the way vsprintf does.
	void assignfv(const char* format, va_list arguments);

	// Formats this string the way sprintf does.
	void assignf(const char* format, ...);

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
	void trim_right(const char* samples);

	// Removes all occurrences of characters from <samples>
	// found at the beginning of this string.
	void trim_left(const char* samples);

	// Removes all characters from <samples> from both ends of this string.
	void trim(const char* samples);

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

	bool is_shared() const;

	static char* empty_string();

	static char* alloc_buffer(size_t capacity, size_t length);

	static buffer* metadata(const char* chars);
	buffer* metadata() const;

	void replace_buffer(char* new_buffer_chars);

	// Make sure that the buffer is not shared with other strings.
	// Reallocate the buffer if it's shared; preserve the original
	// buffer contents.
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

inline bool string::is_locked() const
{
	return metadata()->refs <= 0;
}

inline size_t string::capacity() const
{
	return metadata()->capacity;
}

inline size_t string::length() const
{
	return metadata()->length;
}

inline bool string::is_empty() const
{
	return length() == 0;
}

inline bool string::is_shared() const
{
	return metadata()->refs > 1;
}

inline void string::reserve(size_t new_capacity)
{
	if (capacity() < new_capacity || is_shared())
		alloc_and_copy(extra_capacity(new_capacity));
}

inline void string::shrink_to_fit()
{
	size_t len = length();

	if (!is_shared() && len != capacity())
	{
		char* new_buffer_chars = alloc_buffer(len, len);

		assign_pairwise(new_buffer_chars, chars, len + 1);

		replace_buffer(new_buffer_chars);
	}
}

inline const char* string::c_str() const
{
	return chars;
}

inline string::operator const char*() const
{
	return chars;
}

inline void string::isolate()
{
	if (is_shared())
	{
		size_t len = length();

		char* new_buffer_chars = alloc_buffer(extra_capacity(len), len);

		assign_pairwise(new_buffer_chars, chars, len + 1);

		replace_buffer(new_buffer_chars);
	}
}

inline char* string::lock()
{
	isolate();

	--metadata()->refs;

	return chars;
}

inline void string::unlock()
{
	B_ASSERT(is_locked());

	++metadata()->refs;
}

inline void string::unlock(size_t new_length)
{
	B_ASSERT(is_locked() && new_length <= capacity());

	chars[metadata()->length = new_length] = 0;
	++metadata()->refs;
}

inline char string::at(size_t index) const
{
	B_ASSERT(index < (is_locked() ? capacity() : length()));

	return chars[index];
}

inline char& string::at(size_t index)
{
	B_ASSERT(index < (is_locked() ? capacity() : length()));

	isolate();
	return chars[index];
}

inline char string::operator [](size_t index) const
{
	B_ASSERT(index < (is_locked() ? capacity() : length()));

	return chars[index];
}

inline char& string::operator [](size_t index)
{
	B_ASSERT(index < (is_locked() ? capacity() : length()));

	isolate();
	return chars[index];
}

inline char string::front() const
{
	B_ASSERT(!is_empty());

	return *chars;
}

inline char& string::front()
{
	B_ASSERT(!is_empty());

	isolate();
	return *chars;
}

inline char string::back() const
{
	B_ASSERT(!is_empty());

	return chars[length() - 1];
}

inline char& string::back()
{
	B_ASSERT(!is_empty());

	isolate();
	return chars[length() - 1];
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

inline void string::insert(size_t index, const string& source)
{
	insert(index, source.c_str(), source.length());
}

inline void string::append(const string& source)
{
	append(source.c_str(), source.length());
}

inline string& string::operator +=(const string& source)
{
	append(source.c_str(), source.length());
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

inline int string::compare(const char* rhs) const
{
	return CompareStrings(chars, rhs);
}

inline bool string::operator ==(const string& rhs) const
{
	return compare(rhs) == 0;
}

inline bool string::operator ==(const char* rhs) const
{
	return compare(rhs) == 0;
}

inline bool string::operator !=(const string& rhs) const
{
	return compare(rhs) != 0;
}

inline bool string::operator !=(const char* rhs) const
{
	return compare(rhs) != 0;
}

inline bool string::operator <(const string& rhs) const
{
	return compare(rhs) < 0;
}

inline bool string::operator <(const char* rhs) const
{
	return compare(rhs) < 0;
}

inline bool string::operator >(const string& rhs) const
{
	return compare(rhs) > 0;
}

inline bool string::operator >(const char* rhs) const
{
	return compare(rhs) > 0;
}

inline bool string::operator <=(const string& rhs) const
{
	return compare(rhs) <= 0;
}

inline bool string::operator <=(const char* rhs) const
{
	return compare(rhs) <= 0;
}

inline bool string::operator >=(const string& rhs) const
{
	return compare(rhs) >= 0;
}

inline bool string::operator >=(const char* rhs) const
{
	return compare(rhs) >= 0;
}

inline void string::assignfv(const char* format, va_list arguments)
{
	clear();
	appendfv(format, arguments);
}

inline void string::trim(const char* samples)
{
	trim_right(samples);
	trim_left(samples);
}

inline string operator +(char ch, const string& str)
{
	string result;
	size_t length = str.length();

	result.discard_and_alloc(extra_capacity(length + 1));
	char* chars = result.lock();

	*chars = ch;
	assign_pairwise(chars + 1, str.c_str(), length);

	result.unlock(length + 1);
	return result;
}

inline bool operator ==(const char* c_str, const string& str)
{
	return str.compare(c_str) == 0;
}

inline bool operator !=(const char* c_str, const string& str)
{
	return str.compare(c_str) != 0;
}

inline bool operator <(const char* c_str, const string& str)
{
	return str.compare(c_str) > 0;
}

inline bool operator >(const char* c_str, const string& str)
{
	return str.compare(c_str) < 0;
}

inline bool operator <=(const char* c_str, const string& str)
{
	return str.compare(c_str) >= 0;
}

inline bool operator >=(const char* c_str, const string& str)
{
	return str.compare(c_str) <= 0;
}

// Finds the first occurrence of character 'c' in 'string'.
char* find_char(const char* string, char c);

// Finds the last occurrence of character 'c' in 'string'.
char* find_char_backwards(const char* string, char c);

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
