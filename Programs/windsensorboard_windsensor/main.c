/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "windsensorboard.h"
#include "commandline.h"
#include "measurement.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.14159265358979

// Buffers
struct sMeasurement {
	unsigned int sample_number;
	float angle;
	float likelihood;
};

#define LOG_SIZE 1024
struct sMeasurement log_buffer[LOG_SIZE];
unsigned int sample_number = 0;

// Prints the help text.
void help() {
	printf("Returns the last measurement of the 6 NTC thermistors.\n");
	printf("\n");
	printf("Usage:\n");
	printf("  windsensorboard_windsensor             Measures one set of values\n");
	printf("  windsensorboard_windsensor -r [N]      Measures N sets of values (N=infinity if not provided)\n");
	printf("  windsensorboard_windsensor -r N -f     Measures N sets of values and keeps them in memory before printing (for fast acquisition)\n");
	printf("  windsensorboard_windsensor -r N -F     Measures N sets of values but does not print them\n");
	printf("  windsensorboard_windsensor -w US       Waits US us after each measurement\n");
	printf("\n");
	printf("Output format:\n");
	printf("  $NTC,SAMPLE_NUMBER,ANGLE,LIKELIHOOD\n");
	printf("\n");
}

// Takes one measurement
void measurement_take(int i) {
	windsensorboard_windsensor_get();
	log_buffer[i].sample_number = windsensorboard.windsensor.last.sample_number;
	log_buffer[i].angle = windsensorboard.windsensor.last.angle;
	log_buffer[i].likelihood = windsensorboard.windsensor.last.likelihood;
	sample_number++;
}

// Prints one measurement
void measurement_print(int i) {
	printf("$WIND,%d,%f,%f\n",
	       log_buffer[i].sample_number,
	       log_buffer[i].angle/PI*180,
	       log_buffer[i].likelihood
	      );
}

// Main program.
int main(int argc, char *argv[]) {
	// General initialization
	windsensorboard_init();
	measurement_init();

	// Command line parsing
	commandline_init();
	measurement_commandline_prepare();
	commandline_parse(argc, argv);

	// Help
	if (commandline_option_provided("-h", "--help")) {
		help();
		exit(1);
	}

	// Take continuous measures.
	measurement_configuration.log_size = LOG_SIZE;
	measurement_configuration.hook_measure = &measurement_take;
	measurement_configuration.hook_print = &measurement_print;
	measurement_start();

	return 0;
}

