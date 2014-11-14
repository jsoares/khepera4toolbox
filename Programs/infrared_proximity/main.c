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
struct sMeasurement {
	int sample_number;
	struct sKhepera3SensorsInfrared infrared_proximity;
};

#define LOG_SIZE 1024
struct sMeasurement log_buffer[LOG_SIZE];
unsigned int sample_number = 0;

// Prints the help text.
void help() {
	printf("Reads the infrared sensor values in proximity mode.\n");
	printf("\n");
	printf("Usage:\n");
	printf("  infrared_proximity             Measures one set of values\n");
	printf("  infrared_proximity -r [N]      Measures N sets of values (N=infinity if not provided)\n");
	printf("  infrared_proximity -r N -f     Measures N sets of values and keeps them in memory before printing (for fast acquisition)\n");
	printf("  infrared_proximity -r N -F     Measures N sets of values but does not print them\n");
	printf("  infrared_proximity -w US       Waits US us after each measurement\n");
	printf("\n");
	printf("Output format:\n");
	printf("  $IRPROXIMITY,sample,timestamp,value0,...,value10\n");
}

// Takes one measurement
void measurement_take(int i) {
	khepera3_infrared_proximity_p(&(log_buffer[i].infrared_proximity));
	log_buffer[i].sample_number = sample_number;
	sample_number++;
}

// Prints one measurement
void measurement_print(int i) {
	int s;
	printf("$IRPROXIMITY,%d,%u", log_buffer[i].sample_number, (unsigned int) log_buffer[i].infrared_proximity.timestamp);
	for (s = 0; s < 12; s++) {
		printf(",%d", log_buffer[i].infrared_proximity.sensor[s]);
	}
	printf("\n");
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

	// Take continuous measurements
	measurement_configuration.log_size = LOG_SIZE;
	measurement_configuration.hook_measure = &measurement_take;
	measurement_configuration.hook_print = &measurement_print;
	measurement_start();

	return 0;
}
