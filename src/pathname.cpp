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
			for (; i > 0; --i)
				path.append(".." B_PATH_SEPARATOR_SZ, 3);

			components(path);
		}
		else
		{
			while (--i > 0)
				path.append(".." B_PATH_SEPARATOR_SZ, 3);

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

	const char* component_name;
	const char* suffix;

	can_be_filename = false;

	if (remaining_len == 0)
		return;

	switch (*current_char)
	{
	case B_PATH_SEPARATOR:
		pathname_components.clear();
		up_dir_level = UINT_MAX;

		goto slash;

	case '.':
		goto starts_with_dot;
	}

next_component:
	component_name = current_char;
	suffix = NULL;

continue_parsing_component:
	for (;;)
	{
		++current_char;

		if (--remaining_len == 0)
		{
			append_component(component_name, suffix, current_char);

			can_be_filename = true;

			return;
		}

		switch (*current_char)
		{
		case B_PATH_SEPARATOR:
			append_component(component_name, suffix, current_char);

			goto slash;

		case '.':
			suffix = current_char;
		}
	}

starts_with_dot:
	if (--remaining_len == 0)
		return;

	component_name = current_char;
	suffix = NULL;

	switch (*++current_char)
	{
	case B_PATH_SEPARATOR:
		goto slash;

	case '.':
		suffix = current_char;

		if (--remaining_len == 0)
		{
			go_one_level_up();

			return;
		}

		switch (*++current_char)
		{
		case B_PATH_SEPARATOR:
			go_one_level_up();

			goto slash;

		case '.':
			suffix = current_char;
		}
	}

	goto continue_parsing_component;

slash:
	if (--remaining_len == 0)
		return;

	switch (*++current_char)
	{
	case B_PATH_SEPARATOR:
		goto slash;

	case '.':
		goto starts_with_dot;
	}

	goto next_component;
}

void pathname::components(string& path) const
{
	size_t n = pathname_components.size();

	if (n > 0)
	{
		const component* current_component = pathname_components.data();

		path.append(current_component->component_name,
			current_component->component_name_end -
			current_component->component_name);

		while (--n > 0)
		{
			++current_component;

			path.append(B_PATH_SEPARATOR);

			path.append(current_component->component_name,
				current_component->component_name_end -
				current_component->component_name);
		}
	}
}

B_END_NAMESPACE
