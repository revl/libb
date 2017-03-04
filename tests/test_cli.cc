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
#include <b/array.h>

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
	cl_parser.parse(B_COUNTOF(help_option), help_option,
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

	cl_parser.parse(B_COUNTOF(help_option), help_option,
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

	// The '--version' option without 'version_info' must generate
	// an exception.
	B_REQUIRE_EXCEPTION(cl_parser.parse(
		B_COUNTOF(version_option), version_option),
		"test_cli: unknown option '--version'\n*");

	ss = new b::string_stream;

	B_STATIC_CONST_STRING(app_version, "test_cli version 1.2");

	cl_parser.parse(B_COUNTOF(version_option), version_option,
		(version_info = app_version,
		help_output_stream = ss));

	B_CHECK(ss->str() == app_version + '\n');
}

B_TEST_CASE(default_app_name)
{
	using namespace b::cli_args;

	b::cli cl_parser(app_summary);

	b::ref<b::string_stream> ss = new b::string_stream;

	cl_parser.parse(B_COUNTOF(help_option), help_option,
		help_output_stream = ss);

	B_CHECK(match_pattern(ss->str(), "*\nUsage: test_cli\n*"));

	ss = new b::string_stream;

	B_STATIC_CONST_STRING(app_name, "console_app");

	cl_parser.parse(B_COUNTOF(help_option), help_option,
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

	cl_parser.parse(B_COUNTOF(help_option), help_option,
		(program_description = long_text,
		help_output_stream = ss));

	B_CHECK(match_pattern(ss->str(), "test_cli: Test the b::cli class.\n\n"
		"Usage: test_cli\n\n"
		"letters letters letters letters letters "
		"letters letters letters letters\n"
		"*\n"
		"letters letters letters letters \n\n"));

	ss = new b::string_stream;

	cl_parser.parse(B_COUNTOF(help_option), help_option,
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

	cl_parser.parse(B_COUNTOF(help_option), help_option,
		help_output_stream = ss);

	B_CHECK(match_pattern(ss->str(),
		"*\n  ls (list)           - List directory contents.\n\n"));

	ss = new b::string_stream;

	cl_parser.parse(B_COUNTOF(help_option), help_option,
		(cmd_descr_indent = 16,
		help_output_stream = ss));

	B_CHECK(match_pattern(ss->str(),
		"*\n  ls (list)   - List directory contents.\n\n"));
}

enum
{
	query_arg,
	tabular_report_opt,
	output_file_opt
};

static b::cli create_test_cli()
{
	b::cli cl_parser(app_summary);

	B_STATIC_CONST_STRING(query_cmd_name, "query");
	B_STATIC_CONST_STRING(query_cmd_synopsis, "Query the server.");

	cl_parser.register_command(0, query_cmd_name,
		query_cmd_synopsis, b::string());

	B_STATIC_CONST_STRING(query_arg_name, "QUERY");

	cl_parser.register_one_or_more_positional(query_arg, query_arg_name);

	cl_parser.register_association(0, 0);

	B_STATIC_CONST_STRING(tabular_opt_name, "t|tabular-report");
	B_STATIC_CONST_STRING(tabular_opt_synopsis,
		"Use tabular output format.");

	cl_parser.register_option(tabular_report_opt,
		tabular_opt_name, tabular_opt_synopsis);

	cl_parser.register_association(0, tabular_report_opt);

	B_STATIC_CONST_STRING(output_opt_name, "o|output-file");
	B_STATIC_CONST_STRING(output_opt_param, "FILE");
	B_STATIC_CONST_STRING(output_opt_synopsis,
		"Redirect output to the specified file.");

	cl_parser.register_option_with_parameter(output_file_opt,
		output_opt_name, output_opt_param, output_opt_synopsis);

	cl_parser.register_association(0, output_file_opt);

	return cl_parser;
}

B_TEST_CASE(arg_descr_indent)
{
	b::cli cl_parser = create_test_cli();

	using namespace b::cli_args;

	static const char* const help_cmd[] =
	{
		"/path/to/test_cli",
		"help",
		"query"
	};

	b::ref<b::string_stream> ss = new b::string_stream;

	cl_parser.parse(B_COUNTOF(help_cmd), help_cmd,
		help_output_stream = ss);

	B_CHECK(match_pattern(ss->str(),
		"*\n  -t [--tabular-report]       : "
		"Use tabular output format.\n*"));

	ss = new b::string_stream;

	cl_parser.parse(B_COUNTOF(help_cmd), help_cmd,
		(help_output_stream = ss,
		arg_descr_indent = 40));

	B_CHECK(match_pattern(ss->str(),
		"*\n  -t [--tabular-report]               : "
		"Use tabular output format.\n*"));
}

B_TEST_CASE(options)
{
	b::cli cl_parser = create_test_cli();

	using namespace b::cli_args;

	static const char* const query_cmd[] =
	{
		"/path/to/test_cli",
		"query",
		"-t",
		"query text"
	};

	int cmd = cl_parser.parse(B_COUNTOF(query_cmd), query_cmd);

	B_CHECK(cmd == 0);

	int arg_id;
	const char* opt_value;

	b::array<int> args;

	while (cl_parser.next_arg(&arg_id, &opt_value))
	{
		args.append(arg_id);

		if (arg_id == query_arg)
			B_CHECK(b::compare_strings(opt_value,
				"query text") == 0);
	}

	B_REQUIRE(args.size() == 2);
	B_REQUIRE(args[0] == tabular_report_opt);
	B_REQUIRE(args[1] == query_arg);
}

B_TEST_CASE(exceptions)
{
	b::cli cl_parser = create_test_cli();

	static const char* const unknown_cmd[] =
	{
		"/path/to/test_cli",
		"nosuchcmd"
	};

	B_REQUIRE_EXCEPTION(
		cl_parser.parse(B_COUNTOF(unknown_cmd), unknown_cmd),
		"test_cli: unknown command 'nosuchcmd'\n*");

	static const char* const missing_arg[] =
	{
		"/path/to/test_cli",
		"query"
	};

	B_REQUIRE_EXCEPTION(
		cl_parser.parse(B_COUNTOF(missing_arg), missing_arg),
		"test_cli: missing argument 'QUERY'\n*");

	static const char* const unknown_opt[] =
	{
		"/path/to/test_cli",
		"query",
		"query text",
		"-f"
	};

	B_REQUIRE_EXCEPTION(
		cl_parser.parse(B_COUNTOF(unknown_opt), unknown_opt),
		"test_cli: unknown option '-f'\n*");
}

B_TEST_CASE(free_standing_double_dash)
{
	b::cli cl_parser = create_test_cli();

	static const char* const query_cmd[] =
	{
		"/path/to/test_cli",
		"query",
		"-t",
		"--",
		"pos1",
		"pos2",
		"-f"
	};

	B_REQUIRE(cl_parser.parse(B_COUNTOF(query_cmd), query_cmd) == 0);

	int arg_id;
	const char* opt_value;

	b::array<const char*> pos_arg_values;

	while (cl_parser.next_arg(&arg_id, &opt_value))
		if (arg_id == query_arg)
			pos_arg_values.append(opt_value);

	B_REQUIRE(pos_arg_values.size() == 3U);

	const char* const* expected_positional = query_cmd + 4;

	for (size_t i = 0U; i < 3U; ++i)
		B_CHECK(b::compare_strings(pos_arg_values[i],
			*expected_positional++) == 0);
}

B_TEST_CASE(arg_order)
{
	b::cli cl_parser = create_test_cli();

	static const char* const query_cmd[] =
	{
		"/path/to/test_cli",
		"query",
		"A1",
		"-tt",
		"-tto",
		"F1",
		"A2",
		"-toF2",
		"-o",
		"F3",
		"-oF4",
		"A3"
	};

	B_REQUIRE(cl_parser.parse(B_COUNTOF(query_cmd), query_cmd) == 0);

	b::string arg_order;

	int arg_id;
	const char* opt_value;

	while (cl_parser.next_arg(&arg_id, &opt_value))
		switch (arg_id)
		{
		case tabular_report_opt:
			arg_order.append('t');
			break;

		case output_file_opt:
			arg_order.append('o');
			/* FALL THROUGH */

		case query_arg:
			arg_order.append(opt_value, b::calc_length(opt_value));
		}

	B_CHECK(arg_order == "ttttoF1toF2oF3oF4A1A2A3");
}
