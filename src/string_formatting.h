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
		char* out_ptr;

		string_formatting() : acc_len(0), buffer(NULL), out_ptr(NULL)
		{
		}

		void alloc_buffer();
		void output_int(const char* fmt);
		void output_string(const char* fmt);
		void output_conversion(const char* fmt);
		void output_verbatim(const char* fmt);
	};
}

void string_formatting::alloc_buffer()
{
	if (output_buffer != NULL)
		*(out_ptr = (buffer = output_buffer) + acc_len) = '\0';
	else
		out_ptr = NULL;
	// *(out_ptr = (buffer = (char*) malloc(acc_len + 1)) + acc_len) = '\0';
}

#define MAX_INT_CONV_BUF_LEN (sizeof(int) * 3 >> 1)

void string_formatting::output_int(const char* fmt)
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

	output_verbatim(fmt);

	// TODO Use character-based copy
	if (out_ptr != NULL)
		memcpy(out_ptr -= len, ch, len);
}

void string_formatting::output_string(const char* fmt)
{
	const char* str = va_arg(args, const char*);

	size_t len = b::calc_length(str);
	acc_len += len;

	output_verbatim(fmt);

	if (out_ptr != NULL)
		memcpy(out_ptr -= len, str, len);
}

struct conversion_spec
{
	//int flags;
	//int min_width;
	//int precision;
	//int length_mod;
};

void string_formatting::output_conversion(const char* fmt)
{
	const char* ch = fmt;

	switch (*ch)
	{
	case 'd':
		output_int(ch + 1);
		break;
	case 's':
		output_string(ch + 1);
		break;
	default:
		B_ASSERT("unknown conversion type character" && false);
		break;
	}
}

void string_formatting::output_verbatim(const char* fmt)
{
	size_t copy_len = 0;
	const char* ch = fmt;

	for (;;)
	{
		switch (*ch)
		{
		case '%':
			acc_len += copy_len;
			output_conversion(++ch);
			break;
		case '\0':
			acc_len += copy_len;
			alloc_buffer();
			break;
		default:
			++ch;
			++copy_len;
			continue;
		}
		break;
	}

	if (copy_len > 0 && out_ptr != NULL)
		// TODO Use one of the global functions from misc
		memcpy(out_ptr -= copy_len, fmt, copy_len);
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
