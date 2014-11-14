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
#include <sys/time.h>
#include <signal.h>

// Algorithm parameters
struct sAlgorithm {
  int wait_us;	// Waiting time between stream requests
  int verbosity;	// Level of verbosity (0=quiet, 1=normal, 2=verbose, 3=very verbose, ...)
  int running;
  int nrecv;
  double stime;
};

// Algorithm instance
struct sAlgorithm algorithm;

// Prints the help text.
void help() {
  printf("Reads the relative positions from the relative positioning board.\n");
  printf("It computes the receiving rate.\n");
  printf("Usage: relativepositionboard [OPTIONS]\n");
  printf("  -w --wait-us US       Sets the waiting time in the perception-to-action loop (default: 1)\n");
}

double gettime()
{
  struct timeval time;

  gettimeofday(&time, NULL);
  return (double)time.tv_sec + ((double)time.tv_usec)/1000000.0;
}

void exit_program(int signal)
{
  printf("Statistics: %.2f Hz\n", algorithm.nrecv/(gettime()-algorithm.stime));
  algorithm.running = 0;
}

// Initializes the algorithm.
void algorithm_init() {
  // Initialize modules
  relativepositionboard_init();

  // Catch CTRL+C
  signal(SIGINT,exit_program);

  // Read command line arguments
  algorithm.wait_us = commandline_option_value_int("-w", "--wait-us", 1);
  algorithm.running = 1;
  algorithm.nrecv = 0;
  algorithm.stime = gettime();
}

// Reports NTC sensor values.
void algorithm_run() {
  int i;

  // Start the stream
  relativepositionboard_stream_start();

  // Read from the stream
  while (algorithm.running) {
    // Read the next samples from the stream
    relativepositionboard_stream_step();
    for (i = 0; i < 32; i++) {
      if (relativepositionboard.robot[i].age != 0) continue;
      algorithm.nrecv++;
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
