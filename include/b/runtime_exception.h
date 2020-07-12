// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include "string.h"

#ifndef B_RUNTIME_EXCEPTION_H
#define B_RUNTIME_EXCEPTION_H

#include "exception.h"

B_BEGIN_NAMESPACE

// A base class for runtime exceptions. These errors are usually caused by
// external factors, such as: invalid input, lack of permissions, running
// out of disk space, and so on.
class runtime_exception : public exception
{
public:
	// Returns the detailed description of this error.
	virtual string message() const = 0;
};

B_END_NAMESPACE

#endif /* !defined(B_RUNTIME_EXCEPTION_H) */
