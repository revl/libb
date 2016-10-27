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
		char_t* dest;

		string_formatting(b::allocator* alloc) :
			buffer_allocator(alloc),
			acc_len(0)
		{
		}

		void output_string(const char_t* source, size_t len)
		{
			if (len > 0)
				b::construct_copies(dest -= len, source, len);
		}

		void output_chars(char_t filler, size_t len)
		{
			if (len > 0)
				b::construct_identical_copies(dest -= len,
					filler, len);
		}

		void output_char(char_t ch)
		{
			*--dest = ch;
		}

		void alloc_buffer()
		{
			dest = reinterpret_cast<char_t*>(
				buffer_allocator->allocate(
					acc_len * sizeof(char_t)));

			if (dest != NULL)
				dest += acc_len;
		}

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

		template <size_t Max_buf_len>
		struct int_conv_buffer
		{
			char_t buffer[Max_buf_len];
			char_t* pos;

			int_conv_buffer() : pos(buffer + Max_buf_len)
			{
			}

			template <class Char_type>
			void operator <<(Char_type ch)
			{
				*--pos = (char_t) ch;
			}

			size_t len() const
			{
				return (size_t) (buffer + Max_buf_len - pos);
			}
		};

		template <class T, class Arg>
		void process_decimal(const conversion_spec* spec,
			const char_t* fmt);

		template <class T, class Arg>
		void process_unsigned(const conversion_spec* spec,
			const char_t* fmt);

		void output_unsigned(const conversion_spec* spec,
			const char_t* fmt, const char_t* buffer, size_t digits,
			size_t digits_and_zeros);

		template <class T, class Arg>
		void process_octal(const conversion_spec* spec,
			const char_t* fmt);

		template <class T, class Arg>
		void process_hex(const conversion_spec* spec,
			const char_t* fmt, const char_t* digit_chars);

		void process_string(const char_t* fmt);
		void process_conversion(const char_t* fmt);
		void process_verbatim(const char_t* fmt);

		static char_t lcase_hex[17];
		static char_t ucase_hex[17];
	};
}

template <class T, class Arg>
void string_formatting::process_decimal(const conversion_spec* spec,
	const char_t* fmt)
{
	int_conv_buffer<MAX_DECIMAL_BUF_LEN(T)> buffer;

	char_t sign;

	T number = (T) va_arg(ap, Arg);

	if (number >= 0)
	{
		if (number > 0)
		{
			sign = spec->flags.plus ? B_L_PREFIX('+') :
				spec->flags.space ? B_L_PREFIX(' ') : 0;

			if (!spec->flags.quote)
				do
					buffer << B_L_PREFIX('0') + number % 10;
				while ((number /= 10) != 0);
			else
			{
				unsigned countdown_to_comma = 3;

				for (;;)
				{
					buffer << B_L_PREFIX('0') + number % 10;
					if ((number /= 10) == 0)
						break;
					if (--countdown_to_comma == 0)
					{
						countdown_to_comma = 3;
						buffer << B_L_PREFIX(',');
					}
				}
			}
		}
		else
			if (!spec->flags.precision_defined ||
					spec->precision != 0)
			{
				sign = spec->flags.plus || spec->flags.space ?
					B_L_PREFIX(' ') : 0;

				buffer << B_L_PREFIX('0');
			}
			else
				sign = 0;
	}
	else
	{
		sign = B_L_PREFIX('-');

		if (!spec->flags.quote)
			do
				buffer << B_L_PREFIX('0') - number % 10;
			while ((number /= 10) != 0);
		else
		{
			unsigned countdown_to_comma = 3;

			for (;;)
			{
				buffer << B_L_PREFIX('0') - number % 10;
				if ((number /= 10) == 0)
					break;
				if (--countdown_to_comma == 0)
				{
					countdown_to_comma = 3;
					buffer << B_L_PREFIX(',');
				}
			}
		}
	}

	size_t digits = buffer.len();

	size_t digits_and_zeros = spec->flags.precision_defined &&
		spec->precision > digits ? spec->precision : digits;

	size_t digits_zeros_and_sign =
		sign == 0 ? digits_and_zeros : digits_and_zeros + 1;

	size_t width = spec->flags.min_width_defined &&
		spec->min_width > digits_zeros_and_sign ?
			spec->min_width : digits_zeros_and_sign;

	acc_len += width;

	process_verbatim(fmt);

	if (dest != NULL)
	{
		size_t zeros = digits_and_zeros - digits;
		size_t spaces = width - digits_zeros_and_sign;

		if (!spec->flags.minus)
		{
			output_string(buffer.pos, digits);
			output_chars(B_L_PREFIX('0'), zeros);
			if (!spec->flags.zero)
			{
				if (sign != 0)
					output_char(sign);
				output_chars(B_L_PREFIX(' '), spaces);
			}
			else
			{
				output_chars(B_L_PREFIX('0'), spaces);
				if (sign != 0)
					output_char(sign);
			}
		}
		else
		{
			output_chars(B_L_PREFIX(' '), spaces);
			output_string(buffer.pos, digits);
			output_chars(B_L_PREFIX('0'), zeros);
			if (sign != 0)
				output_char(sign);
		}
	}
}

template <class T, class Arg>
void string_formatting::process_unsigned(const conversion_spec* spec,
	const char_t* fmt)
{
	int_conv_buffer<MAX_UNSIGNED_BUF_LEN(T)> buffer;

	T number = (T) va_arg(ap, Arg);

	if (number > 0)
		if (!spec->flags.quote)
			do
				buffer << B_L_PREFIX('0') + number % 10;
			while ((number /= 10) != 0);
		else
		{
			unsigned countdown_to_comma = 3;

			for (;;)
			{
				buffer << B_L_PREFIX('0') + number % 10;
				if ((number /= 10) == 0)
					break;
				if (--countdown_to_comma == 0)
				{
					countdown_to_comma = 3;
					buffer << B_L_PREFIX(',');
				}
			}
		}
	else
		if (!spec->flags.precision_defined || spec->precision != 0)
			buffer << B_L_PREFIX('0');

	size_t digits = buffer.len();

	output_unsigned(spec, fmt, buffer.pos, digits,
		/*digits_and_zeros=*/ spec->flags.precision_defined &&
			spec->precision > digits ? spec->precision : digits);
}

void string_formatting::output_unsigned(const conversion_spec* spec,
	const char_t* fmt, const char_t* buffer, size_t digits,
	size_t digits_and_zeros)
{
	size_t width = spec->flags.min_width_defined &&
		spec->min_width > digits_and_zeros ?
			spec->min_width : digits_and_zeros;

	acc_len += width;

	process_verbatim(fmt);

	if (dest != NULL)
	{
		size_t zeros = digits_and_zeros - digits;
		size_t spaces = width - digits_and_zeros;

		if (!spec->flags.minus)
		{
			output_string(buffer, digits);
			if (spec->flags.zero)
				output_chars(B_L_PREFIX('0'), zeros + spaces);
			else
			{
				output_chars(B_L_PREFIX('0'), zeros);
				output_chars(B_L_PREFIX(' '), spaces);
			}
		}
		else
		{
			output_chars(B_L_PREFIX(' '), spaces);
			output_string(buffer, digits);
			output_chars(B_L_PREFIX('0'), zeros);
		}
	}
}

template <class T, class Arg>
void string_formatting::process_octal(const conversion_spec* spec,
	const char_t* fmt)
{
	int_conv_buffer<MAX_OCTAL_BUF_LEN(T)> buffer;

	T number = (T) va_arg(ap, Arg);

	if (number > 0)
		do
			buffer << B_L_PREFIX('0') + number % 8;
		while ((number /= 8) != 0);
	else
		if (!spec->flags.precision_defined || spec->precision != 0)
			buffer << B_L_PREFIX('0');

	size_t digits = buffer.len();

	output_unsigned(spec, fmt, buffer.pos, digits,
		/*digits_and_zeros=*/ spec->flags.precision_defined &&
		spec->precision > digits ? spec->precision :
		!spec->flags.hash ? digits : digits + 1);
}

template <class T, class Arg>
void string_formatting::process_hex(const conversion_spec* spec,
	const char_t* fmt, const char_t* digit_chars)
{
	int_conv_buffer<MAX_HEX_BUF_LEN(T)> buffer;

	T number = (T) va_arg(ap, Arg);

	if (number > 0)
		do
			buffer << digit_chars[number % 16];
		while ((number /= 16) != 0);
	else
		if (!spec->flags.precision_defined || spec->precision != 0)
			buffer << B_L_PREFIX('0');

	size_t digits = buffer.len();

	size_t digits_and_zeros = spec->flags.precision_defined &&
		spec->precision > digits ? spec->precision : digits;

	size_t digits_zeros_and_0x =
		!spec->flags.hash ? digits_and_zeros : digits_and_zeros + 2;

	size_t width = spec->flags.min_width_defined &&
		spec->min_width > digits_zeros_and_0x ?
			spec->min_width : digits_zeros_and_0x;

	acc_len += width;

	process_verbatim(fmt);

	if (dest != NULL)
	{
		size_t zeros = digits_and_zeros - digits;
		size_t spaces = width - digits_zeros_and_0x;

		if (!spec->flags.minus)
		{
			output_string(buffer.pos, digits);
			output_chars(B_L_PREFIX('0'), zeros);
			if (!spec->flags.zero)
			{
				if (spec->flags.hash)
					output_string(B_L_PREFIX("0x"), 2);
				output_chars(B_L_PREFIX(' '), spaces);
			}
			else
			{
				output_chars(B_L_PREFIX('0'), spaces);
				if (spec->flags.hash)
					output_string(B_L_PREFIX("0x"), 2);
			}
		}
		else
		{
			output_chars(B_L_PREFIX(' '), spaces);
			output_string(buffer.pos, digits);
			output_chars(B_L_PREFIX('0'), zeros);
			if (spec->flags.hash)
				output_string(B_L_PREFIX("0x"), 2);
		}
	}
}

void string_formatting::process_string(const char_t* fmt)
{
	const char_t* str = va_arg(ap, const char_t*);

	size_t len = b::calc_length(str);
	acc_len += len;

	process_verbatim(fmt);

	if (dest != NULL)
		output_string(str, len);
}

void string_formatting::process_conversion(const char_t* fmt)
{
	const char_t* ch = fmt;

	if (*ch == B_L_PREFIX('%'))
	{
		++acc_len;
		process_verbatim(ch + 1);
		output_char(B_L_PREFIX('%'));
		return;
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
					spec.flags.precision_defined = true;
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
			process_decimal<signed char, int>(&spec, ch + 1);
			break;
		case conversion_spec::h:
			process_decimal<short, int>(&spec, ch + 1);
			break;
		case conversion_spec::l:
			process_decimal<long, long>(&spec, ch + 1);
			break;
		case conversion_spec::j:
			process_decimal<intmax_t, intmax_t>(&spec,
				ch + 1);
			break;
		case conversion_spec::z:
			process_decimal<ssize_t, ssize_t>(&spec, ch + 1);
			break;
		case conversion_spec::t:
			process_decimal<ptrdiff_t, ptrdiff_t>(&spec,
				ch + 1);
			break;
		case conversion_spec::ll:
		case conversion_spec::L:
			B_ASSERT("incompatible length modifier" && false);
		default:
			process_decimal<int, int>(&spec, ch + 1);
		}
		break;
	case B_L_PREFIX('u'):
		switch (spec.length_mod)
		{
		case conversion_spec::hh:
			process_unsigned<unsigned char, unsigned>(
				&spec, ch + 1);
			break;
		case conversion_spec::h:
			process_unsigned<unsigned short, unsigned>(
				&spec, ch + 1);
			break;
		case conversion_spec::l:
			process_unsigned<unsigned long, unsigned long>(
				&spec, ch + 1);
			break;
		case conversion_spec::j:
			process_unsigned<uintmax_t, uintmax_t>(&spec, ch + 1);
			break;
		case conversion_spec::z:
			process_unsigned<size_t, size_t>(&spec, ch + 1);
			break;
		case conversion_spec::t:
		case conversion_spec::ll:
		case conversion_spec::L:
			B_ASSERT("incompatible length modifier" && false);
		default:
			process_unsigned<unsigned, unsigned>(&spec, ch + 1);
		}
		break;
	case B_L_PREFIX('o'):
		switch (spec.length_mod)
		{
		case conversion_spec::hh:
			process_octal<unsigned char, unsigned>(&spec, ch + 1);
			break;
		case conversion_spec::h:
			process_octal<unsigned short, unsigned>(&spec, ch + 1);
			break;
		case conversion_spec::l:
			process_octal<unsigned long, unsigned long>(
				&spec, ch + 1);
			break;
		case conversion_spec::j:
			process_octal<uintmax_t, uintmax_t>(&spec, ch + 1);
			break;
		case conversion_spec::z:
			process_octal<size_t, size_t>(&spec, ch + 1);
			break;
		case conversion_spec::t:
		case conversion_spec::ll:
		case conversion_spec::L:
			B_ASSERT("incompatible length modifier" && false);
		default:
			process_octal<unsigned, unsigned>(&spec, ch + 1);
		}
		break;
	case B_L_PREFIX('X'):
		switch (spec.length_mod)
		{
		case conversion_spec::hh:
			process_hex<unsigned char, unsigned>(&spec, ch + 1,
				ucase_hex);
			break;
		case conversion_spec::h:
			process_hex<unsigned short, unsigned>(&spec, ch + 1,
				ucase_hex);
			break;
		case conversion_spec::l:
			process_hex<unsigned long, unsigned long>(&spec, ch + 1,
				ucase_hex);
			break;
		case conversion_spec::j:
			process_hex<uintmax_t, uintmax_t>(&spec, ch + 1,
				ucase_hex);
			break;
		case conversion_spec::z:
			process_hex<size_t, size_t>(&spec, ch + 1,
				ucase_hex);
			break;
		case conversion_spec::t:
		case conversion_spec::ll:
		case conversion_spec::L:
			B_ASSERT("incompatible length modifier" && false);
		default:
			process_hex<unsigned, unsigned>(&spec, ch + 1,
				ucase_hex);
		}
		break;
	case B_L_PREFIX('x'):
		switch (spec.length_mod)
		{
		case conversion_spec::hh:
			process_hex<unsigned char, unsigned>(&spec, ch + 1,
				lcase_hex);
			break;
		case conversion_spec::h:
			process_hex<unsigned short, unsigned>(&spec, ch + 1,
				lcase_hex);
			break;
		case conversion_spec::l:
			process_hex<unsigned long, unsigned long>(&spec, ch + 1,
				lcase_hex);
			break;
		case conversion_spec::j:
			process_hex<uintmax_t, uintmax_t>(&spec, ch + 1,
				lcase_hex);
			break;
		case conversion_spec::z:
			process_hex<size_t, size_t>(&spec, ch + 1,
				lcase_hex);
			break;
		case conversion_spec::t:
		case conversion_spec::ll:
		case conversion_spec::L:
			B_ASSERT("incompatible length modifier" && false);
		default:
			process_hex<unsigned, unsigned>(&spec, ch + 1,
				lcase_hex);
		}
		break;
	case B_L_PREFIX('s'):
		process_string(ch + 1);
		break;
	default:
		B_ASSERT("unknown conversion type character" && false);
		acc_len = (size_t) -1;
		dest = NULL;
	}
}

void string_formatting::process_verbatim(const char_t* fmt)
{
	const char_t* ch = fmt;
	size_t len = 0;

	for (;;)
	{
		if (*ch == B_L_PREFIX('%'))
		{
			acc_len += len;
			process_conversion(++ch);
			break;
		}
		else
			if (*ch == B_L_PREFIX('\0'))
			{
				acc_len += len;
				alloc_buffer();
				break;
			}
			else
			{
				++ch;
				++len;
			}
	}

	if (dest != NULL)
		output_string(fmt, len);
}

char_t string_formatting::lcase_hex[17] = B_L_PREFIX("0123456789abcdef");
char_t string_formatting::ucase_hex[17] = B_L_PREFIX("0123456789ABCDEF");

B_BEGIN_NAMESPACE

string_view format_buffer(allocator* alloc, const char_t* fmt, ...)
{
	string_formatting formatting(alloc);

	va_start(formatting.ap, fmt);
	formatting.process_verbatim(fmt);
	va_end(formatting.ap);

	return string_view(formatting.dest, formatting.acc_len);
}

string_view format_buffer_va(allocator* alloc, const char_t* fmt, va_list ap)
{
	string_formatting formatting(alloc);

	B_VA_COPY(formatting.ap, ap);
	formatting.process_verbatim(fmt);
	B_VA_COPY_END(formatting.ap);

	return string_view(formatting.dest, formatting.acc_len);
}

B_END_NAMESPACE
