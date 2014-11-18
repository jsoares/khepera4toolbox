/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include "i2c_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Algorithm parameters

struct sAlgorithm {
    int no_reset; // Whether to suppress reset or not
    int wait_us; // Waiting time between stream requests
    int verbosity; // Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
    struct sI2CStreamRead sr; // I2C Stream
};

// Algorithm instance
struct sAlgorithm algorithm;

// Prints the help text.

void help() {
    printf("Reads a data stream from an I2C device. Note that the I2C device must implement the corresponding streaming protocol.\n");
    printf("\n");
    printf("Usage: i2c_readstream [OPTIONS] DEVICE REGISTER BLOCKSIZE\n");
    printf("\n");
    printf("Options:\n");
    printf("  --no-reset            Does not reset the buffer before reading from the stream\n");
    printf("  -w --wait-us US       Waiting time between requests in us (default: 10000)\n");
    printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)\n");
    printf("\n");
    printf("Output:\n");
    printf("  $BLOCK,block_number,buffered_blocks,data\n");
    printf("  Data is written in hex notation, with a space between each byte.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  Streaming 4-byte blocks from register 0x86 (query) of device 0x50:\n");
    printf("    i2c_readstream 0x50 0x86 4\n");
    printf("  The same, but with sleeps of 50ms between queries:\n");
    printf("    i2c_readstream 0x50 0x86 4 -w 50000\n");
    printf("\n");
}

void algorithm_init() {
    // Initialize modules
    i2c_stream_init();

    // Check the number of arguments provided
    if (commandline_argument_count() < 3) {
        help();
        exit(1);
    }

    // Read command line arguments
    algorithm.sr.configuration.device = commandline_argument_int(0, 0);
    algorithm.sr.configuration.reg = commandline_argument_int(1, 0);
    algorithm.sr.configuration.block_size = commandline_argument_int(2, 1);
    if (algorithm.sr.configuration.block_size > 128) {
        algorithm.sr.configuration.block_size = 128;
    }
    if (algorithm.sr.configuration.block_size < 2) {
        algorithm.sr.configuration.block_size = 2;
    }

    // The I2CAL module has a read buffer size of 128 bytes for one transaction (which corresponds to roughly 10 ms transfer time). Hence, the number of blocks we can read at a time is given by:
    algorithm.sr.configuration.blocks_count_max = 128 / algorithm.sr.configuration.block_size;

    // Read command line options
    algorithm.wait_us = commandline_option_value_int("-w", "--wait-us", 50000);
    algorithm.no_reset = commandline_option_provided("-nr", "--no-reset");
    algorithm.verbosity = commandline_option_value_int("-v", "--verbosity", 1);
}

void algorithm_run() {
    int i, n;
    int block_number;

    // Start the stream
    if (algorithm.no_reset) {
        i2c_stream_read_start_without_reset(&algorithm.sr);
    } else {
        i2c_stream_read_start(&algorithm.sr);
    }

    block_number = 0;
    while (1) {
        // Read from the stream
        i2c_stream_read_next(&algorithm.sr);

        // Print all (non-empty) blocks
        for (i = 0; i < algorithm.sr.result.blocks_count; i++) {
            int block_status = i2c_stream_readdata_blockstatus(&algorithm.sr, i);
            if (block_status == I2C_STREAM_BLOCKSTATUS_BUFFER_OVERRUN) {
                // Buffer overrun is a non-recoverable error
                printf("$BUFFER_OVERRUN\n");
                fflush(stdout);
                return;
            } else if (block_status == I2C_STREAM_BLOCKSTATUS_EMPTY) {
                // Empty block? Do nothing ...
            } else {
                // Print the block
                if (algorithm.verbosity > 0) {
                    printf("$BLOCK,%d,%d,", block_number, block_status);
                    printf("%02X", i2c_stream_readdata_uint8(&algorithm.sr, i, 1));
                    for (n = 2; n < algorithm.sr.configuration.block_size; n++) {
                        printf(" %02X", i2c_stream_readdata_uint8(&algorithm.sr, i, n));
                    }
                    printf("\n");
                }
                block_number++;
                fflush(stdout);
            }
        }

        // Wait a little bit
        if (algorithm.wait_us) {
            usleep(algorithm.wait_us);
        }
    }
}

// Main program.

int main(int argc, char *argv[]) {
    // Command line parsing
    commandline_init();
    commandline_option_register("-nr", "--no-reset", cCommandLine_Option);
    commandline_parse(argc, argv);

    // Help
    if (commandline_option_provided("-h", "--help")) {
        help();
        exit(1);
    }

    // Initialization
    algorithm_init();

    // Run
    algorithm_run();
    return 0;
}
