/******************************************************************************
 *
 * Khepera IV Toolbox
 * (c) 2014 EPFL, Lausanne, Switzerland
 * Authors: Jorge Soares et al.
 *
 * Code forked from Khepera III Toolbox
 * (c) 2006 EPFL, Lausanne, Switzerland
 * Authors: Thomas Lochmatter et al.
 *
 * Additional DISAL libraries
 * (c) 2006 EPFL, Lausanne, Switzerland
 * Authors: Thomas Lochmatter, Jim Pugh, Sven Gowal, Pascal Gilbert, and others
 *
 ******************************************************************************/


#include "windsensorboard.h"
#include "commandline.h"
#include "measurement.h"
#include <stdio.h>
#include <stdlib.h>

// Buffers

struct sMeasurement {
    unsigned int sample_number;
    unsigned int sensor[6];
    unsigned int sum;
};

#define LOG_SIZE 1024
struct sMeasurement log_buffer[LOG_SIZE];
unsigned int sample_number = 0;

// Prints the help text.

void help() {
    printf("Returns the last normalized measurement of the 6 NTC thermistors.\n");
    printf("\n");
    printf("Usage:\n");
    printf("  windsensorboard_ntc_normalized             Measures one set of values\n");
    printf("  windsensorboard_ntc_normalized -r [N]      Measures N sets of values (N=infinity if not provided)\n");
    printf("  windsensorboard_ntc_normalized -r N -f     Measures N sets of values and keeps them in memory before printing (for fast acquisition)\n");
    printf("  windsensorboard_ntc_normalized -r N -F     Measures N sets of values but does not print them\n");
    printf("  windsensorboard_ntc_normalized -w US       Waits US us after each measurement\n");
    printf("\n");
    printf("Output format:\n");
    printf("  $NTC_NORMALIZED,SAMPLE_NUMBER,NTC1,NTC2,NTC3,NTC4,NTC5,NTC6,SUM\n");
    printf("  NTC*: 0.16 bit values\n");
    printf("  SUM: 3.13 bit value\n");
    printf("\n");
}

// Takes one measurement

void measurement_take(int i) {
    int s;

    windsensorboard_ntc_normalized_get();
    for (s = 0; s < 6; s++) {
        log_buffer[i].sensor[s] = windsensorboard.ntc_normalized.last.sensor[s];
    }
    log_buffer[i].sample_number = windsensorboard.ntc_normalized.last.sample_number;
    log_buffer[i].sum = windsensorboard.ntc_normalized.last.sum;
}

// Prints one measurement

void measurement_print(int i) {
    printf("$NTC_NORMALIZED,%d,%d,%d,%d,%d,%d,%d,%d\n",
            log_buffer[i].sample_number,
            log_buffer[i].sensor[0],
            log_buffer[i].sensor[1],
            log_buffer[i].sensor[2],
            log_buffer[i].sensor[3],
            log_buffer[i].sensor[4],
            log_buffer[i].sensor[5],
            log_buffer[i].sum
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

