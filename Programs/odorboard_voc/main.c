/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "odorboard.h"
#include "commandline.h"
#include "measurement.h"
#include <stdio.h>
#include <stdlib.h>

// Buffers

struct sMeasurement {
    int sample_number;
    unsigned int sensor[3];
};

#define LOG_SIZE 1024
struct sMeasurement log_buffer[LOG_SIZE];
unsigned int sample_number = 0;

// Prints the help text.

void help() {
    printf("Returns the last concentration values measured by the three odor sensors.\n");
    printf("\n");
    printf("Usage:\n");
    printf("  odorboard_voc             Measures one set of values\n");
    printf("  odorboard_voc -r [N]      Measures N sets of values (N=infinity if not provided)\n");
    printf("  odorboard_voc -r N -f     Measures N sets of values and keeps them in memory before printing (for fast acquisition)\n");
    printf("  odorboard_voc -r N -F     Measures N sets of values but does not print them\n");
    printf("  odorboard_voc -w US       Waits US us after each measurement\n");
    printf("\n");
    printf("Output format:\n");
    printf("  $VOC,SAMPLE_NUMBER,CONCENTRATION1,CONCENTRATION2,CONCENTRATION3\n");
    printf("\n");
}

// Takes one measurement

void measurement_take(int i) {
    int s;

    odorboard_voc_get();
    for (s = 0; s < 3; s++) {
        log_buffer[i].sensor[s] = odorboard.voc.last.sensor[s];
    }
    log_buffer[i].sample_number = odorboard.voc.last.sample_number;
}

// Prints one measurement

void measurement_print(int i) {
    printf("$VOC,%d,%d,%d,%d\n",
            log_buffer[i].sample_number,
            log_buffer[i].sensor[0],
            log_buffer[i].sensor[1],
            log_buffer[i].sensor[2]
            );
}

// Main program.

int main(int argc, char *argv[]) {
    // General initialization
    odorboard_init();
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

    // Take continuous measurements
    measurement_configuration.log_size = LOG_SIZE;
    measurement_configuration.hook_measure = &measurement_take;
    measurement_configuration.hook_print = &measurement_print;
    measurement_start();

    return 0;
}

