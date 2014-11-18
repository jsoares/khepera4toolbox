#include "khepera4_infrared.h"
#include "khepera4.h"

char infrared_name[5] = "ds0";
DeviceTag infrared[11];		//!< Remember DeviceTag equals to const char

void khepera4_infrared_init() {
	int i;

	// Infrared assigment
	khepera4.infrared_proximity.timestamp = robot_get_time();
	for (i = cKhepera4SensorsInfrared_Begin; i < cKhepera4SensorsInfrared_Count; i++) {
		infrared[i] = robot_get_device(infrared_name);
		distance_sensor_enable(infrared[i], CONTROL_STEP);
		if ((i + 1) >= 10) {
			infrared_name[2] = '1';
			infrared_name[3]++;
			if ((i + 1) == 10) {
				infrared_name[3] = '0';
				infrared_name[4] = (char) '\0';
			}
		} else {
			infrared_name[2]++;
		}
	}
}

void khepera4_infrared_proximity() {
	int i;
	for (i = cKhepera4SensorsInfrared_Begin; i < cKhepera4SensorsInfrared_Count; i++) {
		khepera4.infrared_proximity.sensor[i] = (int)distance_sensor_get_value(infrared[i]);
	}
	khepera4.infrared_proximity.timestamp = robot_get_time();
}


