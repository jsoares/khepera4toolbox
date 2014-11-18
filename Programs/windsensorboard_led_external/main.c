/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include "windsensorboard.h"
#include <stdio.h>
#include <stdlib.h>

// Prints the help text.

void help() {
    printf("Sets or reports the LED values which are used if the wind sensor board is in 'external' LED mode. Only the 4 LSBs of VALUE are used.\n");
    printf("Usage: windsensorboard_led_external VALUE\n");
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

    // Set value
    if (commandline_argument_count() > 0) {
        windsensorboard.led.external = commandline_argument_int(0, 0);
        windsensorboard_led_external_set();
    }

    // Report value
    windsensorboard_led_external_get();
    printf("$LED_EXTERNAL,%d\n", windsensorboard.led.external);
    return 0;
}
