/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2020 Damon Revoe
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

// Pathname parsing and normalization

#ifndef B_PATHNAME_H
#define B_PATHNAME_H

#include "string_view.h"
#include "array.h"

#include <limits.h>

B_BEGIN_NAMESPACE

// Pathname parsing, normalization, and manipulation operations.
//
// The lifetime of this object must not exceed the lifetime of any
// string_view object that was used, directly or indirectly (e.g.
// via the join() method), to construct pathname components of this
// object.
class pathname
{
public:
	class component
	{
	public:
		// The full name of the component.
		string_view name() const;

		// Component name without the (optional) suffix.
		// The returned value is equal to 'name()' if
		// the component name does not contain a dot.
		string_view basename() const;

		// The (optional) suffix. For a filename, the
		// method returns its extension, which is the
		// part that starts with the last dot.
		// If the name of the component does not have
		// a suffix, an empty string_view is returned.
		string_view suffix() const;

	private:
		friend class pathname;

		component(const char* name_arg,
			const char* suffix_arg, const char* end_arg);

		const char* component_name;
		const char* component_name_suffix;
		const char* component_name_end;
	};

	typedef array<component> component_array;

	// Constructs an empty pathname object.
	pathname();

	// Parses and normalizes the specified pathname.
	pathname(const string_view& path);

	// Returns the normalized pathname as a string.
	string str() const;

	// Returns the array of named pathname components that
	// this object contains. Double-dot components are not
	// returned by this method.
	const component_array& components() const;

	// Returns true if this pathname starts with a slash.
	bool is_absolute() const;

	// Returns the number of double-dot components that this
	// pathname starts with. This method can only be applied
	// to relative (non-absolute) pathnames.
	unsigned number_of_levels_up() const;

	// Returns true if this pathname can represent a file.
	// A pathname cannot possibly be a filename if it ends
	// with a slash or a component that is "." or "..".
	bool can_represent_file() const;

	// Copies the contents of 'rhs' into this object.
	void assign(const pathname& rhs);

	// Reinitializes this object with components from 'path'.
	void assign(const string_view& path);

	// Adds components from 'rhs' to the current path.
	void join(const pathname& rhs);

	// Adds components from 'path' to the current path.
	void join(const string_view& path);

	// Removes the rightmost named component of the pathname
	// or, if there are no such components left, increments
	// the number of "levels up" that this object represents.
	void go_up_one_level();

	// Has the effect of calling go_up_one_level() the specified
	// number of times.
	void go_up(unsigned levels);

	// Returns a relative path that, when joined to 'basepath',
	// would produce an object equal to 'this'.
	//
	// Throws a 'runtime_exception' if 'this' pathname is
	// absolute and 'basepath' is not, or vice versa, in which
	// case knowing the current working directory would be
	// necessary to establish the relation between 'this' and
	// 'basepath'.
	pathname relative_to(const pathname& basepath) const;

private:
	component_array pathname_components;

	unsigned levels_up;

	bool can_be_filename;

	void append_component(const char* name,
		const char* suffix, const char* end);
};

inline pathname::component::component(const char* name_arg,
		const char* suffix_arg, const char* end_arg) :
	component_name(name_arg),
	component_name_suffix(suffix_arg),
	component_name_end(end_arg)
{
}

inline string_view pathname::component::name() const
{
	return string_view(component_name,
		(size_t) (component_name_end - component_name));
}

inline string_view pathname::component::basename() const
{
	return string_view(component_name,
		(size_t) (component_name_suffix - component_name));
}

inline string_view pathname::component::suffix() const
{
	return string_view(component_name_suffix,
		(size_t) (component_name_end - component_name_suffix));
}

inline pathname::pathname() :
	levels_up(0), can_be_filename(false)
{
}

inline const pathname::component_array& pathname::components() const
{
	return pathname_components;
}

inline bool pathname::is_absolute() const
{
	return levels_up == UINT_MAX;
}

inline unsigned pathname::number_of_levels_up() const
{
	return levels_up;
}

inline bool pathname::can_represent_file() const
{
	return can_be_filename;
}

inline void pathname::assign(const pathname& rhs)
{
	*this = rhs;
}

inline void pathname::assign(const string_view& path)
{
	pathname_components.empty();
	levels_up = 0;

	join(path);
}

inline void pathname::go_up_one_level()
{
	if (!pathname_components.is_empty())
		pathname_components.remove(pathname_components.length() - 1);
	else
		if (levels_up != UINT_MAX)
			++levels_up;

	can_be_filename = false;
}

inline void pathname::go_up(unsigned levels)
{
	if (pathname_components.length() >= levels)
		pathname_components.remove(
			pathname_components.length() - levels, levels);
	else
	{
		levels -= (unsigned) pathname_components.length();

		pathname_components.empty();

		if (levels_up != UINT_MAX)
			levels_up += levels;
	}

	can_be_filename = false;
}

inline void pathname::append_component(const char* name,
		const char* suffix, const char* end)
{
	pathname_components.append(component(name,
		suffix == NULL ? end : suffix, end));
}

B_END_NAMESPACE

#endif /* !defined(B_PATHNAME_H) */
