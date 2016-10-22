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
				bool quote : 1;
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

		template <class T, class Arg>
		char_t* output_decimal(const conversion_spec* spec,
			const char_t* fmt);
		template <class T, class Arg>
		char_t* output_unsigned(const conversion_spec* spec,
			const char_t* fmt);
		char_t* output_string(const char_t* fmt);
		char_t* output_conversion(const char_t* fmt);
		char_t* output_verbatim(const char_t* fmt);
	};
}

char_t* string_formatting::copy(char_t* dest, const char_t* source, size_t len)
{
	if (len > 0 && dest != NULL)
		b::construct_copies(dest -= len, source, len);

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

template <class T, class Arg>
char_t* string_formatting::output_decimal(const conversion_spec* spec,
	const char_t* fmt)
{
	char_t conv_buf[MAX_DECIMAL_BUF_LEN(T)];
	char_t* ch = conv_buf + MAX_DECIMAL_BUF_LEN(T) - 1;

	char_t sign;

	T number = (T) va_arg(ap, Arg);

	bool negative = number < 0;
	unsigned sep = spec->flags.quote ? 3 : 0;

	if (!negative)
	{
		sign = spec->flags.plus ? B_L_PREFIX('+') :
			spec->flags.space ? B_L_PREFIX(' ') : 0;

		for (;;)
		{
			*ch = (char_t) (B_L_PREFIX('0') + number % 10);
			if ((number /= 10) == 0)
				break;
			if (sep != 0 && --sep == 0)
			{
				sep = 3;
				*--ch = B_L_PREFIX(',');
			}
			--ch;
		}
	}
	else
	{
		sign = B_L_PREFIX('-');

		for (;;)
		{
			*ch = (char_t) (B_L_PREFIX('0') - number % 10);
			if ((number /= 10) == 0)
				break;
			if (sep != 0 && --sep == 0)
			{
				sep = 3;
				*--ch = B_L_PREFIX(',');
			}
			--ch;
		}
	}

	size_t digits = (size_t) (conv_buf + MAX_DECIMAL_BUF_LEN(T) - ch);

	size_t digits_and_zeros = spec->flags.precision_defined &&
		spec->precision > digits ? spec->precision : digits;

	size_t digits_zeros_and_sign =
		sign == 0 ? digits_and_zeros : digits_and_zeros + 1;

	size_t width = spec->flags.min_width_defined &&
		spec->min_width > digits_zeros_and_sign ?
			spec->min_width : digits_zeros_and_sign;

	acc_len += width;

	char_t* dest = output_verbatim(fmt);

	if (dest != NULL)
	{
		size_t zeros = digits_and_zeros - digits;
		size_t spaces = width - digits_zeros_and_sign;

		if (!spec->flags.minus)
		{
			dest = copy(dest, ch, digits);
			if (zeros > 0)
				b::construct_identical_copies(dest -= zeros,
					B_L_PREFIX('0'), zeros);
			if (!spec->flags.zero)
			{
				if (sign != 0)
					*--dest = sign;
				if (spaces > 0)
					b::construct_identical_copies(
						dest -= spaces,
						B_L_PREFIX(' '), spaces);
			}
			else
			{
				if (spaces > 0)
					b::construct_identical_copies(
						dest -= spaces,
						B_L_PREFIX('0'), spaces);
				if (sign != 0)
					*--dest = sign;
			}
		}
		else
		{
			if (spaces > 0)
				b::construct_identical_copies(dest -= spaces,
					B_L_PREFIX(' '), spaces);
			dest = copy(dest, ch, digits);
			if (zeros > 0)
				b::construct_identical_copies(dest -= zeros,
					B_L_PREFIX('0'), zeros);
			if (sign != 0)
				*--dest = sign;
		}
	}

	return dest;
}

template <class T, class Arg>
char_t* string_formatting::output_unsigned(const conversion_spec* spec,
	const char_t* fmt)
{
	char_t conv_buf[MAX_DECIMAL_BUF_LEN(T)];
	char_t* ch = conv_buf + MAX_DECIMAL_BUF_LEN(T) - 1;

	T number = (T) va_arg(ap, Arg);

	unsigned sep = spec->flags.quote ? 3 : 0;

	for (;;)
	{
		*ch = (char_t) (B_L_PREFIX('0') + number % 10);
		if ((number /= 10) == 0)
			break;
		if (sep != 0 && --sep == 0)
		{
			sep = 3;
			*--ch = B_L_PREFIX(',');
		}
		--ch;
	}

	size_t digits = (size_t) (conv_buf + MAX_DECIMAL_BUF_LEN(T) - ch);

	size_t digits_and_zeros = spec->flags.precision_defined &&
		spec->precision > digits ? spec->precision : digits;

	size_t width = spec->flags.min_width_defined &&
		spec->min_width > digits_and_zeros ?
			spec->min_width : digits_and_zeros;

	acc_len += width;

	char_t* dest = output_verbatim(fmt);

	if (dest != NULL)
	{
		size_t zeros = digits_and_zeros - digits;
		size_t spaces = width - digits_and_zeros;

		if (!spec->flags.minus)
		{
			dest = copy(dest, ch, digits);
			if (zeros > 0)
				b::construct_identical_copies(dest -= zeros,
					B_L_PREFIX('0'), zeros);
			if (spaces > 0)
				b::construct_identical_copies(dest -= spaces,
					!spec->flags.zero ? B_L_PREFIX(' ') :
					B_L_PREFIX('0'), spaces);
		}
		else
		{
			if (spaces > 0)
				b::construct_identical_copies(dest -= spaces,
					B_L_PREFIX(' '), spaces);
			dest = copy(dest, ch, digits);
			if (zeros > 0)
				b::construct_identical_copies(dest -= zeros,
					B_L_PREFIX('0'), zeros);
		}
	}

	return dest;
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

	b::Memory::Zero(&spec, sizeof(spec));

	// Parse flags
	for (;; ++ch)
	{
		switch (*ch)
		{
		case B_L_PREFIX(' '):
			spec.flags.space = true;
			continue;
		case B_L_PREFIX('#'):
			spec.flags.hash = true;
			continue;
		case B_L_PREFIX('\''):
		case B_L_PREFIX(','):
			spec.flags.quote = true;
			continue;
		case B_L_PREFIX('+'):
			spec.flags.plus = true;
			continue;
		case B_L_PREFIX('-'):
			spec.flags.minus = true;
			continue;
		case B_L_PREFIX('0'):
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
				spec.min_width = (unsigned) n;
			else
			{
				spec.min_width = (unsigned) -n;
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
					spec.precision = (unsigned) n;
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
	case B_L_PREFIX('i'):
		switch (spec.length_mod)
		{
		case conversion_spec::hh:
			return output_decimal<signed char, int>(&spec, ch + 1);
		case conversion_spec::h:
			return output_decimal<short, int>(&spec, ch + 1);
		case conversion_spec::l:
			return output_decimal<long, long>(&spec, ch + 1);
		case conversion_spec::j:
			return output_decimal<intmax_t, intmax_t>(&spec,
				ch + 1);
		case conversion_spec::z:
			return output_decimal<ssize_t, ssize_t>(&spec, ch + 1);
		case conversion_spec::t:
			return output_decimal<ptrdiff_t, ptrdiff_t>(&spec,
				ch + 1);
		case conversion_spec::ll:
		case conversion_spec::L:
			B_ASSERT("incompatible length modifier" && false);
		default:
			return output_decimal<int, int>(&spec, ch + 1);
		}
	case B_L_PREFIX('u'):
		switch (spec.length_mod)
		{
		case conversion_spec::hh:
			return output_unsigned<unsigned char, unsigned>(
				&spec, ch + 1);
		case conversion_spec::h:
			return output_unsigned<unsigned short, unsigned>(
				&spec, ch + 1);
		case conversion_spec::l:
			return output_unsigned<unsigned long, unsigned long>(
				&spec, ch + 1);
		case conversion_spec::j:
			return output_unsigned<uintmax_t, uintmax_t>(
				&spec, ch + 1);
		case conversion_spec::z:
			return output_unsigned<size_t, size_t>(&spec, ch + 1);
		case conversion_spec::t:
		case conversion_spec::ll:
		case conversion_spec::L:
			B_ASSERT("incompatible length modifier" && false);
		default:
			return output_unsigned<unsigned, unsigned>(
				&spec, ch + 1);
		}
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
