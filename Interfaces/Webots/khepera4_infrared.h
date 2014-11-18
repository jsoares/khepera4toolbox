#ifndef KHEPERA4_INFRARED_H
#define KHEPERA4_INFRARED_H

//! Khepera4 Infrared Sensors
enum eKhepera4SensorsInfrared {
	cKhepera4SensorsInfrared_BackLeft = 0,
	cKhepera4SensorsInfrared_Left = 1,
	cKhepera4SensorsInfrared_FrontSideLeft = 2,
	cKhepera4SensorsInfrared_FrontLeft = 3,
	cKhepera4SensorsInfrared_FrontRight = 4,
	cKhepera4SensorsInfrared_FrontSideRight = 5,
	cKhepera4SensorsInfrared_Right = 6,
	cKhepera4SensorsInfrared_BackRight = 7,
	cKhepera4SensorsInfrared_Back = 8,
	cKhepera4SensorsInfrared_FloorRight = 9,
	cKhepera4SensorsInfrared_FloorLeft = 10,

	cKhepera4SensorsInfrared_Begin = 0,
	cKhepera4SensorsInfrared_End = 10,
	cKhepera4SensorsInfrared_Count = 11,
	cKhepera4SensorsInfrared_RingBegin = 0,
	cKhepera4SensorsInfrared_RingEnd = 8,
	cKhepera4SensorsInfrared_RingCount = 9,
	cKhepera4SensorsInfrared_FloorBegin = 9,
	cKhepera4SensorsInfrared_FloorEnd = 10,
	cKhepera4SensorsInfrared_FloorCount = 2
};

//! Khepera4 Infrared Structure
struct sKhepera4SensorsInfrared {
	int sensor[11];			//!< Sensor values
	int timestamp;			//!< Timestamp at which the values were read
};

//! Initializes webots infrared sensors
void khepera4_infrared_init();
//! Assigns webots infrared sensors previously initialized to the khepera4 infrared structure
void khepera4_infrared_proximity();


#endif

