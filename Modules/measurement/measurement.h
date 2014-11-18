/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef MEASUREMENT
#define MEASUREMENT

struct sMeasurementConfiguration {
    int repeat;
    int fast;
    int wait_us;
    void (*hook_measure)(int i);
    void (*hook_print)(int i);
    void (*hook_flush)();
    int log_size;
};

struct sMeasurementConfiguration measurement_configuration;

//! Initializes this module.
void measurement_init();

//! Prepares the command line module before the arguments are parsed.
void measurement_commandline_prepare();

//! Takes one or more measurements (depending on the command line arguments).
void measurement_start();

//! An empty measurement function.
void measurement_hook_measure_default(int i);
//! An empty print function.
void measurement_hook_print_default(int i);
//! The default flush function. Calls fflush(stdout).
void measurement_hook_flush_default();

#endif
