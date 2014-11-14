/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "windsensorboard.h"
#include <stdio.h>
#include <math.h>
#define PI 3.14159265358979

void windsensorboard_init() {
	i2cal_init();
	i2c_stream_init();
	windsensorboard.device = 0x52;
	windsensorboard.ntc.stream.configuration.device = windsensorboard.device;
	windsensorboard.ntc.stream.configuration.reg = 0x91;
	windsensorboard.ntc.stream.configuration.block_size = 14;
	windsensorboard.ntc.stream.configuration.blocks_count_max = 4;
	windsensorboard.ntc_normalized.stream.configuration.device = windsensorboard.device;
	windsensorboard.ntc_normalized.stream.configuration.reg = 0x95;
	windsensorboard.ntc_normalized.stream.configuration.block_size = 16;
	windsensorboard.ntc_normalized.stream.configuration.blocks_count_max = 4;
	windsensorboard.windsensor.stream.configuration.device = windsensorboard.device;
	windsensorboard.windsensor.stream.configuration.reg = 0x99;
	windsensorboard.windsensor.stream.configuration.block_size = 8;
	windsensorboard.windsensor.stream.configuration.blocks_count_max = 8;
	windsensorboard.led.mode = cWindSensorBoardLEDMode_Debug;
	windsensorboard.led.code.length = 14;
	windsensorboard.led.code.id = 1;
	windsensorboard.led.code.chip_length = 200;
	windsensorboard.led.external = 0;
}

int windsensorboard_ntc_get() {
	int commit_result;
	struct i2c_msg *message;

	// Request packet
	i2cal_start();
	i2cal_writedata_uint8(0x90);
	i2cal_write(windsensorboard.device);
	message = i2cal_read(windsensorboard.device, 14);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	// Parse packet
	windsensorboard.ntc.last.sensor[0] = i2cal_readdata_uint16(message, 0);
	windsensorboard.ntc.last.sensor[1] = i2cal_readdata_uint16(message, 2);
	windsensorboard.ntc.last.sensor[2] = i2cal_readdata_uint16(message, 4);
	windsensorboard.ntc.last.sensor[3] = i2cal_readdata_uint16(message, 6);
	windsensorboard.ntc.last.sensor[4] = i2cal_readdata_uint16(message, 8);
	windsensorboard.ntc.last.sensor[5] = i2cal_readdata_uint16(message, 10);
	windsensorboard.ntc.last.sample_number = i2cal_readdata_uint16(message, 12);
	return commit_result;
}

int windsensorboard_ntc_integration_set() {
	i2cal_start();
	i2cal_writedata_uint8(0x92);
	i2cal_writedata_uint8(windsensorboard.ntc.integration);
	i2cal_write(windsensorboard.device);
	return i2cal_commit();
}

int windsensorboard_ntc_integration_get() {
	int commit_result;
	struct i2c_msg *message;

	// Request packet
	i2cal_start();
	i2cal_writedata_uint8(0x92);
	i2cal_write(windsensorboard.device);
	message = i2cal_read(windsensorboard.device, 1);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	// Parse packet
	windsensorboard.ntc.integration = i2cal_readdata_uint8(message, 0);
	return commit_result;
}

int windsensorboard_ntc_normalized_get() {
	int commit_result;
	struct i2c_msg *message;

	// Request packet
	i2cal_start();
	i2cal_writedata_uint8(0x94);
	i2cal_write(windsensorboard.device);
	message = i2cal_read(windsensorboard.device, 16);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	// Parse packet
	windsensorboard.ntc_normalized.last.sensor[0] = i2cal_readdata_uint16(message, 0);
	windsensorboard.ntc_normalized.last.sensor[1] = i2cal_readdata_uint16(message, 2);
	windsensorboard.ntc_normalized.last.sensor[2] = i2cal_readdata_uint16(message, 4);
	windsensorboard.ntc_normalized.last.sensor[3] = i2cal_readdata_uint16(message, 6);
	windsensorboard.ntc_normalized.last.sensor[4] = i2cal_readdata_uint16(message, 8);
	windsensorboard.ntc_normalized.last.sensor[5] = i2cal_readdata_uint16(message, 10);
	windsensorboard.ntc_normalized.last.sum = i2cal_readdata_uint16(message, 12);
	windsensorboard.ntc_normalized.last.sample_number = i2cal_readdata_uint16(message, 14);
	return commit_result;
}

int windsensorboard_ntc_normalized_baseline_set() {
	i2cal_start();
	i2cal_writedata_uint8(0x96);
	i2cal_writedata_uint16(windsensorboard.ntc_normalized.baseline);
	i2cal_write(windsensorboard.device);
	return i2cal_commit();
}

int windsensorboard_ntc_normalized_baseline_get() {
	int commit_result;
	struct i2c_msg *message;

	// Request packet
	i2cal_start();
	i2cal_writedata_uint8(0x96);
	i2cal_write(windsensorboard.device);
	message = i2cal_read(windsensorboard.device, 2);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	// Parse packet
	windsensorboard.ntc_normalized.baseline = i2cal_readdata_uint16(message, 0);
	return commit_result;
}

int windsensorboard_windsensor_get() {
	int commit_result;
	struct i2c_msg *message;

	// Request packet
	i2cal_start();
	i2cal_writedata_uint8(0x98);
	i2cal_write(windsensorboard.device);
	message = i2cal_read(windsensorboard.device, 10);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	// Parse packet
	windsensorboard.windsensor.last.sample_number = i2cal_readdata_int16(message, 0);
	windsensorboard.windsensor.last.angle_14b = i2cal_readdata_int16(message, 2);
	windsensorboard.windsensor.last.angle = (float)(windsensorboard.windsensor.last.angle_14b) / 0x4000 * 2 * PI;
	windsensorboard.windsensor.last.likelihood_16b32 = i2cal_readdata_int32(message, 4);
	windsensorboard.windsensor.last.likelihood = (float)(windsensorboard.windsensor.last.likelihood_16b32) / 0x10000;
	return commit_result;
}

int windsensorboard_windsensor_calibration_parameters_set() {
	i2cal_start();
	i2cal_writedata_uint8(0x9a);
	i2cal_writedata_uint16(windsensorboard.windsensor.calibration.angle_variance);
	i2cal_writedata_uint16(windsensorboard.windsensor.calibration.angle_shift);
	i2cal_writedata_uint16(windsensorboard.windsensor.calibration.full_evaluation_interval);
	i2cal_write(windsensorboard.device);
	return i2cal_commit();
}

int windsensorboard_windsensor_calibration_parameters_get() {
	int commit_result;
	struct i2c_msg *message;

	// Request packet
	i2cal_start();
	i2cal_writedata_uint8(0x9a);
	i2cal_write(windsensorboard.device);
	message = i2cal_read(windsensorboard.device, 6);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	// Parse packet
	windsensorboard.windsensor.calibration.angle_variance = i2cal_readdata_uint16(message, 0);
	windsensorboard.windsensor.calibration.angle_shift = i2cal_readdata_uint16(message, 2);
	windsensorboard.windsensor.calibration.full_evaluation_interval = i2cal_readdata_uint16(message, 4);
	return commit_result;
}

int windsensorboard_windsensor_calibration_angle_upload(unsigned int angle_index_first, unsigned int angle_index_last) {
	int s;
	int angle_index;
	int commit_result;

	angle_index = angle_index_first & 0x3f;
	angle_index_last &= 0x3f;
	while (1) {
		// Send one record
		i2cal_start();
		i2cal_writedata_uint8(0x9b);
		i2cal_writedata_uint8(1);
		i2cal_writedata_uint8(angle_index);
		for (s = 0; s < 6; s++) {
			i2cal_writedata_uint16(windsensorboard.windsensor.calibration.angle[angle_index].sensor[s].mean);
			i2cal_writedata_uint16(windsensorboard.windsensor.calibration.angle[angle_index].sensor[s].variance);
		}
		i2cal_write(windsensorboard.device);
		commit_result = i2cal_commit();
		if (commit_result < 1) {
			return commit_result;
		}

		// Next angle
		if (angle_index == angle_index_last) {
			break;
		}
		angle_index++;
		angle_index &= 0x3f;
	}

	return 1;
}

int windsensorboard_windsensor_calibration_angle_download(unsigned int angle_index_first, unsigned int angle_index_last) {
	int s;
	int angle_index;
	int commit_result;
	struct i2c_msg *message;

	// Set the read pointer
	i2cal_start();
	i2cal_writedata_uint8(0x9b);
	i2cal_writedata_uint8(0);
	i2cal_writedata_uint8(angle_index_first);
	for (s = 0; s < 6; s++) {
		i2cal_writedata_uint16(0);
		i2cal_writedata_uint16(0);
	}
	i2cal_write(windsensorboard.device);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	angle_index = angle_index_first & 0x3f;
	angle_index_last &= 0x3f;
	while (1) {
		// Read the next record
		i2cal_start();
		i2cal_writedata_uint8(0x9b);
		i2cal_write(windsensorboard.device);
		message = i2cal_read(windsensorboard.device, 26);
		commit_result = i2cal_commit();
		if (commit_result < 1) {
			return commit_result;
		}

		// Parse
		if (i2cal_readdata_uint8(message, 0) != 1) {
			return 0;
		}
		if (i2cal_readdata_uint8(message, 1) != angle_index) {
			return 0;
		}
		for (s = 0; s < 6; s++) {
			windsensorboard.windsensor.calibration.angle[angle_index].sensor[s].mean = i2cal_readdata_uint16(message, s * 4 + 2);
			windsensorboard.windsensor.calibration.angle[angle_index].sensor[s].variance = i2cal_readdata_uint16(message, s * 4 + 4);
		}

		// Next angle
		if (angle_index == angle_index_last) {
			break;
		}
		angle_index++;
		angle_index &= 0x3f;
	}

	return 1;
}

int windsensorboard_windsensor_calibrate(unsigned int angle_index) {
	int s;

	// Start calibrating
	i2cal_start();
	i2cal_writedata_uint8(0x9b);
	i2cal_writedata_uint8(2);
	i2cal_writedata_uint8(angle_index);
	for (s = 0; s < 6; s++) {
		i2cal_writedata_uint16(0);
		i2cal_writedata_uint16(0);
	}
	i2cal_write(windsensorboard.device);
	return i2cal_commit();
}

int windsensorboard_windsensor_start() {
	i2cal_start();
	i2cal_writedata_uint8(0x9c);
	i2cal_writedata_uint8(1);
	i2cal_write(windsensorboard.device);
	return i2cal_commit();
}

int windsensorboard_windsensor_stop() {
	i2cal_start();
	i2cal_writedata_uint8(0x9c);
	i2cal_writedata_uint8(0);
	i2cal_write(windsensorboard.device);
	return i2cal_commit();
}

int windsensorboard_windsensor_mode_get() {
	int commit_result;
	struct i2c_msg *message;

	// Request packet
	i2cal_start();
	i2cal_writedata_uint8(0x9c);
	i2cal_write(windsensorboard.device);
	message = i2cal_read(windsensorboard.device, 1);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	// Parse packet
	windsensorboard.windsensor.mode = i2cal_readdata_uint8(message, 0);
	return commit_result;
}

int windsensorboard_led_mode_set() {
	i2cal_start();
	i2cal_writedata_uint8(0xa0);
	i2cal_writedata_uint8(windsensorboard.led.mode);
	i2cal_write(windsensorboard.device);
	return i2cal_commit();
}

int windsensorboard_led_mode_get() {
	int commit_result;
	struct i2c_msg *message;

	// Request packet
	i2cal_start();
	i2cal_writedata_uint8(0xa0);
	i2cal_write(windsensorboard.device);
	message = i2cal_read(windsensorboard.device, 1);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	// Parse packet
	windsensorboard.led.mode = i2cal_readdata_uint8(message, 0);
	return commit_result;
}

int windsensorboard_led_code_set() {
	i2cal_start();
	i2cal_writedata_uint8(0xa1);
	i2cal_writedata_uint16(windsensorboard.led.code.length);
	i2cal_writedata_uint16(windsensorboard.led.code.id);
	i2cal_writedata_uint16(windsensorboard.led.code.chip_length);
	i2cal_write(windsensorboard.device);
	return i2cal_commit();
}

int windsensorboard_led_code_get() {
	int commit_result;
	struct i2c_msg *message;

	// Request packet
	i2cal_start();
	i2cal_writedata_uint8(0xa1);
	i2cal_write(windsensorboard.device);
	message = i2cal_read(windsensorboard.device, 6);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	// Parse packet
	windsensorboard.led.code.length = i2cal_readdata_uint16(message, 0);
	windsensorboard.led.code.id = i2cal_readdata_uint16(message, 2);
	windsensorboard.led.code.chip_length = i2cal_readdata_uint16(message, 4);
	return commit_result;
}

int windsensorboard_led_external_set() {
	i2cal_start();
	i2cal_writedata_uint8(0xa2);
	i2cal_writedata_uint8(windsensorboard.led.external);
	i2cal_write(windsensorboard.device);
	return i2cal_commit();
}

int windsensorboard_led_external_get() {
	int commit_result;
	struct i2c_msg *message;

	// Request packet
	i2cal_start();
	i2cal_writedata_uint8(0xa2);
	i2cal_write(windsensorboard.device);
	message = i2cal_read(windsensorboard.device, 1);
	commit_result = i2cal_commit();
	if (commit_result < 1) {
		return commit_result;
	}

	// Parse packet
	windsensorboard.led.external = i2cal_readdata_uint8(message, 0);
	return commit_result;
}
