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

#include "string.h"
#include "array.h"

#include <limits.h>

B_BEGIN_NAMESPACE

class Pathname
{
// Types
public:
	class Component
	{
	// Construction
	public:
		Component(const char* name,
			const char* suffix, const char* end);

	// Attributes
	public:
		void GetName(const char*& name,
			const char*& end) const;

		void GetBasename(const char*& name,
			const char*& suffix) const;

		void GetSuffix(const char*& suffix,
			const char*& end) const;

		void AppendNameTo(String& pathname) const;

		void AppendBasenameTo(String& pathname) const;

		void AppendSuffixTo(String& pathname) const;

	// Implementation
	private:
		const char* name;
		const char* suffix;
		const char* end;
	};

	typedef array<Component> ComponentArray;

// Construction
public:
	Pathname();

	Pathname(const char* pathname, int count);

// Attributes
public:
	const ComponentArray& GetComponents() const;

	bool IsAbsolute() const;
	int GetUpDirLevel() const;

	bool CanBeFilename() const;

	void AppendPathnameTo(String& pathname) const;

// Operations
public:
	void Assign(const Pathname& rhs);

	void Assign(const char* pathname, int count);

	void ChDir(const Pathname& rhs);

	void ChDir(const char* pathname, int count);

	void GoUpDir();

// Implementation
private:
	ComponentArray components;

	int up_dir_level;

	bool can_be_filename;

	void AddComponent(const char* name,
		const char* suffix, const char* end);

	void GetComponents(String& pathname) const;
};

inline Pathname::Component::Component(const char* name,
	const char* suffix, const char* end) :
	name(name), suffix(suffix), end(end)
{
}

inline void Pathname::Component::GetName(const char*& name,
	const char*& end) const
{
	name = this->name;
	end = this->end;
}

inline void Pathname::Component::GetBasename(const char*& name,
	const char*& suffix) const
{
	name = this->name;
	suffix = this->suffix;
}

inline void Pathname::Component::GetSuffix(const char*& suffix,
	const char*& end) const
{
	suffix = this->suffix;
	end = this->end;
}

inline void Pathname::Component::AppendNameTo(String& pathname) const
{
	pathname.Append(name, end - name);
}

inline void Pathname::Component::AppendBasenameTo(String& pathname) const
{
	pathname.Append(name, suffix - name);
}

inline void Pathname::Component::AppendSuffixTo(String& pathname) const
{
	pathname.Append(suffix, end - suffix);
}

inline Pathname::Pathname() :
	up_dir_level(0), can_be_filename(false)
{
}

inline Pathname::Pathname(const char* pathname, int count) :
	up_dir_level(0), can_be_filename(false)
{
	ChDir(pathname, count);
}

inline const Pathname::ComponentArray& Pathname::GetComponents() const
{
	return components;
}

inline bool Pathname::IsAbsolute() const
{
	return up_dir_level == INT_MAX;
}

inline int Pathname::GetUpDirLevel() const
{
	return up_dir_level;
}

inline bool Pathname::CanBeFilename() const
{
	return can_be_filename;
}

inline void Pathname::Assign(const Pathname& rhs)
{
	*this = rhs;
}

inline void Pathname::Assign(const char* pathname, int count)
{
	components.RemoveAll();
	up_dir_level = 0;

	ChDir(pathname, count);
}

inline void Pathname::GoUpDir()
{
	if (!components.is_empty())
		components.RemoveAt(components.GetSize() - 1);
	else
		if (up_dir_level != INT_MAX)
			++up_dir_level;
}

inline void Pathname::AddComponent(const char* name,
	const char* suffix, const char* end)
{
	components.Append(Component(name,
		suffix == NULL ? end : suffix, end));
}

B_END_NAMESPACE

#endif /* !defined(B_PATHNAME_H) */
