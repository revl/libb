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

#ifndef B_MISC_H
#define B_MISC_H

#include "memory.h"
#include "random.h"

B_BEGIN_NAMESPACE

// Global utility functions

// Increment the specified size value. This function is for use by
// the containers that reserve additional space for future growth.
inline size_t extra_capacity(size_t size)
{
	size_t extra = size >> 3;

	return size + (extra > 4 ? (extra <= 1024 ? extra : 1024) : 4);
}

// Operations on the file system directory structure
bool is_directory(const string& directory)
	throw ();

void make_directory(const string& directory)
	throw (SystemException);

void make_path(const string& path)
	throw (SystemException);

void remove_directory(const string& directory)
	throw (SystemException);

// Global function templates

// Determines the lowest position before which 'value' can be inserted in
// the ordered sequence of 'count' elements preserving the ordering.
template <class Iter, class T>
Iter find_lower_bound(Iter first, size_t count, const T& value)
{
	Iter middle;
	size_t prev_count;

	while (count > 0)
	{
		prev_count = count;

		if (*(middle = first + (count >>= 1)) < value)
		{
			++(first = middle);

			if (!(prev_count & 1))
				--count;
		}
	}

	return first;
}

// Aligns an integer value on the 'alignment' boundary.
template <class T>
inline T align(T value, size_t alignment)
{
	size_t remainder = (size_t) value % alignment;

	return remainder == 0 ? value :
		(T) ((size_t) value + alignment - remainder);
}

// Exchanges values of two variables.
template <class T>
inline void swap(T& object1, T& object2)
{
	T dummy(object1);
	object1 = object2;
	object2 = dummy;
}

// Shuffles the sequence of objects using the pseudo-random number generator.
template <class T>
inline void shuffle(T* objects, size_t count)
{
	size_t i;

	for (i = 1; i < count; i++)
		swap(objects[i], objects[Random(i)]);
}

// Compares two objects. Returns zero, if objects are equal;
// -1, if the first object is less, and 1, if the second is less.
template <class T>
inline int compare(const T& object1, const T& object2)
{
	return object1 < object2 ? -1 : object2 < object1;
}

// Compares two object sequences of the same length.
template <class T>
inline int compare_arrays(const T* array1, const T* array2, size_t count)
{
	register int cmp_res;

	while (count-- > 0)
		if ((cmp_res = compare(*array1++, *array2++)) != 0)
			return cmp_res;

	return 0;
}

// Operations on C strings

// Computes the length of a null-terminated string.
inline size_t calc_length(const char* string)
{
	return ::strlen(string);
}

// Computes the length of a null-terminated string (wchar_t version).
inline size_t calc_length(const wchar_t* string)
{
	return ::wcslen(string);
}

// Computes the length of a string either terminated by
// a null character or limited by the 'limit' parameter.
inline size_t calc_length(const char* string, size_t limit)
{
	const char* null_char_ptr = (const char*) memchr(string, 0, limit);

	return null_char_ptr != NULL ? null_char_ptr - string : limit;
}

// Computes the length of a string either terminated by
// a null character or limited by the 'limit' parameter (wchar_t version).
inline size_t calc_length(const wchar_t* string, size_t limit)
{
	const wchar_t* null_char_ptr = wmemchr(string, L'\0', limit);

	return null_char_ptr != NULL ? null_char_ptr - string : limit;
}

// Compares two strings.
inline int compare_strings(const char* string1, const char* string2)
{
	return ::strcmp(string1, string2);
}

// Compares two strings.
inline int compare_strings(const wchar_t* string1, const wchar_t* string2)
{
	return ::wcscmp(string1, string2);
}

// Formats a string using a format specification.
size_t format_string(char* buffer, const char* fmt, ...);

// Formats a string using a format specification.
size_t format_string(char* buffer, va_list args, const char* fmt);

// Formats a string using a format specification (wchar_t version).
size_t format_string(wchar_t* buffer, const wchar_t* fmt, ...);

// Formats a string using a format specification (wchar_t version).
size_t format_string(wchar_t* buffer, va_list args, const wchar_t* fmt);

// Wildcard pattern matching.

bool MatchPatternZZ(
	const char* string,
	const char* pattern);

bool MatchPatternZR(
	const char* string,
	const char* pattern, const char* pattern_end);

bool MatchPatternRZ(
	const char* string, const char* string_end,
	const char* pattern);

bool MatchPatternRR(
	const char* string, const char* string_end,
	const char* pattern, const char* pattern_end);

// Compares two version strings.
int compare_versions(const char* version1, const char* version2);

// Converts sequence of wide characters to sequence of multibyte characters.
inline void wchar_to_char(char* result, const wchar_t* source, size_t length)
{
	::wcstombs(result, source, reinterpret_cast<size_t&>(length));
}

// Converts a sequence of multibyte characters to a sequence of wide
// characters.
inline void char_to_wchar(wchar_t* result, const char* source, size_t length)
{
	::mbstowcs(result, source, reinterpret_cast<size_t&>(length));
}

// Template functions for construction, destruction,
// copying and moving of arrays of various objects

// Calls the default constructor of the class 'T'.
template <class T>
inline void construct_default(T* objects, size_t count)
{
	while (count-- > 0)
		new (objects++) T;
}

// Calls the copy constructor of the class 'T' (sequence
// initialization with a single value).
template <class T>
inline void construct_identical_copies(T* dest,
	const T& master_copy, size_t count)
{
	while (count-- > 0)
		new (dest++) T(master_copy);
}

// Calls the copy constructor of the class 'T' (initialization
// with objects from the same-sized array).
template <class T>
inline void construct_copies(T* dest, const T* source, size_t count)
{
	while (count-- > 0)
		new (dest++) T(*source++);
}

// Calls the destructors of the 'T' class.
template <class T>
inline void destruct(T* objects, size_t count)
{
	while (count-- > 0)
		(objects++)->~T();
}

// Calls T assignment operator (assignment of a single value
// to each element of the array pointed to by 'dest').
template <class T>
inline void assign_value(T* dest, size_t count, const T& value)
{
	while (count-- > 0)
		*dest++ = value;
}

// Calls T assignment operator (element-based array assignment).
template <class T>
inline void assign_pairwise(T* dest, const T* source, size_t count)
{
	while (count-- > 0)
		*dest++ = *source++;
}

// Repeated calls of the assignment operator (element-based
// overlapping array assignment).
template <class T>
inline void assign_pairwise_backwards(T* dest, const T* source, size_t count)
{
	while (count-- > 0)
		dest[count] = source[count];
}

// Template specializations

// int

template <>
inline int compare(const int& n1, const int& n2)
{
	return n1 - n2;
}

template <>
inline void construct_default(int* dest, size_t count)
{
	Memory::Zero(dest, count * sizeof(*dest));
}

template <>
inline void construct_identical_copies(int* dest,
	const int& master_copy, size_t count)
{
	while (count-- > 0)
		*dest++ = master_copy;
}

template <>
inline void construct_copies(int* dest, const int* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void destruct(int*, size_t)
{
}

template <>
inline void assign_pairwise(int* dest, const int* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(int* dest,
	const int* source, size_t count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

// long

template <>
inline int compare(const long& l1, const long& l2)
{
	return int(l1 - l2);
}

template <>
inline void construct_default(long* dest, size_t count)
{
	Memory::Zero(dest, count * sizeof(long));
}

template <>
inline void construct_identical_copies(long* dest,
	const long& master_copy, size_t count)
{
	while (count-- > 0)
		*dest++ = master_copy;
}

template <>
inline void construct_copies(long* dest, const long* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void destruct(long*, size_t)
{
}

template <>
inline void assign_pairwise(long* dest, const long* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(long* dest,
	const long* source, size_t count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

// char

template <>
inline int compare(const char& c1, const char& c2)
{
	return int(c1) - int(c2);
}

template <>
inline int compare_arrays(const char* array1, const char* array2, size_t count)
{
	return Memory::Compare(array1, array2, count);
}

template <>
inline void construct_default(char* dest, size_t count)
{
	Memory::Zero(dest, count * sizeof(char));
}

template <>
inline void construct_identical_copies(char* dest,
	const char& master_copy, size_t count)
{
	Memory::Fill(dest, count, master_copy);
}

template <>
inline void construct_copies(char* dest, const char* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void destruct(char*, size_t)
{
}

template <>
inline void assign_value(char* dest, size_t count, const char& value)
{
	Memory::Fill(dest, count, value);
}

template <>
inline void assign_pairwise(char* dest, const char* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(char* dest,
	const char* source, size_t count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

// wchar_t

template <>
inline int compare(const wchar_t& c1, const wchar_t& c2)
{
	return int(c1) - int(c2);
}

template <>
inline void construct_default(wchar_t* dest, size_t count)
{
	Memory::Zero(dest, count * sizeof(*dest));
}

template <>
inline void construct_copies(wchar_t* dest, const wchar_t* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void destruct(wchar_t*, size_t)
{
}

template <>
inline void assign_value(wchar_t* dest, size_t count, const wchar_t& value)
{
	while (count-- > 0)
		*dest++ = value;
}

template <>
inline void assign_pairwise(wchar_t* dest, const wchar_t* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(wchar_t* dest,
	const wchar_t* source, size_t count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

B_END_NAMESPACE

#include "string.h"

#endif // !defined(B_MISC_H)
