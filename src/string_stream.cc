// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/string_stream.h>

B_BEGIN_NAMESPACE

size_t string_stream::position() const
{
	return pos;
}

void string_stream::seek(off_t offset, relative_to whence)
{
	switch (whence)
	{
	default: /* beg */
		pos = (size_t) offset;
		break;

	case cur:
		pos = pos + (size_t) offset;
		break;

	case end:
		pos = buf_str.length() + (size_t) offset;
	}
}

size_t string_stream::read(void* buffer, size_t buffer_size)
{
	if (pos >= buf_str.length())
		return 0U;

	size_t available_bytes = buf_str.length() - pos;

	if (buffer_size > available_bytes)
		buffer_size = available_bytes;

	b::memory::copy(buffer, buf_str.data() + pos, buffer_size);

	pos += buffer_size;

	return buffer_size;
}

bool string_stream::eof()
{
	return pos >= buf_str.length();
}

size_t string_stream::size() const
{
	return buf_str.length();
}

size_t string_stream::write(const void* buffer, size_t buffer_size)
{
	if (pos <= buf_str.length())
	{
		buf_str.replace(pos, (const char*) buffer, buffer_size);
		pos += buffer_size;
	}
	else
	{
		buf_str.append(pos - buf_str.length(), ' ');
		buf_str.append((const char*) buffer, buffer_size);
		pos = buf_str.length();
	}

	return buffer_size;
}

B_END_NAMESPACE
