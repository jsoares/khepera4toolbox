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

#ifndef _KH4_UTILS_H
#define _KH4_UTILS_H

#define USE_RDBOARD 1

#include "khepera4.h"
#include "relativepositionboard.h"
#include "windsensorboard.h"
#include "cmdline.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define MAX_SPEED 20000

static int ir_zero[9];

// Khepera 4 stuffs

static void motor_init() {
    /* Put the wheels in normal (control) mode */
    khepera4_drive_start();
    /* Initialize encoders */
    khepera4_drive_reset_position(0, 0);
}

static void rb_init(struct gengetopt_args_info *options) {
    int interval;

    relativepositionboard_init();
    interval = (int) ((1.0 / options->rb_rate_arg)*1000.0); // in ms
    printf("Range and Bearing board started with %d ms interval.\n", interval);
    relativepositionboard.configuration.tx_interval = interval;
    relativepositionboard_configuration_tx_interval_set();
}

static void rb_calibrate() {
    float threshold_factor = 2.0f;
    relativepositionboard_calibrate(threshold_factor);
}

static void rdboard_set_leds(int on) {
    int inty; // LED intensity

    if (on) inty = 100;
    else inty = 0;

    // I2C message
    i2cal_start();
    i2cal_writedata_uint8(0x081); // register
    i2cal_writedata_uint8(inty); // red LED
    i2cal_writedata_uint8(inty); // green LED
    i2cal_write(10); // device address
    i2cal_commit();
}

static void tracking_init() {
#if USE_RDBOARD
    rdboard_set_leds(1);
#else
    /* Turn on tracking LEDS */
    windsensorboard_init();
    windsensorboard.led.mode = cWindSensorBoardLEDMode_Tracking;
    windsensorboard_led_mode_set();
#endif
}

static void ir_calibrate() {
    int i, j;
    int ir_mean[9];

    memset(ir_mean, 0, sizeof (ir_mean));

    for (i = 0; i < 8; i++) {
        khepera4_infrared_proximity();
        for (j = 0; j < 9; j++) {
            ir_mean[j] += khepera4.infrared_proximity.sensor[j];
        }
    }
    for (i = 0; i < 9; i++) {
        ir_zero[i] = ir_mean[i] >> 3; // divide by 8
    }
}

static int braitenberg(int *speed_left, int *speed_right) {
    static float previous_speed[2] = {0.0f, 0.0f};
    static const int weights[2][9] = {
        {0, 70, 100, 100, -100, -100, -70, 0, 0}, // left wheel
        {0, -70, -100, -100, 100, 100, 70, 0, 0}
    }; // right wheel

    int speed[2] = {0, 0}; // left, right
    int ir[9];
    double breaking = 1.0f;
    int i;
    int ret = 0;

    khepera4_infrared_proximity();
    for (i = 0; i < 9; i++) {
        ir[i] = khepera4.infrared_proximity.sensor[i] - ir_zero[i];
    }

    for (i = 0; i < 9; i++) {
        if (ir[i] > 400) {
            speed[0] += weights[0][i] * (ir[i] / 3); // MAGIC
            speed[1] += weights[1][i] * (ir[i] / 3); // MAGIC
            breaking -= (float) ir[i] / (float) (500 * 9); // MAGIC
            if (ir[i] > 1000)
                ret = 1;
        }
    }

    speed[0] += (int) (breaking * (float) (*speed_left));
    speed[1] += (int) (breaking * (float) (*speed_right));

    if (speed[0] > MAX_SPEED) {
        speed[1] *= (float) MAX_SPEED / (float) speed[0];
        speed[0] = MAX_SPEED;
    }
    if (speed[0] < -MAX_SPEED) {
        speed[1] *= -(float) MAX_SPEED / (float) speed[0];
        speed[0] = -MAX_SPEED;
    }
    if (speed[1] > MAX_SPEED) {
        speed[0] *= (float) MAX_SPEED / (float) speed[1];
        speed[1] = MAX_SPEED;
    }
    if (speed[1] < -MAX_SPEED) {
        speed[0] *= -(float) MAX_SPEED / (float) speed[1];
        speed[1] = -MAX_SPEED;
    }

    // smooth speed
    previous_speed[0] = previous_speed[0] * 0.7f + (float) speed[0] * 0.3f;
    previous_speed[1] = previous_speed[1] * 0.7f + (float) speed[1] * 0.3f;

    *speed_left = (int) previous_speed[0];
    *speed_right = (int) previous_speed[1];

    return ret;
}

void khepera_stop() {
    relativepositionboard_stop();
    khepera4_drive_set_speed(0, 0);
#if USE_RDBOARD
    rdboard_set_leds(0);
#else
    windsensorboard.led.mode = cWindSensorBoardLEDMode_External;
    windsensorboard_led_mode_set();
#endif
}

#endif
