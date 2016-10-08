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

B_STATIC_CONST_STRING(slash, B_PATH_SEPARATOR_SZ);
B_STATIC_CONST_STRING(dot, ".");
B_STATIC_CONST_STRING(double_dot, "..");

string pathname::str() const
{
	size_t n = pathname_components.size();

	if (n == 0)
	{
		switch (up_dir_level)
		{
		case UINT_MAX:
			return slash;

		case 0:
			return dot;
		}

		string path = double_dot;

		for (unsigned i = up_dir_level; --i > 0; )
			path.append(B_PATH_SEPARATOR_SZ "..", 3);

		return path;
	}

	string path;

	switch (up_dir_level)
	{
	case UINT_MAX:
		path = slash;
		break;

	case 0:
		break;

	default:
		for (unsigned i = up_dir_level; i > 0; --i)
			path.append(".." B_PATH_SEPARATOR_SZ, 3);
	}

	const component* comp = pathname_components.data();

	path.append(comp->component_name,
		comp->component_name_end - comp->component_name);

	while (--n > 0)
	{
		++comp;

		path.append(B_PATH_SEPARATOR);

		path.append(comp->component_name,
			comp->component_name_end - comp->component_name);
	}

	return path;
}

void pathname::append(const pathname& rhs)
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

void pathname::append(const string_view& path)
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
			go_up_one_level();

			return;
		}

		switch (*++current_char)
		{
		case B_PATH_SEPARATOR:
			go_up_one_level();

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

B_END_NAMESPACE
