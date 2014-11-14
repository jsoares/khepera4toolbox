/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera3.h"
#include "commandline.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#define PI 3.14159265358979

// Algorithm parameters and results
struct sAlgorithm {
	struct {
		float speed;
		int wall_threshold;
		int turn_count;
		int wait_us;
		int verbosity;
	} configuration;
	struct {
		void (*hook)();
		int turn_counter;
	} state;
};

// Algorithm instance
struct sAlgorithm algorithm;

// Prints the help text.
void help() {
	printf("Template program.\n");
	printf("\n");
	printf("Usage: name_of_your_program [OPTIONS]\n");
	printf("\n");
	printf("Options:\n");
	printf("  -s --speed SPEED      Sets the forward speed (default: 10000)\n");
	printf("  --wall-threshold T    Sets the wall threshold (default: 1000)\n");
	printf("  --turn-count COUNT    Sets the number of steps to turn when hitting an obstacle (default: 50)\n");
	printf("  -w --wait-us US       Sets the waiting time in the perception-to-action loop (default: 50000)\n");
	printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=default, 2=verbose, 3=very verbose, ...)\n");
	printf("\n");
}

// State functions
void state_forward();
void state_turn();

// Initializes the algorithm.
void algorithm_init() {
	// Initialize modules
	khepera3_init();

	// Read command line options
	algorithm.configuration.speed = commandline_option_value_float("-s", "--speed", 10000.);
	algorithm.configuration.wall_threshold = commandline_option_value_int("-wt", "--wall-threshold", 1000);
	algorithm.configuration.turn_count = commandline_option_value_int("-tc", "--turn-count", 50);
	algorithm.configuration.wait_us = commandline_option_value_int("-w", "--wait-us", 50 * 1000);
	algorithm.configuration.verbosity = commandline_option_value_int("-v", "--verbosity", 1);

	// Set the initial state
	algorithm.state.hook = &state_forward;
}

// Runs the algorithm by calling the appropriate state function.
void algorithm_run() {
	// Put the wheels in normal (control) mode
	khepera3_drive_start();

	// Execute the current state
	while (1) {
		algorithm.state.hook();
	}
}

void state_forward() {
	// Print the state
	if (algorithm.configuration.verbosity > 0) {
		printf("$STATE,forward\n");
	}

	// Go forward
	khepera3_drive_set_speed_differential(algorithm.configuration.speed, 1., 0.);

	while (1) {
		// Take a measurement with the IR sensors in proximity mode
		// Note that this takes about 3 ms
		khepera3_infrared_proximity();

		// If the front sensors detect an obstacle, switch to turn mode
		if ((khepera3.infrared_proximity.sensor[cKhepera3SensorsInfrared_FrontLeft] > algorithm.configuration.wall_threshold) || (khepera3.infrared_proximity.sensor[cKhepera3SensorsInfrared_FrontRight] > algorithm.configuration.wall_threshold)) {
			algorithm.state.hook = &state_turn;
			break;
		}

		// Print values if necessary
		if (algorithm.configuration.verbosity > 1) {
			printf("$WALL_PROXIMITY");
			printf(",%d", khepera3.infrared_proximity.sensor[cKhepera3SensorsInfrared_FrontLeft]);
			printf(",%d", khepera3.infrared_proximity.sensor[cKhepera3SensorsInfrared_FrontRight]);
			printf("\n");
		}

		// Sleep
		usleep(algorithm.configuration.wait_us);
	}
}

void state_turn() {
	// Print the state
	if (algorithm.configuration.verbosity > 0) {
		printf("$STATE,turn\n");
	}

	// Turn slowly
	khepera3_drive_set_speed_differential(algorithm.configuration.speed, 0., 0.5);

	// Reset the counter
	algorithm.state.turn_counter = 0;

	while (1) {
		// Switch back to forward mode after a certain number of steps
		algorithm.state.turn_counter++;
		if (algorithm.state.turn_counter > algorithm.configuration.turn_count) {
			algorithm.state.hook = &state_forward;
			break;
		}

		// Sleep
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
	algorithm_init();

	// Run
	algorithm_run();
	return 0;
}
