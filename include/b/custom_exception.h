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

#ifndef B_CUSTOMEXCEPTION_H
#define B_CUSTOMEXCEPTION_H

#ifdef __GNUG__
#pragma interface "b/custom_exception.h"
#endif // __GNUG__

#include "runtime_exception.h"

B_BEGIN_NAMESPACE

// Customizable error exception class.
class CustomException : public RuntimeException
{
// Constructors
public:
	// Constructs a new object with an empty description.
	CustomException();

	// Initializes this object with a preformed error message.
	CustomException(const string& initial_message);

	// Initializes this object with a formatted error message.
	CustomException(const B_CHAR* message_format, va_list arguments);

	// Initializes this object with a formatted error message.
	CustomException(const B_CHAR* message_format, ...);

// Attributes
public:
	// Sets the description message.
	void SetMessage(const string& message);

	// Builds an error description from a format string and a va_list.
	void SetMessage(const B_CHAR* message_format, va_list arguments);

	// Builds an error description from a printf-like format string
	void SetMessage(const B_CHAR* message_format, ...);

// Overridden
public:
	virtual void GetMessage(string& target) const
		throw ();

#ifdef B_USE_STL
	virtual const char* what() const
		throw ();
#endif // B_USE_STL

// Implementation
public:
	virtual ~CustomException()
		throw ()
	{
	}

private:
#ifdef B_USE_STL
	mutable
#endif // B_USE_STL
	string message;
};

inline CustomException::CustomException() :
	message()
{
}

inline CustomException::CustomException(const string& initial_message) :
	message(initial_message)
{
}

inline CustomException::CustomException(const B_CHAR* message_format,
	va_list arguments) : message()
{
	message.assignfv(message_format, arguments);
}

inline void CustomException::SetMessage(const string& new_message)
{
	message = new_message;
}

inline void CustomException::SetMessage(const B_CHAR* message_format,
	va_list arguments)
{
	message.assignfv(message_format, arguments);
}

B_END_NAMESPACE

#endif // !defined(B_CUSTOMEXCEPTION_H)
