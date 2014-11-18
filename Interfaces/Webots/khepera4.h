#ifndef KHEPERA4_H
#define KHEPERA4_H

#include "khepera4_infrared.h"
#include "khepera4_ultrasound.h"
#include "khepera4_drive.h"
#include "khepera4_motor.h"
#include <device/robot.h>
#include <device/robot_types.h>
#include <device/differential_wheels.h>
#include <device/distance_sensor.h>

#define CONTROL_STEP 32

//! Khepera4 Structure
struct sKhepera4 {
	struct sKhepera4Motor motor_left;
	struct sKhepera4Motor motor_right;
	struct sKhepera4SensorsUltrasound ultrasound;
	struct sKhepera4SensorsInfrared infrared_proximity;
};

struct sKhepera4 khepera4;

//! Initializes robot by initialization functions with a default CONTROL_STEP of 32 ms
void khepera4_init();

#endif
