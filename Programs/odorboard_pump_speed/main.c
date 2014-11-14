/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include "odorboard.h"
#include <stdio.h>
#include <math.h>
#define PI 3.14159265358979

// Prints the help text.
void help() {
	printf("Sets or reports the pump speeds. Pump speeds are in the range [4096, 0], with 0 being the highest speed and 4096 the lowest (pump switched off). Low speeds (>3072) should be avoided, as the pump may not have enough power to run.\n");
	printf("Usage: odorboard_pump_speed [OPTIONS] [ID [SPEED]]\n");
	printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=default, 2=verbose, 3=very verbose, ...)\n");
	printf("  ID                    Pump ID (0=back, 1=middle, 2=front)\n");
	printf("  SPEED                 Pump speed\n");
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

	// Set pump speed
	if (commandline_argument_count() > 1) {
		int pump_id = commandline_argument_int(0, 10);
		odorboard_pump_speed_set(pump_id, commandline_argument_int(1, 10));
	}

	// Report pump speeds
	odorboard_pump_speed_get();
	printf("$PUMP_SPEED,%d,%d\n", 0, odorboard.pump.speed[0]);
	printf("$PUMP_SPEED,%d,%d\n", 1, odorboard.pump.speed[1]);
	printf("$PUMP_SPEED,%d,%d\n", 2, odorboard.pump.speed[2]);
	return 0;
}
