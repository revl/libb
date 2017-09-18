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

#include <b/array.h>
#include <b/object.h>
#include <b/map.h>
#include <b/set.h>
#include <b/custom_exception.h>

#define VERSION_OPT_ID -1
#define HELP_OPT_ID -2
#define COMMAND_OPT_ID -3

#define HELP_CMD_ID -1

#define DEFAULT_CATEGORY_ID -1

#define DEFAULT_HELP_TEXT_WIDTH 72
#define DEFAULT_CMD_DESCR_INDENT 24
#define DEFAULT_OPT_DESCR_INDENT 32

enum arg_type
{
	// An optional switch that enables a non-default
	// mode of operation.
	option,

	// An optional switch that requires a value.
	option_with_parameter,

	// A required positional argument. Positional
	// arguments are reported by the next_arg() method
	// in the order they were associated with the
	// command.
	positional_argument,

	// Similar to 'positional_argument', but optional.
	optional_positional,

	// An optional sequence of arguments.
	zero_or_more_positional,

	// An non-empty sequence of arguments.
	one_or_more_positional
};

B_BEGIN_NAMESPACE

#define PROGRAM_NAME_ARG 'p'
#define VERSION_INFO_ARG 'v'
#define PROGRAM_DESCRIPTION_ARG 'd'
#define HELP_TEXT_WIDTH_ARG 'w'
#define CMD_DESCR_INDENT_ARG 'i' + 'c'
#define ARG_DESCR_INDENT_ARG 'i' + 'a'
#define HELP_OUTPUT_STREAM_ARG 'o'

namespace cli_args
{
	arg_name<string, PROGRAM_NAME_ARG> program_name;
	arg_name<string, VERSION_INFO_ARG> version_info;
	arg_name<string, PROGRAM_DESCRIPTION_ARG> program_description;
	arg_name<int, HELP_TEXT_WIDTH_ARG> help_text_width;
	arg_name<int, CMD_DESCR_INDENT_ARG> cmd_descr_indent;
	arg_name<int, ARG_DESCR_INDENT_ARG> arg_descr_indent;
	arg_name<ref<output_stream>, HELP_OUTPUT_STREAM_ARG> help_output_stream;
}

// TODO Use string_view instead of string in this class
// where the data is expected to be kept in argv?

typedef array<string> name_variant_list;

struct option_or_command_info
{
	option_or_command_info(int opt_or_cmd_id, const string& names) :
		id(opt_or_cmd_id)
	{
		string_view name, more;

		if (!names.split('|', &name, &more))
			name_variants.append(names);
		else
		{
			do
				name_variants.append(string(name));
			while (more.split('|', &name, &more));

			name_variants.append(string(more));
		}
	}

	const string& primary_name() const
	{
		return name_variants.first();
	}

	bool operator <(const option_or_command_info& rhs) const
	{
		return id < rhs.id;
	}

	bool operator <(int rhs) const
	{
		return id < rhs;
	}

	int id;
	name_variant_list name_variants;
};

inline bool operator <(int lhs, const option_or_command_info& rhs)
{
	return lhs < rhs.id;
}

struct option_info : public option_or_command_info
{
	option_info(int arg_id, const string& opt_names,
			const string& opt_param_name,
			arg_type opt_type,
			const string& arg_descr) :
		option_or_command_info(arg_id, opt_names),
		param_name(opt_param_name),
		type(opt_type),
		description(arg_descr)
	{
	}

	static string with_dashes(const string& opt_name)
	{
		B_STRING_LITERAL(dashes, "--");

		return opt_name.length() == 1 ? '-' + opt_name :
			dashes + opt_name;
	}

	string option_name_variants() const;

	string param_name;

	int type;
	string description;

	virtual ~option_info();
};

string option_info::option_name_variants() const
{
	string result(with_dashes(name_variants.first()));

	if (name_variants.size() > 1)
	{
		result.append(" [", 2);

		name_variant_list::const_iterator name(name_variants.begin());
		result.append(with_dashes(*++name));

		while (++name != name_variants.end())
		{
			result.append(", ", 2);
			result.append(with_dashes(*name));
		}

		result.append(']');
	}

	if (type == option_with_parameter)
	{
		result.append(' ');
		result.append(param_name);
	}

	return result;
}

option_info::~option_info()
{
}

typedef array<const option_info*> option_info_list;

struct command_or_commandless_cli
{
	command_or_commandless_cli(const string& s, const string& u) :
		synopsis(s), usage(u),
		required_sequence_arg(NULL), optional_sequence_arg(NULL)
	{
	}

	string synopsis;
	string usage;

	option_info_list accepted_options;

	// Required positional arguments that go before
	// any optional arguments or sequences.
	option_info_list required_args;

	// One or more values of the same argument type.
	const option_info* required_sequence_arg;

	// A list of optional positional arguments.
	option_info_list optional_args;

	// Zero or more values of the same argument type.
	const option_info* optional_sequence_arg;

	// Any required positional arguments that follow
	// any optional arguments or sequences.
	option_info_list trailing_required_args;

	~command_or_commandless_cli();
};

command_or_commandless_cli::~command_or_commandless_cli()
{
}

struct command_info :
	public option_or_command_info, public command_or_commandless_cli
{
	command_info(int cmd_id, const string& cmd_names,
			const string& s, const string& u) :
		option_or_command_info(cmd_id, cmd_names),
		command_or_commandless_cli(s, u)
	{
	}

	string command_name_variants() const
	{
		if (name_variants.size() == 1)
			return name_variants.first();

		name_variant_list::const_iterator name = name_variants.begin();

		string result(*name);

		result.append(" (", 2);
		result.append(*++name);

		while (++name != name_variants.end())
		{
			result.append(", ", 2);
			result.append(*name);
		}

		result.append(')');

		return result;
	}

	bool is_option_accepted(const option_info* opt_info) const
	{
		for (option_info_list::const_iterator oi_iter =
					accepted_options.begin();
				oi_iter != accepted_options.end(); ++oi_iter)
			if (*oi_iter == opt_info)
				return true;

		return false;
	}

	virtual ~command_info();
};

command_info::~command_info()
{
}

typedef array<const command_info*> command_info_list;

struct category_info : public object
{
	category_info(const string& t) : title(t)
	{
	}

	string title;
	command_info_list commands;
};

typedef array<const char*> positional_argument_list;

class cli::impl : public object, public command_or_commandless_cli
{
public:
	impl(const string& program_summary);

	void print_word_wrapped(int topic_len, int indent,
			const string& text, int cont_indent = -1) const;

	void print_command_list(bool version_info_defined) const;

	void print_help(const positional_argument_list& commands,
			bool using_help_command,
			bool version_info_defined) const;

	void print_help_on_command(const command_or_commandless_cli* ci,
			const string& name_for_synopsis,
			const string& name_for_usage) const;

	void report_error(const string& error_message,
			const string& help_cmd) const;

	void error(const char* err_fmt, ...) const B_PRINTF_STYLE(2, 3);

	void cmd_error(const string& command_name,
			const char* err_fmt, ...) const B_PRINTF_STYLE(3, 4);

	void register_arg(arg_type type, int arg_id,
			const string& name_variants, const string& param_name,
			const string& description);

	void parse_long_option(int* argc, const char* const** argv,
			const char* arg, bool no_version_opt);

	void parse_single_letter_options(int* argc, const char* const** argv,
			const char* arg);

	void append_positional_argument_values(
			const string& command_name,
			const positional_argument_list& argument_values,
			const command_or_commandless_cli* ci);

	int parse_and_validate(int argc, const char* const *argv,
			const string& version_info);

	// Command and argument definitions.
	string program_name;
	const option_info* single_letter_options[256];
	map<string, const option_info*> long_opt_name_to_opt_info;
	set<option_info> opt_id_to_opt_info;
	map<string, const command_info*> cmd_name_to_cmd_info;
	set<command_info> cmd_id_to_cmd_info;
	typedef map<int, ref<category_info> > cat_id_to_cat_info_map;
	cat_id_to_cat_info_map cat_id_to_cat_info;
	option_info version_option_info;
	option_info help_option_info;

	bool commands_are_defined() const
	{
		return !cmd_name_to_cmd_info.is_empty();
	}

	// Parsing results.
	struct arg_value
	{
		const option_info* arg_info;
		const char* arg_value;
	};
	typedef array<arg_value> option_value_array;
	option_value_array option_values;

	void append_option_value(const option_info* opt_info,
			const char* opt_param)
	{
		arg_value av =
		{
			opt_info,
			opt_param
		};

		option_values.append(av);
	}

	option_value_array::const_iterator next_option_value;

	// Help text formatting.
	ref<output_stream> help_output_stream;
	int max_help_text_width;
	int cmd_descr_indent;
	int opt_descr_indent;
};

B_STRING_LITERAL(help, "help");
B_STRING_LITERAL(dash_dash_help, "--help");
B_STRING_LITERAL(version, "version");

cli::impl::impl(const string& program_summary) :
	command_or_commandless_cli(program_summary, string()),
	version_option_info(VERSION_OPT_ID, version,
		string(), option, string()),
	help_option_info(HELP_OPT_ID, help,
		string(), option, string())
{
	memset(single_letter_options, 0, sizeof(single_letter_options));

	long_opt_name_to_opt_info.insert(version, &version_option_info);
	long_opt_name_to_opt_info.insert(help, &help_option_info);

	B_STRING_LITERAL(default_category, "Available commands");

	cat_id_to_cat_info.insert(DEFAULT_CATEGORY_ID,
		new category_info(default_category));
}

void cli::impl::print_word_wrapped(int topic_len,
	int indent, const string& text, int cont_indent) const
{
	if (text.is_empty())
	{
		help_output_stream->write("\n", 1);
		return;
	}

	const char* line = text.data();
	const char* text_end = line + text.length();

	int offset = indent;

	if (topic_len > 0 && (offset -= topic_len) < 1)
	{
		offset = indent;
		help_output_stream->write("\n", 1);
	}

	if (cont_indent < 0)
		cont_indent = indent;

#ifdef __GNUC__
	// A no-op assignment to make older versions of GCC happy.
	const char* next_line = NULL;
#else
	const char* next_line;
#endif
	do
	{
		const char* line_end;

		// Check for verbatim formatting.
		if (*line != ' ')
		{
			const char* pos = line;
			const char* max_pos = line +
				max_help_text_width - indent;

			line_end = NULL;

			for (;;)
			{
				if (*pos == ' ')
				{
					line_end = pos;

					while (pos < text_end && pos[1] == ' ')
						++pos;

					next_line = pos + 1;
				}
				else
					if (*pos == '\n')
					{
						next_line =
							(line_end = pos) + 1;
						break;
					}

				if (++pos > max_pos && line_end != NULL)
					break;

				if (pos == text_end)
				{
					next_line = line_end = pos;
					break;
				}
			}
		}
		else
		{
			// Preformatted text -- do not wrap.
			line_end = strchr(line, '\n');

			next_line = (line_end == NULL ?
				line_end = text_end : line_end + 1);
		}

		ssize_t line_len = line_end - line;

		if (line_len > 0)
		{
			string spaces((size_t) offset, ' ');

			help_output_stream->write(spaces.data(),
				spaces.length());

			help_output_stream->write(line, (size_t) line_len);
		}

		help_output_stream->write("\n", 1);

		offset = indent = cont_indent;
	}
	while ((line = next_line) < text_end);
}

void cli::impl::print_command_list(bool version_info_defined) const
{
	string prog_usage = string::formatted(
		"Usage: %s <command> [options] [args]\n", program_name.data());

	help_output_stream->write(prog_usage.data(), prog_usage.length());
	print_word_wrapped(0, 0, synopsis);

	prog_usage.format("Type '%s help <command>' for help on a "
		"specific command.\n", program_name.data());

	if (version_info_defined)
		prog_usage.format("Type '%s --version' to see the "
			"program version.\n", program_name.data());

	help_output_stream->write(prog_usage.data(),
		prog_usage.length());

	if (!usage.is_empty())
	{
		help_output_stream->write("\n", 1);
		print_word_wrapped(0, 0, usage);
	}

	// There is always at least one category.
	cat_id_to_cat_info_map::const_iterator category =
		cat_id_to_cat_info.begin();

	B_STRING_LITERAL(dash_space, "- ");

	do
	{
		if (category->value->commands.is_empty())
			continue;

		prog_usage.format("\n%s:\n\n", category->value->title.data());

		help_output_stream->write(prog_usage.data(),
			prog_usage.length());

		command_info_list::const_iterator ci_iter =
			category->value->commands.begin();

		do
		{
			size_t command_name_len =
				(*ci_iter)->command_name_variants().length();

			help_output_stream->write("  ", 2);

			help_output_stream->write(
				(*ci_iter)->command_name_variants().data(),
				command_name_len);

			int topic_len = 2 + (int) command_name_len;

			print_word_wrapped(topic_len, cmd_descr_indent - 2,
				dash_space + (*ci_iter)->synopsis,
				cmd_descr_indent);
		}
		while (++ci_iter != category->value->commands.end());
	}
	while (++category != cat_id_to_cat_info.end());

	help_output_stream->write("\n", 1);
}

void cli::impl::print_help(const positional_argument_list& commands,
		bool using_help_command, bool version_info_defined) const
{
	// The 'help' command does not accept any options.
	for (option_value_array::const_iterator ov_iter = option_values.begin();
			ov_iter != option_values.end(); ++ov_iter)
	{
		// Ignore the '--help' option.
		if (ov_iter->arg_info->id == HELP_OPT_ID)
			continue;

		const string& opt_name =
			ov_iter->arg_info->option_name_variants();

		if (using_help_command)
		{
			cmd_error(help, "command 'help' does not "
				"accept option '%s'",
				opt_name.data());
		}
		else
		{
			cmd_error(dash_dash_help, "'--help' cannot be "
				"combined with option '%s'",
				opt_name.data());
		}
	}

	if (!commands_are_defined())
	{
		print_help_on_command(this, program_name, program_name);
		return;
	}

	if (commands.is_empty())
	{
		print_command_list(version_info_defined);
		return;
	}

	B_STRING_LITERAL(help_command_synopsis,
		"Describe the usage of this program or its commands.");

	command_info help_command(HELP_CMD_ID, help,
		help_command_synopsis, string());

	B_STRING_LITERAL(help_command_arg, "COMMAND");

	option_info command_arg(COMMAND_OPT_ID, help_command_arg,
		string(), zero_or_more_positional, string());

	help_command.optional_sequence_arg = &command_arg;

	positional_argument_list::const_iterator cmd_name = commands.begin();

	do
	{
		const command_info** ci = cmd_name_to_cmd_info.find(*cmd_name);

		if (ci != NULL)
			print_help_on_command(*ci,
				(*ci)->command_name_variants(),
				program_name + ' ' + (*ci)->primary_name());
		else
			if (*cmd_name == help)
				print_help_on_command(&help_command, help,
					program_name + ' ' + help);
			else
			{
				string unknown_command = string::formatted(
					"'%s': unknown command.\n\n",
					*cmd_name);

				help_output_stream->write(
					unknown_command.data(),
					unknown_command.length());
			}
	}
	while (++cmd_name != commands.end());
}

static void append_positional_arg_name(string* args,
	const char* arg_name_format, const option_info* arg)
{
	if (!args->is_empty())
		args->append(' ');

	args->append_formatted(arg_name_format, arg->primary_name().data());
}

void cli::impl::print_help_on_command(const command_or_commandless_cli* ci,
	const string& name_for_synopsis, const string& name_for_usage) const
{
	help_output_stream->write(name_for_synopsis.data(),
		name_for_synopsis.length());

	help_output_stream->write(":", 1);

	int text_len = (int) name_for_synopsis.length() + 1;
	print_word_wrapped(text_len, text_len + 1, ci->synopsis);

	help_output_stream->write("\n", 1);

	string args;

	for (option_info_list::const_iterator arg = ci->required_args.begin();
			arg != ci->required_args.end(); ++arg)
		append_positional_arg_name(&args, "%s", *arg);

	if (ci->required_sequence_arg != NULL)
		append_positional_arg_name(&args, "%s...",
				ci->required_sequence_arg);

	for (option_info_list::const_iterator arg = ci->optional_args.begin();
			arg != ci->optional_args.end(); ++arg)
		append_positional_arg_name(&args, "[%s]", *arg);

	if (ci->optional_sequence_arg != NULL)
		append_positional_arg_name(&args, "[%s...]",
				ci->optional_sequence_arg);

	for (option_info_list::const_iterator arg =
				ci->trailing_required_args.begin();
			arg != ci->trailing_required_args.end(); ++arg)
		append_positional_arg_name(&args, "%s", *arg);

	string cmd_usage = string::formatted("Usage: %s",
		name_for_usage.data());

	help_output_stream->write(cmd_usage.data(), cmd_usage.length());

	text_len = (int) cmd_usage.length();
	print_word_wrapped(text_len, text_len + 1, args);

	if (!ci->usage.is_empty())
	{
		help_output_stream->write("\n", 1);
		print_word_wrapped(0, 0, ci->usage);
	}

	if (!ci->accepted_options.is_empty())
	{
		B_STRING_LITERAL(valid_options, "\nValid options:\n");
		help_output_stream->write(valid_options.data(),
			valid_options.length());

		option_info_list::const_iterator opt =
			ci->accepted_options.begin();

		do
		{
			string option_name = string::formatted(
				"  %-*s :", opt_descr_indent - 5,
				(*opt)->option_name_variants().data());

			help_output_stream->write(option_name.data(),
				option_name.length());

			print_word_wrapped((int) option_name.length(),
					opt_descr_indent, (*opt)->description);
		}
		while (++opt != ci->accepted_options.end());
	}

	help_output_stream->write("\n", 1);
}

void cli::impl::report_error(const string& error_message,
		const string& help_cmd) const
{
	throw custom_exception("%s\nType '%s %s' for usage.\n",
			error_message.data(),
			program_name.data(), help_cmd.data());
}

void cli::impl::error(const char* err_fmt, ...) const
{
	string error_message = program_name;
	error_message.append(": ", 2);

	va_list ap;
	va_start(ap, err_fmt);
	error_message.append_formatted_va(err_fmt, ap);
	va_end(ap);

	report_error(error_message, !commands_are_defined() ?
			dash_dash_help : help);
}

void cli::impl::cmd_error(const string& command_name,
		const char* err_fmt, ...) const
{
	string error_message = program_name;
	error_message.append(": ", 2);

	va_list ap;
	va_start(ap, err_fmt);
	error_message.append_formatted_va(err_fmt, ap);
	va_end(ap);

	report_error(error_message, !commands_are_defined() ?
			dash_dash_help : help + ' ' + command_name);
}

void cli::impl::register_arg(arg_type type, int arg_id,
		const string& name_variants, const string& param_name,
		const string& description)
{
	B_ASSERT(arg_id >= 0 && opt_id_to_opt_info.find(arg_id) == NULL &&
			"Option IDs must be unique");

	option_info* opt_info = opt_id_to_opt_info.insert(option_info(arg_id,
		name_variants, param_name, type, description));

	switch (type)
	{
	case option:
	case option_with_parameter:
		for (name_variant_list::const_iterator name =
					opt_info->name_variants.begin();
				name != opt_info->name_variants.end(); ++name)
			if (name->length() == 1)
				single_letter_options[
					(unsigned char) name->at(0)] = opt_info;
			else
				long_opt_name_to_opt_info.insert(
						*name, opt_info);

		accepted_options.append(opt_info);
		return;

	case positional_argument:
		if (required_sequence_arg == NULL &&
				optional_args.is_empty() &&
				optional_sequence_arg == NULL)
			required_args.append(opt_info);
		else
			trailing_required_args.append(opt_info);
		break;

	case optional_positional:
		optional_args.append(opt_info);
		break;

	case zero_or_more_positional:
		optional_sequence_arg = opt_info;
		break;

	case one_or_more_positional:
		required_sequence_arg = opt_info;
	}

	B_ASSERT(opt_info->name_variants.size() == 1 &&
		"Positional arguments do not allow name variants");
}

void cli::impl::parse_long_option(int* argc, const char* const** argv,
		const char* arg, bool no_version_opt)
{
	// Check if a parameter is specified for this option.
	const char* opt_param = strchr(arg, '=');
	string opt_name(arg, opt_param != NULL ?
			(size_t) (opt_param++ - arg) : calc_length(arg));

	const option_info** oi = long_opt_name_to_opt_info.find(opt_name);

	if (oi == NULL || (*oi == &version_option_info && no_version_opt))
		error("unknown option '--%s'", opt_name.data());

	// Check if this option must have a parameter.
	if ((*oi)->type == option)
	{
		// No, it's a switch; it's not supposed to have a parameter.
		if (opt_param != NULL)
			error("option '--%s' does not expect a parameter",
				opt_name.data());

		opt_param = "yes";
	}
	else
		// The option expects a parameter.
		if (opt_param == NULL)
		{
			// Parameter is not specified; use the next command
			// line argument as a parameter for this option.
			if (--*argc == 0)
				error("option '--%s' requires a parameter",
					opt_name.data());

			opt_param = *++*argv;
		}

	append_option_value(*oi, opt_param);
}

void cli::impl::parse_single_letter_options(int* argc, const char* const** argv,
		const char* arg)
{
	const option_info* opt_info;
	const char* opt_param;

	// The current argument is a (series of) one-letter option(s).
	for (;;)
	{
		char opt_letter = *arg++;

		opt_info = single_letter_options[(unsigned char) opt_letter];

		if (opt_info == NULL)
			error("unknown option '-%c'", opt_letter);

		// Check if this option must have a parameter.
		if (opt_info->type == option)
		{
			// It's a switch; it's not supposed to have
			// a parameter.
			opt_param = "yes";

			if (*arg == '\0')
				break;
		}
		else
		{
			// It's an option that expects a parameter.
			if (*arg == '\0')
			{
				// Use the next command line argument
				// as a parameter for this option.
				if (--*argc == 0)
					error("option '-%c' requires "
						"a parameter", opt_letter);

				opt_param = *++*argv;
			}
			else
				opt_param = arg;

			break;
		}

		append_option_value(opt_info, opt_param);
	}

	append_option_value(opt_info, opt_param);
}

void cli::impl::append_positional_argument_values(
		const string& command_name,
		const positional_argument_list& argument_values,
		const command_or_commandless_cli* ci)
{
	size_t number_of_values_required = ci->required_args.size() +
		ci->trailing_required_args.size();

	if (ci->required_sequence_arg != NULL)
		++number_of_values_required;
	else
		if (ci->optional_sequence_arg == NULL &&
				argument_values.size() >
					number_of_values_required +
						ci->optional_args.size())
			cmd_error(command_name,
				"too many positional arguments");

	if (argument_values.size() < number_of_values_required)
		cmd_error(command_name, "too few positional arguments");

	positional_argument_list::const_iterator av_iter =
		argument_values.begin();

	option_info_list::const_iterator arg_iter = ci->required_args.begin();

	while (arg_iter != ci->required_args.end())
		append_option_value(*arg_iter++, *av_iter++);

	positional_argument_list::const_iterator trailing_av_iter =
		argument_values.end() - ci->trailing_required_args.size();

	if (ci->required_sequence_arg != NULL)
		while (av_iter != trailing_av_iter)
			append_option_value(ci->required_sequence_arg,
				*av_iter++);
	else
	{
		arg_iter = ci->optional_args.begin();

		for (;;)
		{
			if (arg_iter != ci->optional_args.end())
			{
				if (av_iter == trailing_av_iter)
					break;

				append_option_value(*arg_iter++, *av_iter++);

				continue;
			}

			if (ci->optional_sequence_arg != NULL)
				while (av_iter != trailing_av_iter)
					append_option_value(
						ci->optional_sequence_arg,
						*av_iter++);

			break;
		}
	}

	arg_iter = ci->trailing_required_args.begin();

	while (arg_iter != ci->trailing_required_args.end())
		append_option_value(*arg_iter++, *av_iter++);
}

int cli::impl::parse_and_validate(int argc, const char* const *argv,
		const string& version_info)
{
	option_values.empty();

	positional_argument_list positional_argument_values;

	// Part one: parse options and save positional argument values.
	while (--argc > 0)
	{
		const char* arg = *++argv;

		// Check if the current argument is a positional argument.
		if (*arg != '-' || arg[1] == '\0')
		{
			positional_argument_values.append(arg);
			continue;
		}

		// No, it's an option. Check whether it's a single-letter
		// option or a long option.
		if (*++arg != '-')
		{
			parse_single_letter_options(&argc, &argv, arg);
			continue;
		}

		// It's a free standing double dash marker;
		// treat the rest of arguments as positional.
		if (*++arg == '\0')
		{
			positional_argument_values.append(++argv,
				(size_t) --argc);
			break;
		}

		// It's a long option.
		parse_long_option(&argc, &argv, arg, version_info.is_empty());
	}

	// Part two: validation.
	option_value_array::const_iterator ov_iter(option_values.begin());

	while (ov_iter != option_values.end())
		switch (ov_iter->arg_info->id)
		{
		case VERSION_OPT_ID:
			help_output_stream->write(version_info.data(),
				version_info.length());
			help_output_stream->write("\n", 1);
			return HELP_CMD_ID;

		case HELP_OPT_ID:
			print_help(positional_argument_values, false,
				!version_info.is_empty());
			return HELP_CMD_ID;

		default:
			++ov_iter;
		}

	if (!commands_are_defined())
	{
		append_positional_argument_values(string(),
			positional_argument_values, this);

		next_option_value = option_values.begin();

		return 0;
	}

	// Extract command name and verify that it accepts
	// the options specified on the command line.
	if (positional_argument_values.is_empty())
	{
		if (option_values.is_empty())
			report_error(synopsis, help);
		else
			error("a command is required");
	}

	string command_name(positional_argument_values.first(),
		calc_length(positional_argument_values.first()));

	positional_argument_values.remove(0);

	const command_info* const* ci = cmd_name_to_cmd_info.find(command_name);

	if (ci == NULL)
	{
		if (command_name == help)
		{
			print_help(positional_argument_values, true,
				!version_info.is_empty());
			return HELP_CMD_ID;
		}

		error("unknown command '%s'", command_name.data());
	}

	for (ov_iter = option_values.begin();
			ov_iter != option_values.end(); ++ov_iter)
		if (!(*ci)->is_option_accepted(ov_iter->arg_info))
			cmd_error(command_name, "command '%s' "
				"does not accept option '%s'",
				command_name.data(),
				ov_iter->arg_info->
					option_name_variants().data());

	append_positional_argument_values(command_name,
		positional_argument_values, *ci);

	next_option_value = option_values.begin();

	return (*ci)->id;
}

cli::cli(const string& program_summary) : impl_ref(new impl(program_summary))
{
}

void cli::register_option(int unique_arg_id, const string& name_variants,
		const string& description)
{
	impl_ref->register_arg(option, unique_arg_id,
		name_variants, string(), description);
}

void cli::register_option_with_parameter(int unique_arg_id,
		const string& name_variants, const string& param_name,
		const string& description)
{
	impl_ref->register_arg(option_with_parameter, unique_arg_id,
		name_variants, param_name, description);
}

void cli::register_positional_argument(int unique_arg_id,
		const string& arg_name)
{
	impl_ref->register_arg(positional_argument, unique_arg_id,
		arg_name, string(), string());
}

void cli::register_optional_positional(int unique_arg_id,
		const string& arg_name)
{
	impl_ref->register_arg(optional_positional, unique_arg_id,
		arg_name, string(), string());
}

void cli::register_zero_or_more_positional(int unique_arg_id,
		const string& arg_name)
{
	impl_ref->register_arg(zero_or_more_positional, unique_arg_id,
		arg_name, string(), string());
}

void cli::register_one_or_more_positional(int unique_arg_id,
		const string& arg_name)
{
	impl_ref->register_arg(one_or_more_positional, unique_arg_id,
		arg_name, string(), string());
}

void cli::register_command_category(int unique_cat_id, const string& title)
{
	B_ASSERT(unique_cat_id >= 0 &&
		impl_ref->cat_id_to_cat_info.find(unique_cat_id) == NULL &&
		"Category IDs must be unique");

	impl_ref->cat_id_to_cat_info.insert(unique_cat_id,
		new category_info(title));
}

void cli::register_command(int unique_cmd_id, const string& name_variants,
	const string& synopsis, const string& usage, int cat_id)
{
	B_ASSERT(unique_cmd_id >= 0 &&
		impl_ref->cmd_id_to_cmd_info.find(unique_cmd_id) == NULL &&
		"Command IDs must be unique");

	B_ASSERT(impl_ref->cat_id_to_cat_info.find(cat_id) != NULL &&
		"No such category ID");

	command_info* ci = impl_ref->cmd_id_to_cmd_info.insert(
		command_info(unique_cmd_id, name_variants, synopsis, usage));

	(*impl_ref->cat_id_to_cat_info.find(cat_id))->commands.append(ci);

	for (name_variant_list::const_iterator name =
				ci->name_variants.begin();
			name != ci->name_variants.end(); ++name)
		impl_ref->cmd_name_to_cmd_info.insert(*name, ci);
}

void cli::register_association(int cmd_id, int arg_id)
{
	B_ASSERT(impl_ref->cmd_id_to_cmd_info.find(cmd_id) != NULL &&
		"No such command ID");

	B_ASSERT(impl_ref->opt_id_to_opt_info.find(arg_id) != NULL &&
		"No such option ID");

	command_info* ci = static_cast<command_info*>(
		impl_ref->cmd_id_to_cmd_info.find(cmd_id));

	option_info* oi = static_cast<option_info*>(
		impl_ref->opt_id_to_opt_info.find(arg_id));

	switch (oi->type)
	{
	case option:
	case option_with_parameter:
		ci->accepted_options.append(oi);
		break;

	case positional_argument:
		if (ci->trailing_required_args.is_empty() &&
				ci->required_sequence_arg == NULL &&
				ci->optional_args.is_empty() &&
				ci->optional_sequence_arg == NULL)
			ci->required_args.append(oi);
		else
			ci->trailing_required_args.append(oi);
		break;

	case optional_positional:
		B_ASSERT(ci->required_sequence_arg == NULL &&
			ci->optional_sequence_arg == NULL &&
			ci->trailing_required_args.is_empty());

		ci->optional_args.append(oi);
		break;

	case zero_or_more_positional:
		B_ASSERT(ci->required_sequence_arg == NULL &&
			ci->optional_sequence_arg == NULL &&
			ci->trailing_required_args.is_empty());

		ci->optional_sequence_arg = oi;
		break;

	case one_or_more_positional:
		B_ASSERT(ci->required_sequence_arg == NULL &&
			ci->optional_args.is_empty() &&
			ci->optional_sequence_arg == NULL &&
			ci->trailing_required_args.is_empty());

		ci->required_sequence_arg = oi;
	}
}

int cli::parse(int argc, const char* const *argv, const arg_list* arg)
{
	impl_ref->help_output_stream = standard_output_stream();

	impl_ref->max_help_text_width = DEFAULT_HELP_TEXT_WIDTH;
	impl_ref->cmd_descr_indent = DEFAULT_CMD_DESCR_INDENT;
	impl_ref->opt_descr_indent = DEFAULT_OPT_DESCR_INDENT;

	string program_name, version_info, program_description;

	for (; arg != NULL; arg = arg->prev_arg)
		switch (arg->tag)
		{
		case PROGRAM_NAME_ARG:
			program_name = cli_args::program_name.value(arg);
			break;

		case VERSION_INFO_ARG:
			version_info = cli_args::version_info.value(arg);
			break;

		case PROGRAM_DESCRIPTION_ARG:
			program_description =
				cli_args::program_description.value(arg);
			break;

		case HELP_TEXT_WIDTH_ARG:
			impl_ref->max_help_text_width =
				cli_args::help_text_width.value(arg);
			break;

		case CMD_DESCR_INDENT_ARG:
			impl_ref->cmd_descr_indent =
				cli_args::cmd_descr_indent.value(arg);
			break;

		case ARG_DESCR_INDENT_ARG:
			impl_ref->opt_descr_indent =
				cli_args::arg_descr_indent.value(arg);
			break;

		case HELP_OUTPUT_STREAM_ARG:
			impl_ref->help_output_stream =
				cli_args::help_output_stream.value(arg);
		}

	if (program_name.is_empty())
	{
		program_name.assign(*argv, calc_length(*argv));

		size_t basename_pos = program_name.rfind('/');

		if (basename_pos != (size_t) -1)
			program_name.remove(0, basename_pos + 1);
	}

	impl_ref->program_name = program_name;
	impl_ref->usage = program_description;

	return impl_ref->parse_and_validate(argc, argv, version_info);
}

bool cli::next_arg(int* arg_id, const char** opt_value)
{
	if (impl_ref->next_option_value == impl_ref->option_values.end())
		return false;

	*arg_id = impl_ref->next_option_value->arg_info->id;
	*opt_value = impl_ref->next_option_value->arg_value;

	++impl_ref->next_option_value;

	return true;
}

B_END_NAMESPACE
