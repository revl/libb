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
#include <b/custom_exception.h>

#define VERSION_OPT_ID -1
#define HELP_OPT_ID -2
#define COMMAND_OPT_ID -3

#define HELP_CMD_ID -1

#define UNSPECIFIED_CATEGORY_ID -1

#define DEFAULT_HELP_TEXT_WIDTH 72
#define DEFAULT_CMD_DESCR_INDENT 24
#define DEFAULT_OPT_DESCR_INDENT 32

B_BEGIN_NAMESPACE

// TODO Use string_view instead of string in this class
// where the data is expected to be kept in argv?

typedef array<string> name_variant_list;

struct option_or_command_info : public object
{
	option_or_command_info(int opt_or_cmd_id, const string& names) :
		id(opt_or_cmd_id)
	{
		// TODO Extract the following code into some sort of universal
		// split method or function.
		const char* name = names.data();
		const char* vertical_bar = ::strchr(name, '|');

		if (vertical_bar == NULL)
			// FIXME Implement string::append(const value&)
			name_variants.append(1, names);
		else
		{
			do
			{
				name_variants.append(1, string(name,
					vertical_bar - name));
				name = vertical_bar + 1;
				vertical_bar = ::strchr(name, '|');
			}
			while (vertical_bar != NULL);

			name_variants.append(1,
					string(name, calc_length(name)));
		}
	}

	const string& primary_name() const
	{
		return name_variants.first();
	}

	int id;
	name_variant_list name_variants;
};

struct option_info : public option_or_command_info
{
	option_info(int arg_id, const string& arg_names,
			cli::arg_type opt_type,
			const string& arg_descr) :
		option_or_command_info(arg_id, arg_names),
		type(opt_type),
		description(arg_descr)
	{
	}

	static string add_dashes(const string& opt_name)
	{
		B_STATIC_CONST_STRING(dashes, "--");

		return opt_name.length() == 1 ? '-' + opt_name :
			dashes + opt_name;
	}

	string option_name_variants() const
	{
		string result(add_dashes(name_variants.first()));
		if (name_variants.size() > 1)
		{
			result.append(" [", 2);
			name_variant_list::const_iterator name(name_variants.begin());
			result.append(add_dashes(*++name));
			while (++name != name_variants.end())
			{
				result.append(", ", 2);
				result.append(add_dashes(*name));
			}
			result.append(1, ']');
		}
		if (type == cli::option_with_parameter)
			result.append(" ARG", 4);
		return result;
	}

	int type;
	string description;

	virtual ~option_info();
};

option_info::~option_info()
{
}

typedef array<const option_info*> option_info_list;

struct common_parts
{
	common_parts(const string& s, const string& u) : synopsis(s), usage(u)
	{
	}

	string synopsis;
	string usage;

	option_info_list positional_arguments;
	option_info_list accepted_options;

	~common_parts();
};

common_parts::~common_parts()
{
}

struct command_info : public option_or_command_info, public common_parts
{
	command_info(int cmd_id, const string& cmd_names,
			const string& s, const string& u) :
		option_or_command_info(cmd_id, cmd_names),
		common_parts(s, u)
	{
	}

	string command_name_variants() const
	{
		if (name_variants.size() == 1)
			return name_variants.first();
		name_variant_list::const_iterator name(name_variants.begin());
		string result(*name);
		result.append(" (", 2);
		result.append(*++name);
		while (++name != name_variants.end())
		{
			result.append(", ", 2);
			result.append(*name);
		}
		result.append(1, ')');
		return result;
	}

	bool is_option_accepted(const option_info* opt_info) const
	{
		option_info_list::const_iterator iter = accepted_options.begin();

		while (iter != accepted_options.end())
			if (*iter++ == opt_info)
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

struct cli::impl : public object, public common_parts
{
	impl(const string& program_summary,
			const string& program_description,
			const string& prog_version);

	void print_word_wrapped(int topic_len, int indent,
			const string& text, int cont_indent = -1) const;
	void print_help(const positional_argument_list& commands,
			bool using_help_command) const;
	void print_help_on_command(const common_parts* cp,
			const string& name_for_synopsis,
			const string& name_for_usage) const;
	void report_error(const string& error_message,
			const string& help_cmd) const;
	void error(const char* err_fmt, ...) const B_PRINTF_STYLE(2, 3);
	void cmd_error(const string& command_name,
			const char* err_fmt, ...) const B_PRINTF_STYLE(3, 4);
	int parse_and_validate(int argc, const char* const *argv);

	// Command and argument definitions.
	string program_name;
	string version_info;
	const option_info* single_letter_options[256];
	// TODO store the structure itself, not just pointer
	map<string, const option_info*> long_opt_name_to_opt_info;
	map<int, ref<option_info> > opt_id_to_opt_info;
	map<string, const command_info*> cmd_name_to_cmd_info;
	map<int, ref<command_info> > cmd_id_to_cmd_info;
	typedef map<int, ref<category_info> > cat_id_to_cat_info_map;
	cat_id_to_cat_info_map cat_id_to_cat_info;
	option_info m_VersionOption;
	option_info m_HelpOption;

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
	option_value_array::const_iterator next_option_value;

	// Help text formatting and error reporting.
	ref<output_stream> help_output_stream;
	ref<output_stream> error_stream;

	int max_help_text_width;
	int cmd_descr_indent;
	int opt_descr_indent;
};

B_STATIC_CONST_STRING(help, "help");
B_STATIC_CONST_STRING(help_option, "--help");
B_STATIC_CONST_STRING(s_Version, "version");
B_STATIC_CONST_STRING(available_commands_title, "Available commands");

cli::impl::impl(const string& program_summary,
		const string& program_description,
		const string& prog_version) :
	common_parts(program_summary, program_description),
	version_info(prog_version),
	m_VersionOption(VERSION_OPT_ID, s_Version,
		cli::option, string()),
	m_HelpOption(HELP_OPT_ID, help,
		cli::option, string()),
	max_help_text_width(DEFAULT_HELP_TEXT_WIDTH),
	cmd_descr_indent(DEFAULT_CMD_DESCR_INDENT),
	opt_descr_indent(DEFAULT_OPT_DESCR_INDENT)
{
	memset(single_letter_options, 0, sizeof(single_letter_options));
	long_opt_name_to_opt_info.insert(s_Version, &m_VersionOption);
	long_opt_name_to_opt_info.insert(help, &m_HelpOption);
	cat_id_to_cat_info.insert(UNSPECIFIED_CATEGORY_ID,
		new category_info(available_commands_title));
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
	const char* next_line = NULL; // A no-op assignment to make GCC happy.
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
			const char* max_pos = line + max_help_text_width - indent;
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
				else if (*pos == '\n')
				{
					next_line = (line_end = pos) + 1;
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
			next_line = (line_end == NULL ? line_end = text_end : line_end + 1);
		}
		int line_len = int(line_end - line);
		if (line_len > 0)
		{
			string output_line = string::formatted("%*.*s\n",
				offset + line_len, line_len, line);

			help_output_stream->write(output_line.data(),
				output_line.length());
		}
		else
			help_output_stream->write("\n", 1);
		offset = indent = cont_indent;
	}
	while ((line = next_line) < text_end);
}

void cli::impl::print_help(const positional_argument_list& commands,
		bool using_help_command) const
{
	for (option_value_array::const_iterator option_value = option_values.begin();
			option_value != option_values.end(); ++option_value)
		if (option_value->arg_info->id != HELP_OPT_ID)
		{
			string opt_name(option_value->arg_info->option_name_variants());
			if (using_help_command)
			{
				cmd_error(help, "command 'help' does not "
					"accept option '%s'",
					opt_name.data());
			}
			else
			{
				cmd_error(help_option, "'--help' cannot be "
					"combined with option '%s'",
					opt_name.data());
			}
		}

	if (!commands_are_defined())
		print_help_on_command(this, program_name, program_name);
	else if (commands.is_empty())
	{
		string prog_usage = string::formatted(
			"Usage: %s <command> [options] [args]\n",
				program_name.data());
		help_output_stream->write(prog_usage.data(),
			prog_usage.length());
		print_word_wrapped(0, 0, synopsis);
		prog_usage.format(
			"Type '%s help <command>' for help on a specific command.\n"
			"Type '%s --version' to see the program version.\n",
			program_name.data(), program_name.data());
		help_output_stream->write(prog_usage.data(),
			prog_usage.length());
		if (!usage.is_empty())
		{
			help_output_stream->write("\n", 1);
			print_word_wrapped(0, 0, usage);
		}
		for (cat_id_to_cat_info_map::const_iterator category =
					cat_id_to_cat_info.begin();
				category != cat_id_to_cat_info.end();
				++category)
		{
			if (!(*category)->commands.is_empty())
			{
				prog_usage.format("\n%s:\n\n",
					(*category)->title.data());
				help_output_stream->write(prog_usage.data(),
					prog_usage.length());
				for (command_info_list::const_iterator cmd =
							(*category)->commands.begin();
						cmd != (*category)->commands.end(); ++cmd)
				{
					size_t command_name_len =
						(*cmd)->command_name_variants().length();
					help_output_stream->write("  ", 2);
					help_output_stream->write((*cmd)->command_name_variants().data(),
						command_name_len);
					int topic_len = 2 + (int) command_name_len;
					print_word_wrapped(topic_len,
						cmd_descr_indent - 2,
						string("- ", 2) + (*cmd)->synopsis,
						cmd_descr_indent);
				}
			}
		}
		help_output_stream->write("\n", 1);
	}
	else
	{
		B_STATIC_CONST_STRING(help_command_synopsis,
				"Describe the usage of this "
				"program or its commands.");

		command_info help_command(HELP_CMD_ID, help,
			help_command_synopsis, string());

		B_STATIC_CONST_STRING(help_command_arg, "COMMAND");

		option_info command_arg(COMMAND_OPT_ID, help_command_arg,
			cli::zero_or_more_positional, string());

		help_command.positional_arguments.append(1, &command_arg);

		for (positional_argument_list::const_iterator cmd_name =
					commands.begin();
				cmd_name != commands.end(); ++cmd_name)
		{
			const command_info** ci =
				cmd_name_to_cmd_info.find(*cmd_name);

			if (ci != NULL)
				print_help_on_command(*ci,
					(*ci)->command_name_variants(),
					program_name + ' ' +
					(*ci)->primary_name());
			else
				if (*cmd_name == help)
					print_help_on_command(&help_command,
						help,
						program_name + ' ' + help);
				else
				{
					string unknown_command = string::formatted(
						"'%s': unknown command.\n\n", *cmd_name);
					help_output_stream->write(unknown_command.data(),
						unknown_command.length());
				}
		}
	}
}

void cli::impl::print_help_on_command(const common_parts* cp,
	const string& name_for_synopsis, const string& name_for_usage) const
{
	help_output_stream->write(name_for_synopsis.data(),
		name_for_synopsis.length());
	help_output_stream->write(":", 1);
	int text_len = (int) name_for_synopsis.length() + 1;
	print_word_wrapped(text_len, text_len + 1, cp->synopsis);
	help_output_stream->write("\n", 1);

	string args;
	for (option_info_list::const_iterator arg =
				cp->positional_arguments.begin();
			arg != cp->positional_arguments.end(); ++arg)
	{
		if (!args.is_empty())
			args.append(1, ' ');
		switch ((*arg)->type)
		{
		case cli::positional_argument:
			args.append((*arg)->primary_name());
			break;
		case cli::optional_positional:
			args.append(1, '[');
			args.append((*arg)->primary_name());
			args.append(1, ']');
			break;
		case cli::zero_or_more_positional:
			args.append(1, '[');
			args.append((*arg)->primary_name());
			args.append("...]", 4);
			break;
		default: // always cli::one_or_more_positional
			args.append((*arg)->primary_name());
			args.append("...", 3);
		}
	}
	string cmd_usage = string::formatted("Usage: %s", name_for_usage.data());
	help_output_stream->write(cmd_usage.data(), cmd_usage.length());
	text_len = (int) cmd_usage.length();
	print_word_wrapped(text_len, text_len + 1, args);

	if (!cp->usage.is_empty())
	{
		help_output_stream->write("\n", 1);
		print_word_wrapped(0, 0, cp->usage);
	}

	if (!cp->accepted_options.is_empty())
	{
		B_STATIC_CONST_STRING(valid_options, "\nValid options:\n");
		help_output_stream->write(valid_options.data(),
			valid_options.length());
		for (option_info_list::const_iterator opt =
					cp->accepted_options.begin();
				opt != cp->accepted_options.end(); ++opt)
		{
			string option_name = string::formatted(
				"  %-*s :", opt_descr_indent - 5,
				(*opt)->option_name_variants().data());
			help_output_stream->write(option_name.data(),
				option_name.length());
			print_word_wrapped((int) option_name.length(),
					opt_descr_indent, (*opt)->description);
		}
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
	error_message.append_format_va(err_fmt, ap);
	va_end(ap);

	report_error(error_message, !commands_are_defined() ?
			help_option : help);
}

void cli::impl::cmd_error(const string& command_name,
		const char* err_fmt, ...) const
{
	string error_message = program_name;
	error_message.append(": ", 2);

	va_list ap;
	va_start(ap, err_fmt);
	error_message.append_format_va(err_fmt, ap);
	va_end(ap);

	report_error(error_message, !commands_are_defined() ?
			help_option : help + ' ' + command_name);
}

int cli::impl::parse_and_validate(int argc, const char* const *argv)
{
	positional_argument_list positional_argument_values;

	// Part one: parsing.
	while (--argc > 0)
	{
		const char* arg = *++argv;

		// Check if the current argument is a positional argument.
		if (*arg != '-' || arg[1] == '\0')
			positional_argument_values.append(1, arg);
		else
		{
			// No, it's an option. Check whether it's a
			// single-letter option or a long option.
			const option_info* opt_info;
			const char* opt_param;
			if (*++arg == '-')
			{
				// It's a long option.
				// If it's a free standing double dash marker,
				// treat the rest of arguments as positional.
				if (*++arg == '\0')
				{
					while (--argc > 0)
						positional_argument_values.append(1, *++argv);
					break;
				}
				// Check if a parameter is specified for this option.
				opt_param = strchr(arg, '=');
				string opt_name(arg, opt_param != NULL ?
						(size_t) (opt_param++ - arg) :
						calc_length(arg));
				const option_info** oi =
					long_opt_name_to_opt_info.find(opt_name);
				if (oi == NULL)
					error("unknown option '--%s'",
							opt_name.data());
				opt_info = *oi;
				// Check if this option must have a parameter.
				if (opt_info->type == cli::option)
				{
					// No, it's a switch; it's not supposed to have a parameter.
					if (opt_param != NULL)
						error("option '--%s' does not "
								"expect a "
								"parameter",
							opt_name.data());
					opt_param = "yes";
				}
				else
					// The option expects a parameter.
					if (opt_param == NULL)
					{
						// Parameter is not specified; use the next
						// command line argument as a parameter for
						// this option.
						if (--argc == 0)
							error("option '--%s' "
								"requires a "
								"parameter",
								opt_name.data()
								);
						opt_param = *++argv;
					}
			}
			else
			{
				// The current argument is a (series of) one-letter option(s).
				for (;;)
				{
					char opt_letter = *arg++;
					opt_info = single_letter_options[
						(unsigned char) opt_letter];
					if (opt_info == NULL)
						error("unknown option '-%c'",
								opt_letter);

					// Check if this option must have a parameter.
					if (opt_info->type == cli::option)
					{
						// It's a switch; it's not supposed to have a parameter.
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
							if (--argc == 0)
								error("option "
									"'-%c' "
									"requir"
									"es a p"
									"aramet"
									"er",
									opt_letter);
							opt_param = *++argv;
						}
						else
							opt_param = arg;
						break;
					}

					arg_value av =
					{
						opt_info,
						opt_param
					};

					option_values.append(1, av);
				}
			}

			arg_value av =
			{
				opt_info,
				opt_param
			};

			option_values.append(1, av);
		}
	}

	// Part two: validation.
	option_value_array::const_iterator option_value(option_values.begin());
	while (option_value != option_values.end())
		switch (option_value->arg_info->id)
		{
		case VERSION_OPT_ID:
			help_output_stream->write(version_info.data(),
				version_info.length());
			help_output_stream->write("\n", 1);
			return HELP_CMD_ID;

		case HELP_OPT_ID:
			print_help(positional_argument_values, false);
			return HELP_CMD_ID;

		default:
			++option_value;
		}

	string command_name;
	const option_info_list* expected_positional_arguments;
	int ret_val;

	if (!commands_are_defined())
	{
		expected_positional_arguments = &positional_arguments;
		ret_val = 0;
	}
	else
	{
		if (positional_argument_values.is_empty())
		{
			if (option_values.is_empty())
				report_error(synopsis, help);
			else
				error("a command is required");
		}

		command_name.assign(positional_argument_values.first(),
				calc_length(positional_argument_values.first()));
		positional_argument_values.remove(0);

		const command_info** command =
			cmd_name_to_cmd_info.find(command_name);

		if (command == NULL)
		{
			if (command_name == help)
			{
				print_help(positional_argument_values, true);
				return HELP_CMD_ID;
			}
			error("unknown command '%s'", command_name.data());
		}

		const command_info* ci = *command;

		for (option_value = option_values.begin();
				option_value != option_values.end(); ++option_value)
			if (!ci->is_option_accepted(
					option_value->arg_info))
				cmd_error(command_name,
						"command '%s' doesn't accept option '%s'",
						command_name.data(),
						option_value->arg_info->option_name_variants().data());

		expected_positional_arguments = &ci->positional_arguments;
		ret_val = ci->id;
	}

	positional_argument_list::const_iterator arg_val =
		positional_argument_values.begin();
	option_info_list::const_iterator expected_arg =
		expected_positional_arguments->begin();

	for (;;)
	{
		if (expected_arg != expected_positional_arguments->end())
			if (arg_val == positional_argument_values.end())
				switch ((*expected_arg)->type)
				{
				case cli::positional_argument:
				case cli::one_or_more_positional:
					cmd_error(command_name,
						"missing argument '%s'",
						(*expected_arg)->primary_name().data());
				}
			else
				switch ((*expected_arg)->type)
				{
				case cli::positional_argument:
				case cli::optional_positional:
					{
						arg_value av =
						{
							*expected_arg,
							*arg_val
						};
						option_values.append(1, av);
						++arg_val;
						++expected_arg;
					}
					continue;
				default:
					do
					{
						arg_value av =
						{
							*expected_arg,
							*arg_val
						};
						option_values.append(1, av);
					}
					while (++arg_val != positional_argument_values.end());
				}
		else
			if (arg_val != positional_argument_values.end())
				cmd_error(command_name,
					"too many positional arguments");
		break;
	}

	next_option_value = option_values.begin();

	return ret_val;
}

cli::cli(const string& version_info,
		const string& program_summary,
		const string& program_description) :
	impl_ref(new impl(program_summary, program_description, version_info))
{
}

void cli::set_help_text_margins(int help_text_width,
	int cmd_descr_indent, int opt_descr_indent)
{
	impl_ref->max_help_text_width = help_text_width;
	impl_ref->cmd_descr_indent = cmd_descr_indent;
	impl_ref->opt_descr_indent = opt_descr_indent;
}

void cli::register_arg(cli::arg_type type,
	int arg_id, const string& name_variants, const string& description)
{
	B_ASSERT(arg_id >= 0 &&
			impl_ref->opt_id_to_opt_info.find(arg_id) == NULL &&
			"Option IDs must be unique");

	option_info* opt_info = new option_info(arg_id,
			name_variants, type, description);

	impl_ref->opt_id_to_opt_info.insert(arg_id, opt_info);

	switch (type)
	{
	default:
		B_ASSERT(opt_info->name_variants.size() == 1 &&
			"Positional arguments do not allow name variants");

		impl_ref->positional_arguments.append(1, opt_info);
		break;

	case option:
	case option_with_parameter:
		for (name_variant_list::const_iterator name =
					opt_info->name_variants.begin();
				name != opt_info->name_variants.end(); ++name)
			if (name->length() == 1)
				impl_ref->single_letter_options[
					(unsigned char) name->at(0)] = opt_info;
			else
				impl_ref->long_opt_name_to_opt_info.insert(
						*name, opt_info);

		impl_ref->accepted_options.append(1, opt_info);
	}
}

void cli::register_command_category(int cat_id, const string& title)
{
	B_ASSERT(cat_id >= 0 &&
			impl_ref->cat_id_to_cat_info.find(cat_id) == NULL &&
			"Category IDs must be unique");

	impl_ref->cat_id_to_cat_info.insert(cat_id, new category_info(title));
}

void cli::register_command(int cmd_id, const string& name_variants,
	const string& synopsis, const string& usage, int cat_id)
{
	B_ASSERT(cmd_id >= 0 &&
			impl_ref->cmd_id_to_cmd_info.find(cmd_id) == NULL &&
			"Command IDs must be unique");

	B_ASSERT(impl_ref->cat_id_to_cat_info.find(cat_id) != NULL &&
			"No such category ID");

	command_info* ci = new command_info(cmd_id,
			name_variants, synopsis, usage);
	impl_ref->cmd_id_to_cmd_info.insert(cmd_id, ci);

	(*impl_ref->cat_id_to_cat_info.find(cat_id))->commands.append(1, ci);

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

	command_info* ci = *impl_ref->cmd_id_to_cmd_info.find(cmd_id);
	option_info* oi = *impl_ref->opt_id_to_opt_info.find(arg_id);

	switch (oi->type)
	{
	case option:
	case option_with_parameter:
		ci->accepted_options.append(1, oi);
		break;

	default:
		B_ASSERT("Invalid sequence of optional positional arguments" &&
			(ci->positional_arguments.is_empty() ||
			ci->positional_arguments.last()->type ==
				positional_argument ||
			(ci->positional_arguments.last()->type ==
				optional_positional &&
					oi->type != positional_argument)));

		ci->positional_arguments.append(1, oi);
	}
}

arg_name<string, 0> cli::program_name;

arg_name<ref<output_stream>, 1> cli::help_output_stream;

arg_name<ref<output_stream>, 2> cli::error_stream;

int cli::parse(int argc, const char* const *argv, const arg_list* arg)
{
	impl_ref->help_output_stream = standard_output_stream();
	impl_ref->error_stream = standard_error_stream();

	string prog_name;

	for (; arg != NULL; arg = arg->prev_arg)
		if (program_name.is_name_for(arg))
			prog_name = program_name.value(arg);
		else
			if (help_output_stream.is_name_for(arg))
				impl_ref->help_output_stream =
					help_output_stream.value(arg);
			else
				if (error_stream.is_name_for(arg))
					impl_ref->error_stream =
						error_stream.value(arg);

	if (prog_name.is_empty())
	{
		prog_name.assign(*argv, calc_length(*argv));

		size_t basename_pos = prog_name.rfind('/');

		if (basename_pos != (size_t) -1)
			prog_name.remove(0, basename_pos + 1);
	}

	impl_ref->program_name = prog_name;

	return impl_ref->parse_and_validate(argc, argv);
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
