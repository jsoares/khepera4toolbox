/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include "relativepositionboard.h"
#include <stdlib.h>
#include <stdio.h>

// Prints the help text.
void help() {
  printf("Stops the emitter and receiver on the relative positioning board.\n");
  printf("Usage: relativepositionboard_stop\n");
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
  relativepositionboard_init();

  // Stop
  relativepositionboard_stop();
  return 0;
}
