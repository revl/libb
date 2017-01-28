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

#ifndef B_INPUT_OUTPUT_STREAM_H
#define B_INPUT_OUTPUT_STREAM_H

#include "seekable_input_stream.h"
#include "seekable_output_stream.h"

B_BEGIN_NAMESPACE

// Read/write stream interface.
class input_output_stream : public seekable_input_stream,
	public seekable_output_stream
{
};

B_END_NAMESPACE

#endif /* !defined(B_INPUT_OUTPUT_STREAM_H) */
