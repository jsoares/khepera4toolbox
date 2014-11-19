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


#ifndef ODORBOARD
#define ODORBOARD

#include "i2c_stream.h"

enum eOdorBoardLEDMode {
    cOdorBoardLEDMode_Debug = 0,
    cOdorBoardLEDMode_Off = 1,
    cOdorBoardLEDMode_External = 2
};

struct sOdorboard {
    int device;

    struct {

        struct {
            unsigned int sensor[3];
            unsigned int sample_number;
        } last;
        unsigned int integration;
        struct sI2CStreamRead stream;
    } voc;

    struct {

        struct {
            int timestamp;
            float temperature;
            float humidity;
            float dewpoint_temperature;
        } last;
        struct sI2CStreamRead stream;
    } sht11;

    struct {
        unsigned int speed[3];
    } pump;

    struct {
        enum eOdorBoardLEDMode mode;
        int external;
    } led;
};

struct sOdorboard odorboard;

//! Initializes this module.
void odorboard_init();

//! Updates the VOC sensor values.
int odorboard_voc_get();
//! Sets the integration interval.
int odorboard_voc_integration_set(unsigned int integration);
//! Sets the integration interval.
int odorboard_voc_integration_get();

//! Updates the SHT11 sensor values.
int odorboard_sht11_get();
//! Sets the sht11 measurement interval.
int odorboard_sht11_set_interval(unsigned int interval);

//! Sets the pump speed (4096..0).
int odorboard_pump_speed_set(unsigned int pump_id, unsigned int speed);
//! Updates the pump speed values.
int odorboard_pump_speed_get();

//! Sets the LED mode.
int odorboard_led_mode_set();
//! Gets the LED mode.
int odorboard_led_mode_get();
//! Sets the value for the LED mode "external".
int odorboard_led_external_set();
//! Gets the value for the LED mode "external".
int odorboard_led_external_get();

#endif
