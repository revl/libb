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

#include <b/fn.h>

#include <unistd.h>

namespace
{
	template <class C>
	struct string_stream_base
	{
		typedef C char_type;

		string_stream_base(const C* p) : ptr(p)
		{
		}

		const C* ptr;
	};

	template <class C>
	struct zero_terminated_string_stream : string_stream_base<C>
	{
		zero_terminated_string_stream(const C* p) :
			string_stream_base<C>(p)
		{
		}

		bool eos() const
		{
			return *this->ptr == (C) 0;
		}
	};

	template <class C>
	struct string_view_stream : string_stream_base<C>
	{
		string_view_stream(const C* p, size_t len) :
			string_stream_base<C>(p),
			end(p + len)
		{
		}

		bool eos() const
		{
			return this->ptr == end;
		}

		const C* end;
	};

	template <class Input_stream, class Pattern_stream>
	bool match_pattern_impl(Input_stream input, Pattern_stream pattern)
	{
		for (;;)
		{
			if (pattern.eos())
				return input.eos();

			if (*pattern.ptr == '*')
				break;

			if (input.eos() || (*input.ptr != *pattern.ptr &&
					*pattern.ptr != '?'))
				return false;

			++input.ptr;
			++pattern.ptr;
		}

		const typename Input_stream::char_type* saved_input;
		const typename Pattern_stream::char_type* saved_pattern;

		for (;;)
		{
			do
			{
				++pattern.ptr;
				if (pattern.eos())
					return true;
			}
			while (*pattern.ptr == '*');

			saved_input = input.ptr;
			saved_pattern = pattern.ptr;

			do
			{
				if (input.eos())
					return false;

				if (*input.ptr == *pattern.ptr ||
						*pattern.ptr == '?')
				{
					++input.ptr;

					++pattern.ptr;
					if (!pattern.eos())
						continue;

					if (input.eos())
						return true;
				}

				input.ptr = ++saved_input;
				pattern.ptr = saved_pattern;
			}
			while (*pattern.ptr != '*');
		}
	}
}

B_BEGIN_NAMESPACE

bool match_pattern(const char* input, const char* pattern)
{
	return match_pattern_impl(
		zero_terminated_string_stream<char>(input),
		zero_terminated_string_stream<char>(pattern));
}

bool match_pattern(const char* input, const string_view& pattern)
{
	return match_pattern_impl(
		zero_terminated_string_stream<char>(input),
		string_view_stream<char>(pattern.data(), pattern.length()));
}

bool match_pattern(const string_view& input, const char* pattern)
{
	return match_pattern_impl(
		string_view_stream<char>(input.data(), input.length()),
		zero_terminated_string_stream<char>(pattern));
}

bool match_pattern(const string_view& input, const string_view& pattern)
{
	return match_pattern_impl(
		string_view_stream<char>(input.data(), input.length()),
		string_view_stream<char>(pattern.data(), pattern.length()));
}

bool match_pattern(const wchar_t* input, const wchar_t* pattern)
{
	return match_pattern_impl(
		zero_terminated_string_stream<wchar_t>(input),
		zero_terminated_string_stream<wchar_t>(pattern));
}

bool match_pattern(const wchar_t* input, const wstring_view& pattern)
{
	return match_pattern_impl(
		zero_terminated_string_stream<wchar_t>(input),
		string_view_stream<wchar_t>(pattern.data(), pattern.length()));
}

bool match_pattern(const wstring_view& input, const wchar_t* pattern)
{
	return match_pattern_impl(
		string_view_stream<wchar_t>(input.data(), input.length()),
		zero_terminated_string_stream<wchar_t>(pattern));
}

bool match_pattern(const wstring_view& input, const wstring_view& pattern)
{
	return match_pattern_impl(
		string_view_stream<wchar_t>(input.data(), input.length()),
		string_view_stream<wchar_t>(pattern.data(), pattern.length()));
}

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

namespace args
{
	arg_name<bool, 0> create_parents;
	arg_name<mode_t, 1> mode;
	arg_name<mode_t, 2> parent_mode;
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
