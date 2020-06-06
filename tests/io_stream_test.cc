/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2020 Damon Revoe
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

#include <b/io_streams.h>

#include "test_case.h"

B_TEST_CASE(std_stream_singletons)
{
	// Verify that the returned poiners do not change between calls.
	B_CHECK(b::standard_input_stream() == b::standard_input_stream());
	B_CHECK(b::standard_output_stream() == b::standard_output_stream());
	B_CHECK(b::standard_error_stream() == b::standard_error_stream());
}

B_TEST_CASE(std_stream_ref_count)
{
	// Verify that reference counting is inhibited
	// for the standard stream objects.
	b::standard_input_stream()->add_ref();
	b::standard_input_stream()->release();
	b::standard_input_stream()->release();

	b::standard_output_stream()->add_ref();
	b::standard_output_stream()->release();
	b::standard_output_stream()->release();

	b::standard_error_stream()->add_ref();
	b::standard_error_stream()->release();
	b::standard_error_stream()->release();
}

B_TEST_CASE(exceptions)
{
	B_STRING_LITERAL(no_such_file, "/no/such/directory/and/file");

	B_REQUIRE_EXCEPTION(b::open_file_for_reading(no_such_file),
			"/no/such/*");
	B_REQUIRE_EXCEPTION(b::open_file_for_writing(no_such_file),
			"/no/such/*");
}
