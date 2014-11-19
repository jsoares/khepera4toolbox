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


#include "khepera4.h"
#include "commandline.h"
#include "measurement.h"
#include <stdio.h>
#include <stdlib.h>

// Buffers

struct sMeasurement {
    int sample_number;
    struct sKhepera4SensorsUltrasound ultrasound;
};

#define LOG_SIZE 1024
struct sMeasurement log_buffer[LOG_SIZE];
unsigned int sample_number = 0;

// Prints the help text.

void help() {
    printf("Measures distances with the ultrasound sensors.\n");
    printf("\n");
    printf("Usage:\n");
    printf("  ultrasound             Measures one set of values\n");
    printf("  ultrasound -r [N]      Measures N sets of values (N=infinity if not provided)\n");
    printf("  ultrasound -r N -f     Measures N sets of values and keeps them in memory before printing (for fast acquisition)\n");
    printf("  ultrasound -r N -F     Measures N sets of values but does not print them\n");
    printf("  ultrasound -w US       Waits US us after each measurement\n");
    printf("\n");
    printf("Output format:\n");
    printf("  $ULTRASOUND,sample,timestamp,value0,...,value10\n");
    printf("\n");
}

// Takes one measurement

void measurement_take(int i) {
    khepera4_ultrasound_p(&(log_buffer[i].ultrasound));
    log_buffer[i].sample_number = sample_number;
    sample_number++;
}

// Prints one measurement

void measurement_print(int i) {
    int s;
    printf("$ULTRASOUND,%d,%u", log_buffer[i].sample_number, (unsigned int) log_buffer[i].ultrasound.timestamp);
    for (s = 0; s < 5; s++) {
        printf(",%d", log_buffer[i].ultrasound.sensor[s]);
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

