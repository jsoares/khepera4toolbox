/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera4.h"
#include "commandline.h"
#include "motion_arrowkeys.h"
#include "interactiveinput.h"
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

// Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
int verbosity;
// Loop speed
int wait_us;

// Prints the help text.

void help() {
    printf("Allows to control the robot with arrow keys.\n");
    printf("\n");
    printf("Usage: motion_arrowkeys [OPTIONS]\n");
    printf("\n");
    printf("Options:\n");
    printf("  -w --wait-us US    Sets the waiting time in the perception-to-action loop (default: 50000)\n");
    printf("  -v --verbosity V   Sets the verbosity level (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)\n");
    printf("\n");
    printf("Keys:\n");
    printf("  +-         increase/decrease speed step\n");
    printf("  123456789  set speed and direction\n");
    printf("  qweasdyxc  set speed and direction\n");
    printf("  <enter>    quit\n");
}

int motion_arrowkeys_run(struct sMotionArrowKeys * motion_arrowkeys) {
    int key, timediff;
    struct timeval timprev;
    struct timeval timnext;
    int speed_left, speed_right;

    // Put the wheels in normal (control) mode
    khepera4_drive_start();

    gettimeofday(&timprev, 0);
    while (1) {
        // Wait for the next key
        key = interactiveinput_waitkey(wait_us);

        // Quit on ENTER, SPACE or 0
        if ((key == 10) || (key == 32) || (key == '0')) {
            break;
        }

        // Update speed
        gettimeofday(&timnext, 0);
        timediff = (timnext.tv_sec - timprev.tv_sec) * 1000000 + (timnext.tv_usec - timprev.tv_usec);
        timprev = timnext;
        motion_arrowkeys_update(motion_arrowkeys, key, timediff);

        // Set motor speed
        speed_left = motion_arrowkeys_getspeedleft(motion_arrowkeys);
        speed_right = motion_arrowkeys_getspeedright(motion_arrowkeys);
        printf("Speeds: %d %d\n", speed_left, speed_right);
        if ((abs(speed_left) < 1) && (abs(speed_right) < 1)) {
            khepera4_drive_set_speed(0, 0);
        } else {
            khepera4_drive_set_speed(speed_left, speed_right);
        }

        khepera4_drive_get_current_speed();
        printf("$SPEED,%d,%d\n", khepera4.motor_left.current_speed, khepera4.motor_right.current_speed);
    }
    return 0;
}

// Main program.

int main(int argc, char *argv[]) {
    struct sMotionArrowKeys motion_arrowkeys;

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
    khepera4_drive_init();
    interactiveinput_init();
    motion_arrowkeys_init(&motion_arrowkeys);

    // Read command line arguments
    wait_us = commandline_option_value_int("-w", "--wait-us", 50 * 1000);
    verbosity = commandline_option_value_int("-v", "--verbosity", 1);

    // Run the algorithm
    motion_arrowkeys_run(&motion_arrowkeys);
    return 0;
}

