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


#ifndef WINDSENSORBOARD
#define WINDSENSORBOARD

#include "i2c_stream.h"

enum eWindSensorBoardMode {
    cWindSensorBoardMode_None = 0,
    cWindSensorBoardMode_Sample = 1,
    cWindSensorBoardMode_Calibrate = 2
};

enum eWindSensorBoardLEDMode {
    cWindSensorBoardLEDMode_Debug = 0,
    cWindSensorBoardLEDMode_Tracking = 1,
    cWindSensorBoardLEDMode_TrackingCode = 2,
    cWindSensorBoardLEDMode_External = 3
};

struct sWindSensorBoard {
    int device;

    struct {

        struct {
            unsigned int sample_number;
            unsigned int sensor[6];
        } last;
        unsigned int integration;
        struct sI2CStreamRead stream;
    } ntc;

    struct {

        struct {
            unsigned int sample_number;
            unsigned int sensor[6];
            unsigned int sum;
        } last;
        unsigned int baseline;
        struct sI2CStreamRead stream;
    } ntc_normalized;

    struct {
        enum eWindSensorBoardMode mode;

        struct {
            unsigned int sample_number;
            unsigned int angle_14b;
            unsigned int likelihood_16b32;
            float angle;
            float likelihood;
        } last;
        struct sI2CStreamRead stream;

        struct {
            unsigned int angle_variance;
            unsigned int angle_shift;
            unsigned int full_evaluation_interval;

            struct {

                struct {
                    unsigned int mean;
                    unsigned int variance;
                } sensor[6];
            } angle[64];
        } calibration;
    } windsensor;

    struct {
        enum eWindSensorBoardLEDMode mode;

        struct {
            int length;
            int id;
            int chip_length;
        } code;
        int external;
    } led;
};

struct sWindSensorBoard windsensorboard;

//! Initializes this module.
void windsensorboard_init();

//! Updates the NTC sensor values.
int windsensorboard_ntc_get();
//! Sets the integration interval.
int windsensorboard_ntc_integration_set();
//! Gets the integration interval.
int windsensorboard_ntc_integration_get();

//! Updates the normalized NTC sensor values.
int windsensorboard_ntc_normalized_get();
//! Sets the baseline for normalization.
int windsensorboard_ntc_normalized_baseline_set();
//! Gets the baseline for normalization.
int windsensorboard_ntc_normalized_baseline_get();

//! Updates the wind sensor values.
int windsensorboard_windsensor_get();
//! Sets the angle parameters.
int windsensorboard_windsensor_calibration_parameters_set();
//! Gets the angle parameters.
int windsensorboard_windsensor_calibration_parameters_get();
//! Uploads calibration data onto the wind sensor board.
int windsensorboard_windsensor_calibration_angle_upload(unsigned int angle_index_first, unsigned int angle_index_last);
//! Downloads calibration data from the wind sensor board.
int windsensorboard_windsensor_calibration_angle_download(unsigned int angle_index_first, unsigned int angle_index_last);
//! Calibrates one angle.
int windsensorboard_windsensor_calibrate(unsigned int angle_index);
//! Resets the state and starts calculating the wind direction.
int windsensorboard_windsensor_start();
//! Stops calculating the wind direction.
int windsensorboard_windsensor_stop();
//! Reads the mode.
int windsensorboard_windsensor_mode_get();

//! Sets the LED mode.
int windsensorboard_led_mode_set();
//! Gets the LED mode.
int windsensorboard_led_mode_get();
//! Sets the LED code.
int windsensorboard_led_code_set();
//! Gets the LED code.
int windsensorboard_led_code_get();
//! Sets the value for the LED mode "external".
int windsensorboard_led_external_set();
//! Gets the value for the LED mode "external".
int windsensorboard_led_external_get();

#endif
