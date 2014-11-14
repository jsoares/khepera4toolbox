/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera3.h"
#include "commandline.h"
#include "measurement.h"
#include <stdio.h>
#include <stdlib.h>

// Buffers
struct sMeasurementMotor {
	int current_speed;
};

struct sMeasurement {
	int sample_number;
	struct sMeasurementMotor motor_left;
	struct sMeasurementMotor motor_right;
};

#define LOG_SIZE 1024
struct sMeasurement log_buffer[LOG_SIZE];
unsigned int sample_number = 0;

// Prints the help text.
void help() {
	printf("Returns the current effective speed of both motors. This may be different from the speed setpoint.\n");
	printf("\n");
	printf("Usage:\n");
	printf("  motor_getspeed             Measures one set of values\n");
	printf("  motor_getspeed -r [N]      Measures N sets of values (N=infinity if not provided)\n");
	printf("  motor_getspeed -r N -f     Measures N sets of values and keeps them in memory before printing (for fast acquisition)\n");
	printf("  motor_getspeed -r N -F     Measures N sets of values but does not print them\n");
	printf("  motor_getspeed -w USEC     Waits USEC usec after each measurement\n");
	printf("\n");
	printf("Output format:\n");
	printf("  $MOTOR_SPEED,sample,speed_left,speed_right\n");
	printf("\n");
}

// Takes one measurement
void measurement_take(int i) {
	log_buffer[i].sample_number = sample_number;
	khepera3_drive_get_current_speed();
	log_buffer[i].motor_left.current_speed = khepera3.motor_left.current_speed;
	log_buffer[i].motor_right.current_speed = khepera3.motor_right.current_speed;
	sample_number++;
}

// Prints one measurement
void measurement_print(int i) {
	printf("$MOTOR_SPEED,%d,%d,%d\n", log_buffer[i].sample_number, log_buffer[i].motor_left.current_speed, log_buffer[i].motor_right.current_speed);
}

// Main program.
int main(int argc, char *argv[]) {
	// Command line parsing
	commandline_init();
	measurement_init();
	measurement_commandline_prepare();
	commandline_parse(argc, argv);

	// Help
	if (commandline_option_provided("-h", "--help")) {
		help();
		exit(1);
	}

	// Initialization
	khepera3_init();

	// Take continuous measures
	measurement_configuration.log_size = LOG_SIZE;
	measurement_configuration.hook_measure = &measurement_take;
	measurement_configuration.hook_print = &measurement_print;
	measurement_start();

	return 0;
}
