/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera4.h"
#include "commandline.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Algorithm parameters and results

struct sForwardBackward {
    int speed;
    long int move_us;
    long int stop_us;
};

// Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
int verbosity;

// Prints the help text.

void help() {
    printf("Alternately moves forwards and backwards.\n");
    printf("\n");
    printf("Usage: motion_forwardbackward [OPTIONS]\n");
    printf("\n");
    printf("Options:\n");
    printf("  --speed SPEED         Sets the speed (default: 10000)\n");
    printf("  --move-us US          Sets the time the robot moves back or forth (default: 2000000)\n");
    printf("  --stop-us US          Sets the time the robot waits when changing direction (default: 0)\n");
    printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)\n");
    printf("\n");
    printf("Output:\n");
    printf("  $DIRECTION,[FORWARD|BACKWARD|STOP]\n");
    printf("\n");
}

void forwardbackward_init(struct sForwardBackward *forwardbackward) {
    forwardbackward->speed = commandline_option_value_int("-s", "--speed", 10000);
    forwardbackward->move_us = commandline_option_value_int("-m", "--move-us", 2000 * 1000);
    forwardbackward->stop_us = commandline_option_value_int("-p", "--stop-us", 0);
}

// Follows a line.

void forwardbackward_run(struct sForwardBackward *forwardbackward) {
    // Put the wheels in normal (control) mode
    khepera4_drive_start();

    while (1) {
        // Move forward
        if (verbosity > 0) {
            printf("$DIRECTION,FORWARD\n");
        }
        khepera4_drive_set_speed(forwardbackward->speed, forwardbackward->speed);
        usleep(forwardbackward->move_us);

        // Stop
        if (forwardbackward->stop_us > 0) {
            if (verbosity > 0) {
                printf("$DIRECTION,STOP\n");
            }
            khepera4_drive_set_speed(0, 0);
            usleep(forwardbackward->stop_us);
        }

        // Move backward
        if (verbosity > 0) {
            printf("$DIRECTION,BACKWARD\n");
        }
        khepera4_drive_set_speed(-forwardbackward->speed, -forwardbackward->speed);
        usleep(forwardbackward->move_us);

        // Stop
        if (forwardbackward->stop_us > 0) {
            if (verbosity > 0) {
                printf("$DIRECTION,STOP\n");
            }
            khepera4_drive_set_speed(0, 0);
            usleep(forwardbackward->stop_us);
        }
    }
}

// Main program.

int main(int argc, char *argv[]) {
    struct sForwardBackward forwardbackward;

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
    forwardbackward_init(&forwardbackward);

    // Read command line arguments
    verbosity = commandline_option_value_int("-v", "--verbosity", 1);

    // Run the algorithm
    forwardbackward_run(&forwardbackward);
    return 0;
}
