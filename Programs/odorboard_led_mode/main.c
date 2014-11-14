/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include "odorboard.h"
#include <stdio.h>
#include <stdlib.h>

// Prints the help text.
void help() {
	printf("Sets or reports the LED mode.\n");
	printf("Usage: odorboard_led_mode [debug|off|external]\n");
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
	odorboard_init();

	// Set value
	if (commandline_argument_count() > 0) {
		const char *mode = commandline_argument(0, "");
		if (strcmp(mode, "debug") == 0) {
			odorboard.led.mode = cOdorBoardLEDMode_Debug;
		} else if (strcmp(mode, "off") == 0) {
			odorboard.led.mode = cOdorBoardLEDMode_Off;
		} else if (strcmp(mode, "external") == 0) {
			odorboard.led.mode = cOdorBoardLEDMode_External;
		} else {
			help();
			exit(1);
		}
		odorboard_led_mode_set();
	}

	// Report value
	odorboard_led_mode_get();
	if (odorboard.led.mode == cOdorBoardLEDMode_Debug) {
		printf("$LED_MODE,debug\n");
	} else if (odorboard.led.mode == cOdorBoardLEDMode_Off) {
		printf("$LED_MODE,off\n");
	} else if (odorboard.led.mode == cOdorBoardLEDMode_External) {
		printf("$LED_MODE,external\n");
	} else {
		printf("$LED_MODE,unknown\n");
	}

	return 0;
}
