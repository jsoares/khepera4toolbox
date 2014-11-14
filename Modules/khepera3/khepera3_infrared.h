/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef KHEPERA3_INFRARED
#define KHEPERA3_INFRARED

#include "i2cal.h"
#include "time.h"

//! Infrared sensors
enum eKhepera3SensorsInfrared {
	cKhepera3SensorsInfrared_BackLeft = 0,
	cKhepera3SensorsInfrared_Left = 1,
	cKhepera3SensorsInfrared_FrontLeft = 2,
	cKhepera3SensorsInfrared_Front = 3,
	cKhepera3SensorsInfrared_FrontRight = 4,
	cKhepera3SensorsInfrared_Right = 5,
	cKhepera3SensorsInfrared_BackRight = 6,
	cKhepera3SensorsInfrared_Back = 7,
	cKhepera3SensorsInfrared_FloorLeft = 8,
	cKhepera3SensorsInfrared_FloorFrontLeft = 9,
	cKhepera3SensorsInfrared_FloorFrontRight = 10,
	cKhepera3SensorsInfrared_FloorRight = 11,

	cKhepera3SensorsInfrared_Begin = 0,
	cKhepera3SensorsInfrared_End = 11,
	cKhepera3SensorsInfrared_Count = 12,
	cKhepera3SensorsInfrared_RingBegin = 0,
	cKhepera3SensorsInfrared_RingEnd = 7,
	cKhepera3SensorsInfrared_RingCount = 8,
	cKhepera3SensorsInfrared_FloorBegin = 8,
	cKhepera3SensorsInfrared_FloorEnd = 11,
	cKhepera3SensorsInfrared_FloorCount = 4
};

//! One measurement of all infrared sensors
struct sKhepera3SensorsInfrared {
	int sensor[12];		//!< Sensor values
	time_t timestamp;		//!< Timestamp at which the values were read
};

//! (private) Initializes this submodule.
void khepera3_infrared_init();

//! (private) General function to read infrared measures. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 31 bytes.
int khepera3_infrared_p(struct sKhepera3SensorsInfrared *result, char infrared_command);

//! Reads the last ambient infrared values and fills them into the khepera3 structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 31 bytes.
int khepera3_infrared_ambient();
//! Reads the last ambient infrared values and fills them into the provided structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 31 bytes.
int khepera3_infrared_ambient_p(struct sKhepera3SensorsInfrared *result);
//! Reads the last infrared proximity values and fills them into the khepera3 structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 31 bytes.
int khepera3_infrared_proximity();
//! Reads the last infrared proximity values and fills them into the provided structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 31 bytes.
int khepera3_infrared_proximity_p(struct sKhepera3SensorsInfrared *result);

#endif
