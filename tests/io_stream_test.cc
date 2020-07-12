// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

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
	b::standard_input_stream()->add_ref();
	b::standard_input_stream()->add_ref();

	b::standard_output_stream()->add_ref();
	b::standard_output_stream()->release();
	b::standard_output_stream()->release();
	b::standard_output_stream()->add_ref();
	b::standard_output_stream()->add_ref();

	b::standard_error_stream()->add_ref();
	b::standard_error_stream()->release();
	b::standard_error_stream()->release();
	b::standard_error_stream()->add_ref();
	b::standard_error_stream()->add_ref();
}

B_TEST_CASE(exceptions)
{
	B_STRING_LITERAL(no_such_file, "/no/such/directory/and/file");

	B_REQUIRE_EXCEPTION(b::open_file_for_reading(no_such_file),
			"/no/such/*");
	B_REQUIRE_EXCEPTION(b::open_file_for_writing(no_such_file),
			"/no/such/*");
}
