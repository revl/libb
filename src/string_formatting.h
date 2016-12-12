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

// This file contains implementation of the printf-style string
// formatting functions format_buffer() and format_buffer_va(),
// which are also used by the string::format family of methods.
//
// The reason for implementing string formatting from scratch was
// to avoid dealing with missing runtime library functions on
// different platforms as well as to have a single implementation
// with known, predictable characteristics.
//
// Implementation notes:
//
// These formatting functions are designed to never require more than
// a single pass of the format string parsing or more than a single
// memory allocation regardless of the format string complexity.
//
// The 'string_formatting' structure below is what implements this goal.
// Its methods call each other recursively to parse the format string
// and estimate the length of the result string. Each recursive call
// saves its intermediate data on the stack. The deepest call allocates
// the exact amount of memory needed for the target buffer.
// When the recursion unwinds, each recursive call copies its data to
// the designated place in that target buffer.

namespace
{
	struct string_formatting
	{
		b::allocator* buffer_allocator;
		const char_t* fmt;
		va_list ap;
		size_t acc_len;
		char_t* allocated;
		char_t* dest;

		string_formatting(b::allocator* alloc, const char_t* fmt_arg) :
			buffer_allocator(alloc),
			fmt(fmt_arg),
			acc_len(0)
		{
		}

		void output_string(const char_t* source, size_t len)
		{
			switch (len)
			{
			case 0:
				break;
			case 1:
				*--dest = *source;
				break;
			default:
				b::construct_copies(dest -= len, source, len);
			}
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
			if (acc_len > 0)
			{
				allocated = reinterpret_cast<char_t*>(
					buffer_allocator->allocate(
						acc_len * sizeof(char_t)));

				if (allocated != NULL)
					dest = allocated + acc_len;
				else
				{
					acc_len = 0;
					dest = NULL;
				}
			}
			else
				allocated = dest = NULL;
		}

		struct conversion_flags
		{
			bool space : 1;
			bool hash : 1;
			bool quote : 1;
			bool plus : 1;
			bool minus : 1;
			bool zero : 1;
			bool min_width_defined : 1;
			bool precision_defined : 1;
		};

		conversion_flags flags;
		unsigned min_width;
		unsigned precision;

		struct int_conv_pos
		{
			char_t* pos;

			void add_char(char_t ch)
			{
				*--pos = ch;
			}

			template <class T>
			void add_digit(T digit)
			{
				*--pos = (char_t) (B_L_PREFIX('0') + digit);
			}
		};

		template <size_t Max_buf_len>
		struct int_conv_buffer : public int_conv_pos
		{
			char_t buffer[Max_buf_len];

			int_conv_buffer()
			{
				pos = buffer + Max_buf_len;
			}

			size_t len() const
			{
				return (size_t) (buffer + Max_buf_len - pos);
			}
		};

		template <class T>
		void convert_positive_to_decimal(T number, int_conv_pos* buffer)
		{
			if (!flags.quote)
				do
					buffer->add_digit(number % 10);
				while ((number /= 10) != 0);
			else
			{
				unsigned countdown_to_comma = 3;

				for (;;)
				{
					buffer->add_digit(number % 10);
					if ((number /= 10) == 0)
						break;
					if (--countdown_to_comma == 0)
					{
						countdown_to_comma = 3;
						buffer->add_char(
							B_L_PREFIX(','));
					}
				}
			}
		}

		// Offset within the string containing all possible prefixes.
		// The offset has the property that, when shifted right by 3
		// bits, it gives the lengths of the prefix.
		enum prefix_offset
		{
			no_prefix = 0,
			// Prefixes of length 1
			prefix_space = 8,
			prefix_plus = 10,
			prefix_minus = 12,
			// Prefixes of length 2
			prefix_0b = 16,
			prefix_0x = 18
		};

		void recurse_then_output_int(const char_t* buffer,
			size_t digits, size_t digits_and_zeros,
			prefix_offset prefix);

		void recurse_then_output_non_zero_int(const char_t* buffer,
			size_t digits, prefix_offset prefix)
		{
			recurse_then_output_int(buffer, digits,
				/*digits_and_zeros=*/ flags.precision_defined &&
					precision > digits ? precision : digits,
				prefix);
		}

		void recurse_then_output_zero_int()
		{
			recurse_then_output_int(/*buffer=*/ NULL,
				/*digits=*/ 0,
				/*digits_and_zeros=*/ !flags.precision_defined ?
					1 : precision, no_prefix);
		}

		void recurse_then_output_zero_int(prefix_offset prefix)
		{
			if (flags.precision_defined)
				recurse_then_output_int(/*buffer=*/ NULL,
					/*digits=*/ 0,
					/*digits_and_zeros=*/ precision,
					precision > 0 ? prefix : no_prefix);
			else
				recurse_then_output_int(/*buffer=*/ NULL,
					/*digits=*/ 0,
					/*digits_and_zeros=*/ 1, prefix);
		}

		template <class T, class U, class Arg>
		void process_d_conversion();

		template <class T, class Arg>
		void process_u_conversion();

		template <class T, class Arg>
		void process_o_conversion();

		template <class T, class Arg>
		void process_x_conversion(const char_t* digit_chars);

		template <class T, class Arg>
		void process_b_conversion();

		template <class T>
		void process_n_conversion();

		void process_s_conversion();

		void process_conversion();

		void recurse();

		static const char_t lcase_hex[17];
		static const char_t ucase_hex[17];
	};
}

void string_formatting::recurse_then_output_int(const char_t* buffer,
	size_t digits, size_t digits_and_zeros, prefix_offset prefix)
{
	static const char_t prefix_chars[21] =
		B_L_PREFIX("          + -   0b0x");

	unsigned prefix_length = prefix >> 3;

	size_t digits_zeros_and_prefix = prefix_length > 0 ?
		digits_and_zeros + prefix_length : digits_and_zeros;

	// Preserve the contents of the 'flags' member variable,
	// which will be overwritten by the 'recurse()' call below.
	conversion_flags saved_flags = flags;

	size_t width = saved_flags.min_width_defined &&
		min_width > digits_zeros_and_prefix ?
			min_width : digits_zeros_and_prefix;

	acc_len += width;

	recurse();

	if (dest != NULL)
	{
		size_t zeros = digits_and_zeros - digits;
		size_t spaces = width - digits_zeros_and_prefix;

		if (!saved_flags.minus)
		{
			output_string(buffer, digits);

			if (!saved_flags.zero || saved_flags.precision_defined)
			{
				output_chars(B_L_PREFIX('0'), zeros);
				output_string(prefix_chars + prefix,
					prefix_length);
				output_chars(B_L_PREFIX(' '), spaces);
			}
			else
			{
				output_chars(B_L_PREFIX('0'), zeros + spaces);
				output_string(prefix_chars + prefix,
					prefix_length);
			}
		}
		else
		{
			output_chars(B_L_PREFIX(' '), spaces);
			output_string(buffer, digits);
			output_chars(B_L_PREFIX('0'), zeros);
			output_string(prefix_chars + prefix, prefix_length);
		}
	}
}

template <class T, class U, class Arg>
void string_formatting::process_d_conversion()
{
	T number = (T) va_arg(ap, Arg);

	if (number == 0)
		recurse_then_output_zero_int(flags.plus || flags.space ?
			prefix_space : no_prefix);
	else
	{
		int_conv_buffer<MAX_DECIMAL_BUF_LEN(T)> buffer;

		if (number > 0)
		{
			convert_positive_to_decimal(number, &buffer);

			recurse_then_output_non_zero_int(buffer.pos,
				buffer.len(), flags.plus ? prefix_plus :
					flags.space ? prefix_space : no_prefix);
		}
		else
		{
			convert_positive_to_decimal((U) -number, &buffer);

			recurse_then_output_non_zero_int(buffer.pos,
				buffer.len(), prefix_minus);
		}
	}
}

template <class T, class Arg>
void string_formatting::process_u_conversion()
{
	T number = (T) va_arg(ap, Arg);

	if (number > 0)
	{
		int_conv_buffer<MAX_UNSIGNED_BUF_LEN(T)> buffer;

		convert_positive_to_decimal(number, &buffer);

		recurse_then_output_non_zero_int(buffer.pos,
			buffer.len(), no_prefix);
	}
	else
		recurse_then_output_zero_int();
}

template <class T, class Arg>
void string_formatting::process_o_conversion()
{
	T number = (T) va_arg(ap, Arg);

	if (number > 0)
	{
		int_conv_buffer<MAX_OCTAL_BUF_LEN(T)> buffer;

		do
			buffer.add_digit(number % 8);
		while ((number /= 8) != 0);

		size_t digits = buffer.len();

		recurse_then_output_int(buffer.pos, digits,
			/*digits_and_zeros=*/ flags.precision_defined &&
				precision > digits ? precision :
				!flags.hash ? digits : digits + 1, no_prefix);
	}
	else
		recurse_then_output_zero_int();
}

template <class T, class Arg>
void string_formatting::process_x_conversion(const char_t* digit_chars)
{
	int_conv_buffer<MAX_HEX_BUF_LEN(T)> buffer;

	T number = (T) va_arg(ap, Arg);

	if (number > 0)
	{
		do
			buffer.add_char(digit_chars[number % 16]);
		while ((number /= 16) != 0);

		recurse_then_output_non_zero_int(buffer.pos, buffer.len(),
			flags.hash ? prefix_0x : no_prefix);
	}
	else
		recurse_then_output_zero_int(
			flags.hash ? prefix_0x : no_prefix);
}

template <class T, class Arg>
void string_formatting::process_b_conversion()
{
	int_conv_buffer<MAX_BINARY_BUF_LEN(T)> buffer;

	T number = (T) va_arg(ap, Arg);

	if (number > 0)
	{
		do
			buffer.add_digit(number % 2);
		while ((number /= 2) != 0);

		recurse_then_output_non_zero_int(buffer.pos, buffer.len(),
			flags.hash ? prefix_0b : no_prefix);
	}
	else
		recurse_then_output_zero_int(
			flags.hash ? prefix_0b : no_prefix);
}

template <class T>
void string_formatting::process_n_conversion()
{
	T* pos = va_arg(ap, T*);

	recurse();

	if (dest != NULL)
		*pos = (T) (dest - allocated);
}

void string_formatting::process_s_conversion()
{
	const char_t* str = va_arg(ap, const char_t*);

	// The 'flags' member variable will be overwritten by the
	// 'recurse()' call below. Make a local copy.
	conversion_flags saved_flags = flags;

	size_t str_len = saved_flags.precision_defined ?
		b::calc_length(str, precision) : b::calc_length(str);

	size_t width = saved_flags.min_width_defined &&
		min_width > str_len ? min_width : str_len;

	acc_len += width;

	recurse();

	if (dest != NULL)
	{
		size_t spaces = width - str_len;

		if (!saved_flags.minus)
		{
			output_string(str, str_len);
			output_chars(B_L_PREFIX(' '), spaces);
		}
		else
		{
			output_chars(B_L_PREFIX(' '), spaces);
			output_string(str, str_len);
		}
	}
}

void string_formatting::process_conversion()
{
	if (*fmt == B_L_PREFIX('%'))
	{
		++fmt;
		++acc_len;
		recurse();
		if (dest != NULL)
			output_char(B_L_PREFIX('%'));
		return;
	}

	b::memory::zero(&flags, sizeof(flags));

	// Parse flags
	for (;; ++fmt)
	{
		switch (*fmt)
		{
		case B_L_PREFIX(' '):
			flags.space = true;
			continue;
		case B_L_PREFIX('#'):
			flags.hash = true;
			continue;
		case B_L_PREFIX('\''):
		case B_L_PREFIX(','):
			flags.quote = true;
			continue;
		case B_L_PREFIX('+'):
			flags.plus = true;
			continue;
		case B_L_PREFIX('-'):
			flags.minus = true;
			continue;
		case B_L_PREFIX('0'):
			flags.zero = true;
			continue;
		}
		break;
	}

	// Parse min width
	if (*fmt > B_L_PREFIX('0') && *fmt <= B_L_PREFIX('9'))
	{
		min_width = unsigned(*fmt) - unsigned(B_L_PREFIX('0'));

		int digit;

		while ((digit = int(*++fmt) - int(B_L_PREFIX('0'))) >= 0 &&
				digit <= 9)
			min_width = min_width * 10 + unsigned(digit);

		flags.min_width_defined = true;
	}
	else
		if (*fmt == B_L_PREFIX('*'))
		{
			++fmt;

			int n = va_arg(ap, int);

			if (n >= 0)
				min_width = (unsigned) n;
			else
			{
				min_width = (unsigned) -n;
				flags.minus = true;
			}

			flags.min_width_defined = true;
		}

	// Parse precision
	if (*fmt == B_L_PREFIX('.'))
	{
		int digit = int(*++fmt) - int(B_L_PREFIX('0'));

		if (digit >= 0 && digit <= 9)
		{
			precision = (unsigned) digit;

			while ((digit = int(*++fmt) -
					int(B_L_PREFIX('0'))) >= 0 &&
					digit <= 9)
				precision = precision * 10 +
					unsigned(digit);
			flags.precision_defined = true;
		}
		else
			if (*fmt == B_L_PREFIX('*'))
			{
				++fmt;

				int n = va_arg(ap, int);

				if (n >= 0)
				{
					precision = (unsigned) n;
					flags.precision_defined = true;
				}
			}
			else
			{
				precision = 0;
				flags.precision_defined = true;
			}
	}

	switch (*fmt++)
	{
	case B_L_PREFIX('h'):
		switch (*fmt++)
		{
		case B_L_PREFIX('d'):
		case B_L_PREFIX('i'):
			process_d_conversion<short, unsigned short, int>();
			return;
		case B_L_PREFIX('u'):
			process_u_conversion<unsigned short, unsigned>();
			return;
		case B_L_PREFIX('o'):
			process_o_conversion<unsigned short, unsigned>();
			return;
		case B_L_PREFIX('X'):
			process_x_conversion<unsigned short, unsigned>(
				ucase_hex);
			return;
		case B_L_PREFIX('x'):
			process_x_conversion<unsigned short, unsigned>(
				lcase_hex);
			return;
		case B_L_PREFIX('b'):
			process_b_conversion<unsigned short, unsigned>();
			return;
		case B_L_PREFIX('n'):
			process_n_conversion<short>();
			return;
		}
		break;

	case B_L_PREFIX('j'):
		switch (*fmt++)
		{
		case B_L_PREFIX('d'):
		case B_L_PREFIX('i'):
			process_d_conversion<intmax_t, uintmax_t, intmax_t>();
			return;
		case B_L_PREFIX('u'):
			process_u_conversion<uintmax_t, uintmax_t>();
			return;
		case B_L_PREFIX('o'):
			process_o_conversion<uintmax_t, uintmax_t>();
			return;
		case B_L_PREFIX('X'):
			process_x_conversion<uintmax_t, uintmax_t>(ucase_hex);
			return;
		case B_L_PREFIX('x'):
			process_x_conversion<uintmax_t, uintmax_t>(lcase_hex);
			return;
		case B_L_PREFIX('b'):
			process_b_conversion<uintmax_t, uintmax_t>();
			return;
		case B_L_PREFIX('n'):
			process_n_conversion<intmax_t>();
			return;
		}
		break;

	case B_L_PREFIX('l'):
		switch (*fmt++)
		{
		case B_L_PREFIX('d'):
		case B_L_PREFIX('i'):
			process_d_conversion<long, unsigned long, long>();
			return;
		case B_L_PREFIX('u'):
			process_u_conversion<unsigned long, unsigned long>();
			return;
		case B_L_PREFIX('o'):
			process_o_conversion<unsigned long, unsigned long>();
			return;
		case B_L_PREFIX('X'):
			process_x_conversion<unsigned long, unsigned long>(
				ucase_hex);
			return;
		case B_L_PREFIX('x'):
			process_x_conversion<unsigned long, unsigned long>(
				lcase_hex);
			return;
		case B_L_PREFIX('b'):
			process_b_conversion<unsigned long, unsigned long>();
			return;
		case B_L_PREFIX('n'):
			process_n_conversion<long>();
			return;
		}
		break;

	case B_L_PREFIX('t'):
		switch (*fmt++)
		{
		case B_L_PREFIX('d'):
		case B_L_PREFIX('i'):
			process_d_conversion<ptrdiff_t, size_t, ptrdiff_t>();
			return;
		case B_L_PREFIX('b'):
		case B_L_PREFIX('o'):
		case B_L_PREFIX('u'):
		case B_L_PREFIX('X'):
		case B_L_PREFIX('x'):
			B_ASSERT("incompatible length modifier" && false);
			return;
		case B_L_PREFIX('n'):
			process_n_conversion<ptrdiff_t>();
			return;
		}
		break;

	case B_L_PREFIX('z'):
		switch (*fmt++)
		{
		case B_L_PREFIX('d'):
		case B_L_PREFIX('i'):
			process_d_conversion<ssize_t, size_t, ssize_t>();
			return;
		case B_L_PREFIX('u'):
			process_u_conversion<size_t, size_t>();
			return;
		case B_L_PREFIX('o'):
			process_o_conversion<size_t, size_t>();
			return;
		case B_L_PREFIX('X'):
			process_x_conversion<size_t, size_t>(ucase_hex);
			return;
		case B_L_PREFIX('x'):
			process_x_conversion<size_t, size_t>(lcase_hex);
			return;
		case B_L_PREFIX('b'):
			process_b_conversion<size_t, size_t>();
			return;
		case B_L_PREFIX('n'):
			process_n_conversion<size_t>();
			return;
		}
		break;

	case B_L_PREFIX('d'):
	case B_L_PREFIX('i'):
		process_d_conversion<int, unsigned, int>();
		return;

	case B_L_PREFIX('u'):
		process_u_conversion<unsigned, unsigned>();
		return;

	case B_L_PREFIX('o'):
		process_o_conversion<unsigned, unsigned>();
		return;

	case B_L_PREFIX('X'):
		process_x_conversion<unsigned, unsigned>(ucase_hex);
		return;

	case B_L_PREFIX('x'):
		process_x_conversion<unsigned, unsigned>(lcase_hex);
		return;

	case B_L_PREFIX('b'):
		process_b_conversion<unsigned, unsigned>();
		return;

	case B_L_PREFIX('n'):
		process_n_conversion<int>();
		return;

	case B_L_PREFIX('p'):
		flags.hash = true;

		if (!flags.precision_defined)
		{
			flags.precision_defined = true;
			precision = sizeof(void*) * 2;
		}

		process_x_conversion<size_t, void*>(ucase_hex);
		return;

	case B_L_PREFIX('s'):
		process_s_conversion();
		return;
	}

	B_ASSERT("unknown conversion type character" && false);
	acc_len = 0;
	dest = NULL;
}

void string_formatting::recurse()
{
	const char_t* start = fmt;
	size_t len = 0;

	for (;;)
	{
		if (*fmt == B_L_PREFIX('%'))
		{
			acc_len += len;
			++fmt;
			process_conversion();
			break;
		}

		if (*fmt == B_L_PREFIX('\0'))
		{
			acc_len += len;
			alloc_buffer();
			break;
		}

		++fmt;
		++len;
	}

	if (dest != NULL)
		output_string(start, len);
}

const char_t string_formatting::lcase_hex[17] = B_L_PREFIX("0123456789abcdef");
const char_t string_formatting::ucase_hex[17] = B_L_PREFIX("0123456789ABCDEF");

B_BEGIN_NAMESPACE

string_view format_buffer(allocator* alloc, const char_t* fmt, ...)
{
	string_formatting formatting(alloc, fmt);

	va_start(formatting.ap, fmt);
	formatting.recurse();
	va_end(formatting.ap);

	B_ASSERT(formatting.dest == formatting.allocated);

	return string_view(formatting.dest, formatting.acc_len);
}

string_view format_buffer_va(allocator* alloc, const char_t* fmt, va_list ap)
{
	string_formatting formatting(alloc, fmt);

	B_VA_COPY(formatting.ap, ap);
	formatting.recurse();
	B_VA_COPY_END(formatting.ap);

	B_ASSERT(formatting.dest == formatting.allocated);

	return string_view(formatting.dest, formatting.acc_len);
}

B_END_NAMESPACE
