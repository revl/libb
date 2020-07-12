// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#ifndef B_CUSTOM_EXCEPTION_H
#define B_CUSTOM_EXCEPTION_H

#include "runtime_exception.h"

B_BEGIN_NAMESPACE

// A type of runtime_exception with a couple of convenient constructors.
class custom_exception : public runtime_exception
{
public:
	// Initializes this object with a preformatted error message.
	custom_exception(const string& msg);

	// Initializes the error message using a format string
	// and a variable number of arguments.
	custom_exception(const char* fmt, ...) B_PRINTF_STYLE(2, 3);

	// Returns the message that was passed to the constructor.
	virtual string message() const;

#ifdef B_USE_STL
	virtual const char* what() const
		throw ();
#endif /* defined(B_USE_STL) */

	virtual ~custom_exception()
		throw ();

private:
	string error_message;
};

inline custom_exception::custom_exception(const string& msg) :
	error_message(msg)
{
}

B_END_NAMESPACE

#endif /* !defined(B_CUSTOM_EXCEPTION_H) */
