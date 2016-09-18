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
#include <b/system_exception.h>

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

bool MatchPatternZZ(const char* string, const char* pattern)
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

bool MatchPatternZR(const char* string,
	const char* pattern, const char* pattern_end)
{
	for (;;)
	{
		if (pattern == pattern_end)
			return *string == '\0';

		if (*pattern == '*')
			break;

		if (*string == '\0' || (*string != *pattern && *pattern != '?'))
			return false;

		++string;
		++pattern;
	}

	const char* saved_string;
	const char* saved_pattern;

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

bool MatchPatternRZ(const char* string, const char* string_end,
	const char* pattern)
{
	while (*pattern != '*')
	{
		if (*pattern == '\0')
			return string == string_end;

		if (string == string_end ||
			(*string != *pattern && *pattern != '?'))
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

bool MatchPatternRR(const char* string, const char* string_end,
	const char* pattern, const char* pattern_end)
{
	for (;;)
	{
		if (pattern == pattern_end)
			return string == string_end;

		if (*pattern == '*')
			break;

		if (string == string_end ||
			(*string != *pattern && *pattern != '?'))
			return false;

		++string;
		++pattern;
	}

	const char* saved_string;
	const char* saved_pattern;

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
