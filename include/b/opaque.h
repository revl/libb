/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2018 Damon Revoe
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

#include "object.h"
#include "ref.h"

#ifndef B_OPAQUE_H
#define B_OPAQUE_H

B_BEGIN_NAMESPACE

template <class Impl>
class opaque_ref : public ref<object>
{
public:
	opaque_ref();

	opaque_ref(Impl* impl_ptr);

	Impl* operator ->() const;
};

template <class Impl>
inline opaque_ref<Impl>::opaque_ref()
{
}

template <class Impl>
inline opaque_ref<Impl>::opaque_ref(Impl* impl_ptr) :
	ref<object>(impl_ptr)
{
}

template <class Impl>
inline Impl* opaque_ref<Impl>::operator ->() const
{
	return static_cast<Impl*>(ptr());
}

B_END_NAMESPACE

// Declares an opaque pointer to the implementation class.
// This macro is for use in the declaration of the interface class.
#define B_OPAQUE \
    protected: \
    class impl; \
    b::opaque_ref<impl> impl_ref; \
    public

#endif /* !defined(B_OPAQUE_H) */
