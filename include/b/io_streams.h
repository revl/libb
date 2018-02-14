/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2018 Damon Revoe
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

#ifndef B_IO_STREAMS_H
#define B_IO_STREAMS_H

#include "object.h"

B_BEGIN_NAMESPACE

// Interface for serial input.
class input_stream : public virtual object
{
public:
	// Reads from this input stream. The method returns the number
	// of bytes read. The interface allows this number to be smaller
	// than the number of bytes requested.
	virtual size_t read(void* buffer, size_t buffer_size) = 0;

	// Returns true if the end-of-file condition is reached.
	// Depending on the implementation, the method may require
	// an additional read operation to detect the EOF condition.
	virtual bool eof() = 0;
};

// Interface for serial output.
class output_stream : public virtual object
{
public:
	// Writes to the output stream. The interface allows
	// implementations to return before the entire buffer is sent.
	// The method returns the number of bytes written.
	virtual size_t write(const void* buffer, size_t buffer_size) = 0;
};

// Interface to retrieve and change the current read/write position
// within a stream.
class seekable : public virtual object
{
public:
	// Returns the current stream position value.
	virtual size_t position() const = 0;

	// Constants that define how the 'offset' argument is
	// interpreted by the 'seek()' method.
	enum relative_to
	{
		// The offset argument sets the new position in the stream.
		beg,

		// The offset is relative to the current position
		// in the stream.
		cur,

		// The offset is relative to the of the stream.
		end
	};

	// Change the current stream position.
	virtual void seek(off_t offset, relative_to whence = beg) = 0;

	// Returns the file size for the open stream.
	virtual size_t size() const = 0;

	virtual ~seekable();
};

// Input stream that supports random access.
class seekable_input_stream : public virtual seekable, public input_stream
{
public:
	virtual ~seekable_input_stream();
};

// Output stream that supports random access.
class seekable_output_stream : public virtual seekable, public output_stream
{
};

// Read/write stream interface.
class input_output_stream : public seekable_input_stream,
	public seekable_output_stream
{
public:
	input_output_stream();

	virtual ~input_output_stream();
};

// Returns an object that encapsulates the standard input stream.
ref<input_stream> standard_input_stream();

// Returns an object that encapsulates the standard output stream.
ref<output_stream> standard_output_stream();

// Returns an object that encapsulates the standard error stream.
ref<output_stream> standard_error_stream();

B_END_NAMESPACE

#endif /* !defined(B_IO_STREAMS_H) */
