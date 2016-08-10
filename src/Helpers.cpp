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

#include <B/Host.h>

#ifdef B_USE_PRECOMPILED_HEADER
#pragma hdrstop
#endif // B_USE_PRECOMPILED_HEADER

#include <B/Helpers.h>

#ifndef B_USE_WIN32_API
#include <unistd.h>
#endif

B_BEGIN_NAMESPACE

bool IsDirectory(const String& directory)
	throw ()
{
#if defined(B_USE_WIN32_API)
	DWORD attributes = ::GetFileAttributes(directory);

	return attributes != 0xFFFFFFFF &&
		attributes & FILE_ATTRIBUTE_DIRECTORY;
#else
	struct stat stat_struct;

	return ::stat(directory, &stat_struct) != -1 &&
		S_ISDIR(stat_struct.st_mode);
#endif // B_USE_WIN32_API
}

void MakeDirectory(const String& directory)
	throw (SystemException)
{
#if defined(B_USE_WIN32_API)
	if (!directory.IsEmpty() && !::CreateDirectory(directory, NULL))
	{
		DWORD error = ::GetLastError();

		if (error != 0 && !IsDirectory(directory))
			throw SystemException(error);
	}
#else
	if (!directory.IsEmpty() && mkdir(directory,
		S_IRUSR | S_IWUSR | S_IXUSR |
		S_IRGRP | S_IWGRP | S_IXGRP |
		S_IROTH | S_IWOTH | S_IXOTH) == -1)
	{
		int error = errno;

		if (error != EEXIST || !IsDirectory(directory))
			throw SystemException(error);
	}
#endif // B_USE_WIN32_API
}

void MakePath(const String& path)
	throw (SystemException)
{
	try
	{
		MakeDirectory(path);
	}
	catch (SystemException& e)
	{
		if (e.GetErrorCode() != SystemException::PathNotFound)
			throw;

		int slash_pos;

		if ((slash_pos = path.ReverseFind(B_PATH_SEPARATOR)) > 0)
		{
			MakePath(String(path, slash_pos));
			MakeDirectory(path);
		}
	}
}

void RemoveDirectory(const String& directory)
	throw (SystemException)
{
	if (!directory.IsEmpty() &&
#if defined(B_USE_WIN32_API)
		!::RemoveDirectory(directory)
#else
		rmdir(directory) == -1
#endif // B_USE_WIN32_API
		)
		throw SystemException();
}

bool MatchPatternZZ(const B_CHAR* string, const B_CHAR* pattern)
{
	while (*pattern != '*')
	{
		if (*pattern == '\0')
			return *string == '\0';

		if (*string == '\0' || *string != *pattern && *pattern != '?')
			return false;

		++string;
		++pattern;
	}

	const B_CHAR* saved_string;
	const B_CHAR* saved_pattern;

	for (;;)
	{
		while (*++pattern == '*')
			;

		if (*pattern == '\0')
			return true;

		saved_string = string;
		saved_pattern = pattern;

		do
		{
			if (*string == '\0')
				return false;

			if (*string == *pattern || *pattern == '?')
			{
				++string;

				if (*++pattern == '\0')
					return *string == '\0';
			}
			else
			{
				string = ++saved_string;
				pattern = saved_pattern;
			}
		}
		while (*pattern != '*');
	}
}

bool MatchPatternZR(const B_CHAR* string,
	const B_CHAR* pattern, const B_CHAR* pattern_end)
{
	for (;;)
	{
		if (pattern == pattern_end)
			return *string == '\0';

		if (*pattern == '*')
			break;

		if (*string == '\0' || *string != *pattern && *pattern != '?')
			return false;

		++string;
		++pattern;
	}

	const B_CHAR* saved_string;
	const B_CHAR* saved_pattern;

	for (;;)
	{
		do
			if (++pattern == pattern_end)
				return true;
		while (*pattern == '*');

		saved_string = string;
		saved_pattern = pattern;

		do
		{
			if (*string == '\0')
				return false;

			if (*string == *pattern || *pattern == '?')
			{
				++string;

				if (++pattern == pattern_end)
					return *string == '\0';
			}
			else
			{
				string = ++saved_string;
				pattern = saved_pattern;
			}
		}
		while (*pattern != '*');
	}
}

bool MatchPatternRZ(const B_CHAR* string, const B_CHAR* string_end,
	const B_CHAR* pattern)
{
	while (*pattern != '*')
	{
		if (*pattern == '\0')
			return string == string_end;

		if (string == string_end ||
			*string != *pattern && *pattern != '?')
			return false;

		++string;
		++pattern;
	}

	const B_CHAR* saved_string;
	const B_CHAR* saved_pattern;

	for (;;)
	{
		while (*++pattern == '*')
			;

		if (*pattern == '\0')
			return true;

		saved_string = string;
		saved_pattern = pattern;

		do
		{
			if (string == string_end)
				return false;

			if (*string == *pattern || *pattern == '?')
			{
				++string;

				if (*++pattern == '\0')
					return string == string_end;
			}
			else
			{
				string = ++saved_string;
				pattern = saved_pattern;
			}
		}
		while (*pattern != '*');
	}
}

bool MatchPatternRR(const B_CHAR* string, const B_CHAR* string_end,
	const B_CHAR* pattern, const B_CHAR* pattern_end)
{
	for (;;)
	{
		if (pattern == pattern_end)
			return string == string_end;

		if (*pattern == '*')
			break;

		if (string == string_end ||
			*string != *pattern && *pattern != '?')
			return false;

		++string;
		++pattern;
	}

	const B_CHAR* saved_string;
	const B_CHAR* saved_pattern;

	for (;;)
	{
		do
			if (++pattern == pattern_end)
				return true;
		while (*pattern == '*');

		saved_string = string;
		saved_pattern = pattern;

		do
		{
			if (string == string_end)
				return false;

			if (*string == *pattern || *pattern == '?')
			{
				++string;

				if (++pattern == pattern_end)
					return string == string_end;
			}
			else
			{
				string = ++saved_string;
				pattern = saved_pattern;
			}
		}
		while (*pattern != '*');
	}
}

int CompareVersionStrings(const B_CHAR* version1, const B_CHAR* version2)
{
	for (; *version1 == *version2 && *version1 != B_TEXT('\0');
		++version1, ++version2)
		;

	int result = *version1 - *version2;

	for (; *version1 != B_TEXT('.') && *version1 != B_TEXT('\0');
		++version1, ++version2)
		if (*version2 == B_TEXT('.') || *version2 == B_TEXT('\0'))
			return 1;

	return *version2 == B_TEXT('.') ||
		*version2 == B_TEXT('\0') ? result : -1;
}

B_END_NAMESPACE

#if defined(B_USE_WIN32_API) && defined(B_DLL_BUILD)
BOOL WINAPI DllMain(HINSTANCE /*hInstance*/,
	DWORD /*dwReason*/, LPVOID /*pvReserved*/)
{
	return TRUE;
}
#endif // B_USE_WIN32_API && B_DLL_BUILD
