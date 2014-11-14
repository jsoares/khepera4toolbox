/*!
 * (c) 2009 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Prints the help text.
void help() {
	printf("Tests the performance of the CPU. Operations can be combined.\n");
	printf("\n");
	printf("Usage: time cpu_performance [OPTIONS]\n");
	printf("\n");
	printf("Options:\n");
	printf("  --int-add N      Carries out N 32-bit integer additions in a loop\n");
	printf("  --int-mul N      Carries out N 32-bit integer multiplications in a loop\n");
	printf("  --int-div N      Carries out N 32-bit integer division in a loop\n");
	printf("  --long-add N     Carries out N 64-bit integer additions in a loop\n");
	printf("  --long-mul N     Carries out N 64-bit integer multiplications in a loop\n");
	printf("  --long-div N     Carries out N 64-bit integer division in a loop\n");
	printf("  --float-add N    Carries out N 32-bit floating point additions in a loop\n");
	printf("  --float-mul N    Carries out N 32-bit floating point multiplications in a loop\n");
	printf("  --float-div N    Carries out N 32-bit floating point division in a loop\n");
	printf("  --double-add N   Carries out N 64-bit floating point additions in a loop\n");
	printf("  --double-mul N   Carries out N 64-bit floating point multiplications in a loop\n");
	printf("  --double-div N   Carries out N 64-bit floating point division in a loop\n");
	printf("  --empty N        Runs only the loop for N times, without any operation\n");
	printf("Note that N will be rounded down to multiples of 16. The largest allowed N is 0xffffff00.\n");
	printf("All tests are running in a 'while' loop executing 16 operations per iteration.\n");
	printf("No overflow checks are performed. Results may depend on compiler.\n");
	printf("\n");
}

// Runs the empty test.
void run_empty() {
	unsigned int N;

	// Empty loop
	N = (unsigned int)commandline_option_value_int(0, "--empty", 0) >> 4;
	while (N > 0) {
		N--;
	}
}

// Runs the int tests.
void run_int() {
	unsigned int N;
	int a = rand();
	int b = rand();
	int c = rand();
	int d = 10;

	// Additions
	N = (unsigned int)commandline_option_value_int(0, "--int-add", 0) >> 4;
	while (N > 0) {
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		N--;
	}

	// Multiplications
	N = (unsigned int)commandline_option_value_int(0, "--int-mul", 0) >> 4;
	while (N > 0) {
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		N--;
	}

	// Divisions
	N = (unsigned int)commandline_option_value_int(0, "--int-div", 0) >> 4;
	while (N > 0) {
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		N--;
	}

	// Print result (to avoid the compiler to optimize everything away)
	printf("int-result %d %d %d (size: %d bytes)\n", a, b, c, sizeof(int));
}

// Runs the long int tests.
void run_long() {
	unsigned int N;
	long long int a = (long long int)rand();
	long long int b = (long long int)rand();
	long long int c = (long long int)rand();
	long long int d = 10;

	// Additions
	N = (unsigned int)commandline_option_value_int(0, "--long-add", 0) >> 4;
	while (N > 0) {
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		N--;
	}

	// Multiplications
	N = (unsigned int)commandline_option_value_int(0, "--long-mul", 0) >> 4;
	while (N > 0) {
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		N--;
	}

	// Divisions
	N = (unsigned int)commandline_option_value_int(0, "--long-div", 0) >> 4;
	while (N > 0) {
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		N--;
	}
	
	// Print result (to avoid the compiler to optimize everything away)
	printf("long-result %d %d %d (size: %d bytes)\n", (int)a, (int)b, (int)c, sizeof(long long int));
}

// Runs the float tests.
void run_float() {
	unsigned int N;
	float a = (float)rand();
	float b = (float)rand();
	float c = (float)rand();
	float d = 10;

	// Additions
	N = (unsigned int)commandline_option_value_int(0, "--float-add", 0) >> 4;
	while (N > 0) {
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		N--;
	}

	// Multiplications
	N = (unsigned int)commandline_option_value_int(0, "--float-mul", 0) >> 4;
	while (N > 0) {
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		N--;
	}

	// Divisions
	N = (unsigned int)commandline_option_value_int(0, "--float-div", 0) >> 4;
	while (N > 0) {
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		N--;
	}

	// Print result (to avoid the compiler to optimize everything away)
	printf("float-result %f %f %f (size: %d bytes)\n", a, b, c, sizeof(float));
}

// Runs the double tests.
void run_double() {
	unsigned int N;
	double a = (double)rand();
	double b = (double)rand();
	double c = (double)rand();
	double d = 10;

	// Additions
	N = (unsigned int)commandline_option_value_int(0, "--double-add", 0) >> 4;
	while (N > 0) {
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		b = c + a;
		c = a + b;
		a = b + c;
		N--;
	}

	// Multiplications
	N = (unsigned int)commandline_option_value_int(0, "--double-mul", 0) >> 4;
	while (N > 0) {
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		b = c * a;
		c = a * b;
		a = b * c;
		N--;
	}

	// Divisions
	N = (unsigned int)commandline_option_value_int(0, "--double-div", 0) >> 4;
	while (N > 0) {
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		b = c / d;
		c = a / d;
		a = b / d;
		N--;
	}

	// Print result (to avoid the compiler to optimize everything away)
	printf("double-result %f %f %f (size: %d bytes)\n", a, b, c, sizeof(double));
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
	run_empty();
	run_int();
	run_long();
	run_float();
	run_double();
	return 0;
}
