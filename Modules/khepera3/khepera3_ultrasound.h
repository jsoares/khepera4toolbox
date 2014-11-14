/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef KHEPERA3_ULTRASOUND
#define KHEPERA3_ULTRASOUND

#include "i2cal.h"

//! Ultrasound sensors
enum eKhepera3SensorsUltrasound {
	cKhepera3SensorsUltrasound_Left = 0,
	cKhepera3SensorsUltrasound_FrontLeft = 1,
	cKhepera3SensorsUltrasound_Front = 2,
	cKhepera3SensorsUltrasound_FrontRight = 3,
	cKhepera3SensorsUltrasound_Right = 4,

	cKhepera3SensorsUltrasound_Begin = 0,
	cKhepera3SensorsUltrasound_End = 5,
	cKhepera3SensorsUltrasound_Count = 5
};

//! Ultrasound sensor bit masks
enum eKhepera3SensorsUltrasoundBit {
	cKhepera3SensorsUltrasoundBit_Left = 1,
	cKhepera3SensorsUltrasoundBit_FrontLeft = 2,
	cKhepera3SensorsUltrasoundBit_Front = 4,
	cKhepera3SensorsUltrasoundBit_FrontRight = 8,
	cKhepera3SensorsUltrasoundBit_Right = 16,

	cKhepera3SensorsUltrasoundBit_None = 0,
	cKhepera3SensorsUltrasoundBit_All = 31
};

//! The ultrasound sensors
struct sKhepera3SensorsUltrasound {
	int sensor[5];	//!< The 5 ultrasound sensors
	float distance_per_increment;						//!< Conversion factor to convert from distance values to meters
        time_t timestamp;
};

//! (private) Initializes this submodule.
void khepera3_ultrasound_init();

//! Reads the last values of an ultrasound sensor and fills them into the provided structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 52 bytes.
int khepera3_ultrasound_p(struct sKhepera3SensorsUltrasound *result);

//! Reads the last values of an ultrasound sensor and fills them into the khepera3 structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 52 bytes.
int khepera3_ultrasound();
//! Enables or disables ultrasound sensors. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 8 bytes.
int khepera3_ultrasound_enable(enum eKhepera3SensorsUltrasoundBit bitmask);


#endif
