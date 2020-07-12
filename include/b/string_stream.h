// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#ifndef B_STRING_STREAM_H
#define B_STRING_STREAM_H

#include "io_streams.h"

B_BEGIN_NAMESPACE

// An input-output stream that uses a string for storage.
class string_stream : public input_output_stream
{
public:
	// Constructs a new string_stream object.
	string_stream();

	// Defines the initial contents of the buffer string and
	// the initial position in it.
	string_stream(const string& b, size_t p = 0);

	// Returns the current contents of the buffer string.
	string str() const;

	// Returns the current read/write position within the buffer string.
	virtual size_t position() const;

	// Changes the read/write position. The new position is allowed
	// to exceed the current length of the buffer string.
	virtual void seek(off_t offset, relative_to whence = beg);

	// Reads from the string starting from the current position.
	virtual size_t read(void* buffer, size_t buffer_size);

	// Returns true if the current position points at or past the end
	// of the buffer string.
	virtual bool eof();

	// Returns the current length of the buffer string.
	virtual size_t size() const;

	// Appends data to the buffer string or overwrites a fragment
	// of the string.
	virtual size_t write(const void* buffer, size_t buffer_size);

private:
	b::string buf_str;
	size_t pos;
};

inline string_stream::string_stream() : pos(0)
{
}

inline string_stream::string_stream(const string& b, size_t p) :
	buf_str(b), pos(p)
{
}

inline string string_stream::str() const
{
	return buf_str;
}

B_END_NAMESPACE

#endif /* !defined(B_STRING_STREAM_H) */
