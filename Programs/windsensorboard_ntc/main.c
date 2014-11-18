/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "windsensorboard.h"
#include "commandline.h"
#include "measurement.h"
#include <stdio.h>
#include <stdlib.h>

// Buffers

struct sMeasurement {
    unsigned int sample_number;
    unsigned int sensor[6];
};

#define LOG_SIZE 1024
struct sMeasurement log_buffer[LOG_SIZE];
unsigned int sample_number = 0;

// Prints the help text.

void help() {
    printf("Returns the last measurement of the 6 NTC thermistors.\n");
    printf("\n");
    printf("Usage:\n");
    printf("  windsensorboard_ntc             Measures one set of values\n");
    printf("  windsensorboard_ntc -r [N]      Measures N sets of values (N=infinity if not provided)\n");
    printf("  windsensorboard_ntc -r N -f     Measures N sets of values and keeps them in memory before printing (for fast acquisition)\n");
    printf("  windsensorboard_ntc -r N -F     Measures N sets of values but does not print them\n");
    printf("  windsensorboard_ntc -w US       Waits US us after each measurement\n");
    printf("\n");
    printf("Output format:\n");
    printf("  $NTC,SAMPLE_NUMBER,NTC1,NTC2,NTC3,NTC4,NTC5,NTC6\n");
    printf("\n");
}

// Takes one measurement

void measurement_take(int i) {
    int s;

    windsensorboard_ntc_get();
    for (s = 0; s < 6; s++) {
        log_buffer[i].sensor[s] = windsensorboard.ntc.last.sensor[s];
    }
    log_buffer[i].sample_number = windsensorboard.ntc.last.sample_number;
}

// Prints one measurement

void measurement_print(int i) {
    printf("$NTC,%d,%d,%d,%d,%d,%d,%d\n",
            log_buffer[i].sample_number,
            log_buffer[i].sensor[0],
            log_buffer[i].sensor[1],
            log_buffer[i].sensor[2],
            log_buffer[i].sensor[3],
            log_buffer[i].sensor[4],
            log_buffer[i].sensor[5]
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

