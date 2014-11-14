/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera3.h"
#include "commandline.h"
#include <stdio.h>
#include <stdlib.h>

// Prints the help text.
void help() {
	printf("Enabled or disables the ultrasound sensors.\n");
	printf("\n");
	printf("Usage:\n");
	printf("  ultrasound_enable [left|0] [frontleft|1] [front|2] [frontright|3] [right|4]\n");
	printf("  ultrasound_enable [--mask 0-31]\n");
	printf("\n");
}

// Main program.
int main(int argc, char *argv[]) {
	int i;
	int mask = 0;
	int sensor_number;
	const int sensor[5] = {1, 2, 4, 8, 16};
	struct sCommandLineOption *option_mask;

	// Command line parsing
	commandline_init();
	option_mask = commandline_option_register("-m", "--mask", cCommandLine_Option_Value);
	commandline_parse(argc, argv);

	// Help
	if (commandline_option_provided("-h", "--help")) {
		help();
		exit(1);
	}

	// Initialization
	khepera3_init();

	// Read command line (--mask option and arguments)
	if (option_mask->value) {
		mask = strtol(option_mask->value, 0, 0);
	}
	for (i = 0; i < commandline_argument_count(); i++) {
		sensor_number = strtol(commandline_argument(i, 0),0,0);
		if (sensor_number > -1) {
			mask |= sensor[sensor_number];
		}
	}

	// Enable ultrasound sensors
	khepera3_ultrasound_enable(mask);

	// Print report
	for (i = 0; i < 5; i++) {
		if (mask & sensor[i]) {
			printf("+ ");
		} else {
			printf("- ");
		}
		printf("\n");
	}

	return 0;
}
