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

#ifndef B_HELPERS_H
#define B_HELPERS_H

#include "Memory.h"
#include "Random.h"

B_BEGIN_NAMESPACE

// Operations on the file system directory structure
bool IsDirectory(const String& directory)
	throw ();

void MakeDirectory(const String& directory)
	throw (SystemException);

void MakePath(const String& path)
	throw (SystemException);

void RemoveDirectory(const String& directory)
	throw (SystemException);

// Template functions and helpers

// Determines the lowest position before which <value> can be inserted in
// the ordered sequence of <count> elements preserving the ordering.
template <class Iterator, class Type>
Iterator FindLowerBound(Iterator first, int count, const Type& value)
{
	Iterator middle;
	int prev_count;

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
inline void Shuffle(TYPE* objects, int count)
{
	int i;

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
inline int Compare(const TYPE* buffer1, const TYPE* buffer2, int count)
{
	register int cmp_res;

	while (--count >= 0)
		if ((cmp_res = Compare(*buffer1++, *buffer2++)) != 0)
			return cmp_res;

	return 0;
}

// String helpers

// Computes the length of null-terminated strings.
inline int CalcLength(const char* string)
{
#ifdef B_USE_WIN32_API
	return ::lstrlenA(string);
#else
	return ::strlen(string);
#endif // B_USE_WIN32_API
}

// Computes the length of null-terminated strings (Unicode version).
inline int CalcLength(const wchar_t* string)
{
#ifdef B_USE_WIN32_API
	return ::lstrlenW(string);
#else
	return ::wcslen(string);
#endif // B_USE_WIN32_API
}

// Computes the length of strings limited either by the
// terminating null character or the <limit> parameter.
inline int CalcLength(const char* string, int limit)
{
	const char* null_char_ptr = (const char*) memchr(string, 0, limit);

	return null_char_ptr != NULL ? null_char_ptr - string : limit;
}

// Computes the length of strings limited either by the
// terminating null character or the limit parameter (Unicode version).
inline int CalcLength(const wchar_t* string, int limit)
{
	const wchar_t* null_char_ptr = wmemchr(string, L'\0', limit);

	return null_char_ptr != NULL ? null_char_ptr - string : limit;
}

// Compares two strings.
inline int CompareStrings(const char* string1, const char* string2)
{
#ifdef B_USE_WIN32_API
	return ::lstrcmpA(string1, string2);
#else
	return ::strcmp(string1, string2);
#endif // B_USE_WIN32_API
}

// Compares two strings.
inline int CompareStrings(const wchar_t* string1, const wchar_t* string2)
{
#ifdef B_USE_WIN32_API
	return ::lstrcmpW(string1, string2);
#else
	return ::wcscmp(string1, string2);
#endif // B_USE_WIN32_API
}

// Formats a string as vsprintf does.
inline int FormatString(char* buffer, const char* format, va_list arguments)
{
	return vsprintf(buffer, format, arguments);
}

// Formats a string as vswprintf does.
#if defined(B_WIN32) && !defined(__WATCOM_CPLUSPLUS__)
inline int FormatString(wchar_t* buffer,
	const wchar_t* format, va_list arguments)
{
	return vswprintf(buffer, format, arguments);
}
#else
inline int FormatString(wchar_t*, const wchar_t*, va_list)
{
	B_ASSERT(0);

	return 0;
}
#endif

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
inline void Copy(char* result, const wchar_t* source, int length)
{
#ifdef B_USE_WIN32_API
	::WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_SEPCHARS,
		source, length, result, length, NULL, NULL);
#else
	::wcstombs(result, source, reinterpret_cast<size_t&>(length));
#endif // B_USE_WIN32_API
}

// Converts a sequence of multibyte characters to a sequence of wide
// characters.
inline void Copy(wchar_t* result, const char* source, int length)
{
#ifdef B_USE_WIN32_API
	::MultiByteToWideChar(CP_ACP, MB_COMPOSITE,
		source, length, result, length);
#else
	::mbstowcs(result, source, reinterpret_cast<size_t&>(length));
#endif // B_USE_WIN32_API
}

// Template functions for construction, destruction,
// copying and moving of arrays of various objects

// Calls TYPE default constructor.
template <class TYPE>
inline void Construct(TYPE* objects, int count)
{
	while (--count >= 0)
		new (objects++) TYPE;
}

// Calls TYPE copy constructor (sequence initialization
// with a single value).
template <class TYPE>
inline void Construct(TYPE* dest, const TYPE& value, int count)
{
	while (--count >= 0)
		new (dest++) TYPE(value);
}

// Calls TYPE copy constructor (initialization with objects
// from the same-sized array).
template <class TYPE>
inline void Construct(TYPE* dest, const TYPE* source, int count)
{
	while (--count >= 0)
		new (dest++) TYPE(*source++);
}

// MS C++ issues misplaced C4100: 'objects' : unreferenced formal parameter.
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4100)
#endif // _MSC_VER

// Calls TYPE destructors.
template <class TYPE>
inline void Destroy(TYPE* objects, int count)
{
	while (--count >= 0)
		(objects++)->~TYPE();
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif // _MSC_VER

// Calls TYPE assignment operator (element-based array assignment).
template <class TYPE>
inline void Copy(TYPE* dest, const TYPE* source, int count)
{
	while (--count >= 0)
		*dest++ = *source++;
}

// Calls TYPE assignment operator (assignment of a single value
// to each element of the array pointed to by <dest>).
template <class TYPE>
inline void Copy(TYPE* dest, const TYPE& value, int count)
{
	while (--count >= 0)
		*dest++ = value;
}

// Repeated calls of TYPE assignment operator (element-based
// overlapping array assignment).
template <class TYPE>
inline void ReverseCopy(TYPE* dest, const TYPE* source, int count)
{
	while (--count >= 0)
		dest[count] = source[count];
}

// Template specializations

#ifndef __WATCOM_CPLUSPLUS__
#define B_TEMPLATE_SPECIALIZATION template <>
#else
#define B_TEMPLATE_SPECIALIZATION
#endif // __WATCOM_CPLUSPLUS__

// int

B_TEMPLATE_SPECIALIZATION
inline int Compare(const int& n1, const int& n2)
{
	return n1 - n2;
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(int* dest, int count)
{
	Memory::Zero(dest, count * sizeof(*dest));
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(int* dest, const int& value, int count)
{
	while (--count >= 0)
		*dest++ = value;
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(int* dest, const int* source, int count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

B_TEMPLATE_SPECIALIZATION
inline void Destroy(int*, int)
{
}

B_TEMPLATE_SPECIALIZATION
inline void Copy(int* dest, const int* source, int count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

B_TEMPLATE_SPECIALIZATION
inline void ReverseCopy(int* dest, const int* source, int count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

// long

B_TEMPLATE_SPECIALIZATION
inline int Compare(const long& l1, const long& l2)
{
	return int(l1 - l2);
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(long* dest, int count)
{
	Memory::Zero(dest, count * sizeof(long));
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(long* dest, const long& value, int count)
{
	while (--count >= 0)
		*dest++ = value;
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(long* dest, const long* source, int count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

B_TEMPLATE_SPECIALIZATION
inline void Destroy(long*, int)
{
}

B_TEMPLATE_SPECIALIZATION
inline void Copy(long* dest, const long* source, int count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

B_TEMPLATE_SPECIALIZATION
inline void ReverseCopy(long* dest, const long* source, int count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

// char

B_TEMPLATE_SPECIALIZATION
inline int Compare(const char& c1, const char& c2)
{
	return int(c1) - int(c2);
}

B_TEMPLATE_SPECIALIZATION
inline int Compare(const char* buffer1, const char* buffer2, int count)
{
	return Memory::Compare(buffer1, buffer2, count);
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(char* dest, int count)
{
	Memory::Zero(dest, count * sizeof(char));
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(char* dest, const char& value, int count)
{
	Memory::Fill(dest, count, value);
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(char* dest, const char* source, int count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

B_TEMPLATE_SPECIALIZATION
inline void Destroy(char*, int)
{
}

B_TEMPLATE_SPECIALIZATION
inline void Copy(char* dest, const char* source, int count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

B_TEMPLATE_SPECIALIZATION
inline void Copy(char* dest, const char& value, int count)
{
	Memory::Fill(dest, count, value);
}

B_TEMPLATE_SPECIALIZATION
inline void ReverseCopy(char* dest, const char* source, int count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

// wchar_t

B_TEMPLATE_SPECIALIZATION
inline int Compare(const wchar_t& c1, const wchar_t& c2)
{
	return int(c1) - int(c2);
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(wchar_t* dest, int count)
{
	Memory::Zero(dest, count * sizeof(*dest));
}

B_TEMPLATE_SPECIALIZATION
inline void Construct(wchar_t* dest, const wchar_t* source, int count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

B_TEMPLATE_SPECIALIZATION
inline void Destroy(wchar_t*, int)
{
}

B_TEMPLATE_SPECIALIZATION
inline void Copy(wchar_t* dest, const wchar_t* source, int count)
{
	Memory::Copy(dest, source, count * sizeof(*dest));
}

B_TEMPLATE_SPECIALIZATION
inline void Copy(wchar_t* dest, const wchar_t& value, int count)
{
	while (--count >= 0)
		*dest++ = value;
}

B_TEMPLATE_SPECIALIZATION
inline void ReverseCopy(wchar_t* dest, const wchar_t* source, int count)
{
	Memory::Move(dest, source, count * sizeof(*dest));
}

B_END_NAMESPACE

#include "String.h"

#endif // !defined(B_HELPERS_H)
