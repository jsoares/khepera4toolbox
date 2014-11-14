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
  printf("Reads the relative positions from the relative positioning board.\n");
  printf("Usage: relativepositionboard [OPTIONS]\n");
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
  // Start the stream
  relativepositionboard_stream_start();

  // Read from the stream
  while (1) {
    // Read the next samples from the stream
    relativepositionboard_stream_step();

    // Print all updated information
    int i;
    for (i = 0; i < 32; i++) {
      if (relativepositionboard.robot[i].age == 0) {
        printf("$RELATIVEPOSITION,%d,%f,%f\n", i,
               relativepositionboard.robot[i].range,
               relativepositionboard.robot[i].bearing
               );
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
