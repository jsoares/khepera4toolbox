/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera3.h"
#include "commandline.h"
#include "odometry_track.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// The waiting time in the main loop
int wait_us;
// The odometry track object
struct sOdometryTrack ot;
// Variables for time stamping
struct timeval start_time,now_time;

// Prints the help text.
void help() {
	printf("Tracks the robot position using odometry information.\n");
	printf("\n");
	printf("Usage: odometry_track [OPTIONS]\n");
	printf("\n");
	printf("Options:\n");
	printf("  -w --wait-us US     Waits US us the main loop (default: 0 ms, max speed)\n");
	printf("\n");
}

// Read the command line arguments
void run_initialize() {
	wait_us = commandline_option_value_int("-w", "--wait-us", 0);
}

int run() {
	int sample_number;
	long time_difference_us;

	// Get the start time
	gettimeofday(&start_time, NULL);

	// Start tracking
	odometry_track_start(&ot);
	if (ot.configuration.is_default) {
		printf("WARNING: Odometry configuration file (/etc/khepera/odometry) not found. Using default values.\n");
	}

	// Continuously read motor positions
	sample_number = 0;
	while (1) {
		odometry_track_step(&ot);

		// Get the current time
		gettimeofday(&now_time, NULL);
		time_difference_us = (long)(now_time.tv_sec-start_time.tv_sec)*1000000L+(long)(now_time.tv_usec-start_time.tv_usec);
		
		printf("$POSITION,%d,%f,%f,%f,%ld\n", sample_number, ot.result.x, ot.result.y, ot.result.theta, time_difference_us);
		printf("$MOTOR_POSITION,%d,%d,%d,%ld\n", sample_number, ot.state.pos_left_prev, ot.state.pos_right_prev, time_difference_us);
		sample_number++;
		fflush(stdout);

		if (wait_us) {
			usleep(wait_us);
		}
	}

	return 0;
}

// Main program.
int main(int argc, char *argv[]) {
	// Command line parsing
	commandline_init();
	commandline_option_register("-w", "--wait-us", cCommandLine_Option_Value);
	commandline_parse(argc, argv);

	// Help
	if (commandline_option_provided("-h", "--help")) {
		help();
		exit(1);
	}

	// Module initialization
	khepera3_init();
	odometry_track_init();

	// Read command line arguments and run
	run_initialize();
	run();
	return 0;
}
