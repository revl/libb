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

#include <b/pathname.h>

B_BEGIN_NAMESPACE

void pathname::AppendPathnameTo(string& path) const
{
	switch (up_dir_level)
	{
	case UINT_MAX:
		path.append(B_PATH_SEPARATOR);

		if (!pathname_components.is_empty())
			components(path);
		break;

	case 0:
		if (!pathname_components.is_empty())
			components(path);
		else
			path.append('.');
		break;

	default:
		unsigned i = up_dir_level;

		if (!pathname_components.is_empty())
		{
			while (--i >= 0)
				path.append(".."
					B_PATH_SEPARATOR_SZ, 3);

			components(path);
		}
		else
		{
			while (--i > 0)
				path.append(".."
					B_PATH_SEPARATOR_SZ, 3);

			path.append("..", 2);
		}
	}
}

void pathname::ChDir(const pathname& rhs)
{
	switch (rhs.up_dir_level)
	{
	case UINT_MAX:
		*this = rhs;
		break;

	default:
		if (rhs.up_dir_level <= pathname_components.size())
			pathname_components.erase(pathname_components.size() -
				rhs.up_dir_level,
				rhs.up_dir_level);
		else
		{
			up_dir_level += rhs.up_dir_level -
				(unsigned) pathname_components.size();

			pathname_components.clear();
		}

	case 0:
		pathname_components.append(rhs.pathname_components);

		can_be_filename = rhs.can_be_filename;
	}
}

void pathname::ChDir(const string_view& path)
{
	const char* current_char = path.data();
	size_t remaining_len = path.length();

	const char* component_start;
	const char* suffix;

	can_be_filename = false;

	if (remaining_len == 0)
		return;

	switch (*current_char)
	{
	case B_PATH_SEPARATOR:
		pathname_components.clear();
		up_dir_level = UINT_MAX;

		goto Slash;

	case '.':
		goto DotFirst;
	}

NextComponent:
	component_start = current_char;
	suffix = NULL;

Continue:
	for (;;)
	{
		++current_char;

		if (--remaining_len == 0)
		{
			AddComponent(component_start, suffix, current_char);

			can_be_filename = true;

			return;
		}

		switch (*current_char)
		{
		case B_PATH_SEPARATOR:
			AddComponent(component_start, suffix, current_char);

			goto Slash;

		case '.':
			suffix = current_char;
		}
	}

DotFirst:
	if (--remaining_len == 0)
		return;

	component_start = current_char;
	suffix = NULL;

	switch (*++current_char)
	{
	case B_PATH_SEPARATOR:
		goto Slash;

	case '.':
		suffix = current_char;

		if (--remaining_len == 0)
		{
			GoUpDir();

			return;
		}

		switch (*++current_char)
		{
		case B_PATH_SEPARATOR:
			GoUpDir();

			goto Slash;

		case '.':
			suffix = current_char;
		}
	}

	goto Continue;

Slash:
	if (--remaining_len == 0)
		return;

	switch (*++current_char)
	{
	case B_PATH_SEPARATOR:
		goto Slash;

	case '.':
		goto DotFirst;
	}

	goto NextComponent;
}

void pathname::components(string& path) const
{
	unsigned remaining_len = (unsigned) pathname_components.size();

	if (remaining_len > 0)
	{
		const component* current_component = pathname_components.data();

		current_component->AppendNameTo(path);

		while (--remaining_len > 0)
		{
			path.append(B_PATH_SEPARATOR);

			(++current_component)->AppendNameTo(path);
		}
	}
}

B_END_NAMESPACE
