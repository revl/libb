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

#ifdef __GNUG__
#pragma implementation "b/exception.h"
#pragma implementation "b/runtime_exception.h"
#pragma implementation "b/system_exception.h"
#pragma implementation "b/custom_exception.h"
#endif // __GNUG__

#include <b/system_exception.h>
#include <b/custom_exception.h>

B_BEGIN_NAMESPACE

void SystemException::GetMessage(string& target) const
	throw ()
{
	try
	{
		const char* message_buffer = strerror(code);
		target.assign(message_buffer, calc_length(message_buffer));
	}
	catch (Memory::Exception&)
	{
	}
}

#ifdef B_USE_STL
const char* SystemException::what() const
	throw ()
{
	return strerror(code);
}
#endif // B_USE_STL

CustomException::CustomException(const char* message_format, ...) :
	message()
{
	va_list arguments;
	va_start(arguments, message_format);
	message.format(arguments, message_format);
	va_end(arguments);
}

void CustomException::SetMessage(const char* message_format, ...)
{
	va_list arguments;
	va_start(arguments, message_format);
	message.format(arguments, message_format);
	va_end(arguments);
}

void CustomException::GetMessage(string& target) const
	throw ()
{
	target = message;
}

#ifdef B_USE_STL
const char* CustomException::what() const
	throw ()
{
	GetMessage(message);
	return message;
}
#endif // B_USE_STL

B_END_NAMESPACE
