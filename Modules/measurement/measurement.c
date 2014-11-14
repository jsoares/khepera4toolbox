/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "measurement.h"
#include "commandline.h"

void measurement_init() {
	measurement_configuration.fast = 0;
	measurement_configuration.repeat = 1;
	measurement_configuration.wait_us = 0;
	measurement_configuration.log_size = 1;
	measurement_configuration.hook_measure = &measurement_hook_measure_default;
	measurement_configuration.hook_print = &measurement_hook_print_default;
	measurement_configuration.hook_flush = &measurement_hook_flush_default;
}

void measurement_commandline_veryfast(struct sCommandLineOption *option) {
	measurement_configuration.fast = 2;
}

void measurement_commandline_fast(struct sCommandLineOption *option) {
	measurement_configuration.fast = 1;
}

void measurement_commandline_repeat(struct sCommandLineOption *option) {
	measurement_configuration.repeat = -1;
	if (option->value) {
		measurement_configuration.repeat = strtol(option->value, 0, 0);
		if (measurement_configuration.repeat < 1) {
			measurement_configuration.repeat = -1;
		}
	}
}

void measurement_commandline_wait(struct sCommandLineOption *option) {
	measurement_configuration.wait_us = 0;
	if (option->value) {
		measurement_configuration.wait_us = strtol(option->value, 0, 0);
	}
}

void measurement_commandline_prepare() {
	commandline_option_register_hook("-f", "--fast", cCommandLine_Option, &measurement_commandline_fast);
	commandline_option_register_hook("-F", "--very-fast", cCommandLine_Option, &measurement_commandline_veryfast);
	commandline_option_register_hook("-r", "--repeat", cCommandLine_Option_Value, &measurement_commandline_repeat);
	commandline_option_register_hook("-w", "--wait-us", cCommandLine_Option_Value, &measurement_commandline_wait);
}

void measurement_start() {
	int i;
	int log_w = 0;
	int log_full = 0;

	// Take as many samples as requested
	while (measurement_configuration.repeat != 0) {
		// Get measures
		measurement_configuration.hook_measure(log_w);

		// Increment the log counter
		log_w++;
		if (log_w >= measurement_configuration.log_size) {
			log_w = 0;
			log_full = 0;
		}

		// Print values
		if (measurement_configuration.fast == 0) {
			measurement_configuration.hook_print(0);
			measurement_configuration.hook_flush();
			log_w = 0;
		}

		// Wait
		if (measurement_configuration.wait_us) {
			usleep(measurement_configuration.wait_us);
		}

		// Continuous mode?
		if (measurement_configuration.repeat > 0) {
			measurement_configuration.repeat--;
		}
	}

	// Print what is remaining in the buffer
	if (measurement_configuration.fast == 1) {
		if (log_full) {
			for (i = log_w; i < measurement_configuration.log_size; i++) {
				measurement_configuration.hook_print(i);
			}
		}
		for (i = 0; i < log_w; i++) {
			measurement_configuration.hook_print(i);
		}
		measurement_configuration.hook_flush();
	}
}

void measurement_hook_measure_default(int i) {}

void measurement_hook_print_default(int i) {}

void measurement_hook_flush_default() {
	fflush(stdout);
}
