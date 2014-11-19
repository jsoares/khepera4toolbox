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
#include <unistd.h>

// Algorithm parameters and results

struct sFollowLine {
    int speed;
    int aggressivity;
    long int wait_us;
};

// Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
int verbosity;

// Prints the help text.

void help() {
    printf("Follows a line on the ground with a simple Braitenberg algorithm.\n");
    printf("\n");
    printf("Usage: motion_followline [OPTIONS]\n");
    printf("\n");
    printf("Options:\n");
    printf("  --speed SPEED         Sets the forward speed (default: 10000)\n");
    printf("  --aggressivity AGG    Sets the aggressivity (default: 4)\n");
    printf("  -w --wait-us US       Sets the waiting time in the perception-to-action loop (default: 50000)\n");
    printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)\n");
    printf("  Positive aggressivity will follow black/dark lines.\n");
    printf("  Negative aggressivity will follow white/bright lines.\n");
    printf("\n");
    printf("Output:\n");
    printf("  $IRFLOOR,sensor_left,sensor_right\n");
    printf("  $SPEED,speed_left,speed_right\n");
    printf("\n");
}

void followline_init(struct sFollowLine *followline) {
    followline->speed = commandline_option_value_int("-s", "--speed", 10000);
    followline->aggressivity = commandline_option_value_int("-a", "--aggressivity", 4);
    followline->wait_us = commandline_option_value_int("-w", "--wait-us", 50 * 1000);

}

// Follows a line.

void followline_run(struct sFollowLine *followline) {
    int speeddiff = 0;

    // Put the wheels in normal (control) mode
    khepera4_drive_start();

    while (1) {
        // Get new IR proximity values
        khepera4_infrared_proximity();

        // Calculate the speed difference
        speeddiff = 0;
        speeddiff += khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorLeft] * followline->aggressivity;
        speeddiff -= khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorRight] * followline->aggressivity;

        // Set the speed
        khepera4_drive_set_speed(followline->speed - speeddiff, followline->speed + speeddiff);

        // Print values
        if (verbosity > 1) {
            printf("$IRFLOOR,%d,%d\n",
                    khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorLeft],
                    khepera4.infrared_proximity.sensor[cKhepera4SensorsInfrared_FloorRight]
                    );
        }
        if (verbosity > 0) {
            printf("$SPEED,%d,%d\n", followline->speed - speeddiff, followline->speed + speeddiff);
        }

        // Wait
        usleep(followline->wait_us);
    }
}

// Main program.

int main(int argc, char *argv[]) {
    struct sFollowLine followline;

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
    followline_init(&followline);

    // Read command line arguments
    verbosity = commandline_option_value_int("-v", "--verbosity", 1);

    // Follow the line
    followline_run(&followline);
    return 0;
}
