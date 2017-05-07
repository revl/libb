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

B_BEGIN_NAMESPACE

size_t string_view::find(char_t c) const
{
	size_t counter = length();

	const char_t* vc = view;

	while (counter-- > 0)
	{
		if (*vc == c)
			return (size_t) (vc - view);

		++vc;
	}

	return (size_t) -1;
}

size_t string_view::rfind(char_t c) const
{
	size_t index = length();
	const char_t* vc = view + index;

	while (index-- > 0 && *--vc != c)
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

void string_view::trim_right(const char_t* samples)
{
	const char_t* end = view + view_length;

	while (view_length > 0 && find_char(samples, *--end) != NULL)
		--view_length;
}

void string_view::trim_left(const char_t* samples)
{
	while (view_length > 0 && find_char(samples, *view) != NULL)
	{
		++view;
		--view_length;
	}
}

B_END_NAMESPACE
