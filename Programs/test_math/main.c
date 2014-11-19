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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.14159265358979

// Prints the help text.

void help() {
    printf("Tests some mathematical functions.\n");
    printf("\n");
}

// Tests common floating point functions.

void test_floating_point_functions() {
    int angle_degrees;
    float angle, ratio;
    float dx, dy;

    for (angle_degrees = 0; angle_degrees < 360; angle_degrees += 5) {
        angle = (float) angle_degrees / 180. * PI;
        dx = cosf(angle);
        dy = sinf(angle);
        ratio = tanf(angle);
        printf("%d, %f, %f, %f, %f, %f, %f\n", angle_degrees, dx, dy, ratio, atanf(ratio), atan2f(dy, dx), sqrtf(angle));
    }
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

    // Test some functions
    test_floating_point_functions();

    return 0;
}
