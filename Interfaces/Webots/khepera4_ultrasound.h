#ifndef KHEPERA4_ULTRASOUND_H
#define KHEPERA4_ULTRASOUND_H

#define AMPLITUDE 1

const char *cKhepera4SensorsUltrasound[5];

//! Khepera4 Ultrasound Sensors
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

//! One measurement of one ultrasound sensor
struct sKhepera4SensorsUltrasoundSensor {
	int echos_count;		//!< Number of received echos
	int distance[10];		//!< Received distance in increments (cm at the moment, multiply with distance_per_increment to get meters)
	int amplitude[10];		//!< Received amplitude
	int timestamp[10];		//!< Timestamp at which the echo was received
};

//! Khepera4 Ultrasound Structure
struct sKhepera4SensorsUltrasound {
	struct sKhepera4SensorsUltrasoundSensor sensor[5];
	float distance_per_increment;	//!< Conversion factor to convert from distance values to meters
};

//! Initializes webots ultrasound sensors
void khepera4_ultrasound_init();
//! Initializes sKhepera4SensorsUltrasoundSensor structure for each ultrasound sensor
void ultrasound_structure_init();
//! Assigns webots ultrasound sensors previously initialized to the khepera4 ultrasound structure
int khepera4_ultrasound(enum eKhepera4SensorsUltrasound sensor);
//! Gets sensor number by name
enum eKhepera4SensorsUltrasound khepera4_ultrasound_getsensorbyname(const char *name, enum eKhepera4SensorsUltrasound defaultvalue);
//! Returns a sensor bitmask by sensor ID.
enum eKhepera4SensorsUltrasoundBit khepera4_ultrasound_getsensorbitbysensor(enum eKhepera4SensorsUltrasound sensor);



#endif
