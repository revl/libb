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

#ifndef B_PATHNAME_H
#define B_PATHNAME_H

#include "string_view.h"
#include "array.h"

#include <limits.h>

B_BEGIN_NAMESPACE

// Pathname parsing and normalization with additional
// modification operations.
class pathname
{
// Types
public:
	class component
	{
	// Accessors
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

	// Implementation
	private:
		friend class pathname;

		component(const char* name_arg,
			const char* suffix_arg, const char* end_arg);

		const char* component_name;
		const char* component_name_suffix;
		const char* component_name_end;
	};

	typedef array<component> component_array;

// Construction
public:
	pathname();

	pathname(const string_view& path);

// Attributes
public:
	const component_array& components() const;

	bool IsAbsolute() const;
	int GetUpDirLevel() const;

	// Returns true if this pathname can represent a file.
	// A pathname cannot be a filename if it ends with a
	// slash, '.', or '..'.
	bool can_represent_file() const;

	void AppendPathnameTo(string& path) const;

// Operations
public:
	void assign(const pathname& rhs);

	void assign(const string_view& path);

	void ChDir(const pathname& rhs);

	void ChDir(const string_view& path);

	void go_one_level_up();

// Implementation
private:
	component_array pathname_components;

	unsigned up_dir_level;

	bool can_be_filename;

	void append_component(const char* name,
		const char* suffix, const char* end);

	void components(string& path) const;
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
	return string_view(component_name, component_name_end - component_name);
}

inline string_view pathname::component::basename() const
{
	return string_view(component_name,
		component_name_suffix - component_name);
}

inline string_view pathname::component::suffix() const
{
	return string_view(component_name_suffix,
		component_name_end - component_name_suffix);
}

inline pathname::pathname() :
	up_dir_level(0), can_be_filename(false)
{
}

inline pathname::pathname(const string_view& path) :
	up_dir_level(0), can_be_filename(false)
{
	ChDir(path);
}

inline const pathname::component_array& pathname::components() const
{
	return pathname_components;
}

inline bool pathname::IsAbsolute() const
{
	return up_dir_level == UINT_MAX;
}

inline int pathname::GetUpDirLevel() const
{
	return up_dir_level;
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
	pathname_components.clear();
	up_dir_level = 0;

	ChDir(path);
}

inline void pathname::go_one_level_up()
{
	if (!pathname_components.is_empty())
		pathname_components.erase(pathname_components.size() - 1);
	else
		if (up_dir_level != UINT_MAX)
			++up_dir_level;
}

inline void pathname::append_component(const char* name,
	const char* suffix, const char* end)
{
	pathname_components.append(1, component(name,
		suffix == NULL ? end : suffix, end));
}

B_END_NAMESPACE

#endif /* !defined(B_PATHNAME_H) */
