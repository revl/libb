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

class pathname
{
// Types
public:
	class component
	{
	// Construction
	public:
		component(const char* name,
			const char* suffix, const char* end);

	// Attributes
	public:
		void GetName(const char*& name,
			const char*& end) const;

		void GetBasename(const char*& name,
			const char*& suffix) const;

		void GetSuffix(const char*& suffix,
			const char*& end) const;

		void AppendNameTo(string& path) const;

		void AppendBasenameTo(string& path) const;

		void AppendSuffixTo(string& path) const;

	// Implementation
	private:
		const char* component_name;
		const char* component_suffix;
		const char* component_end;
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

	bool CanBeFilename() const;

	void AppendPathnameTo(string& path) const;

// Operations
public:
	void Assign(const pathname& rhs);

	void Assign(const string_view& path);

	void ChDir(const pathname& rhs);

	void ChDir(const string_view& path);

	void GoUpDir();

// Implementation
private:
	component_array pathname_components;

	unsigned up_dir_level;

	bool can_be_filename;

	void AddComponent(const char* name,
		const char* suffix, const char* end);

	void components(string& path) const;
};

inline pathname::component::component(const char* name_arg,
		const char* suffix_arg, const char* end_arg) :
	component_name(name_arg),
	component_suffix(suffix_arg),
	component_end(end_arg)
{
}

inline void pathname::component::GetName(const char*& name,
	const char*& end) const
{
	name = this->component_name;
	end = this->component_end;
}

inline void pathname::component::GetBasename(const char*& name,
	const char*& suffix) const
{
	name = this->component_name;
	suffix = this->component_suffix;
}

inline void pathname::component::GetSuffix(const char*& suffix,
	const char*& end) const
{
	suffix = this->component_suffix;
	end = this->component_end;
}

inline void pathname::component::AppendNameTo(string& path) const
{
	path.append(component_name, component_end - component_name);
}

inline void pathname::component::AppendBasenameTo(string& path) const
{
	path.append(component_name, component_suffix - component_name);
}

inline void pathname::component::AppendSuffixTo(string& path) const
{
	path.append(component_suffix, component_end - component_suffix);
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

inline bool pathname::CanBeFilename() const
{
	return can_be_filename;
}

inline void pathname::Assign(const pathname& rhs)
{
	*this = rhs;
}

inline void pathname::Assign(const string_view& path)
{
	pathname_components.clear();
	up_dir_level = 0;

	ChDir(path);
}

inline void pathname::GoUpDir()
{
	if (!pathname_components.is_empty())
		pathname_components.erase(pathname_components.size() - 1);
	else
		if (up_dir_level != UINT_MAX)
			++up_dir_level;
}

inline void pathname::AddComponent(const char* name,
	const char* suffix, const char* end)
{
	pathname_components.append(1, component(name,
		suffix == NULL ? end : suffix, end));
}

B_END_NAMESPACE

#endif /* !defined(B_PATHNAME_H) */
