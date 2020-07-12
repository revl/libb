// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#ifndef B_EXCEPTION_H
#define B_EXCEPTION_H

#include "host.h"

B_BEGIN_NAMESPACE

// The base class for all other exception types in the library.
class exception
#if defined(B_USE_STL)
	: public std::exception
#endif /* defined(B_USE_STL) */
{
#if !defined(B_USE_STL)
public:
	virtual ~exception()
		throw ()
	{
	}
#endif /* !defined(B_USE_STL) */
};

B_END_NAMESPACE

#endif /* !defined(B_EXCEPTION_H) */
