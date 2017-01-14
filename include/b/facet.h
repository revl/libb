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

#ifndef B_FACET_H
#define B_FACET_H

#include "ref_count.h"

B_BEGIN_NAMESPACE

// The base class for all interfaces in the library.
class facet
{
public:
	// Initializes the reference count with zero.
	facet();

	// Initializes the reference count with zero.
	facet(const facet&);

	// Increases the reference count by one.
	void add_ref() const;

	// Decreases the reference count and, if it becomes
	// zero, calls the release_object() method.
	void release() const;

	// Decrements the reference count in the object that
	// implenets this interface and, if this is the last
	// facet of the object, the object gets deleted.
	virtual void release_object() = 0;

	// Makes sure the counter is not overwritten when
	// one object is assigned to another.
	facet& operator =(facet&);

protected:
	// Protected destructor prohibits deleting the object
	// via a pointer to this interface.
	virtual ~facet()
	{
		B_ASSERT(refs <= 0);
	}

	// The reference counter.
	mutable ref_count refs;
};

inline facet::facet()
{
	refs = 0;
}

inline facet::facet(const facet&)
{
	refs = 0;
}

inline void facet::add_ref() const
{
	++refs;
}

inline void facet::release() const
{
	B_ASSERT(this != NULL);

	if (!--refs)
		const_cast<facet*>(this)->release_object();
}

inline facet& facet::operator =(facet&)
{
	return *this;
}

B_END_NAMESPACE

#include <b/ref.h>

#endif /* !defined(B_FACET_H) */
