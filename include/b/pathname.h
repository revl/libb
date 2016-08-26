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

// Disable MS C++ warning C4251: 'components' : class
// 'b::array<class b::Pathname::Component>' needs to have
// dll-interface to be used by clients of class 'b::Pathname'
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4251)
#endif // _MSC_VER

class Pathname
{
// Types
public:
	class Component
	{
	// Construction
	public:
		Component(const B_CHAR* name,
			const B_CHAR* suffix, const B_CHAR* end);

	// Attributes
	public:
		void GetName(const B_CHAR*& name,
			const B_CHAR*& end) const;

		void GetBasename(const B_CHAR*& name,
			const B_CHAR*& suffix) const;

		void GetSuffix(const B_CHAR*& suffix,
			const B_CHAR*& end) const;

		void AppendNameTo(String& pathname) const;

		void AppendBasenameTo(String& pathname) const;

		void AppendSuffixTo(String& pathname) const;

	// Implementation
	private:
		const B_CHAR* name;
		const B_CHAR* suffix;
		const B_CHAR* end;
	};

	typedef array<Component> ComponentArray;

// Construction
public:
	Pathname();

	Pathname(const B_CHAR* pathname, int count);

// Attributes
public:
	const ComponentArray& GetComponents() const;

	bool IsAbsolute() const;
	int GetUpDirLevel() const;

	bool CanBeFilename() const;

	void AppendPathnameTo(String& pathname) const;

// Operations
public:
	void Assign(const Pathname& right_side);

	void Assign(const B_CHAR* pathname, int count);

	void ChDir(const Pathname& right_side);

	void ChDir(const B_CHAR* pathname, int count);

	void GoUpDir();

// Implementation
private:
	ComponentArray components;

	int up_dir_level;

	bool can_be_filename;

	void AddComponent(const B_CHAR* name,
		const B_CHAR* suffix, const B_CHAR* end);

	void GetComponents(String& pathname) const;
};

#ifdef _MSC_VER
#pragma warning (pop)
#endif // _MSC_VER

inline Pathname::Component::Component(const B_CHAR* name,
	const B_CHAR* suffix, const B_CHAR* end) :
	name(name), suffix(suffix), end(end)
{
}

inline void Pathname::Component::GetName(const B_CHAR*& name,
	const B_CHAR*& end) const
{
	name = this->name;
	end = this->end;
}

inline void Pathname::Component::GetBasename(const B_CHAR*& name,
	const B_CHAR*& suffix) const
{
	name = this->name;
	suffix = this->suffix;
}

inline void Pathname::Component::GetSuffix(const B_CHAR*& suffix,
	const B_CHAR*& end) const
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

inline Pathname::Pathname(const B_CHAR* pathname, int count) :
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

inline void Pathname::Assign(const Pathname& right_side)
{
	*this = right_side;
}

inline void Pathname::Assign(const B_CHAR* pathname, int count)
{
	components.RemoveAll();
	up_dir_level = 0;

	ChDir(pathname, count);
}

inline void Pathname::GoUpDir()
{
	if (!components.empty())
		components.RemoveAt(components.GetSize() - 1);
	else
		if (up_dir_level != INT_MAX)
			++up_dir_level;
}

inline void Pathname::AddComponent(const B_CHAR* name,
	const B_CHAR* suffix, const B_CHAR* end)
{
	components.Append(Component(name,
		suffix == NULL ? end : suffix, end));
}

B_END_NAMESPACE

#endif // !defined(B_PATHNAME_H)
