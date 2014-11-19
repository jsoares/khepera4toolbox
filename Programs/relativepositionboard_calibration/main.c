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
#include "nmea.h"
#include "relativepositionboard.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Prints the help text.

void help() {
    printf("Reads and writes calibration records, or launches the calibration procedure.\n");
    printf("Usage: windsensorboard_windsensor_calibration [OPTIONS]\n");
    printf("  -d --download             Downloads the calibration values and prints them on STDOUT\n");
    printf("  -u --upload               Uploads the calibration values read from STDIN\n");
    printf("  -c --calibrate            Calibrates the relative position board\n");
    printf("  -s --save                 Saves the calibration values to EEPROM\n");
    printf("  -l --load                 Loads the calibration values from EEPROM\n");
    printf("  -tf --threshold-factor F  Sets the threshold factor used for calibration with --calibrate (default: 2.0)\n");
    printf("NMEA Messages:\n");
    printf("  $RELATIVE_POSITION_CALIBRATION,MEAN1,THRESHOLD1,MEAN2,THRESHOLD2,...\n");
}

// Download operation.

void operation_download() {
    relativepositionboard_calibration_get();
    printf("$RELATIVE_POSITION_CALIBRATION");
    int s, c;
    for (c = 0; c < 2; c++) {
        for (s = 0; s < 8; s++) {
            printf(",%d,%d", relativepositionboard.calibration.sector[s][c].mean, relativepositionboard.calibration.sector[s][c].threshold);
        }
    }
    printf("\n");
}

// Processes one NMEA message read from STDIN.

void operation_upload_hook_process_message(struct sNMEAMessage *m, int withchecksum) {
    // Process the message
    if (strcmp(m->command, "RELATIVE_POSITION_CALIBRATION") == 0) {
        if (m->argument_count >= 16) {
            int s, c;
            for (c = 0; c < 2; c++) {
                for (s = 0; s < 8; s++) {
                    relativepositionboard.calibration.sector[s][c].mean = (unsigned int) strtol(m->argument[s * 2 + c * 32], 0, 0);
                    relativepositionboard.calibration.sector[s][c].threshold = (unsigned int) strtol(m->argument[s * 2 + 1 + c * 32], 0, 0);
                }
            }
            relativepositionboard_calibration_set();
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
    float threshold_factor = commandline_option_value_float("-tf", "--threshold-factor", 2.0);
    relativepositionboard_calibrate(threshold_factor);
    printf("Calibration of all 8 sectors with threshold factor %f started. This will take about 50 ms to complete.\n", threshold_factor);
}

// Load operation.

void operation_load() {
    relativepositionboard_calibration_load();
    printf("Calibration values loaded from EEPROM.\n");
}

// Load operation.

void operation_save() {
    relativepositionboard_calibration_save();
    printf("Calibration values saved from EEPROM. Note that these values will *not* be automatically loaded upon restart of the board.\n");
}

// Main program.

int main(int argc, char *argv[]) {
    struct sCommandLineOption *option_download;
    struct sCommandLineOption *option_upload;
    struct sCommandLineOption *option_calibrate;
    struct sCommandLineOption *option_save;
    struct sCommandLineOption *option_load;

    // Command line parsing
    commandline_init();
    option_download = commandline_option_register("-d", "--download", cCommandLine_Option);
    option_upload = commandline_option_register("-u", "--upload", cCommandLine_Option);
    option_calibrate = commandline_option_register("-c", "--calibrate", cCommandLine_Option);
    option_save = commandline_option_register("-s", "--save", cCommandLine_Option);
    option_load = commandline_option_register("-l", "--load", cCommandLine_Option);
    commandline_parse(argc, argv);

    // Help
    if (commandline_option_provided("-h", "--help")) {
        help();
        exit(1);
    }

    // Initialization
    relativepositionboard_init();

    // Execute operation
    if (option_download->provided) {
        operation_download();
    } else if (option_upload->provided) {
        operation_upload();
    } else if (option_calibrate->provided) {
        operation_calibrate();
    } else if (option_save->provided) {
        operation_save();
    } else if (option_load->provided) {
        operation_load();
    } else {
        help();
        exit(1);
    }

    return 0;
}
