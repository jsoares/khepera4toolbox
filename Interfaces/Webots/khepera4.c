#include "khepera4.h"

void khepera4_init() {
	khepera4_infrared_init();
	khepera4_ultrasound_init();
	khepera4_drive_init();

	// Not yet implemented;
	//khepera4_motor_init();
	//khepera4_battery_init();
}

