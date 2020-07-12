// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/fn.h>

#include <unistd.h>

namespace
{
	template <class C>
	struct char_feed_base
	{
		typedef C char_type;

		char_feed_base(const C* p) : ptr(p)
		{
		}

		const C* ptr;
	};

	template <class C>
	struct null_terminated_char_feed : char_feed_base<C>
	{
		null_terminated_char_feed(const C* p) : char_feed_base<C>(p)
		{
		}

		bool eos() const
		{
			return *this->ptr == (C) 0;
		}
	};

	template <class C>
	struct sized_char_feed : char_feed_base<C>
	{
		sized_char_feed(const C* p, size_t len) :
			char_feed_base<C>(p),
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

string tag_to_string(int tag)
{
	string s;

	while (tag)
	{
		s += char(96 + (tag % 27));
		tag /= 27;
	}

	return s;
}

bool match_pattern(const char* input, const char* pattern)
{
	return match_pattern_impl(
		null_terminated_char_feed<char>(input),
		null_terminated_char_feed<char>(pattern));
}

bool match_pattern(const char* input, const string_view& pattern)
{
	return match_pattern_impl(
		null_terminated_char_feed<char>(input),
		sized_char_feed<char>(pattern.data(), pattern.length()));
}

bool match_pattern(const string_view& input, const char* pattern)
{
	return match_pattern_impl(
		sized_char_feed<char>(input.data(), input.length()),
		null_terminated_char_feed<char>(pattern));
}

bool match_pattern(const string_view& input, const string_view& pattern)
{
	return match_pattern_impl(
		sized_char_feed<char>(input.data(), input.length()),
		sized_char_feed<char>(pattern.data(), pattern.length()));
}

bool match_pattern(const wchar_t* input, const wchar_t* pattern)
{
	return match_pattern_impl(
		null_terminated_char_feed<wchar_t>(input),
		null_terminated_char_feed<wchar_t>(pattern));
}

bool match_pattern(const wchar_t* input, const wstring_view& pattern)
{
	return match_pattern_impl(
		null_terminated_char_feed<wchar_t>(input),
		sized_char_feed<wchar_t>(pattern.data(), pattern.length()));
}

bool match_pattern(const wstring_view& input, const wchar_t* pattern)
{
	return match_pattern_impl(
		sized_char_feed<wchar_t>(input.data(), input.length()),
		null_terminated_char_feed<wchar_t>(pattern));
}

bool match_pattern(const wstring_view& input, const wstring_view& pattern)
{
	return match_pattern_impl(
		sized_char_feed<wchar_t>(input.data(), input.length()),
		sized_char_feed<wchar_t>(pattern.data(), pattern.length()));
}

int compare_strings(const char* lhs, size_t lhs_len,
	const char* rhs, size_t rhs_len)
{
	if (lhs_len < rhs_len)
	{
		int cmp_res = memcmp(lhs, rhs, lhs_len);

		return cmp_res != 0 ? cmp_res : -1;
	}

	int cmp_res = memcmp(lhs, rhs, rhs_len);

	return lhs_len == rhs_len || cmp_res != 0 ? cmp_res : 1;
}

int compare_strings(const wchar_t* lhs, size_t lhs_len,
	const wchar_t* rhs, size_t rhs_len)
{
	if (lhs_len < rhs_len)
	{
		int cmp_res = wmemcmp(lhs, rhs, lhs_len);

		return cmp_res != 0 ? cmp_res : -1;
	}

	int cmp_res = wmemcmp(lhs, rhs, rhs_len);

	return lhs_len == rhs_len || cmp_res != 0 ? cmp_res : 1;
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

static const unsigned char base64url_alphabet[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

size_t base64url_encode(const void* src_buf, size_t src_size,
	void* dst_buf, size_t dst_size)
{
	const size_t result_len = ((src_size << 2) + 2) / 3;

	if (result_len > dst_size)
		return result_len;

	B_ASSERT(dst_buf != NULL && (src_size == 0 || src_buf != NULL));

	const unsigned char* src = (const unsigned char*) src_buf;
	unsigned char* dst = (unsigned char*) dst_buf;

	const unsigned char* offset;

	while (src_size > 2)
	{
		*dst++ = base64url_alphabet[*src >> 2];

		offset = base64url_alphabet + ((*src & 003) << 4);
		*dst++ = offset[*++src >> 4];

		offset = base64url_alphabet + ((*src & 017) << 2);
		*dst++ = offset[*++src >> 6];

		*dst++ = base64url_alphabet[*src++ & 077];

		src_size -= 3;
	}

	if (src_size > 0)
	{
		*dst = base64url_alphabet[*src >> 2];

		offset = base64url_alphabet + ((*src & 003) << 4);

		if (src_size == 1)
			*++dst = *offset;
		else
		{
			/* src_size == 2 */
			*++dst = offset[*++src >> 4];
			*++dst = base64url_alphabet[(*src & 017) << 2];
		}
	}

	return result_len;
}

string base64url_encode(const void* src_buf, size_t src_size)
{
	// Calculate the size of the result.
	size_t encoded_size = base64url_encode(NULL, src_size, NULL, 0U);

	string encoded;

	encoded.reserve(encoded_size);

	// Lock the output string and encode the source buffer.
	base64url_encode(src_buf, src_size, encoded.lock(), encoded_size);

	encoded.unlock(encoded_size);

	return encoded;
}

string base64url_encode(const string_view& src)
{
	return base64url_encode(src.data(), src.length());
}

static const unsigned char base64url_decode_table[256] =
{
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200,   62, 0200, 0200,
	52,   53,   54,   55,   56,   57,   58,   59,
	60,   61,   0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0,    1,    2,    3,    4,    5,    6,
	7,    8,    9,    10,   11,   12,   13,   14,
	15,   16,   17,   18,   19,   20,   21,   22,
	23,   24,   25,   0200, 0200, 0200, 0200, 63,
	0200, 26,   27,   28,   29,   30,   31,   32,
	33,   34,   35,   36,   37,   38,   39,   40,
	41,   42,   43,   44,   45,   46,   47,   48,
	49,   50,   51,   0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200,
	0200, 0200, 0200, 0200, 0200, 0200, 0200, 0200
};

namespace
{
	class invalid_input_exception : public runtime_exception
	{
		virtual string message() const;
	};

	string invalid_input_exception::message() const
	{
		B_STRING_LITERAL(invalid_input,
			"Invalid input passed to base64url_decode()");

		return invalid_input;
	}
}

#define XLAT_BASE64_CHAR(var) \
	if ((signed char) (var = base64url_decode_table[*src++]) < 0) \
		throw invalid_input_exception();

size_t base64url_decode(const void* src_buf, size_t src_size,
	void* dst_buf, size_t dst_size)
{
	const size_t result_len = (src_size * 3) >> 2;

	if (result_len > dst_size)
		return result_len;

	B_ASSERT(dst_buf != NULL && (src_size == 0 || src_buf != NULL));

	const unsigned char* src = (const unsigned char*) src_buf;
	unsigned char* dst = (unsigned char*) dst_buf;

	unsigned char src_ch0, src_ch1;

	while (src_size > 3)
	{
		XLAT_BASE64_CHAR(src_ch0);
		XLAT_BASE64_CHAR(src_ch1);
		*dst++ = (unsigned char) (src_ch0 << 2 | src_ch1 >> 4);

		XLAT_BASE64_CHAR(src_ch0);
		*dst++ = (unsigned char) (src_ch1 << 4 | src_ch0 >> 2);

		XLAT_BASE64_CHAR(src_ch1);
		*dst++ = (unsigned char) (src_ch0 << 6 | src_ch1);

		src_size -= 4;
	}

	if (src_size > 1)
	{
		XLAT_BASE64_CHAR(src_ch0);
		XLAT_BASE64_CHAR(src_ch1);
		*dst++ = (unsigned char) (src_ch0 << 2 | src_ch1 >> 4);

		if (src_size > 2)
		{
			XLAT_BASE64_CHAR(src_ch0);
			*dst = (unsigned char) (src_ch1 << 4 | src_ch0 >> 2);
		}
	}
	else
		if (src_size == 1)
			throw invalid_input_exception();

	return result_len;
}

string base64url_decode(const void* src_buf, size_t src_size)
{
	// Calculate the size of the decoded data.
	size_t decoded_size = base64url_decode(NULL, src_size, NULL, 0U);

	string decoded;

	decoded.reserve(decoded_size);

	// Lock the output string and decode the encoded data.
	base64url_decode(src_buf, src_size, decoded.lock(), decoded_size);

	decoded.unlock(decoded_size);

	return decoded;
}

string base64url_decode(const string_view& src)
{
	return base64url_decode(src.data(), src.length());
}

B_END_NAMESPACE
