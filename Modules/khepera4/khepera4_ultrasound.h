/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef KHEPERA4_ULTRASOUND
#define KHEPERA4_ULTRASOUND

#include "i2cal.h"

//! Ultrasound sensors

enum eKhepera4SensorsUltrasound {
    cKhepera4SensorsUltrasound_Left = 0,
    cKhepera4SensorsUltrasound_FrontLeft = 1,
    cKhepera4SensorsUltrasound_Front = 2,
    cKhepera4SensorsUltrasound_FrontRight = 3,
    cKhepera4SensorsUltrasound_Right = 4,

    cKhepera4SensorsUltrasound_Begin = 0,
    cKhepera4SensorsUltrasound_End = 5,
    cKhepera4SensorsUltrasound_Count = 5
};

//! Ultrasound sensor bit masks

enum eKhepera4SensorsUltrasoundBit {
    cKhepera4SensorsUltrasoundBit_Left = 1,
    cKhepera4SensorsUltrasoundBit_FrontLeft = 2,
    cKhepera4SensorsUltrasoundBit_Front = 4,
    cKhepera4SensorsUltrasoundBit_FrontRight = 8,
    cKhepera4SensorsUltrasoundBit_Right = 16,

    cKhepera4SensorsUltrasoundBit_None = 0,
    cKhepera4SensorsUltrasoundBit_All = 31
};

//! The ultrasound sensors

struct sKhepera4SensorsUltrasound {
    int sensor[5]; //!< The 5 ultrasound sensors
    float distance_per_increment; //!< Conversion factor to convert from distance values to meters
    time_t timestamp;
};

//! (private) Initializes this submodule.
void khepera4_ultrasound_init();

//! Reads the last values of an ultrasound sensor and fills them into the provided structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 52 bytes.
int khepera4_ultrasound_p(struct sKhepera4SensorsUltrasound *result);

//! Reads the last values of an ultrasound sensor and fills them into the khepera4 structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 52 bytes.
int khepera4_ultrasound();
//! Enables or disables ultrasound sensors. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 8 bytes.
int khepera4_ultrasound_enable(enum eKhepera4SensorsUltrasoundBit bitmask);


#endif
