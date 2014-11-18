/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera4.h"
#include "commandline.h"
#include "measurement.h"
#include <stdio.h>
#include <stdlib.h>

// Buffers

struct sMeasurement {
    int sample_number;
    struct sKhepera4SensorsInfrared infrared_ambient;
};

#define LOG_SIZE 1024
struct sMeasurement log_buffer[LOG_SIZE];
unsigned int sample_number = 0;

// Prints the help text.

void help() {
    printf("Measures the ambient infrared light with the infrared sensors.\n");
    printf("\n");
    printf("Usage:\n");
    printf("  infrared_ambient             Measures one set of values\n");
    printf("  infrared_ambient -r [N]      Measures N sets of values (N=infinity if not provided)\n");
    printf("  infrared_ambient -r N -f     Measures N sets of values and keeps them in memory before printing (for fast acquisition)\n");
    printf("  infrared_ambient -r N -F     Measures N sets of values but does not print them\n");
    printf("  infrared_ambient -w US       Waits US us after each measurement\n");
    printf("\n");
    printf("Output format:\n");
    printf("  $IRAMBIENT,sample,timestamp,value0,...,value10\n");
    printf("\n");
}

// Takes one measurement

void measurement_take(int i) {
    khepera4_infrared_ambient_p(&(log_buffer[i].infrared_ambient));
    log_buffer[i].sample_number = sample_number;
    sample_number++;
}

// Prints one measurement

void measurement_print(int i) {
    int s;
    printf("$IRAMBIENT,%d,%u", log_buffer[i].sample_number, (unsigned int) log_buffer[i].infrared_ambient.timestamp);
    for (s = 0; s < 12; s++) {
        printf(",%d", log_buffer[i].infrared_ambient.sensor[s]);
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
    khepera4_init();

    // Take continuous measurements
    measurement_configuration.log_size = LOG_SIZE;
    measurement_configuration.hook_measure = &measurement_take;
    measurement_configuration.hook_print = &measurement_print;
    measurement_start();

    return 0;
}

