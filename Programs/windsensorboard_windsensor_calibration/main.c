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
#include "windsensorboard.h"
#include "nmea.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <math.h>
#define PI 3.14159265358979

// Prints the help text.

void help() {
    printf("Reads and writes calibration records, or launches the calibration procedure.\n");
    printf("Usage: windsensorboard_windsensor_calibration [OPTIONS] [ID [SPEED]]\n");
    //printf("  -v --verbosity V       Sets the verbosity level (0=quiet, 1=default, 2=verbose, 3=very verbose, ...)\n");
    printf("  -d --download          Downloads the calibration values and prints them on STDOUT\n");
    printf("  -u --upload            Uploads the calibration values read from STDIN\n");
    printf("  -c --calibrate         Calibrates the wind sensor board\n");
    printf("  -v --verify            Verifies the calibration\n");
    printf("  -t --turn              Turn on the spot\n");
    printf("  -s --steps-per-turn S  Sets the number steps per turn (default: 5830)\n");
    printf("NMEA Messages:\n");
    printf("  $WINDSENSOR_CALIBRATION_ANGLE,ANGLE_INDEX,MEAN1_16b,VARIANCE1_24b16,MEAN2_16b,VARIANCE2_24b16,...\n");
    printf("  $WINDSENSOR_CALIBRATION_ANGLE_PARAMETERS,STDEV_DEGREES,SHIFT_DEGREES,FULL_EVALUATION_INTERVAL\n");
}

// Normalizes an angle (in degrees).

double normalize_angle_0_360(double angle) {
    while (angle >= 360) {
        angle -= 360;
    }
    while (angle < 0) {
        angle += 360;
    }
    return angle;
}

// Download operation.

void operation_download() {
    int angle_index;
    int s;

    windsensorboard_windsensor_calibration_angle_download(0, 63);
    for (angle_index = 0; angle_index < 64; angle_index++) {
        printf("$WINDSENSOR_CALIBRATION_ANGLE,%d", angle_index);
        for (s = 0; s < 6; s++) {
            printf(",%d,%d", windsensorboard.windsensor.calibration.angle[angle_index].sensor[s].mean, windsensorboard.windsensor.calibration.angle[angle_index].sensor[s].variance);
        }
        printf("\n");
    }
    windsensorboard_windsensor_calibration_parameters_get();
    printf("$WINDSENSOR_CALIBRATION_PARAMETERS,%f,%f,%d\n", sqrt((double) windsensorboard.windsensor.calibration.angle_variance / 16777216)*360, (double) windsensorboard.windsensor.calibration.angle_shift / 16384 * 360, windsensorboard.windsensor.calibration.full_evaluation_interval);
}

// Processes one NMEA message read from STDIN.

void operation_upload_hook_process_message(struct sNMEAMessage *m, int withchecksum) {
    int s;

    // Process the message
    if (strcmp(m->command, "WINDSENSOR_CALIBRATION_ANGLE") == 0) {
        if (m->argument_count >= 13) {
            int angle_index = strtol(m->argument[0], 0, 0) & 0x3f;
            for (s = 0; s < 6; s++) {
                windsensorboard.windsensor.calibration.angle[angle_index].sensor[s].mean = (unsigned int) strtol(m->argument[s * 2 + 1], 0, 0);
                windsensorboard.windsensor.calibration.angle[angle_index].sensor[s].variance = (unsigned int) strtol(m->argument[s * 2 + 2], 0, 0);
            }
            windsensorboard_windsensor_calibration_angle_upload(angle_index, angle_index);
        }
    } else if (strcmp(m->command, "WINDSENSOR_CALIBRATION_PARAMETERS") == 0) {
        if (m->argument_count >= 3) {
            windsensorboard.windsensor.calibration.angle_variance = (int) floor(pow(strtod(m->argument[0], 0) / 360, 2) * 16777216 + 0.5);
            windsensorboard.windsensor.calibration.angle_shift = (int) floor(normalize_angle_0_360(strtod(m->argument[1], 0)) / 360 * 16384 + 0.5);
            windsensorboard.windsensor.calibration.full_evaluation_interval = strtod(m->argument[2], 0);
            windsensorboard_windsensor_calibration_parameters_set();
        }
    }
}

// Upload operation.

void operation_upload() {
    int len;
    char buffer[1024];
    struct sNMEAParser parser;

    // Initialize the parser
    nmea_parser_init(&parser);
    parser.hook_process_message = operation_upload_hook_process_message;

    // Read and parse until there is nothing available any more
    while (1) {
        len = read(STDIN_FILENO, buffer, 1024);
        if (len < 1) {
            return;
        }
        nmea_parser_process_data(&parser, buffer, len);
    }
}

// Calibrate operation.

void operation_calibrate() {
    // Initialize the drive system
    khepera4_init();
    khepera4_drive_start();
    khepera4_drive_reset_position(0, 0);

    // Calculate the conversion factor
    float conversion = commandline_option_value_float("-s", "--steps-per-turn", 5830.) / 64.;

    // Calibrate each angle
    printf("I assume that you placed the robot looking in DOWNWIND direction into the flow.\n");
    int a;
    for (a = 0; a < 64; a++) {
        int angle_index = (a + 32) & 0x3f;
        printf("Calibrating angle: %f (%d) ", (float) angle_index / 64. * 360., angle_index);
        fflush(stdout);

        // Move the robot
        khepera4_drive_goto_position(-(int) floor(a * conversion + 0.5), (int) floor(a * conversion + 0.5));
        usleep(1000000);

        // Launch the calibration procedure and wait until it is finished
        windsensorboard_windsensor_calibrate(angle_index);
        while (1) {
            windsensorboard_windsensor_mode_get();
            if (windsensorboard.windsensor.mode != cWindSensorBoardMode_Calibrate) {
                break;
            }
            printf(".");
            fflush(stdout);
            usleep(2000000);
        }
        printf("\n");

        // Print calibration result
        windsensorboard_windsensor_calibration_angle_download(angle_index, angle_index);
        printf("$WINDSENSOR_CALIBRATION_ANGLE,%d", angle_index);
        int s;
        for (s = 0; s < 6; s++) {
            printf(",%d,%d", windsensorboard.windsensor.calibration.angle[angle_index].sensor[s].mean, windsensorboard.windsensor.calibration.angle[angle_index].sensor[s].variance);
        }
        printf("\n");
    }

    // Finish
    printf("Full calibration done.\n");
}

// Verify operation.

void operation_verify() {
    // Initialize the drive system
    khepera4_init();
    khepera4_drive_start();
    khepera4_drive_reset_position(0, 0);

    // Calculate the conversion factor
    float conversion = commandline_option_value_float("-s", "--steps-per-turn", 5830.) / 128.;

    // Verify each angle
    printf("I assume that you placed the robot looking in DOWNWIND direction into the flow.\n");
    int a;
    for (a = 0; a < 128; a++) {
        float angle = (float) a / 128. * PI * 2. + PI;
        printf("$WINDSENSOR_VERIFICATION_ANGLE,%d,%f\n", a, (float) angle / PI * 180.);
        fflush(stdout);

        // Move the robot
        khepera4_drive_goto_position(-(int) floor(a * conversion + 0.5), (int) floor(a * conversion + 0.5));
        usleep(1000000);

        // Launch acquisition
        windsensorboard_windsensor_start();
        i2c_stream_read_start(&windsensorboard.windsensor.stream);

        // Read from the stream
        int block_number = 0;
        while (1) {
            // Read the next samples from the stream
            i2c_stream_read_next(&windsensorboard.windsensor.stream);

            // Print all (non-empty) blocks
            int i;
            for (i = 0; i < windsensorboard.windsensor.stream.result.blocks_count; i++) {
                int block_status = i2c_stream_readdata_blockstatus(&windsensorboard.windsensor.stream, i);
                if (block_status == I2C_STREAM_BLOCKSTATUS_BUFFER_OVERRUN) {
                    // Buffer overrun is a non-recoverable error
                    printf("$BUFFER_OVERRUN\n");
                    return;
                } else if (block_status == I2C_STREAM_BLOCKSTATUS_EMPTY) {
                    // Empty block? Do nothing ...
                } else {
                    // Print the block
                    printf("$WIND,%d,%d,%f,%f\n", block_number,
                            i2c_stream_readdata_uint8(&windsensorboard.windsensor.stream, i, 1),
                            (double) (i2c_stream_readdata_uint16(&windsensorboard.windsensor.stream, i, 2)) / 0x4000 * 360,
                            (double) (i2c_stream_readdata_uint32(&windsensorboard.windsensor.stream, i, 4)) / 0x10000
                            );
                    block_number++;
                }
            }

            // Stop after 256 samples
            if (block_number >= 256) {
                break;
            }

            // Sleep
            usleep(100000);
        }

        windsensorboard_windsensor_stop();
    }

    // Finish
    printf("Full verification done.\n");
}

// Turn operation.

void operation_turn() {
    float conversion;

    // Initialize the drive system
    khepera4_init();
    khepera4_drive_start();
    khepera4_drive_reset_position(0, 0);

    // Calculate the conversion factor (in the same way as it is done during calibration)
    conversion = commandline_option_value_float("-s", "--steps-per-turn", 5830.) / 64.;

    // Make one complete turn
    khepera4_drive_goto_position((int) floor(64 * conversion + 0.5), -(int) floor(64 * conversion + 0.5));
}

// Main program.

int main(int argc, char *argv[]) {
    struct sCommandLineOption *option_download;
    struct sCommandLineOption *option_upload;
    struct sCommandLineOption *option_calibrate;
    struct sCommandLineOption *option_verify;
    struct sCommandLineOption *option_turn;

    // Command line parsing
    commandline_init();
    option_download = commandline_option_register("-d", "--download", cCommandLine_Option);
    option_upload = commandline_option_register("-u", "--upload", cCommandLine_Option);
    option_calibrate = commandline_option_register("-c", "--calibrate", cCommandLine_Option);
    option_verify = commandline_option_register("-v", "--verify", cCommandLine_Option);
    option_turn = commandline_option_register("-t", "--turn", cCommandLine_Option);
    commandline_parse(argc, argv);

    // Help
    if (commandline_option_provided("-h", "--help")) {
        help();
        exit(1);
    }

    // Initialization
    windsensorboard_init();

    // Execute operation
    if (option_download->provided) {
        operation_download();
    } else if (option_upload->provided) {
        operation_upload();
    } else if (option_calibrate->provided) {
        operation_calibrate();
    } else if (option_verify->provided) {
        operation_verify();
    } else if (option_turn->provided) {
        operation_turn();
    } else {
        help();
        exit(1);
    }

    return 0;
}
