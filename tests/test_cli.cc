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

#include <b/cli.h>

#include "unit_test.h"

#include <b/string_stream.h>

B_STATIC_CONST_STRING(app_summary, "Test the b::cli class.");

static const char* const help_option[] =
{
	"/path/to/test_cli",
	"--help"
};

B_TEST_CASE(app_description)
{
	using namespace b::cli_args;

	b::cli cl_parser(app_summary);

	b::ref<b::string_stream> ss = new b::string_stream;

	// Without program description.
	cl_parser.parse(sizeof(help_option) / sizeof(*help_option),
		help_option,
		help_output_stream = ss);

	B_CHECK(ss->str() == "test_cli: Test the b::cli class.\n\n"
		"Usage: test_cli\n\n");

	ss = new b::string_stream;

	// With program description.
	B_STATIC_CONST_STRING(app_description,
		"Here goes a description of things the app does. "
		"The description can be as long as needed to "
		"convey the purpose of this application. However, "
		"it does not substitute a proper manual page.");

	cl_parser.parse(sizeof(help_option) / sizeof(*help_option),
		help_option,
		(program_description = app_description,
		help_output_stream = ss));

	B_CHECK(match_pattern(ss->str(),
		"test_cli: Test the b::cli class.\n\n"
		"Usage: test_cli\n\n"
		"Here goes *\n*\n* manual page.\n\n"));
}

B_TEST_CASE(version_option)
{
	using namespace b::cli_args;

	b::cli cl_parser(app_summary);

	static const char* const version_option[] =
	{
		"test_cli",
		"--version"
	};

	b::ref<b::string_stream> ss = new b::string_stream;

	try
	{
		cl_parser.parse(
			sizeof(version_option) / sizeof(*version_option),
			version_option);

		B_CHECK("The '--version' option without 'version_info' "
			"must generate an exception." && false);
	}
	catch (b::runtime_exception& e)
	{
		B_CHECK(match_pattern(e.message(),
			"test_cli: unknown option '--version'\n*"));
	}

	ss = new b::string_stream;

	B_STATIC_CONST_STRING(app_version, "test_cli version 1.2");

	cl_parser.parse(sizeof(version_option) / sizeof(*version_option),
		version_option,
		(version_info = app_version,
		help_output_stream = ss));

	B_CHECK(ss->str() == app_version + '\n');
}

B_TEST_CASE(default_app_name)
{
	using namespace b::cli_args;

	b::cli cl_parser(app_summary);

	b::ref<b::string_stream> ss = new b::string_stream;

	cl_parser.parse(sizeof(help_option) / sizeof(*help_option),
		help_option,
		help_output_stream = ss);

	B_CHECK(match_pattern(ss->str(), "*\nUsage: test_cli\n*"));

	ss = new b::string_stream;

	B_STATIC_CONST_STRING(app_name, "console_app");

	cl_parser.parse(sizeof(help_option) / sizeof(*help_option),
		help_option,
		(program_name = app_name,
		help_output_stream = ss));

	B_CHECK(match_pattern(ss->str(), "*\nUsage: console_app\n*"));
}

B_TEST_CASE(help_text_width)
{
	using namespace b::cli_args;

	b::cli cl_parser(app_summary);

	B_STATIC_CONST_STRING(word_and_space, "letters ");

	b::string long_text = word_and_space.repeat(40);

	b::ref<b::string_stream> ss = new b::string_stream;

	cl_parser.parse(sizeof(help_option) / sizeof(*help_option),
		help_option,
		(program_description = long_text,
		help_output_stream = ss));

	B_CHECK(match_pattern(ss->str(), "test_cli: Test the b::cli class.\n\n"
		"Usage: test_cli\n\n"
		"letters letters letters letters letters "
		"letters letters letters letters\n"
		"*\n"
		"letters letters letters letters \n\n"));

	ss = new b::string_stream;

	cl_parser.parse(sizeof(help_option) / sizeof(*help_option),
		help_option,
		(program_description = long_text,
		help_text_width = 24,
		help_output_stream = ss));

	B_CHECK(match_pattern(ss->str(), "test_cli: Test the\n"
		"          b::cli class.\n\n"
		"Usage: test_cli\n\n"
		"letters letters letters\n"
		"*\n"
		"letters \n\n"));
}

B_TEST_CASE(cmd_descr_indent)
{
	using namespace b::cli_args;

	b::cli cl_parser(app_summary);

	B_STATIC_CONST_STRING(ls_cmd_name, "ls|list");
	B_STATIC_CONST_STRING(ls_cmd_synopsis, "List directory contents.");

	cl_parser.register_command(0, ls_cmd_name,
		ls_cmd_synopsis, b::string());

	b::ref<b::string_stream> ss = new b::string_stream;

	cl_parser.parse(sizeof(help_option) / sizeof(*help_option),
		help_option,
		help_output_stream = ss);

	B_CHECK(match_pattern(ss->str(),
		"*\n  ls (list)           - List directory contents.\n\n"));

	ss = new b::string_stream;

	cl_parser.parse(sizeof(help_option) / sizeof(*help_option),
		help_option,
		(cmd_descr_indent = 16,
		help_output_stream = ss));

	B_CHECK(match_pattern(ss->str(),
		"*\n  ls (list)   - List directory contents.\n\n"));
}

B_TEST_CASE(arg_descr_indent)
{
	using namespace b::cli_args;

	b::cli cl_parser(app_summary);

	B_STATIC_CONST_STRING(query_cmd_name, "query");
	B_STATIC_CONST_STRING(query_cmd_synopsis, "Query the server.");

	cl_parser.register_command(0, query_cmd_name,
		query_cmd_synopsis, b::string());

	B_STATIC_CONST_STRING(query_arg_name, "QUERY");

	cl_parser.register_positional_argument(0, query_arg_name);

	cl_parser.register_association(0, 0);

	B_STATIC_CONST_STRING(tabular_opt_name, "t|tabular-report");
	B_STATIC_CONST_STRING(tabular_opt_synopsis,
		"Use tabular output format.");

	cl_parser.register_option(1, tabular_opt_name, tabular_opt_synopsis);

	cl_parser.register_association(0, 1);

	static const char* const help_cmd[] =
	{
		"/path/to/test_cli",
		"help",
		"query"
	};

	b::ref<b::string_stream> ss = new b::string_stream;

	cl_parser.parse(sizeof(help_cmd) / sizeof(*help_cmd),
		help_cmd,
		help_output_stream = ss);

	B_CHECK(match_pattern(ss->str(),
		"*\n  -t [--tabular-report]       : "
		"Use tabular output format.\n\n"));

	ss = new b::string_stream;

	cl_parser.parse(sizeof(help_cmd) / sizeof(*help_cmd),
		help_cmd,
		(help_output_stream = ss,
		arg_descr_indent = 40));

	B_CHECK(match_pattern(ss->str(),
		"*\n  -t [--tabular-report]               : "
		"Use tabular output format.\n\n"));
}
