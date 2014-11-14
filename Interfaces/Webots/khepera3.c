#include "khepera3.h"

void khepera3_init() {
	khepera3_infrared_init();
	khepera3_ultrasound_init();
	khepera3_drive_init();

	// Not yet implemented;
	//khepera3_motor_init();
	//khepera3_battery_init();
}

