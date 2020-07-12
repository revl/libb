// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

// Declarations for the opaque pointer technique (pointer to the hidden
// implementation)

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
