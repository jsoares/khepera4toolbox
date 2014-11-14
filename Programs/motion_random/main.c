/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera3.h"
#include "commandline.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

// Algorithm parameters
struct sMotionRandom {
	int speed;
	float deviation_max;
	long int interval_us;
};

// Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
int verbosity;

// Prints the help text.
void help() {
	printf("Moves the robot randomly around.\n");
	printf("\n");
	printf("Usage: motion_random [OPTIONS]\n");
	printf("\n");
	printf("Options:\n");
	printf("  -s --speed SPEED      Sets the forward speed (default: 10000)\n");
	printf("  -d --deviation DEV    Sets the maximum deviation (default: 10000)\n");
	printf("  -i --interval-us US   Sets the interval between speed changes (default: 1000000)\n");
	printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)\n");
	printf("\n");
}

void motion_random_init(struct sMotionRandom *motion_random) {
	motion_random->speed = commandline_option_value_int("-s", "--speed", 10000);
	motion_random->deviation_max = commandline_option_value_float("-d", "--deviation", 10000);
	motion_random->interval_us = commandline_option_value_int("-i", "--interval-us", 1000 * 1000);
}

void motion_random_run(struct sMotionRandom *motion_random) {
	int speed_left, speed_right;
	float direction = 0;

	// Put the wheels in normal (control) mode
	khepera3_drive_start();

	while (1) {
		direction = ((float)rand() / (float)RAND_MAX) * motion_random->deviation_max * 2 - motion_random->deviation_max;
		speed_left = (int)floorf(motion_random->speed + direction + 0.5);
		speed_right = (int)floorf(motion_random->speed - direction + 0.5);
		printf("$SPEED,%d,%d\n", speed_left, speed_right);
		khepera3_drive_set_speed(speed_left, speed_right);
		//khepera3_drive_set_speed(1000, 1000);

		usleep(motion_random->interval_us);
	}
}

// Main program.
int main(int argc, char *argv[]) {
	struct sMotionRandom motion_random;

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
	motion_random_init(&motion_random);

	// Read arguments
	verbosity = commandline_option_value_int("-v", "--verbosity", 1);

	// Move randomly
	motion_random_run(&motion_random);
	return 0;
}

