/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include "windsensorboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.14159265358979

// Prints the help text.

void help() {
    printf("Sets or reports the baseline for normalizing the NTC values.\n");
    printf("Usage: windsensorboard_ntc_normalized_baseline [OPTIONS] [BASELINE]\n");
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
    windsensorboard_init();

    // Set integration value
    if (commandline_argument_count() > 0) {
        windsensorboard.ntc_normalized.baseline = commandline_argument_int(0, 10);
        windsensorboard_ntc_normalized_baseline_set();
    }

    // Report integration value
    windsensorboard_ntc_normalized_baseline_get();
    printf("$NTC_NORMALIZED_BASELINE,%d\n", windsensorboard.ntc_normalized.baseline);
    return 0;
}
