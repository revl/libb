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
		b::allocator* buffer_allocator;
		va_list ap;
		size_t acc_len;

		string_formatting(b::allocator* alloc) :
			buffer_allocator(alloc),
			acc_len(0)
		{
		}

		char_t* copy(char_t* dest, const char_t* source, size_t len);

		char_t* alloc_buffer();

		struct conversion_spec
		{
			//int flags;
			//int min_width;
			//int precision;
			//int length_mod;
		};

		char_t* output_int(const char_t* fmt);
		char_t* output_string(const char_t* fmt);
		char_t* output_conversion(const char_t* fmt);
		char_t* output_verbatim(const char_t* fmt);
	};
}

char_t* string_formatting::copy(char_t* dest, const char_t* source, size_t len)
{
	if (len > 0 && dest != NULL)
		// TODO Use one of the global functions from misc
		memcpy(dest -= len, source, len);

	return dest;
}

char_t* string_formatting::alloc_buffer()
{
	char_t* buffer = reinterpret_cast<char_t*>(
		buffer_allocator->allocate(acc_len * sizeof(char_t)));

	if (buffer == NULL)
		return NULL;

	return buffer + acc_len;
}

#define MAX_INT_CONV_BUF_LEN (sizeof(int) * 3 >> 1)

char_t* string_formatting::output_int(const char_t* fmt)
{
	char_t conv_buf[MAX_INT_CONV_BUF_LEN];
	char_t* ch = conv_buf + MAX_INT_CONV_BUF_LEN - 1;

	int number = va_arg(ap, int);

	for (;;)
	{
		*ch = (char_t) (B_L_PREFIX('0') + number % 10);
		if ((number /= 10) == 0)
			break;
		--ch;
	}

	size_t len = conv_buf + MAX_INT_CONV_BUF_LEN - ch;
	acc_len += len;

	return copy(output_verbatim(fmt), ch, len);
}

char_t* string_formatting::output_string(const char_t* fmt)
{
	const char_t* str = va_arg(ap, const char_t*);

	size_t len = b::calc_length(str);
	acc_len += len;

	return copy(output_verbatim(fmt), str, len);
}

char_t* string_formatting::output_conversion(const char_t* fmt)
{
	const char_t* ch = fmt;

	switch (*ch)
	{
	case B_L_PREFIX('%'):
		++acc_len;
		return copy(output_verbatim(ch + 1), ch, 1);
	case B_L_PREFIX('d'):
		return output_int(ch + 1);
	case B_L_PREFIX('s'):
		return output_string(ch + 1);
	default:
		B_ASSERT("unknown conversion type character" && false);
		acc_len = (size_t) -1;
		return NULL;
	}
}

char_t* string_formatting::output_verbatim(const char_t* fmt)
{
	const char_t* ch = fmt;
	size_t len = 0;

	for (;;)
	{
		if (*ch == B_L_PREFIX('%'))
		{
			acc_len += len;
			return copy(output_conversion(++ch), fmt, len);
		}
		else
			if (*ch == B_L_PREFIX('\0'))
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

string_view format_buffer(allocator* alloc, const char_t* fmt, ...)
{
	string_formatting formatting(alloc);

	va_start(formatting.ap, fmt);
	const char_t* buffer = formatting.output_verbatim(fmt);
	va_end(formatting.ap);

	return string_view(buffer, formatting.acc_len);
}

string_view format_buffer_va(allocator* alloc, const char_t* fmt, va_list ap)
{
	string_formatting formatting(alloc);

	B_VA_COPY(formatting.ap, ap);
	const char_t* buffer = formatting.output_verbatim(fmt);
	B_VA_COPY_END(formatting.ap);

	return string_view(buffer, formatting.acc_len);
}

B_END_NAMESPACE
