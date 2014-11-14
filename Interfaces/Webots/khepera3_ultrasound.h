#ifndef KHEPERA3_ULTRASOUND_H
#define KHEPERA3_ULTRASOUND_H

#define AMPLITUDE 1

const char *cKhepera3SensorsUltrasound[5];

//! Khepera3 Ultrasound Sensors
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

//! One measurement of one ultrasound sensor
struct sKhepera3SensorsUltrasoundSensor {
	int echos_count;		//!< Number of received echos
	int distance[10];		//!< Received distance in increments (cm at the moment, multiply with distance_per_increment to get meters)
	int amplitude[10];		//!< Received amplitude
	int timestamp[10];		//!< Timestamp at which the echo was received
};

//! Khepera3 Ultrasound Structure
struct sKhepera3SensorsUltrasound {
	struct sKhepera3SensorsUltrasoundSensor sensor[5];
	float distance_per_increment;	//!< Conversion factor to convert from distance values to meters
};

//! Initializes webots ultrasound sensors
void khepera3_ultrasound_init();
//! Initializes sKhepera3SensorsUltrasoundSensor structure for each ultrasound sensor
void ultrasound_structure_init();
//! Assigns webots ultrasound sensors previously initialized to the khepera3 ultrasound structure
int khepera3_ultrasound(enum eKhepera3SensorsUltrasound sensor);
//! Gets sensor number by name
enum eKhepera3SensorsUltrasound khepera3_ultrasound_getsensorbyname(const char *name, enum eKhepera3SensorsUltrasound defaultvalue);
//! Returns a sensor bitmask by sensor ID.
enum eKhepera3SensorsUltrasoundBit khepera3_ultrasound_getsensorbitbysensor(enum eKhepera3SensorsUltrasound sensor);



#endif
