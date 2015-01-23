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


#include "commandline.h"
#include "i2cal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Prints the help text.

void help() {
    printf("Enables or disables the LED mode.\n");
    printf("Usage: rndboard_led_mode [on|off]\n");
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

    int led_intensity = 0;

    // Set value
    if (commandline_argument_count() > 0) {
        const char *mode = commandline_argument(0, "");
        if (strcmp(mode, "on") == 0) {
            led_intensity = 100;
        } else if (strcmp(mode, "off") == 0) {
            led_intensity = 0;
        } else {
            help();
            exit(1);
        }

        i2cal_init();
        i2cal_start();
        i2cal_writedata_uint8(0x081);
        i2cal_writedata_uint8(led_intensity);
        i2cal_writedata_uint8(led_intensity);
        i2cal_write(10);
        i2cal_commit();
    }

    return 0;
}
