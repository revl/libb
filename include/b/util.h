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

#ifndef B_UTIL_H
#define B_UTIL_H

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
bool IsDirectory(const string& directory)
	throw ();

void MakeDirectory(const string& directory)
	throw (SystemException);

void MakePath(const string& path)
	throw (SystemException);

void RemoveDirectory(const string& directory)
	throw (SystemException);

// Global function templates

// Determines the lowest position before which <value> can be inserted in
// the ordered sequence of <count> elements preserving the ordering.
template <class Iterator, class Type>
Iterator FindLowerBound(Iterator first, size_t count, const Type& value)
{
	Iterator middle;
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

// Aligns an integer value on the <alignment> boundary.
template <class TYPE>
inline TYPE Align(TYPE value, size_t alignment)
{
	size_t remainder = (size_t) value % alignment;

	return remainder == 0 ? value :
		(TYPE) ((size_t) value + alignment - remainder);
}

// Exchanges values of two variables.
template <class TYPE>
inline void Swap(TYPE& object1, TYPE& object2)
{
	TYPE dummy(object1);
	object1 = object2;
	object2 = dummy;
}

// Shuffles the sequence of objects using the pseudo-random number generator.
template <class TYPE>
inline void Shuffle(TYPE* objects, size_t count)
{
	size_t i;

	for (i = 1; i < count; i++)
		Swap(objects[i], objects[Random(i)]);
}

// Compares two objects. Returns zero, if objects are equal;
// -1, if the first object is less, and 1, if the second is less.
template <class TYPE>
inline int Compare(const TYPE& object1, const TYPE& object2)
{
	return object1 < object2 ? -1 : object2 < object1;
}

// Compares two object sequences of the same length.
template <class TYPE>
inline int Compare(const TYPE* buffer1, const TYPE* buffer2, size_t count)
{
	register int cmp_res;

	while (count-- > 0)
		if ((cmp_res = Compare(*buffer1++, *buffer2++)) != 0)
			return cmp_res;

	return 0;
}

// Operations on C strings

// Computes the length of null-terminated strings.
inline size_t CalcLength(const char* string)
{
	return ::strlen(string);
}

// Computes the length of null-terminated strings (Unicode version).
inline size_t CalcLength(const wchar_t* string)
{
	return ::wcslen(string);
}

// Computes the length of strings limited either by the
// terminating null character or the <limit> parameter.
inline size_t CalcLength(const char* string, size_t limit)
{
	const char* null_char_ptr = (const char*) memchr(string, 0, limit);

	return null_char_ptr != NULL ? null_char_ptr - string : limit;
}

// Computes the length of strings limited either by the
// terminating null character or the limit parameter (Unicode version).
inline size_t CalcLength(const wchar_t* string, size_t limit)
{
	const wchar_t* null_char_ptr = wmemchr(string, L'\0', limit);

	return null_char_ptr != NULL ? null_char_ptr - string : limit;
}

// Compares two strings.
inline int CompareStrings(const char* string1, const char* string2)
{
	return ::strcmp(string1, string2);
}

// Compares two strings.
inline int CompareStrings(const wchar_t* string1, const wchar_t* string2)
{
	return ::wcscmp(string1, string2);
}

// Formats a string as vsprintf does.
inline int FormatString(char* buffer, const char* format, va_list arguments)
{
	return vsprintf(buffer, format, arguments);
}

// Formats a string as vswprintf does.
inline int FormatString(wchar_t*, const wchar_t*, va_list)
{
	B_ASSERT(0);

	return 0;
}

// Wildcard pattern matching.

bool MatchPatternZZ(
	const B_CHAR* string,
	const B_CHAR* pattern);

bool MatchPatternZR(
	const B_CHAR* string,
	const B_CHAR* pattern, const B_CHAR* pattern_end);

bool MatchPatternRZ(
	const B_CHAR* string, const B_CHAR* string_end,
	const B_CHAR* pattern);

bool MatchPatternRR(
	const B_CHAR* string, const B_CHAR* string_end,
	const B_CHAR* pattern, const B_CHAR* pattern_end);

// Compares two version strings.
int CompareVersionStrings(const B_CHAR* version1, const B_CHAR* version2);

// Converts sequence of wide characters to sequence of multibyte characters.
inline void Copy(char* result, const wchar_t* source, size_t length)
{
	::wcstombs(result, source, reinterpret_cast<size_t&>(length));
}

// Converts a sequence of multibyte characters to a sequence of wide
// characters.
inline void Copy(wchar_t* result, const char* source, size_t length)
{
	::mbstowcs(result, source, reinterpret_cast<size_t&>(length));
}

// Template functions for construction, destruction,
// copying and moving of arrays of various objects

// Calls TYPE default constructor.
template <class TYPE>
inline void Construct(TYPE* objects, size_t count)
{
	while (count-- > 0)
		new (objects++) TYPE;
}

// Calls TYPE copy constructor (sequence initialization
// with a single value).
template <class TYPE>
inline void Construct(TYPE* dest, const TYPE& value, size_t count)
{
	while (count-- > 0)
		new (dest++) TYPE(value);
}

// Calls TYPE copy constructor (initialization with objects
// from the same-sized array).
template <class TYPE>
inline void Construct(TYPE* dest, const TYPE* source, size_t count)
{
	while (count-- > 0)
		new (dest++) TYPE(*source++);
}

// MS C++ issues misplaced C4100: 'objects' : unreferenced formal parameter.
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4100)
#endif // _MSC_VER

// Calls TYPE destructors.
template <class TYPE>
inline void Destroy(TYPE* objects, size_t count)
{
	while (count-- > 0)
		(objects++)->~TYPE();
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif // _MSC_VER

// Calls TYPE assignment operator (element-based array assignment).
template <class TYPE>
inline void Copy(TYPE* dest, const TYPE* source, size_t count)
{
	while (count-- > 0)
		*dest++ = *source++;
}

// Calls TYPE assignment operator (assignment of a single value
// to each element of the array pointed to by <dest>).
template <class TYPE>
inline void Copy(TYPE* dest, const TYPE& value, size_t count)
{
	while (count-- > 0)
		*dest++ = value;
}

// Repeated calls of TYPE assignment operator (element-based
// overlapping array assignment).
template <class TYPE>
inline void ReverseCopy(TYPE* dest, const TYPE* source, size_t count)
{
	while (count-- > 0)
		dest[count] = source[count];
}

// Template specializations

// int

template <>
inline int Compare(const int& n1, const int& n2)
{
	return n1 - n2;
}

template <>
inline void Construct(int* dest, size_t count)
{
	Memory::Zero(dest, count * sizeof(*dest));
}

template <>
inline void Construct(int* dest, const int& value, size_t count)
{
	while (count-- > 0)
		*dest++ = value;
}

template <>
inline void Construct(int* dest, const int* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void Destroy(int*, size_t)
{
}

template <>
inline void Copy(int* dest, const int* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void ReverseCopy(int* dest, const int* source, size_t count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

// long

template <>
inline int Compare(const long& l1, const long& l2)
{
	return int(l1 - l2);
}

template <>
inline void Construct(long* dest, size_t count)
{
	Memory::Zero(dest, count * sizeof(long));
}

template <>
inline void Construct(long* dest, const long& value, size_t count)
{
	while (count-- > 0)
		*dest++ = value;
}

template <>
inline void Construct(long* dest, const long* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void Destroy(long*, size_t)
{
}

template <>
inline void Copy(long* dest, const long* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void ReverseCopy(long* dest, const long* source, size_t count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

// char

template <>
inline int Compare(const char& c1, const char& c2)
{
	return int(c1) - int(c2);
}

template <>
inline int Compare(const char* buffer1, const char* buffer2, size_t count)
{
	return Memory::Compare(buffer1, buffer2, count);
}

template <>
inline void Construct(char* dest, size_t count)
{
	Memory::Zero(dest, count * sizeof(char));
}

template <>
inline void Construct(char* dest, const char& value, size_t count)
{
	Memory::Fill(dest, count, value);
}

template <>
inline void Construct(char* dest, const char* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void Destroy(char*, size_t)
{
}

template <>
inline void Copy(char* dest, const char* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void Copy(char* dest, const char& value, size_t count)
{
	Memory::Fill(dest, count, value);
}

template <>
inline void ReverseCopy(char* dest, const char* source, size_t count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

// wchar_t

template <>
inline int Compare(const wchar_t& c1, const wchar_t& c2)
{
	return int(c1) - int(c2);
}

template <>
inline void Construct(wchar_t* dest, size_t count)
{
	Memory::Zero(dest, count * sizeof(*dest));
}

template <>
inline void Construct(wchar_t* dest, const wchar_t* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void Destroy(wchar_t*, size_t)
{
}

template <>
inline void Copy(wchar_t* dest, const wchar_t* source, size_t count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

template <>
inline void Copy(wchar_t* dest, const wchar_t& value, size_t count)
{
	while (count-- > 0)
		*dest++ = value;
}

template <>
inline void ReverseCopy(wchar_t* dest, const wchar_t* source, size_t count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

B_END_NAMESPACE

#include "string.h"

#endif // !defined(B_UTIL_H)
