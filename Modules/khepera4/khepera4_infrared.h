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


#ifndef KHEPERA4_INFRARED
#define KHEPERA4_INFRARED

#include "i2cal.h"
#include "time.h"

//! Infrared sensors

enum eKhepera4SensorsInfrared {
    cKhepera4SensorsInfrared_BackLeft = 0,
    cKhepera4SensorsInfrared_Left = 1,
    cKhepera4SensorsInfrared_FrontLeft = 2,
    cKhepera4SensorsInfrared_Front = 3,
    cKhepera4SensorsInfrared_FrontRight = 4,
    cKhepera4SensorsInfrared_Right = 5,
    cKhepera4SensorsInfrared_BackRight = 6,
    cKhepera4SensorsInfrared_Back = 7,
    cKhepera4SensorsInfrared_FloorLeft = 8,
    cKhepera4SensorsInfrared_FloorFrontLeft = 9,
    cKhepera4SensorsInfrared_FloorFrontRight = 10,
    cKhepera4SensorsInfrared_FloorRight = 11,

    cKhepera4SensorsInfrared_Begin = 0,
    cKhepera4SensorsInfrared_End = 11,
    cKhepera4SensorsInfrared_Count = 12,
    cKhepera4SensorsInfrared_RingBegin = 0,
    cKhepera4SensorsInfrared_RingEnd = 7,
    cKhepera4SensorsInfrared_RingCount = 8,
    cKhepera4SensorsInfrared_FloorBegin = 8,
    cKhepera4SensorsInfrared_FloorEnd = 11,
    cKhepera4SensorsInfrared_FloorCount = 4
};

//! One measurement of all infrared sensors

struct sKhepera4SensorsInfrared {
    int sensor[12]; //!< Sensor values
    time_t timestamp; //!< Timestamp at which the values were read
};

//! (private) Initializes this submodule.
void khepera4_infrared_init();

//! (private) General function to read infrared measures. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 31 bytes.
int khepera4_infrared_p(struct sKhepera4SensorsInfrared *result, char infrared_command);

//! Reads the last ambient infrared values and fills them into the khepera4 structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 31 bytes.
int khepera4_infrared_ambient();
//! Reads the last ambient infrared values and fills them into the provided structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 31 bytes.
int khepera4_infrared_ambient_p(struct sKhepera4SensorsInfrared *result);
//! Reads the last infrared proximity values and fills them into the khepera4 structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 31 bytes.
int khepera4_infrared_proximity();
//! Reads the last infrared proximity values and fills them into the provided structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 31 bytes.
int khepera4_infrared_proximity_p(struct sKhepera4SensorsInfrared *result);

#endif
