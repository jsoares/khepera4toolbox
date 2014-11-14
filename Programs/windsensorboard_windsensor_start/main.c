/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include "windsensorboard.h"
#include <stdio.h>
#include <stdlib.h>

// Prints the help text.
void help() {
	printf("Starts calculating the wind direction (with a state reset to zero) on the wind sensor board.\n");
	printf("Usage: windsensorboard_windsensor_start\n");
}

// Main program.
int main(int argc, char *argv[]) {
	// Command line parsing
	commandline_init();
	commandline_parse(argc, argv);

	// Help
	if (commandline_option_provided("-h", "--help")) {
		help();
		exit(1);
	}

	// Initialization
	windsensorboard_init();

	// Start
	windsensorboard_windsensor_start();
	return 0;
}
