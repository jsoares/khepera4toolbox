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
#include "windsensorboard.h"
#include <stdio.h>
#include <stdlib.h>

// Prints the help text.

void help() {
    printf("Sets or reports the the LED code used for blinking code tracking.\n");
    printf("Usage: windsensorboard_led_mode LENGTH ID CHIP_LENGTH\n");
    printf("  LENGTH         The length of the code in number of chips.\n");
    printf("  ID             The ID (bitmask of length LENGTH).\n");
    printf("  CHIP_LENGTH    The length of one chip [ms].\n");
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

    // Initialization
    windsensorboard_init();

    // Set code
    if (commandline_argument_count() > 2) {
        windsensorboard.led.code.length = commandline_argument_int(0, 0);
        windsensorboard.led.code.id = commandline_argument_int(1, 0);
        windsensorboard.led.code.chip_length = commandline_argument_int(2, 0);
        windsensorboard_led_code_set();
    }

    // Report code
    windsensorboard_led_mode_get();
    printf("$LED_CODE,%d,%d,%d\n", windsensorboard.led.code.length, windsensorboard.led.code.id, windsensorboard.led.code.chip_length);
    return 0;
}
