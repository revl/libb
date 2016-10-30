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

#if defined(__GNUG__) && !defined(__clang__)
#pragma implementation "b/exception.h"
#pragma implementation "b/runtime_exception.h"
#pragma implementation "b/system_exception.h"
#pragma implementation "b/custom_exception.h"
#endif /* defined(__GNUG__) && !defined(__clang__) */

#include <b/system_exception.h>
#include <b/custom_exception.h>

B_BEGIN_NAMESPACE

system_exception::system_exception(const string& context_or_subject,
		int errno_value) :
	exception_context(context_or_subject),
	system_error_code(errno_value)
{
}

B_STATIC_CONST_STRING(oom_message, "Insufficient memory to "
	"allocate a buffer for a system error message.");

string system_exception::message() const
{
	try
	{
		return string::formatted("%s: %s", exception_context.data(),
			strerror(system_error_code));
	}
	catch (system_exception&)
	{
		return oom_message;
	}
}

#ifdef B_USE_STL
const char* system_exception::what() const
	throw ()
{
	message_buffer = message();

	return message_buffer.data();
}
#endif // B_USE_STL

custom_exception::custom_exception(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	error_message.format_va(fmt, ap);
	va_end(ap);
}

string custom_exception::message() const
{
	return error_message;
}

#ifdef B_USE_STL
const char* custom_exception::what() const
	throw ()
{
	return error_message.data();
}
#endif // B_USE_STL

B_END_NAMESPACE
