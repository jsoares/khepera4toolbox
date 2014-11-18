/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera4.h"
#include "commandline.h"
#include <termios.h>

// Algorithm parameters and results
struct sAlgorithm {
	struct {
		int speed;
		int floor_threshold_on;
		int floor_threshold_off;
		double real_distance;
		int overshoot_count;
		int wait_us;
	} configuration;
	struct {
		void (*hook)();
		int overshoot_counter;
		struct timeval timestamp1;
		struct timeval timestamp2;
	} state;
};

// An instance of the algorithm structure
struct sAlgorithm algorithm;

// Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
int verbosity;

// Prints the help text.
void help() {
	printf("Allows to measure real speed <=> speed setpoint by driving over a sheet of paper.\r\n");
	printf("Usage: measure_real_speed [OPTIONS]\r\n");
	printf("  --speed SPEED              Sets the speed setpoint (default: 20000)\r\n");
	printf("  --floor-threshold-on TH    Sets the first floor sensor threshold (default: 3900)\r\n");
	printf("  --floor-threshold-off TH   Sets the second floor sensor threshold (default: 3200)\r\n");
	printf("  --real-distance D          Sets real distance in meters (default: 0.297)\r\n");
	printf("  --overshoot-count C        Sets the number of steps to overshoot (default: 10)\r\n");
	printf("  -w --wait-us US            Sets the waiting time in the perception-to-action loop (default: 50000)\r\n");
	printf("  -v --verbosity V           Sets the verbosity level (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)\r\n");
	printf("Output:\r\n");
	printf("  $MEASURE_REAL_SPEED,speed_setpoint,real_speed,real_distance,real_time\r\n");
}

// State functions
void state_measure();
void state_success();

void algorithm_init() {
	algorithm.configuration.speed = commandline_option_value_int("-s", "--speed", 20000);
	algorithm.configuration.floor_threshold_on = commandline_option_value_int("-ft1", "--floor-threshold-on", 3900);
	algorithm.configuration.floor_threshold_off = commandline_option_value_int("-ft2", "--floor-threshold-off", 3200);
	algorithm.configuration.real_distance = commandline_option_value_double("-d", "--real-distance", 0.297);
	algorithm.configuration.overshoot_count = commandline_option_value_int("-oc", "--overshoot-count", 10);
	algorithm.configuration.wait_us = commandline_option_value_int("-w", "--wait-us", 50 * 1000);
}

void algorithm_run() {
	// Put the wheels in normal (control) mode
	khepera4_drive_start();

	// Run the state machine
	algorithm.state.hook = state_measure;
	while (1) {
		algorithm.state.hook();
	}
}

void overshoot() {
	algorithm.state.overshoot_counter = 0;
	while (1) {
		// Count the number of steps
		algorithm.state.overshoot_counter++;
		if (algorithm.state.overshoot_counter > algorithm.configuration.overshoot_count) {
			break;
		}

		// Wait
		usleep(algorithm.configuration.wait_us);
	}
}

void state_measure() {
	int floorstate, floorstate_prev;
	double real_time, real_speed;

	// Move forwards
	khepera4_drive_set_speed(algorithm.configuration.speed, algorithm.configuration.speed);

	// Initial measurement
	khepera4_infrared_proximity();
	floorstate_prev = (khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorLeft] < algorithm.configuration.floor_threshold_on);
	if (verbosity>1) {
		printf("$FLOOR,%d\n", khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorLeft]);
	}

	// Wait for the first transition
	while (1) {
		khepera4_infrared_proximity();
		floorstate = (khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorLeft] < algorithm.configuration.floor_threshold_on);
		if (verbosity>1) {
			printf("$FLOOR,%d\n", khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorLeft]);
		}
		if (floorstate_prev!=floorstate) {
			floorstate_prev = (khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorLeft] < algorithm.configuration.floor_threshold_off);
			gettimeofday(&algorithm.state.timestamp1, 0);
			break;
		}
	}

	// Wait 100 ms to make sure that we crossed the border
	usleep(100*1000);

	// Wait for the second transition
	while (1) {
		khepera4_infrared_proximity();
		floorstate = (khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorLeft] < algorithm.configuration.floor_threshold_off);
		if (verbosity>1) {
			printf("$FLOOR,%d\n", khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorLeft]);
		}
		if (floorstate_prev!=floorstate) {
			gettimeofday(&algorithm.state.timestamp2, 0);
			break;
		}
	}

	// Print the result
	real_time = (double)(algorithm.state.timestamp2.tv_sec - algorithm.state.timestamp1.tv_sec) + (double)(algorithm.state.timestamp2.tv_usec - algorithm.state.timestamp1.tv_usec) / 1000000;
	real_speed = algorithm.configuration.real_distance / real_time;
	printf("$CALIBRATION_SPEED,%d,%f,%f,%f\n", algorithm.configuration.speed, real_speed, algorithm.configuration.real_distance, real_time);

	// Continue with crosswind movement
	overshoot();
	algorithm.state.hook = state_success;
}

void state_success() {
	// Stop the motors
	khepera4_drive_stop();
	exit(0);
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
	khepera4_init();
	algorithm_init();

	// Read command line arguments
	verbosity = commandline_option_value_int("-v", "--verbosity", 1);

	// Follow the line until the plume has been found
	algorithm_run();
	return 0;
}
