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


#ifndef RELATIVEPOSITIONINGBOARD
#define RELATIVEPOSITIONINGBOARD

#include "i2c_stream.h"

enum sRelativePositionBoard_TXPower {
    cRelativePositionBoard_TXPower_Default = 0xff
};

enum sRelativePositionBoard_TXSectors {
    cRelativePositionBoard_TXSectors_Default = 0xff
};

struct sRelativePositionBoard {
    int device;
    struct sI2CStreamRead stream;
    int robot_id;

    struct {
        float rssi[8]; //!< Intensity of sensors
        float range; //!< Range of sending robot
        float bearing; //!< Bearing of sending robot
        int age; //!< Age of the information (in number of calls to relativepositionboard_step)
    } robot[32];

    struct {
        int rx_enabled; //!< The receiver is enabled.
        int tx_enabled; //!< The transmitter is enabled.
        int tx_data; //!< The data packet that the transmitted transmits. This should always be the robot ID for this module to work correctly.
        enum sRelativePositionBoard_TXPower tx_power; //!< The transmit power.
        enum sRelativePositionBoard_TXSectors tx_sectors; //!< The enabled transmit sectors (bitmask).
        int tx_interval; //!< The transmission interval.
    } configuration;

    struct {

        struct {
            float scaling_left;
            float scaling_center;
            float scaling_right;
            float alpha;
            float beta;
            float gamma;
            float u;
            float v;
        } rssi[8];

        struct {
            unsigned int mean;
            unsigned int threshold;
        } sector[8][2];
    } calibration;
};

extern struct sRelativePositionBoard relativepositionboard;

//! Initialize this module.
void relativepositionboard_init();

//! Start sending robot_id (0..31) and receiving data from other robots.
void relativepositionboard_start(unsigned int robot_id);
//! Switch off the transmitter and receiver.
void relativepositionboard_stop();

//! Start acquireing.
void relativepositionboard_stream_start();
//! Acquire new data.
void relativepositionboard_stream_step();

//! Sets the corresponding configuration value.
void relativepositionboard_configuration_rx_enabled_set();
//! Sets the corresponding configuration value.
void relativepositionboard_configuration_tx_enabled_set();
//! Sets the corresponding configuration value.
void relativepositionboard_configuration_tx_data_set();
//! Sets the corresponding configuration value.
void relativepositionboard_configuration_tx_power_set();
//! Sets the corresponding configuration value.
void relativepositionboard_configuration_tx_sectors_set();
//! Sets the corresponding configuration value.
void relativepositionboard_configuration_tx_interval_set();

//! Reads the corresponding configuration value.
void relativepositionboard_configuration_rx_enabled_get();
//! Reads the corresponding configuration value.
void relativepositionboard_configuration_tx_enabled_get();
//! Reads the corresponding configuration value.
void relativepositionboard_configuration_tx_data_get();
//! Reads the corresponding configuration value.
void relativepositionboard_configuration_tx_power_get();
//! Reads the corresponding configuration value.
void relativepositionboard_configuration_tx_sectors_get();
//! Reads the corresponding configuration value.
void relativepositionboard_configuration_tx_interval_get();

//! Starts the calibration of all sectors. This will take about 6 ms per sector.
void relativepositionboard_calibrate(float threshold_factor);
//! Sets the calibration values.
void relativepositionboard_calibration_set();
//! Reads the calibration values.
int relativepositionboard_calibration_get();
//! Saves the calibration values to EEPROM. These values will *not* be automatically loaded upon reset of the board.
void relativepositionboard_calibration_save();
//! Loads the calibration values from EEPROM.
void relativepositionboard_calibration_load();

#endif
