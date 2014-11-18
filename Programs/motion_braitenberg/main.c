/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera4.h"
#include "commandline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#define MAXBUFFERSIZE 100
#define PI 3.14159265358979

// Algorithm parameters and results

struct sBraitenberg {

    struct {
        int left;
        int right;
    } speed;
    int repeat;
    int wait_us;
    int weight_left[12];
    int weight_right[12];
    int bias[12];
};

// Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
int verbosity;

// Prints the help text.

void help() {
    printf("Runs a Braitenberg obstacle avoidance algorithm.\n");
    printf("\n");
    printf("Usage: motion_braitenberg [OPTIONS] [ALGORITHM]\n");
    printf("\n");
    printf("Options:\n");
    printf("  --speed SPEED         Sets the forward speed (default: 10000)\n");
    printf("  --speed-left SPEED    Sets the left wheel forward speed (default: --speed)\n");
    printf("  --speed-right SPEED   Sets the right wheel forward speed (default: --speed)\n");
    printf("  -r --repeat N         Sets the execution time in number of steps (default: infinity)\n");
    printf("  -w --wait-us US       Sets the waiting time in the perception-to-action loop (default: 5000)\n");
    printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)\n");
    printf("\n");
    printf("Algorithms:\n");
    printf("  obstacle_avoidance (default)\n");
    printf("  wall_following_left\n");
    printf("  wall_following_right\n");
    printf("  object_following\n");
    printf("  line_following_white\n");
    printf("  line_following_black\n");
    printf("\n");
}

void braitenberg_set_obstacle_avoidance(struct sBraitenberg *braitenberg, int aggressivity) {
    braitenberg->weight_left[cKhepera4SensorsInfrared_BackLeft] = -1 * aggressivity;
    braitenberg->weight_left[cKhepera4SensorsInfrared_Left] = 1 * aggressivity;
    braitenberg->weight_left[cKhepera4SensorsInfrared_FrontLeft] = 3 * aggressivity;
    braitenberg->weight_left[cKhepera4SensorsInfrared_Front] = 0;
    braitenberg->weight_left[cKhepera4SensorsInfrared_FrontRight] = -3 * aggressivity;
    braitenberg->weight_left[cKhepera4SensorsInfrared_Right] = -1 * aggressivity;
    braitenberg->weight_left[cKhepera4SensorsInfrared_BackRight] = 1 * aggressivity;
    braitenberg->weight_left[cKhepera4SensorsInfrared_Back] = 0;
    braitenberg->weight_left[cKhepera4SensorsInfrared_FloorLeft] = 0;
    braitenberg->weight_left[cKhepera4SensorsInfrared_FloorFrontLeft] = 0;
    braitenberg->weight_left[cKhepera4SensorsInfrared_FloorFrontRight] = 0;
    braitenberg->weight_left[cKhepera4SensorsInfrared_FloorRight] = 0;

    int i;
    for (i = 0; i < 12; i++) {
        braitenberg->weight_right[i] = -braitenberg->weight_left[i];
        braitenberg->bias[i] = 0;
    }
}

void braitenberg_set_wall_following_left(struct sBraitenberg *braitenberg) {
    braitenberg_set_obstacle_avoidance(braitenberg, 5);
    braitenberg->bias[cKhepera4SensorsInfrared_BackLeft] = 0;
    braitenberg->bias[cKhepera4SensorsInfrared_Left] = -1500;
    braitenberg->bias[cKhepera4SensorsInfrared_FrontLeft] = 0;
}

void braitenberg_set_wall_following_right(struct sBraitenberg *braitenberg) {
    braitenberg_set_obstacle_avoidance(braitenberg, 5);
    braitenberg->bias[cKhepera4SensorsInfrared_BackRight] = 0;
    braitenberg->bias[cKhepera4SensorsInfrared_Right] = -1500;
    braitenberg->bias[cKhepera4SensorsInfrared_FrontRight] = 0;
}

void braitenberg_set_object_following(struct sBraitenberg *braitenberg) {
    int i;

    braitenberg_set_obstacle_avoidance(braitenberg, 5);
    for (i = 0; i < 8; i++) {
        braitenberg->weight_left[i] = -braitenberg->weight_left[i] - 10;
        braitenberg->weight_right[i] = -braitenberg->weight_right[i] - 10;
    }
}

void braitenberg_set_line_following_white(struct sBraitenberg *braitenberg) {
    braitenberg_set_obstacle_avoidance(braitenberg, 5);
    braitenberg->weight_left[cKhepera4SensorsInfrared_FloorFrontLeft] = -10;
    braitenberg->weight_left[cKhepera4SensorsInfrared_FloorFrontRight] = 10;
    braitenberg->weight_right[cKhepera4SensorsInfrared_FloorFrontLeft] = 10;
    braitenberg->weight_right[cKhepera4SensorsInfrared_FloorFrontRight] = -10;
}

void braitenberg_set_line_following_black(struct sBraitenberg *braitenberg) {
    braitenberg_set_obstacle_avoidance(braitenberg, 5);
    braitenberg->weight_left[cKhepera4SensorsInfrared_FloorFrontLeft] = 10;
    braitenberg->weight_left[cKhepera4SensorsInfrared_FloorFrontRight] = -10;
    braitenberg->weight_right[cKhepera4SensorsInfrared_FloorFrontLeft] = -10;
    braitenberg->weight_right[cKhepera4SensorsInfrared_FloorFrontRight] = 10;
}

void braitenberg_init(struct sBraitenberg *braitenberg) {
    braitenberg_set_obstacle_avoidance(braitenberg, 5);
    int speed = commandline_option_value_int("-s", "--speed", 10000);
    braitenberg->speed.left = commandline_option_value_int("-sl", "--speed-left", speed);
    braitenberg->speed.right = commandline_option_value_int("-sr", "--speed-right", speed);
    braitenberg->repeat = commandline_option_value_int("-r", "--repeat", 0);
    braitenberg->wait_us = commandline_option_value_int("-w", "--wait-us", 5000);
}

void braitenberg_run(struct sBraitenberg *braitenberg) {
    int i;
    int speed_left, speed_right;

    // Put the wheels in normal (control) mode
    khepera4_drive_start();

    while (1) {
        // Take the next measurement
        khepera4_infrared_proximity();

        // Sum all sensor values with the Braitenberg bias and weights
        speed_left = 0;
        speed_right = 0;
        for (i = 0; i < 12; i++) {
            speed_left += (khepera4.infrared_proximity.sensor[i] + braitenberg->bias[i]) * braitenberg->weight_left[i];
            speed_right += (khepera4.infrared_proximity.sensor[i] + braitenberg->bias[i]) * braitenberg->weight_right[i];
        }

        // Shift to be in the right order of magnitude
        speed_left = speed_left / 10;
        speed_right = speed_right / 10;

        // Add the bias speed
        speed_left += braitenberg->speed.left;
        speed_right += braitenberg->speed.right;

        // Set that speed
        khepera4_drive_set_speed(speed_left, speed_right);

        // Print values if necessary
        if (verbosity > 1) {
            printf("$IRPROXIMITY");
            for (i = 0; i < 12; i++) {
                printf(",%d", khepera4.infrared_proximity.sensor[i]);
            }
            printf(",%d\n", (unsigned int) khepera4.infrared_proximity.timestamp);
        }
        if (verbosity > 1) {
            printf("$SPEED,%d,%d\n", speed_left, speed_right);
        }

        // Quit if desired
        if (braitenberg->repeat > 0) {
            braitenberg->repeat--;
            if (braitenberg->repeat == 0) {
                break;
            }
        }

        // Sleep
        usleep(braitenberg->wait_us);
    }

    khepera4_drive_set_speed(0, 0);
}

// Main program.

int main(int argc, char *argv[]) {
    struct sBraitenberg braitenberg;
    const char *algorithm = "obstacle_avoidance";

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
    braitenberg_init(&braitenberg);

    // Read arguments
    verbosity = commandline_option_value_int("-v", "--verbosity", 1);

    // Select the algorithm
    algorithm = commandline_argument(0, algorithm);
    if (strcmp(algorithm, "obstacle_avoidance") == 0) {
        braitenberg_set_obstacle_avoidance(&braitenberg, 1);
    } else if (strcmp(algorithm, "wall_following_left") == 0) {
        braitenberg_set_wall_following_left(&braitenberg);
    } else if (strcmp(algorithm, "wall_following_right") == 0) {
        braitenberg_set_wall_following_right(&braitenberg);
    } else if (strcmp(algorithm, "object_following") == 0) {
        braitenberg_set_object_following(&braitenberg);
    } else if (strcmp(algorithm, "line_following_white") == 0) {
        braitenberg_set_line_following_white(&braitenberg);
    } else if (strcmp(algorithm, "line_following_black") == 0) {
        braitenberg_set_line_following_black(&braitenberg);
    }

    // Run the algorithm
    braitenberg_run(&braitenberg);
    return 0;
}
