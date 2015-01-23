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


#ifndef KHEPERA4_IMU
#define KHEPERA4_IMU

#include "i2cal.h"

#define KH4_GYRO_DEG_S 0.066

//! Battery

struct sKhepera4SensorsImuSample {
    double x;
    double y;
    double z;
    time_t timestamp;
};

struct sKhepera4SensorsImu {
    struct sKhepera4SensorsImuSample accel[10];
    struct sKhepera4SensorsImuSample gyro[10];
    struct sKhepera4SensorsImuSample accel_mean;
    struct sKhepera4SensorsImuSample gyro_mean;
    time_t timestamp;
};



//! (private) Initializes this submodule.
void khepera4_imu_init();

//! (private) Reads a value from the IMU and returns a plain i2c message. Transfer on I2C bus: 12 bytes.
int khepera4_imu_p(struct sKhepera4SensorsImu *imu);

//! Reads the IMU data and updates the khepera4 structure
int khepera4_imu();


#endif
