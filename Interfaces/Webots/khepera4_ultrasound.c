#include "khepera4_ultrasound.h"
#include "khepera4.h"

char ultrasound_name[5] = "us0";
DeviceTag ultrasound[5]; 				//!< Remember DeviceTag equals to const char
const char *cKhepera4SensorsUltrasound[5] = {"left", "frontleft", "front", "frontright", "right"};

void khepera4_ultrasound_init() {
	int i;

	// Ultrasound assigment
	for (i = cKhepera4SensorsUltrasound_Begin; i < cKhepera4SensorsUltrasound_End; i++) {
		ultrasound[i] = robot_get_device(ultrasound_name);
		distance_sensor_enable(ultrasound[i], CONTROL_STEP);
		if ((i + 1) >= 10) {
			ultrasound_name[2] = '1';
			ultrasound_name[3]++;
			if ((i + 1) == 10) {
				ultrasound_name[3] = '0';
				ultrasound_name[4] = (char) '\0';
			}

		} else {
			ultrasound_name[2]++;
		}

	}

	khepera4.ultrasound.distance_per_increment = 0.01;
	ultrasound_structure_init();
}

void ultrasound_structure_init() {
	int i, j;

	for (i = cKhepera4SensorsUltrasound_Begin; i < cKhepera4SensorsUltrasound_End; i++) {
		for (j = 0; j < 10; j++) {
			khepera4.ultrasound.sensor[i].distance[j] = 0;
			khepera4.ultrasound.sensor[i].amplitude[j] = 0;
			khepera4.ultrasound.sensor[i].distance[j] = 0;
			khepera4.ultrasound.sensor[i].timestamp[j] = 0;
		}

		khepera4.ultrasound.sensor[i].amplitude[0] = AMPLITUDE;
	}
}

int khepera4_ultrasound(enum eKhepera4SensorsUltrasound sensor) {
	khepera4.ultrasound.sensor[sensor].distance[0] = (int)distance_sensor_get_value(ultrasound[sensor]);
	khepera4.ultrasound.sensor[sensor].timestamp[0] = robot_get_time();
}

enum eKhepera4SensorsUltrasound khepera4_ultrasound_getsensorbyname(const char *name, enum eKhepera4SensorsUltrasound defaultvalue) {
	int i;

	if (name == 0) {
		return defaultvalue;
	}

	// By name
	for (i = 0; i < 5; i++) {
		if (strncmp(name, cKhepera4SensorsUltrasound[i]) == 0) {
			return i;
		}
	}

	// Nothing
	return defaultvalue;
}

enum eKhepera4SensorsUltrasoundBit khepera4_ultrasound_getsensorbitbysensor(enum eKhepera4SensorsUltrasound sensor) {
	return (1 << sensor);
}
