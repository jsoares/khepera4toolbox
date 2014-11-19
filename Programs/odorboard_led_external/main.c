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
#include "odorboard.h"
#include <stdio.h>
#include <stdlib.h>

// Prints the help text.

void help() {
    printf("Sets or reports the LED values which are used if the wind sensor board is in 'external' LED mode. Only the 4 LSBs of VALUE are used.\n");
    printf("Usage: odorboard_led_external VALUE\n");
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
    odorboard_init();

    // Set value
    if (commandline_argument_count() > 0) {
        odorboard.led.external = commandline_argument_int(0, 0);
        odorboard_led_external_set();
    }

    // Report integration value
    odorboard_led_external_get();
    printf("$LED_EXTERNAL,%d\n", odorboard.led.external);
    return 0;
}
