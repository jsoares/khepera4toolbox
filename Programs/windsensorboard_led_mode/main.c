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
#include <string.h>

// Prints the help text.

void help() {
    printf("Sets or reports the LED mode.\n");
    printf("Usage: windsensorboard_led_mode [debug|tracking|tracking_code|external]\n");
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
        const char *mode = commandline_argument(0, "");
        if (strcmp(mode, "debug") == 0) {
            windsensorboard.led.mode = cWindSensorBoardLEDMode_Debug;
        } else if (strcmp(mode, "tracking") == 0) {
            windsensorboard.led.mode = cWindSensorBoardLEDMode_Tracking;
        } else if (strcmp(mode, "tracking_code") == 0) {
            windsensorboard.led.mode = cWindSensorBoardLEDMode_TrackingCode;
        } else if (strcmp(mode, "external") == 0) {
            windsensorboard.led.mode = cWindSensorBoardLEDMode_External;
        } else {
            help();
            exit(1);
        }
        windsensorboard_led_mode_set();
    }

    // Report value
    windsensorboard_led_mode_get();
    if (windsensorboard.led.mode == cWindSensorBoardLEDMode_Debug) {
        printf("$LED_MODE,debug\n");
    } else if (windsensorboard.led.mode == cWindSensorBoardLEDMode_Tracking) {
        printf("$LED_MODE,tracking\n");
    } else if (windsensorboard.led.mode == cWindSensorBoardLEDMode_TrackingCode) {
        printf("$LED_MODE,tracking_code\n");
    } else if (windsensorboard.led.mode == cWindSensorBoardLEDMode_External) {
        printf("$LED_MODE,external\n");
    } else {
        printf("$LED_MODE,unknown\n");
    }

    return 0;
}
