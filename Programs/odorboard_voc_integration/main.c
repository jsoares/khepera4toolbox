/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include "odorboard.h"
#include <stdio.h>
#include <math.h>
#define PI 3.14159265358979

// Prints the help text.

void help() {
    printf("Sets or reports the integration value of the .\n");
    printf("Usage: odorboard_voc_integration [OPTIONS] [INTEGRATION]\n");
    printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=default, 2=verbose, 3=very verbose, ...)\n");
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

    // Set integration value
    if (commandline_argument_count() > 0) {
        odorboard_voc_integration_set(commandline_argument_int(0, 10));
    }

    // Report integration value
    odorboard_voc_integration_get();
    printf("$VOC_INTEGRATION,%d\n", odorboard.voc.integration);
    return 0;
}
