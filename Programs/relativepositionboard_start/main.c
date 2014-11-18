/*!
 * (c) 2009 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera4.h"
#include "commandline.h"
#include "relativepositionboard.h"
#include <stdlib.h>
#include <stdio.h>

// Prints the help text.

void help() {
    printf("Starts emitting and receiving on the relative positioning board.\n");
    printf("Usage: relativepositionboard_start [OPTIONS]\n");
    printf("  -d --data              Sets the data to transmit (default: /etc/khepera/robotid minus 200)\n");
    printf("  -i --tx-interval       Sets the transmission interval [ms] (default: left unchanged)\n");
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
    relativepositionboard_init();

    // Set the send interval
    if (commandline_option_provided("-i", "--tx-interval")) {
        relativepositionboard.configuration.tx_interval = commandline_option_value_int("-i", "--tx-interval", 500);
        relativepositionboard_configuration_tx_interval_set();
    }

    // Determine the ID
    int data = khepera4.robot_id - 100;
    if (commandline_option_provided("-d", "--data")) {
        data = commandline_option_value_int("-d", "--data", data);
    }

    // Start
    relativepositionboard_start(data);
    return 0;
}
