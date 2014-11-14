/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "commandline.h"

void commandline_init() {
	commandline_hash_len = 0;
	commandline_list_len = 0;
}

struct sCommandLineOption *commandline_option_register(const char *option_short, const char *option_long, enum eCommandLineOptionType type) {
	return commandline_option_register_hook(option_short, option_long, type, 0);
}

struct sCommandLineOption *commandline_option_register_hook(const char *option_short, const char *option_long, enum eCommandLineOptionType type, void (*hook)(struct sCommandLineOption *option)) {
	struct sCommandLineOption *option;

	// Check array bounds
	assert(commandline_hash_len < COMMANDLINE_HASH_SIZE);

	// Check if there is such a option already
	option = commandline_option(option_short);
	if (option) {
		return option;
	}
	option = commandline_option(option_long);
	if (option) {
		return option;
	}

	// If not, create a new empty option
	option = &(commandline_hash[commandline_hash_len]);
	commandline_hash_len++;
	option->type = type;
	option->option_short = option_short;
	option->option_long = option_long;
	option->provided = 0;
	option->value = 0;
	option->hook = hook;

	return option;
}

int commandline_isletter(char c) {
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

void commandline_parse(int argc, char *argv[]) {
	int i;
	int arglen;
	struct sCommandLineOption *option = 0;

	for (i = 1; i < argc; i++) {
		arglen = strlen(argv[i]);
		if (arglen > 0) {
			if ((arglen > 2) && (argv[i][0] == '-') && (argv[i][1] == '-') && commandline_isletter(argv[i][2])) {
				// If we are still expecting a value, call the hook
				if ((option) && (option->hook)) {
					option->hook(option);
				}

				// Long option
				option = commandline_option_register(0, argv[i], cCommandLine_Option_Value);
				option->provided = 1;
				if (option->type == cCommandLine_Option) {
					// Not expecting any value, hence call the hook
					if (option->hook) {
						option->hook(option);
					}
					option = 0;
				} else {
					// Expecting a value
					option->value = 0;
				}
			} else if ((arglen > 1) && (argv[i][0] == '-') && commandline_isletter(argv[i][1])) {
				// If we are still expecting a value, call the hook
				if ((option) && (option->hook)) {
					option->hook(option);
				}

				// Short option
				option = commandline_option_register(argv[i], 0, cCommandLine_Option_Value);
				option->provided = 1;
				if (option->type == cCommandLine_Option) {
					// Not expecting any value, hence call the hook
					if (option->hook) {
						option->hook(option);
					}
					option = 0;
				} else {
					// Expecting a value
					option->value = 0;
				}
			} else if (option) {
				// Option value
				option->value = argv[i];
				if (option->hook) {
					option->hook(option);
				}
				option = 0;
			} else {
				// Other argument
				commandline_list[commandline_list_len] = argv[i];
				commandline_list_len++;
			}
		}
	}

	// If we are still expecting a value, call the hook
	if ((option) && (option->hook)) {
		option->hook(option);
	}
}

struct sCommandLineOption *commandline_option(const char *option) {
	int i;

	// No option? Then do nothing
	if (option == 0) {
		return 0;
	}

	// Check all hash entries
	for (i = 0; i < commandline_hash_len; i++) {
		// Check short option
		if ((commandline_hash[i].option_short) && (strcmp(option, commandline_hash[i].option_short) == 0)) {
			return &(commandline_hash[i]);
		}

		// Check long option
		if ((commandline_hash[i].option_long) && (strcmp(option, commandline_hash[i].option_long) == 0)) {
			return &(commandline_hash[i]);
		}
	}

	// If not found at all, return NULL
	return 0;
}

const char *commandline_option_value(const char *option_short, const char *option_long, const char *defaultvalue) {
	struct sCommandLineOption *option;

	// Search for short option
	option = commandline_option(option_short);
	if (option) {
		return (option->value ? option->value : defaultvalue);
	}

	// Search for long option
	option = commandline_option(option_long);
	if (option) {
		return (option->value ? option->value : defaultvalue);
	}

	return defaultvalue;
}

int commandline_option_value_int(const char *option_short, const char *option_long, int defaultvalue) {
	const char *value;

	value = commandline_option_value(option_short, option_long, 0);
	if (! value) {
		return defaultvalue;
	}
	return strtol(value, 0, 0);
}

float commandline_option_value_float(const char *option_short, const char *option_long, float defaultvalue) {
	const char *value;

	value = commandline_option_value(option_short, option_long, 0);
	if (! value) {
		return defaultvalue;
	}
	return strtod(value, 0);
}

double commandline_option_value_double(const char *option_short, const char *option_long, double defaultvalue) {
	const char *value;

	value = commandline_option_value(option_short, option_long, 0);
	if (! value) {
		return defaultvalue;
	}
	return strtod(value, 0);
}

int commandline_option_provided(const char *option_short, const char *option_long) {
	struct sCommandLineOption *option;

	// Search for short option
	option = commandline_option(option_short);
	if (option) {
		return option->provided;
	}

	// Search for long option
	option = commandline_option(option_long);
	if (option) {
		return option->provided;
	}

	return 0;
}

const char *commandline_argument(int i, const char *defaultvalue) {
	if (i >= commandline_list_len) {
		return defaultvalue;
	}
	return commandline_list[i];
}

int commandline_argument_int(int i, int defaultvalue) {
	if (i >= commandline_list_len) {
		return defaultvalue;
	}
	return strtol(commandline_list[i], 0, 0);
}

float commandline_argument_float(int i, float defaultvalue) {
	if (i >= commandline_list_len) {
		return defaultvalue;
	}
	return strtod(commandline_list[i], 0);
}

double commandline_argument_double(int i, double defaultvalue) {
	if (i >= commandline_list_len) {
		return defaultvalue;
	}
	return strtod(commandline_list[i], 0);
}

int commandline_argument_count() {
	return commandline_list_len;
}
