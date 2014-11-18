/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera4.h"
#include "odometry_track.h"
#include "odometry_goto.h"
#include "commandline.h"
#include "nmea.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define PI 3.14159265358979

// Instances of the odometry objects
struct sOdometryTrack ot;
struct sOdometryGoto og;

// Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
int verbosity;

// Prints the help text.

void help() {
    printf("Drives the robot to a given target position, while keeping track of its position using odometry.\n");
    printf("Usage:\n");
    printf("  odometry_goto [OPTIONS] [COMMANDS]\n");
    printf("Commands: a sequence of\n");
    printf("  position X Y    Go to position (X, Y)\n");
    printf("  heading ANGLE   Turn to ANGLE\n");
    printf("  wait USEC       Wait US microseconds\n");
    printf("  file FILE       Process FILE (containing NMEA commands)\n");
    printf("Options:\n");
    printf("  -v --verbosity V       Sets the verbosity level (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)\n");
}

// Initialize everything.

void run_initialize() {
    // Read command line arguments
    verbosity = commandline_option_value_int("-v", "--verbosity", 1);

    // Start a new track
    odometry_track_start(&ot);
    odometry_goto_start(&og, &ot);

    // Put the wheels in normal (control) mode
    khepera4_drive_start();
}

void run_goto_position(float x, float y) {
    // Set and announce the new target position
    odometry_goto_set_goal(&og, x, y);
    if (verbosity > 0) {
        printf("$GOTO_POSITION,%f,%f\n", og.state.goal_x, og.state.goal_y);
    }

    // Move until we have reached the target position
    while (og.result.atgoal == 0) {
        // Update position and calculate new speeds
        odometry_track_step(og.track);
        odometry_goto_step(&og);

        // Set speed
        khepera4_drive_set_speed(og.result.speed_left, og.result.speed_right);

        // Print the current position
        if (verbosity > 1) {
            printf("$POSITION,%f,%f,%f\n", og.track->result.x, og.track->result.y, og.track->result.theta);
        }
    }

    // Announce when we have reached the target position
    if (verbosity > 0) {
        printf("$POSITION,%f,%f,%f\n", og.track->result.x, og.track->result.y, og.track->result.theta);
        fflush(stdout);
    }
}

void run_goto_heading(float goal_theta) {
    float diff_theta;

    // Announce the new target heading
    if (verbosity > 0) {
        printf("$GOTO_HEADING,%f\n", goal_theta);
    }

    // Move until we have reached the target position
    while (1) {
        // Update position and calculate new speeds
        odometry_track_step(og.track);

        // Calculate the current heading error
        diff_theta = goal_theta - og.track->result.theta;
        while (diff_theta > PI) {
            diff_theta -= 2 * PI;
        }
        while (diff_theta < -PI) {
            diff_theta += 2 * PI;
        }

        // Termination condition
        if (fabs(diff_theta) < 0.01) {
            break;
        }

        // Set speed
        khepera4_drive_set_speed_differential_bounded(og.configuration.speed_max, 0, 0, diff_theta * 8., 1);

        // Print the current position
        if (verbosity > 1) {
            printf("$POSITION,%f,%f,%f\n", og.track->result.x, og.track->result.y, og.track->result.theta);
        }
    }

    // Stop the motors
    khepera4_drive_set_speed(0, 0);

    // Announce when we have reached the target heading
    if (verbosity > 0) {
        printf("$POSITION,%f,%f,%f\n", og.track->result.x, og.track->result.y, og.track->result.theta);
        fflush(stdout);
    }
}

void run_goto_wait(int wait_usec) {
    // Announce the waiting time
    if (verbosity > 0) {
        printf("$GOTO_WAIT,%d\n", wait_usec);
    }

    // Wait
    usleep(wait_usec);

    // Announce when we are done
    if (verbosity > 0) {
        printf("$POSITION,%f,%f,%f\n", og.track->result.x, og.track->result.y, og.track->result.theta);
        fflush(stdout);
    }
}

// Processes one NMEA message.

void run_nmea_process_message(struct sNMEAMessage *m, int withchecksum) {
    float x, y, theta;

    if ((strcmp(m->command, "GOTO_POSITION") == 0) && (m->argument_count >= 2)) {
        x = strtod(m->argument[0], 0);
        y = strtod(m->argument[1], 0);
        run_goto_position(x, y);
    } else if ((strcmp(m->command, "GOTO_HEADING") == 0) && (m->argument_count >= 1)) {
        theta = strtod(m->argument[0], 0);
        run_goto_heading(theta);
    } else if ((strcmp(m->command, "GOTO_WAIT") == 0) && (m->argument_count >= 1)) {
        usleep(strtol(m->argument[0], 0, 0));
    }
}

// Reads a stream and passes it to the NMEA parser.

void run_nmea(FILE *file) {
    struct sNMEAParser parser;
    char buffer[128];
    int len;

    // Initialize the NMEA parser
    nmea_parser_init(&parser);
    parser.hook_process_message = run_nmea_process_message;

    // Read the whole stream
    while ((len = read(fileno(file), buffer, 128)) > 0) {
        nmea_parser_process_data(&parser, buffer, len);
    }
}

// Reads a file and passes it to the NMEA parser.

void run_file(const char *filename) {
    FILE *file;

    // Open the file
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Unable to open file '%s' for reading\n", filename);
        exit(2);
    }

    // Read the whole file
    run_nmea(file);

    // Close the file
    fclose(file);
}

// Go to all positions given on the command line.

void run_commandline() {
    int i;
    float x, y, theta, wait_usec;
    int state;

    // Process each command
    state = 50;
    for (i = 0; i < commandline_argument_count(); i++) {
        if (strcmp(commandline_argument(i, ""), "position") == 0) {
            state = 0;
        } else if (strcmp(commandline_argument(i, ""), "heading") == 0) {
            state = 10;
        } else if (strcmp(commandline_argument(i, ""), "wait") == 0) {
            state = 20;
        } else if (strcmp(commandline_argument(i, ""), "file") == 0) {
            state = 30;
        } else if (strcmp(commandline_argument(i, ""), "stdin") == 0) {
            state = 40;
            run_nmea(stdin);
        } else if (state == 0) {
            x = commandline_argument_float(i, 0);
            state = 1;
        } else if (state == 1) {
            y = commandline_argument_float(i, 0);
            run_goto_position(x, y);
            state = 0;
        } else if (state == 10) {
            theta = commandline_argument_float(i, 0);
            run_goto_heading(theta);
        } else if (state == 20) {
            wait_usec = commandline_argument_int(i, 0);
            run_goto_wait(wait_usec);
        } else if (state == 30) {
            run_file(commandline_argument(i, ""));
        }
    }

    // If not commands were provided, read the STDIN
    if (state == 50) {
        run_nmea(stdin);
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

    // Module initialization
    khepera4_init();
    odometry_track_init();
    odometry_goto_init();

    // If the user provided a file, read that file - otherwise, go to the position(s) given on the command line
    run_initialize();
    if (commandline_option_provided("-f", "--file")) {
        run_file(commandline_option_value("-f", "--file", ""));
    } else {
        run_commandline();
    }
    return 0;
}
