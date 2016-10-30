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

#ifndef B_STRING_VIEW_H

#if defined(B_STRING_VIEW_DECL)

B_BEGIN_NAMESPACE

class string;

// Pointer to a substring within a self-contained string.
class string_view
{
// Construction
public:
	// Constructs an empty string_view object.
	string_view();

	// Constructs a copy of an existing string_view object.
	string_view(const string_view& source);

	// Makes this new string_view object point to a character array.
	string_view(const char_t* source, size_t len);

	// Constructs a string_view object from a string object.
	string_view(const string& str);

// Data access
public:
	// Returns the buffer that this object points to.
	const char_t* data() const
	{
		return view;
	}

	// Returns the length of this string_view in characters.
	size_t length() const
	{
		return view_length;
	}

	// Returns true if the length of this string_view is zero.
	bool is_empty() const;

	// Returns the character with the specified index.
	char_t at(size_t index) const;

	// Returns the character with the specified index.
	char_t operator [](size_t index) const;

	// Returns the first character of the string_view.
	char_t front() const;

	// Returns the last character of the string_view.
	char_t back() const;

// Assignment
public:
	// Makes this string_view object point to the contents
	// of the 'source' string.
	void assign(const string_view& source);

	// Operator version of assign(const string_view&).
	string_view& operator =(const string_view& source);

	// Resets this object to point to another character buffer.
	void assign(const char_t* source, size_t len);

	// Makes this string_view object point to the contents
	// of the 'source' string.
	void assign(const string& source);

	// Operator version of assign(const string&).
	string_view& operator =(const string& source);

// Comparison
public:
	// Compares this string_view with 'rhs' lexicographically.
	int compare(const char_t* rhs) const;

	// Compares the contents of two string_view objects
	// lexicographically.
	int compare(const string_view& rhs) const;

	// Checks if two string_view objects are equal.
	bool operator ==(const string_view& rhs) const;

	// Checks if this string_view is euqal to a null terminated
	// string.
	bool operator ==(const char_t* rhs) const;

	// Checks if two string_view objects differ.
	bool operator !=(const string_view& rhs) const;

	// Returns true if this string_view is not equal to a null
	// terminated string.
	bool operator !=(const char_t* rhs) const;

	// Returns true if this string_view is less than 'rhs'.
	bool operator <(const string_view& rhs) const;

	// Returns true if this string_view is less than 'rhs'.
	bool operator <(const char_t* rhs) const;

	// Returns true if this string_view is greater than 'rhs'.
	bool operator >(const string_view& rhs) const;

	// Returns true if this string_view is greater than 'rhs'.
	bool operator >(const char_t* rhs) const;

	// Returns true if this string_view is less or equal than 'rhs'.
	bool operator <=(const string_view& rhs) const;

	// Returns true if this string_view is less or equal than 'rhs'.
	bool operator <=(const char_t* rhs) const;

	// Returns true if this string_view is greater or equal than 'rhs'.
	bool operator >=(const string_view& rhs) const;

	// Returns true if this string_view is greater or equal than 'rhs'.
	bool operator >=(const char_t* rhs) const;

// Searching
public:
	// Finds a character in this string_view.
	size_t find(char_t c) const;

	// Finds the last occurrence of a character in this string_view.
	size_t rfind(char_t c) const;

// Whitespace trimming
public:
	// Shrink this string_view by removing the characters that are
	// listed in 'samples' from the end of this string_view.
	void trim_right(const char_t* samples);

	// Shrink this string_view by removing the characters that are
	// listed in 'samples' from the beginning of this string_view.
	void trim_left(const char_t* samples);

	// Removes all characters that are listed in 'samples' from
	// both ends of this string_view.
	void trim(const char_t* samples);

// Implementation
private:
	const char_t* view;
	size_t view_length;
};

B_END_NAMESPACE

#elif defined(B_STRING_VIEW_INLINE)

B_BEGIN_NAMESPACE

inline string_view::string_view() : view(NULL), view_length(0)
{
}

inline string_view::string_view(const string_view& source) :
	view(source.data()), view_length(source.length())
{
}

inline string_view::string_view(const char_t* source, size_t len) :
	view(source), view_length(len)
{
}

inline string_view::string_view(const string& str) :
	view(str.data()), view_length(str.length())
{
}

inline bool string_view::is_empty() const
{
	return length() == 0;
}

inline char_t string_view::at(size_t index) const
{
	B_ASSERT(index < length());

	return view[index];
}

inline char_t string_view::operator [](size_t index) const
{
	B_ASSERT(index < length());

	return view[index];
}

inline char_t string_view::front() const
{
	B_ASSERT(!is_empty());

	return *view;
}

inline char_t string_view::back() const
{
	B_ASSERT(!is_empty());

	return view[length() - 1];
}

inline void string_view::assign(const string_view& source)
{
	view = source.view;
	view_length = source.view_length;
}

inline string_view& string_view::operator =(const string_view& source)
{
	assign(source);
	return *this;
}

inline void string_view::assign(const char_t* source, size_t len)
{
	view = source;
	view_length = len;
}

inline void string_view::assign(const string& source)
{
	view = source.data();
	view_length = source.length();
}

inline string_view& string_view::operator =(const string& source)
{
	assign(source);
	return *this;
}

inline int string_view::compare(const char_t* rhs) const
{
	int diff = compare_strings(view, rhs, view_length);

	return diff != 0 ? diff : -(rhs[view_length] != B_L_PREFIX('\0'));
}

inline int string_view::compare(const string_view& rhs) const
{
	if (view_length < rhs.view_length)
	{
		int diff = Memory::Compare(view, rhs.view,
			view_length * sizeof(char_t));

		return diff != 0 ? diff : -1;
	}
	else
	{
		int diff = Memory::Compare(view, rhs.view,
			rhs.view_length * sizeof(char_t));

		return diff != 0 ? diff : view_length > rhs.view_length;
	}
}

inline bool string_view::operator ==(const string_view& rhs) const
{
	return compare(rhs) == 0;
}

inline bool string_view::operator ==(const char_t* rhs) const
{
	return compare(rhs) == 0;
}

inline bool string_view::operator !=(const string_view& rhs) const
{
	return compare(rhs) != 0;
}

inline bool string_view::operator !=(const char_t* rhs) const
{
	return compare(rhs) != 0;
}

inline bool string_view::operator <(const string_view& rhs) const
{
	return compare(rhs) < 0;
}

inline bool string_view::operator <(const char_t* rhs) const
{
	return compare(rhs) < 0;
}

inline bool string_view::operator >(const string_view& rhs) const
{
	return compare(rhs) > 0;
}

inline bool string_view::operator >(const char_t* rhs) const
{
	return compare(rhs) > 0;
}

inline bool string_view::operator <=(const string_view& rhs) const
{
	return compare(rhs) <= 0;
}

inline bool string_view::operator <=(const char_t* rhs) const
{
	return compare(rhs) <= 0;
}

inline bool string_view::operator >=(const string_view& rhs) const
{
	return compare(rhs) >= 0;
}

inline bool string_view::operator >=(const char_t* rhs) const
{
	return compare(rhs) >= 0;
}

inline void string_view::trim(const char_t* samples)
{
	trim_right(samples);
	trim_left(samples);
}

inline bool operator ==(const char_t* chars, const string_view& str)
{
	return str.compare(chars) == 0;
}

inline bool operator !=(const char_t* chars, const string_view& str)
{
	return str.compare(chars) != 0;
}

inline bool operator <(const char_t* chars, const string_view& str)
{
	return str.compare(chars) > 0;
}

inline bool operator >(const char_t* chars, const string_view& str)
{
	return str.compare(chars) < 0;
}

inline bool operator <=(const char_t* chars, const string_view& str)
{
	return str.compare(chars) >= 0;
}

inline bool operator >=(const char_t* chars, const string_view& str)
{
	return str.compare(chars) <= 0;
}

B_END_NAMESPACE

#else /* !defined(B_STRING_VIEW_DECL) && !defined(B_STRING_VIEW_INLINE) */

#include "host.h"

#define B_STRING_VIEW_DECL

#define string_view wstring_view
#define char_t wchar_t
#define string wstring
#include "string_view.h"
#undef string
#undef char_t
#undef string_view

#define char_t char
#include "string_view.h"
#undef char_t

#undef B_STRING_VIEW_DECL

#define B_STRING_VIEW_H
#include "string.h"
#include "misc.h"
#undef B_STRING_VIEW_H

#define B_STRING_VIEW_INLINE

#define string_view wstring_view
#define char_t wchar_t
#define string wstring
#define B_L_PREFIX(ch) L##ch
#include "string_view.h"
#undef B_L_PREFIX
#undef string
#undef char_t
#undef string_view

#define char_t char
#define B_L_PREFIX(ch) ch
#include "string_view.h"
#undef B_L_PREFIX
#undef char_t

#undef B_STRING_VIEW_INLINE

#define B_STRING_VIEW_H

#endif /* defined(B_STRING_VIEW_DECL) || defined(B_STRING_VIEW_INLINE) */

#endif /* !defined(B_STRING_VIEW_H) */
