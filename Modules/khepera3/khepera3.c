/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include <assert.h>
#include <stdio.h>
#include "khepera3.h"

/*** Module Initialization ***/

void khepera3_init() {
	// Initialize the i2cal module
	i2cal_init();

	// Set default configuration
	khepera3.dspic.i2c_address = 0x30;

	// Read the robot id
	khepera3.robot_id = 0;
	FILE *file = fopen("/etc/khepera/robotid", "r");
	if (file) {
		fscanf(file, "%d", &khepera3.robot_id);
		fclose(file);
	}

	// Initialize submodules
	khepera3_drive_init();
	khepera3_infrared_init();
	khepera3_ultrasound_init();
	khepera3_battery_init();
}

/*** General Functionality ***/

int khepera3_timestamp_reset() {
	i2cal_start();
	i2cal_writedata_uint8(2);
	i2cal_writedata_uint8('Z');
	i2cal_write(khepera3.dspic.i2c_address);
	if (i2cal_commit() < 0) {
		return 0;
	}
	return -1;
}

int khepera3_dspic_firmware_version() {
	// Send command and read result
	i2cal_start();
	i2cal_writedata_uint8(2);
	i2cal_writedata_uint8('B');
	i2cal_write(khepera3.dspic.i2c_address);
	struct i2c_msg *msg_read = i2cal_read(khepera3.dspic.i2c_address, 7);
	if (i2cal_commit() < 0) {
		return 0;
	}

	// Decode firmware version and revision
	khepera3.dspic.firmware_version = i2cal_readdata_uint16(msg_read, 3);
	khepera3.dspic.firmware_revision = i2cal_readdata_uint16(msg_read, 5);
	return -1;
}
