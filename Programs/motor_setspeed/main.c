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
#include <stdio.h>
#include <stdlib.h>

// Prints the help text.

void help() {
    printf("Sets the motor speed.\r\n");
    printf("Usage:\r\n");
    printf("  motor_setspeed [OPTIONS] SPEED          Sets the speed of both motors\r\n");
    printf("  motor_setspeed [OPTIONS] SPEED1 SPEED2  Sets an individual speed for both motors\r\n");
    printf("Options:\r\n");
    printf("  -p     Uses a trapezoidal speed profile\r\n");
    exit(1);
}

// Main program.

int main(int argc, char *argv[]) {
    // Command line parsing
    commandline_init();
    commandline_option_register("-p", "--profile", cCommandLine_Option);
    commandline_parse(argc, argv);

    // Help
    if (commandline_option_provided("-h", "--help")) {
        help();
        exit(1);
    }

    // Initialization
    khepera4_init();

    // Put the wheels in normal (control) mode
    khepera4_drive_start();

    // Set the speed
    if (commandline_argument_count() == 1) {
        if (commandline_option_provided("-p", "--profile")) {
            khepera4_drive_set_speed_using_profile(commandline_argument_int(0, 0), commandline_argument_int(0, 0));
        } else {
            khepera4_drive_set_speed(commandline_argument_int(0, 0), commandline_argument_int(0, 0));
        }
    } else if (commandline_argument_count() == 2) {
        if (commandline_option_provided("-p", "--profile")) {
            khepera4_drive_set_speed_using_profile(commandline_argument_int(0, 0), commandline_argument_int(1, 0));
        } else {
            khepera4_drive_set_speed(commandline_argument_int(0, 0), commandline_argument_int(1, 0));
        }
    }

    return 0;
}
