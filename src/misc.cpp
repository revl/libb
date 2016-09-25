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

#include <b/misc.h>

#include <unistd.h>

B_BEGIN_NAMESPACE

bool is_directory(const string& directory)
{
	struct stat stat_struct;

	return ::stat(directory.c_str(), &stat_struct) != -1 &&
		S_ISDIR(stat_struct.st_mode);
}

void make_directory(const string& directory)
{
	if (!directory.is_empty() && mkdir(directory.c_str(),
		S_IRUSR | S_IWUSR | S_IXUSR |
		S_IRGRP | S_IWGRP | S_IXGRP |
		S_IROTH | S_IWOTH | S_IXOTH) == -1)
	{
		int error = errno;

		if (error != EEXIST || !is_directory(directory))
			throw system_exception(directory, error);
	}
}

void make_path(const string& path)
{
	try
	{
		make_directory(path);
	}
	catch (system_exception& e)
	{
		if (e.error_code() != ENOENT)
			throw;

		size_t slash_pos;

		if ((slash_pos = path.rfind(B_PATH_SEPARATOR)) > 0)
		{
			make_path(string(path.c_str(), slash_pos));
			make_directory(path);
		}
	}
}

void remove_directory(const string& directory)
{
	if (!directory.is_empty() && rmdir(directory.c_str()) == -1)
		throw system_exception(directory, errno);
}

bool match_pattern(const char* string, const char* pattern)
{
	while (*pattern != '*')
	{
		if (*pattern == '\0')
			return *string == '\0';

		if (*string == '\0' || (*string != *pattern && *pattern != '?'))
			return false;

		++string;
		++pattern;
	}

	const char* saved_string;
	const char* saved_pattern;

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

bool match_pattern(const char* string, const string_view& pattern)
{
	const char* pattern_char = pattern.data();
	const char* pattern_end = pattern_char + pattern.length();

	for (;;)
	{
		if (pattern_char == pattern_end)
			return *string == '\0';

		if (*pattern_char == '*')
			break;

		if (*string == '\0' || (*string != *pattern_char &&
				*pattern_char != '?'))
			return false;

		++string;
		++pattern_char;
	}

	const char* saved_string;
	const char* saved_pattern;

	for (;;)
	{
		do
			if (++pattern_char == pattern_end)
				return true;
		while (*pattern_char == '*');

		saved_string = string;
		saved_pattern = pattern_char;

		do
		{
			if (*string == '\0')
				return false;

			if (*string == *pattern_char || *pattern_char == '?')
			{
				++string;

				if (++pattern_char == pattern_end)
					return *string == '\0';
			}
			else
			{
				string = ++saved_string;
				pattern_char = saved_pattern;
			}
		}
		while (*pattern_char != '*');
	}
}

bool match_pattern(const string_view& string, const char* pattern)
{
	const char* string_char = string.data();
	const char* string_end = string_char + string.length();

	while (*pattern != '*')
	{
		if (*pattern == '\0')
			return string_char == string_end;

		if (string_char == string_end ||
			(*string_char != *pattern && *pattern != '?'))
			return false;

		++string_char;
		++pattern;
	}

	const char* saved_string;
	const char* saved_pattern;

	for (;;)
	{
		while (*++pattern == '*')
			;

		if (*pattern == '\0')
			return true;

		saved_string = string_char;
		saved_pattern = pattern;

		do
		{
			if (string_char == string_end)
				return false;

			if (*string_char == *pattern || *pattern == '?')
			{
				++string_char;

				if (*++pattern == '\0')
					return string_char == string_end;
			}
			else
			{
				string_char = ++saved_string;
				pattern = saved_pattern;
			}
		}
		while (*pattern != '*');
	}
}

bool match_pattern(const string_view& string, const string_view& pattern)
{
	const char* string_char = string.data();
	const char* string_end = string_char + string.length();
	const char* pattern_char = pattern.data();
	const char* pattern_end = pattern_char + pattern.length();

	for (;;)
	{
		if (pattern_char == pattern_end)
			return string_char == string_end;

		if (*pattern_char == '*')
			break;

		if (string_char == string_end ||
			(*string_char != *pattern_char && *pattern_char != '?'))
			return false;

		++string_char;
		++pattern_char;
	}

	const char* saved_string;
	const char* saved_pattern;

	for (;;)
	{
		do
			if (++pattern_char == pattern_end)
				return true;
		while (*pattern_char == '*');

		saved_string = string_char;
		saved_pattern = pattern_char;

		do
		{
			if (string_char == string_end)
				return false;

			if (*string_char == *pattern_char ||
					*pattern_char == '?')
			{
				++string_char;

				if (++pattern_char == pattern_end)
					return string_char == string_end;
			}
			else
			{
				string_char = ++saved_string;
				pattern_char = saved_pattern;
			}
		}
		while (*pattern_char != '*');
	}
}

int compare_versions(const char* version1, const char* version2)
{
	for (; *version1 == *version2 && *version1 != '\0';
			++version1, ++version2)
		;

	int result = *version1 - *version2;

	for (; *version1 != '.' && *version1 != '\0'; ++version1, ++version2)
		if (*version2 == '.' || *version2 == '\0')
			return 1;

	return *version2 == '.' || *version2 == '\0' ? result : -1;
}

B_END_NAMESPACE
