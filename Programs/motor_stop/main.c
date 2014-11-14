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
	printf("Stops the motors immediately.\r\n");
	printf("Usage: motor_stop [OPTIONS]\r\n");
	printf("Options:\r\n");
	printf("  -i --idle    Put the motors in idle mode instead of stop mode.\r\n");
}

// Main program.
int main(int argc, char *argv[]) {
	// Command line parsing
	commandline_init();
	commandline_option_register("-i", "--idle", cCommandLine_Option);
	commandline_parse(argc, argv);

	// Help
	if (commandline_option_provided("-h", "--help")) {
		help();
		exit(1);
	}

	// Initialization
	khepera3_init();

	// Stop the motors
	if (commandline_option_provided("-i", "--idle")) {
		khepera3_drive_idle();
	} else {
		khepera3_drive_stop();
	}
	return 0;
}

