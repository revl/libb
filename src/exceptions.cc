// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/system_exception.h>
#include <b/custom_exception.h>

B_BEGIN_NAMESPACE

system_exception::system_exception(const string& context_or_subject,
		int errno_value) :
	exception_context(context_or_subject),
	system_error_code(errno_value)
{
}

B_STRING_LITERAL(oom_message, "Insufficient memory to "
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

#if defined(B_USE_STL)
const char* system_exception::what() const
	throw ()
{
	message_buffer = message();

	return message_buffer.data();
}
#endif /* defined(B_USE_STL) */

system_exception::~system_exception()
	throw ()
{
}

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

#if defined(B_USE_STL)
const char* custom_exception::what() const
	throw ()
{
	return error_message.data();
}
#endif /* defined(B_USE_STL) */

custom_exception::~custom_exception()
	throw ()
{
}

B_END_NAMESPACE
