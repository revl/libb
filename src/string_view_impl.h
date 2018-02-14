/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2018 Damon Revoe
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

B_BEGIN_NAMESPACE

size_t string_view::find(char_t ch) const
{
	const char_t* ptr = view;

	for (size_t counter = view_length; counter > 0; --counter, ++ptr)
		if (*ptr == ch)
			return (size_t) (ptr - view);

	return (size_t) -1;
}

size_t string_view::rfind(char_t ch) const
{
	size_t index = view_length;
	const char_t* ptr = view + index;

	while (index-- > 0 && *--ptr != ch)
		;

	return index;
}

// This method cannot be made 'const' because either of the
// output pointers can point to *this* string_view.
bool string_view::split(char_t delim, string_view* slice, string_view* rest)
{
	size_t delim_pos = find(delim);

	if (delim_pos == (size_t) -1)
		return false;

	if (slice == NULL)
	{
		if (rest != NULL)
		{
			++delim_pos;

			rest->assign(view + delim_pos, view_length - delim_pos);
		}

		return true;
	}


	if (rest == NULL)
	{
		slice->assign(view, delim_pos);

		return true;
	}

	size_t this_view_length = view_length;

	slice->assign(view, delim_pos);

	++delim_pos;

	rest->assign(view + delim_pos, this_view_length - delim_pos);

	return true;
}

void string_view::trim_right(const string_view& samples)
{
	const char_t* end = view + view_length;

	while (view_length > 0 && samples.find(*--end) != (size_t) -1)
		--view_length;
}

void string_view::trim_left(const string_view& samples)
{
	while (view_length > 0 && samples.find(*view) != (size_t) -1)
	{
		++view;
		--view_length;
	}
}

B_END_NAMESPACE
