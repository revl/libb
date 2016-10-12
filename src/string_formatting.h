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
			struct
			{
				bool space : 1;
				bool hash : 1;
				bool plus : 1;
				bool minus : 1;
				bool zero : 1;
				bool min_width_defined : 1;
				bool precision_defined : 1;
			} flags;
			unsigned min_width;
			unsigned precision;
			enum
			{
				no_length_mod,
				hh,
				h,
				l,
				ll,
				j,
				z,
				t,
				L
			} length_mod;
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

	if (*ch == B_L_PREFIX('%'))
	{
		++acc_len;
		return copy(output_verbatim(ch + 1), ch, 1);
	}

	conversion_spec spec;

	memset(&spec, 0, sizeof(spec));

	// Parse flags
	for (;; ++ch)
	{
		switch (*ch)
		{
		case B_L_PREFIX(' '):
			B_ASSERT(!spec.flags.space && "duplicate ' ' flag");
			spec.flags.space = true;
			continue;
		case B_L_PREFIX('#'):
			B_ASSERT(!spec.flags.hash && "duplicate '#' flag");
			spec.flags.hash = true;
			continue;
		case B_L_PREFIX('+'):
			B_ASSERT(!spec.flags.plus && "duplicate '+' flag");
			spec.flags.plus = true;
			continue;
		case B_L_PREFIX('-'):
			B_ASSERT(!spec.flags.minus && "duplicate '-' flag");
			spec.flags.minus = true;
			continue;
		case B_L_PREFIX('0'):
			B_ASSERT(!spec.flags.zero && "duplicate '0' flag");
			spec.flags.zero = true;
			continue;
		}
		break;
	}

	// Parse min width
	if (*ch > B_L_PREFIX('0') && *ch <= B_L_PREFIX('9'))
	{
		spec.min_width = unsigned(*ch) - unsigned(B_L_PREFIX('0'));

		int digit;

		while ((digit = int(*++ch) - int(B_L_PREFIX('0'))) >= 0 &&
				digit <= 9)
			spec.min_width = spec.min_width * 10 + unsigned(digit);

		spec.flags.min_width_defined = true;
	}
	else
		if (*ch == B_L_PREFIX('*'))
		{
			++ch;

			int n = va_arg(ap, int);

			if (n >= 0)
				spec.min_width = n;
			else
			{
				spec.min_width = -n;
				spec.flags.minus = true;
			}

			spec.flags.min_width_defined = true;
		}

	// Parse precision
	if (*ch == B_L_PREFIX('.'))
	{
		int digit = int(*++ch) - int(B_L_PREFIX('0'));

		if (digit >= 0 && digit <= 9)
		{
			spec.precision = (unsigned) digit;

			while ((digit = int(*++ch) -
					int(B_L_PREFIX('0'))) >= 0 &&
					digit <= 9)
				spec.precision = spec.precision * 10 +
					unsigned(digit);
			spec.flags.precision_defined = true;
		}
		else
			if (*ch == B_L_PREFIX('*'))
			{
				++ch;

				int n = va_arg(ap, int);

				if (n >= 0)
				{
					spec.precision = n;
					spec.flags.min_width_defined = true;
				}
			}
			else
			{
				spec.precision = 0;
				spec.flags.precision_defined = true;
			}
	}

	switch (*ch)
	{
	case B_L_PREFIX('L'):
		spec.length_mod = conversion_spec::L;
		break;

	case B_L_PREFIX('h'):
		if (*++ch != B_L_PREFIX('h'))
			spec.length_mod = conversion_spec::h;
		else
		{
			++ch;
			spec.length_mod = conversion_spec::hh;
		}
		break;

	case B_L_PREFIX('j'):
		spec.length_mod = conversion_spec::j;
		break;

	case B_L_PREFIX('l'):
		if (*++ch != B_L_PREFIX('l'))
			spec.length_mod = conversion_spec::l;
		else
		{
			++ch;
			spec.length_mod = conversion_spec::ll;
		}
		break;

	case B_L_PREFIX('t'):
		spec.length_mod = conversion_spec::t;
		break;

	case B_L_PREFIX('z'):
		spec.length_mod = conversion_spec::z;
	}

	switch (*ch)
	{
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
