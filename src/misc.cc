/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016, 2017 Damon Revoe
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

	return ::stat(directory.data(), &stat_struct) != -1 &&
		S_ISDIR(stat_struct.st_mode);
}

static void make_directory(const string& directory, mode_t mode)
{
	if (!directory.is_empty() &&
			::mkdir(directory.data(), mode) == -1)
	{
		int error = errno;

		if (error != EEXIST || !is_directory(directory))
			throw system_exception(directory, error);
	}
}

static void make_directory_and_parents(const string& path,
		mode_t mode, mode_t parent_mode)
{
	try
	{
		make_directory(path, mode);
	}
	catch (system_exception& e)
	{
		size_t slash_pos;

		if (e.error_code() != ENOENT ||
				(slash_pos = path.rfind(B_PATH_SEPARATOR)) <= 0)
			throw;

		// Create the missing parent directory.
		make_directory_and_parents(string(path.data(), slash_pos),
				// Pass 'parent_mode' as 'mode'
				// for all recursive calls.
				parent_mode, parent_mode);

		make_directory(path, mode);
	}
}

void create_directory(const string& path, const arg_list* arg)
{
	bool create_parents = false;

	mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR |
			S_IRGRP | S_IWGRP | S_IXGRP |
			S_IROTH | S_IWOTH | S_IXOTH;

	mode_t* parent_mode = &mode;

	mode_t parent_mode_arg;

	for (; arg != NULL; arg = arg->prev_arg)
		if (args::create_parents.is_name_for(arg))
			create_parents = args::create_parents.value(arg);
		else
			if (args::mode.is_name_for(arg))
				mode = args::mode.value(arg);
			else
				if (args::parent_mode.is_name_for(arg))
				{
					parent_mode_arg =
						args::parent_mode.value(arg);

					parent_mode = &parent_mode_arg;
				}

	if (!create_parents)
		make_directory(path, mode);
	else
		make_directory_and_parents(path, mode, *parent_mode);
}

void remove_directory(const string& directory)
{
	if (!directory.is_empty() && rmdir(directory.data()) == -1)
		throw system_exception(directory, errno);
}

bool match_pattern(const char* input, const char* pattern)
{
	while (*pattern != '*')
	{
		if (*pattern == '\0')
			return *input == '\0';

		if (*input == '\0' || (*input != *pattern && *pattern != '?'))
			return false;

		++input;
		++pattern;
	}

	const char* saved_input;
	const char* saved_pattern;

	for (;;)
	{
		while (*++pattern == '*')
			;

		if (*pattern == '\0')
			return true;

		saved_input = input;
		saved_pattern = pattern;

		do
		{
			if (*input == '\0')
				return false;

			if (*input == *pattern || *pattern == '?')
			{
				++input;

				if (*++pattern == '\0')
					return *input == '\0';
			}
			else
			{
				input = ++saved_input;
				pattern = saved_pattern;
			}
		}
		while (*pattern != '*');
	}
}

bool match_pattern(const char* input, const string_view& pattern)
{
	const char* pattern_char = pattern.data();
	const char* pattern_end = pattern_char + pattern.length();

	for (;;)
	{
		if (pattern_char == pattern_end)
			return *input == '\0';

		if (*pattern_char == '*')
			break;

		if (*input == '\0' || (*input != *pattern_char &&
				*pattern_char != '?'))
			return false;

		++input;
		++pattern_char;
	}

	const char* saved_input;
	const char* saved_pattern;

	for (;;)
	{
		do
			if (++pattern_char == pattern_end)
				return true;
		while (*pattern_char == '*');

		saved_input = input;
		saved_pattern = pattern_char;

		do
		{
			if (*input == '\0')
				return false;

			if (*input == *pattern_char || *pattern_char == '?')
			{
				++input;

				if (++pattern_char == pattern_end)
					return *input == '\0';
			}
			else
			{
				input = ++saved_input;
				pattern_char = saved_pattern;
			}
		}
		while (*pattern_char != '*');
	}
}

bool match_pattern(const string_view& input, const char* pattern)
{
	const char* input_char = input.data();
	const char* input_end = input_char + input.length();

	while (*pattern != '*')
	{
		if (*pattern == '\0')
			return input_char == input_end;

		if (input_char == input_end ||
			(*input_char != *pattern && *pattern != '?'))
			return false;

		++input_char;
		++pattern;
	}

	const char* saved_input;
	const char* saved_pattern;

	for (;;)
	{
		while (*++pattern == '*')
			;

		if (*pattern == '\0')
			return true;

		saved_input = input_char;
		saved_pattern = pattern;

		do
		{
			if (input_char == input_end)
				return false;

			if (*input_char == *pattern || *pattern == '?')
			{
				++input_char;

				if (*++pattern == '\0')
					return input_char == input_end;
			}
			else
			{
				input_char = ++saved_input;
				pattern = saved_pattern;
			}
		}
		while (*pattern != '*');
	}
}

bool match_pattern(const string_view& input, const string_view& pattern)
{
	const char* input_char = input.data();
	const char* input_end = input_char + input.length();
	const char* pattern_char = pattern.data();
	const char* pattern_end = pattern_char + pattern.length();

	for (;;)
	{
		if (pattern_char == pattern_end)
			return input_char == input_end;

		if (*pattern_char == '*')
			break;

		if (input_char == input_end ||
			(*input_char != *pattern_char && *pattern_char != '?'))
			return false;

		++input_char;
		++pattern_char;
	}

	const char* saved_input;
	const char* saved_pattern;

	for (;;)
	{
		do
			if (++pattern_char == pattern_end)
				return true;
		while (*pattern_char == '*');

		saved_input = input_char;
		saved_pattern = pattern_char;

		do
		{
			if (input_char == input_end)
				return false;

			if (*input_char == *pattern_char ||
					*pattern_char == '?')
			{
				++input_char;

				if (++pattern_char == pattern_end)
					return input_char == input_end;
			}
			else
			{
				input_char = ++saved_input;
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
