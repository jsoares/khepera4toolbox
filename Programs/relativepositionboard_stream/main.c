/*!
 * (c) 2009 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include "relativepositionboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#define PI 3.14159265358979

// Algorithm parameters
struct sAlgorithm {
  int wait_us;	// Waiting time between stream requests
  int verbosity;	// Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
};

// Algorithm instance
struct sAlgorithm algorithm;

// Prints the help text.
void help() {
  printf("Reads the relative positioning board stream.\n");
  printf("Usage: relativepositionboard_stream [OPTIONS]\n");
  printf("  -w --wait-us US       Sets the waiting time in the perception-to-action loop (default: 50000)\n");
  printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=default, 2=verbose, 3=very verbose, ...)\n");
}

// Initializes the algorithm.
void algorithm_init() {
  // Initialize modules
  relativepositionboard_init();

  // Read command line arguments
  algorithm.wait_us = commandline_option_value_int("-w", "--wait-us", 50 * 1000);
  algorithm.verbosity = commandline_option_value_int("-v", "--verbosity", 1);
}

// Reports NTC sensor values.
void algorithm_run() {
  int i;
  int block_number = 0;

  // Start the stream
  i2c_stream_read_start(&relativepositionboard.stream);

  // Read from the stream
  while (1) {
    // Read the next samples from the stream
    i2c_stream_read_next(&relativepositionboard.stream);

    // Print all (non-empty) blocks
    for (i = 0; i < relativepositionboard.stream.result.blocks_count; i++) {
      int block_status = i2c_stream_readdata_blockstatus(&relativepositionboard.stream, i);
      if (block_status == I2C_STREAM_BLOCKSTATUS_BUFFER_OVERRUN) {
        // Buffer overrun is a non-recoverable error
        printf("$BUFFER_OVERRUN\n");
        return;
      } else if (block_status == I2C_STREAM_BLOCKSTATUS_EMPTY) {
        // Empty block? Do nothing ...
      } else {
        // Print the block
        if (algorithm.verbosity > 0) {
          printf("$RELATIVEPOSITION_STREAM,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", block_number,
                 i2c_stream_readdata_uint8(&relativepositionboard.stream, i, 1),
                 i2c_stream_readdata_int16(&relativepositionboard.stream, i, 2),
                 i2c_stream_readdata_int16(&relativepositionboard.stream, i, 4),
                 i2c_stream_readdata_int16(&relativepositionboard.stream, i, 6),
                 i2c_stream_readdata_int16(&relativepositionboard.stream, i, 8),
                 i2c_stream_readdata_int16(&relativepositionboard.stream, i, 10),
                 i2c_stream_readdata_int16(&relativepositionboard.stream, i, 12),
                 i2c_stream_readdata_int16(&relativepositionboard.stream, i, 14),
                 i2c_stream_readdata_int16(&relativepositionboard.stream, i, 16)
                 );
        }
        block_number++;
      }
    }

    // Sleep
    usleep(algorithm.wait_us);
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

  // Initialization
  algorithm_init();

  // Run
  algorithm_run();
  return 0;
}
