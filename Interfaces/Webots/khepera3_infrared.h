#ifndef KHEPERA3_INFRARED_H
#define KHEPERA3_INFRARED_H

//! Khepera3 Infrared Sensors
enum eKhepera3SensorsInfrared {
	cKhepera3SensorsInfrared_BackLeft = 0,
	cKhepera3SensorsInfrared_Left = 1,
	cKhepera3SensorsInfrared_FrontSideLeft = 2,
	cKhepera3SensorsInfrared_FrontLeft = 3,
	cKhepera3SensorsInfrared_FrontRight = 4,
	cKhepera3SensorsInfrared_FrontSideRight = 5,
	cKhepera3SensorsInfrared_Right = 6,
	cKhepera3SensorsInfrared_BackRight = 7,
	cKhepera3SensorsInfrared_Back = 8,
	cKhepera3SensorsInfrared_FloorRight = 9,
	cKhepera3SensorsInfrared_FloorLeft = 10,

	cKhepera3SensorsInfrared_Begin = 0,
	cKhepera3SensorsInfrared_End = 10,
	cKhepera3SensorsInfrared_Count = 11,
	cKhepera3SensorsInfrared_RingBegin = 0,
	cKhepera3SensorsInfrared_RingEnd = 8,
	cKhepera3SensorsInfrared_RingCount = 9,
	cKhepera3SensorsInfrared_FloorBegin = 9,
	cKhepera3SensorsInfrared_FloorEnd = 10,
	cKhepera3SensorsInfrared_FloorCount = 2
};

//! Khepera3 Infrared Structure
struct sKhepera3SensorsInfrared {
	int sensor[11];			//!< Sensor values
	int timestamp;			//!< Timestamp at which the values were read
};

//! Initializes webots infrared sensors
void khepera3_infrared_init();
//! Assigns webots infrared sensors previously initialized to the khepera3 infrared structure
void khepera3_infrared_proximity();


#endif

