/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera4.h"
#include "commandline.h"
#include <stdio.h>
#include <stdlib.h>

// Prints the help text.

void help() {
    printf("Resets the motor encoder values.\n");
    printf("\n");
    printf("Usage:\n");
    printf("  motor_resetposition        Resets both positions to 0\n");
    printf("\n");
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
    khepera4_init();

    // Put the wheels in stop mode
    khepera4_drive_stop();

    // Set the position
    khepera4_drive_reset_position();

    return 0;
}
