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

#if defined(va_copy)
#define B_VA_COPY(dest_list, src_list) va_copy(dest_list, src_list)
#define B_VA_COPY_END(list) va_end(list)
#elif defined(__va_copy)
#define B_VA_COPY(dest_list, src_list) __va_copy(dest_list, src_list)
#define B_VA_COPY_END(list) va_end(list)
#else
#error Must implement B_VA_COPY for this platform
#endif

namespace
{
	struct string_formatting
	{
		char* output_buffer;
		va_list args;
		size_t acc_len;
		char* buffer;

		string_formatting() : acc_len(0), buffer(NULL)
		{
		}

		char* copy(char* out_ptr, const char* source, size_t len);

		char* alloc_buffer();
		char* output_int(const char* fmt);
		char* output_string(const char* fmt);
		char* output_conversion(const char* fmt);
		char* output_verbatim(const char* fmt);
	};
}

char* string_formatting::copy(char* out_ptr, const char* source, size_t len)
{
	if (len > 0 && out_ptr != NULL)
		// TODO Use one of the global functions from misc
		memcpy(out_ptr -= len, source, len);

	return out_ptr;
}

char* string_formatting::alloc_buffer()
{
	if (output_buffer == NULL)
		return NULL;

	char* out_ptr = (buffer = output_buffer) + acc_len;

	*out_ptr = '\0';

	return out_ptr;
	// *(out_ptr = (buffer = (char*) malloc(acc_len + 1)) + acc_len) = '\0';
}

#define MAX_INT_CONV_BUF_LEN (sizeof(int) * 3 >> 1)

char* string_formatting::output_int(const char* fmt)
{
	char conv_buf[MAX_INT_CONV_BUF_LEN];
	char* ch = conv_buf + MAX_INT_CONV_BUF_LEN - 1;

	int number = va_arg(args, int);

	for (;;)
	{
		*ch = (char) ('0' + number % 10);
		if ((number /= 10) == 0)
			break;
		--ch;
	}

	size_t len = conv_buf + MAX_INT_CONV_BUF_LEN - ch;
	acc_len += len;

	return copy(output_verbatim(fmt), ch, len);
}

char* string_formatting::output_string(const char* fmt)
{
	const char* str = va_arg(args, const char*);

	size_t len = b::calc_length(str);
	acc_len += len;

	return copy(output_verbatim(fmt), str, len);
}

struct conversion_spec
{
	//int flags;
	//int min_width;
	//int precision;
	//int length_mod;
};

char* string_formatting::output_conversion(const char* fmt)
{
	const char* ch = fmt;

	switch (*ch)
	{
	case 'd':
		return output_int(ch + 1);
	case 's':
		return output_string(ch + 1);
	default:
		B_ASSERT("unknown conversion type character" && false);
		return NULL;
	}
}

char* string_formatting::output_verbatim(const char* fmt)
{
	const char* ch = fmt;
	size_t len = 0;

	for (;;)
	{
		if (*ch == '%')
		{
			acc_len += len;
			return copy(output_conversion(++ch), fmt, len);
		}
		else
			if (*ch == 0)
			{
				acc_len += len;
				return copy(alloc_buffer(), fmt, len);
			}
			else
			{
				++ch;
				++len;
			}
	}
}

B_BEGIN_NAMESPACE

size_t format_string(char* buffer, const char* fmt, ...)
{
	string_formatting formatting;

	formatting.output_buffer = buffer;

	va_start(formatting.args, fmt);
	formatting.output_verbatim(fmt);
	va_end(formatting.args);

	return formatting.acc_len;
}

size_t format_string(char* buffer, va_list args, const char* fmt)
{
	string_formatting formatting;

	formatting.output_buffer = buffer;

	B_VA_COPY(formatting.args, args);
	formatting.output_verbatim(fmt);
	B_VA_COPY_END(formatting.args);

	return formatting.acc_len;
}

B_END_NAMESPACE
