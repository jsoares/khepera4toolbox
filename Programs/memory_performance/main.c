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
#include <time.h>

// Memory
int memory[65536];

// Prints the help text.

void help() {
    printf("Tests the performance of the CPU. Operations can be combined.\n");
    printf("\n");
    printf("Usage: time cpu_performance [OPTIONS]\n");
    printf("\n");
    printf("Options:\n");
    printf("  --memory-write N      Writes N sequential values to the memory\n");
    printf("  --memory-write-sim N  The same, but without memory accesses (just loop overhead)\n");
    printf("  --memory-read N       Reads N sequential values from the memory\n");
    printf("  --memory-read-sim N   The same, but without memory accesses (just loop overhead)\n");
    printf("  --memory-random N     Performs N random read/write operations\n");
    printf("  --memory-random-sim N The same, but without memory accesses (just loop overhead)\n");
    printf("The largest allowed N is 0xffffffff.\n");
    printf("Results may depend on compiler.\n");
    printf("\n");
}

// Runs the memory tests.

void run_memory() {
    unsigned int N;
    int a = rand();
    int b = rand();
    int c = rand();

    // Memory write
    N = (unsigned int) commandline_option_value_int(0, "--memory-write", 0);
    while (N > 0) {
        a = N & 0xffff;
        memory[a] = b;
        N--;
    }

    // Memory write (loop overhead)
    N = (unsigned int) commandline_option_value_int(0, "--memory-write-sim", 0);
    while (N > 0) {
        a = N & 0xffff;
        N--;
    }

    // Memory read
    N = (unsigned int) commandline_option_value_int(0, "--memory-read", 0);
    while (N > 0) {
        a = N & 0xffff;
        b = memory[a];
        N--;
    }

    // Memory read (loop overhead)
    N = (unsigned int) commandline_option_value_int(0, "--memory-read-sim", 0);
    while (N > 0) {
        a = N & 0xffff;
        N--;
    }

    // Random memory access
    N = (unsigned int) commandline_option_value_int(0, "--memory-random", 0);
    while (N > 0) {
        a = rand();
        b = ~a;
        a = a & 0xffff;
        b = b & 0xffff;
        c = memory[a];
        memory[b] = c;
        N--;
    }

    // Random memory access (loop overhead)
    N = (unsigned int) commandline_option_value_int(0, "--memory-random-sim", 0);
    while (N > 0) {
        a = rand();
        b = ~a;
        a = a & 0xffff;
        b = b & 0xffff;
        N--;
    }

    // Print result (to avoid the compiler to optimize everything away)
    printf("memory-result %d %d %d\n", a, b, c);
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

    // Initialize random number generator
    srand(time(0));

    // Run
    run_memory();
    return 0;
}
