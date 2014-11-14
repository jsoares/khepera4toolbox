/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Loic Matthey, Thomas Lochmatter
 */

#include "khepera3.h"
#include "commandline.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Algorithm parameters
struct sAlgorithm {
	struct {
		int speed;
		long int wait_us;
		float a;
		float skip_steps;
	} configuration;
	struct {
		float step;
	} state;
};

// Algorithm instance
struct sAlgorithm algorithm;

// Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
int verbosity;

// Prints the help text.
void help() {
	printf("Moves the robot along a archimedean spiral of increasing radius.\n");
	printf("\n");
	printf("Usage: motion_spiral [OPTIONS]\n");
	printf("\n");
	printf("Options:\n");
	printf("  -a A                  Sets A, which defines the spiral gap (default: 2000)\n");
	printf("  --skip-steps T        Sets the number of steps to skip at the beginning (default: 10)\n");
	printf("  -s --speed SPEED      Sets the speed (default: 10000)\n");
	printf("  -w --wait-us US       Waits US us in the main loop (default: 20000)\n");
	printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)\n");
	printf("\n");
	printf("Output:\n");
	printf("  $SPIRAL,TIME,SPEED_DIFF\n");
	printf("\n");
}

void algorithm_init() {
	algorithm.configuration.a = commandline_option_value_float("-a", "--gap", 2000.);
	algorithm.configuration.skip_steps = commandline_option_value_float("-ss", "--skip-steps", 10.);
	algorithm.configuration.speed = commandline_option_value_int("-s", "--speed", 10000);
	algorithm.configuration.wait_us = commandline_option_value_int("-w", "--wait-us", 20 * 1000);
	algorithm.state.step = 1.0;
}

void algorithm_run() {
	float speed_diff;
	int speed_left, speed_right;

	// Put the wheels in normal (control) mode
	khepera3_drive_start();

	algorithm.state.step = algorithm.configuration.skip_steps;

	while (1) {
		// This speed difference makes that the resulting trajectory is an archimedean spiral
		speed_diff = algorithm.configuration.a / sqrtf(algorithm.state.step);
		algorithm.state.step += 1;

		// Calculate and set speeds
		speed_left = (int)floorf(algorithm.configuration.speed - speed_diff + 0.5);
		speed_right = (int)floorf(algorithm.configuration.speed + speed_diff + 0.5);
		khepera3_drive_set_speed(speed_left, speed_right);

		// Report
		if (verbosity > 1) {
			printf("$SPIRAL,%f,%f\n", algorithm.state.step, speed_diff);
		}

		// Wait
		usleep(algorithm.configuration.wait_us);
	}
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
	khepera3_init();
	algorithm_init();

	// Read arguments
	verbosity = commandline_option_value_int("-v", "--verbosity", 1);

	// Run server
	algorithm_run();
	return 0;
}
