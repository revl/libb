/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016, 2017 Damon Revoe
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

#ifndef B_INPUT_STREAM_H
#define B_INPUT_STREAM_H

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

B_END_NAMESPACE

#endif /* !defined(B_INPUT_STREAM_H) */
