/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef COMMANDLINE
#define COMMANDLINE

enum eCommandLineOptionType {
	cCommandLine_Option,				//!< A simple option which doesn't need any further arguments (e.g. -h for help).
	cCommandLine_Option_Value,			//!< An option which takes one arguments (e.g. -r 10).
};

struct sCommandLineOption {
	enum eCommandLineOptionType type;	//!< Type.
	const char *option_short;			//!< Short form (e.g. -h), if any.
	const char *option_long;			//!< Long form (e.g. --help), if any.
	int provided;						//!< If this option was provided on the command line (-1) or not (0).
	char *value;						//!< Value, if any.
	void (*hook)(struct sCommandLineOption *option);	//!< Hook function, if any.
};

#define COMMANDLINE_HASH_SIZE 64
struct sCommandLineOption commandline_hash[COMMANDLINE_HASH_SIZE];
int commandline_hash_len;
#define COMMANDLINE_LIST_SIZE 64
char *commandline_list[COMMANDLINE_LIST_SIZE];
int commandline_list_len;

//! Initializes this module.
void commandline_init();

//! Registers an option with a specific type. (Options that are not registered are assumed to be of type sCommandLine_Option.) Note that option_short and option_long are not copied - hence these string must not disappear.
struct sCommandLineOption *commandline_option_register(const char *option_short, const char *option_long, enum eCommandLineOptionType type);
//! The same as commandline_option_register, but also registers a function which is called each time the option is read from the command line.
struct sCommandLineOption *commandline_option_register_hook(const char *option_short, const char *option_long, enum eCommandLineOptionType type, void (*hook)(struct sCommandLineOption *option));
//! Parses the command line.
void commandline_parse(int argc, char *argv[]);

//! Searches for an option in all parsed and registered options. Returns 0 if the option was not found.
struct sCommandLineOption *commandline_option(const char *option);
//! Returns the value of a command line option. Returns defaultvalue if the option was not specified, if no value was provided to the option, or if the option was registered as sCommandLine_Option type.
const char *commandline_option_value(const char *option_short, const char *option_long, const char *defaultvalue);
//! Returns the value of a command line option as integer.
int commandline_option_value_int(const char *option_short, const char *option_long, int defaultvalue);
//! Returns the value of a command line option as float.
float commandline_option_value_float(const char *option_short, const char *option_long, float defaultvalue);
//! Returns the value of a command line option as double.
double commandline_option_value_double(const char *option_short, const char *option_long, double defaultvalue);
//! Returns -1 if an option was provided and 0 otherwise.
int commandline_option_provided(const char *option_short, const char *option_long);

//! Returns a non-option command line argument.
const char *commandline_argument(int i, const char *defaultvalue);
//! Returns a non-option command line argument as integer.
int commandline_argument_int(int i, int defaultvalue);
//! Returns a non-option command line argument as double.
float commandline_argument_float(int i, float defaultvalue);
//! Returns a non-option command line argument as double.
double commandline_argument_double(int i, double defaultvalue);
//! Returns the number of non-option command line arguments.
int commandline_argument_count();

//! (private) Returns c =~ [A-Za-z].
int commandline_isletter(char c);

#endif
