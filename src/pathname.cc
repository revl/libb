// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/pathname.h>

#include <b/custom_exception.h>

B_BEGIN_NAMESPACE

B_STRING_LITERAL(slash, B_PATH_SEPARATOR_STR);
B_STRING_LITERAL(dot, ".");
B_STRING_LITERAL(double_dot, "..");

pathname::pathname(const string_view& path) :
	levels_up(0), can_be_filename(false)
{
	join(path);
}

string pathname::str() const
{
	size_t n = pathname_components.length();

	if (n == 0)
	{
		switch (levels_up)
		{
		case UINT_MAX:
			return slash;

		case 0:
			return dot;
		}

		string path = double_dot;

		for (unsigned i = levels_up; --i > 0; )
			path.append(B_PATH_SEPARATOR_STR "..", 3);

		return path;
	}

	string path;

	switch (levels_up)
	{
	case UINT_MAX:
		path = slash;
		break;

	case 0:
		break;

	default:
		for (unsigned i = levels_up; i > 0; --i)
			path.append(".." B_PATH_SEPARATOR_STR, 3);
	}

	const component* comp = pathname_components.data();

	path.append(comp->component_name,
		(size_t) (comp->component_name_end - comp->component_name));

	while (--n > 0)
	{
		++comp;

		path.append(B_PATH_SEPARATOR);

		path.append(comp->component_name,
			(size_t) (comp->component_name_end -
				comp->component_name));
	}

	return path;
}

void pathname::join(const pathname& rhs)
{
	// Handle the case of "/absolute/pathname".
	if (rhs.is_absolute())
	{
		*this = rhs;
		return;
	}

	can_be_filename = rhs.can_be_filename;

	// Handle the case of "relative/pathname".
	if (rhs.levels_up == 0)
	{
		pathname_components.append(rhs.pathname_components);
		return;
	}

	// Handle the case of "../relative/to/parent".

	// Check if 'rhs' completely wipes out all named
	// components in 'this'.
	if (pathname_components.length() <= rhs.levels_up)
	{
		if (levels_up != UINT_MAX)
			levels_up += rhs.levels_up -
				(unsigned) pathname_components.length();

		pathname_components = rhs.pathname_components;
		return;
	}

	pathname_components.overwrite(
		pathname_components.length() - rhs.levels_up,
		rhs.pathname_components);

	if (rhs.levels_up > rhs.pathname_components.length())
	{
		size_t remainder = (size_t) rhs.levels_up -
			rhs.pathname_components.length();

		pathname_components.remove(
			pathname_components.length() - remainder, remainder);
	}
}

void pathname::join(const string_view& path)
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
		pathname_components.empty();
		levels_up = UINT_MAX;

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

pathname pathname::relative_to(const pathname& basepath) const
{
	if (basepath.is_absolute() ^ this->is_absolute() ||
			basepath.levels_up > this->levels_up)
		throw custom_exception("Cannot make '%s' relative to '%s'.",
			this->str().data(), basepath.str().data());

	pathname relative(*this);

	const unsigned basepath_components =
		(unsigned) basepath.pathname_components.length();

	if (basepath.levels_up < this->levels_up)
	{
		relative.levels_up -= basepath.levels_up;
		relative.levels_up += basepath_components;
	}
	else
	{
		unsigned equal = 0;

		while (equal < basepath_components &&
				equal < pathname_components.length() &&
				basepath.pathname_components[equal].name() ==
					pathname_components[equal].name())
			++equal;

		relative.levels_up = basepath_components - equal;
		relative.pathname_components.remove(0, equal);
	}

	return relative;
}

B_END_NAMESPACE
