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
	printf("Runs the motors to a certain encoder position.\n");
	printf("\n");
	printf("Usage:\n");
	printf("  motor_gotoposition [OPTIONS] POS         Runs both motors to POS\n");
	printf("  motor_gotoposition [OPTIONS] POS1 POS2   Runs the left motor until POS1 and right motor to POS2\n");
	printf("\n");
}

// Main program.
int main(int argc, char *argv[]) {
	// Command line parsing
	commandline_init();
	commandline_option_register("-p", "--profile", cCommandLine_Option);
	commandline_parse(argc, argv);

	// Help
	if (commandline_option_provided("-h", "--help")) {
		help();
		exit(1);
	}

	// Initialization
	khepera3_init();

	// Put the wheels in normal (control) mode
	khepera3_drive_start();

	// Set the speed
	if (commandline_argument_count() == 1) {
		khepera3_drive_goto_position(commandline_argument_int(0, 0), commandline_argument_int(0, 0));

	} else if (commandline_argument_count() == 2) {
		khepera3_drive_goto_position(commandline_argument_int(0, 0), commandline_argument_int(1, 0));
	}

	return 0;
}

