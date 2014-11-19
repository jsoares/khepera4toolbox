/******************************************************************************
 *
 * Khepera IV Toolbox
 * (c) 2014 EPFL, Lausanne, Switzerland
 * Authors: Jorge Soares et al.
 *
 * Code forked from Khepera III Toolbox
 * (c) 2006 EPFL, Lausanne, Switzerland
 * Authors: Thomas Lochmatter et al.
 *
 * Additional DISAL libraries
 * (c) 2006 EPFL, Lausanne, Switzerland
 * Authors: Thomas Lochmatter, Jim Pugh, Sven Gowal, Pascal Gilbert, and others
 *
 ******************************************************************************/


#include <stdio.h>
#include "odorboard.h"

void odorboard_init() {
    i2cal_init();
    i2c_stream_init();
    odorboard.device = 0x50;
    odorboard.voc.stream.configuration.device = odorboard.device;
    odorboard.voc.stream.configuration.reg = 0x91;
    odorboard.voc.stream.configuration.block_size = 8;
    odorboard.voc.stream.configuration.blocks_count_max = 8;
    odorboard.sht11.stream.configuration.device = odorboard.device;
    odorboard.sht11.stream.configuration.reg = 0x99;
    odorboard.sht11.stream.configuration.block_size = 8;
    odorboard.sht11.stream.configuration.blocks_count_max = 8;
}

int odorboard_voc_get() {
    int commit_result;
    struct i2c_msg *message;

    // Request packet
    i2cal_start();
    i2cal_writedata_uint8(0x90);
    i2cal_write(odorboard.device);
    message = i2cal_read(odorboard.device, 8);
    commit_result = i2cal_commit();
    if (commit_result < 1) {
        return -1;
    }

    // Parse packet
    odorboard.voc.last.sensor[0] = i2cal_readdata_uint16(message, 0);
    odorboard.voc.last.sensor[1] = i2cal_readdata_uint16(message, 2);
    odorboard.voc.last.sensor[2] = i2cal_readdata_uint16(message, 4);
    odorboard.voc.last.sample_number = i2cal_readdata_uint16(message, 6);
    return commit_result;
}

int odorboard_voc_integration_set(unsigned int integration) {
    i2cal_start();
    i2cal_writedata_uint8(0x92);
    i2cal_writedata_uint8(integration);
    i2cal_write(odorboard.device);
    return i2cal_commit();
}

int odorboard_voc_integration_get() {
    int commit_result;
    struct i2c_msg *message;

    // Request packet
    i2cal_start();
    i2cal_writedata_uint8(0x92);
    i2cal_write(odorboard.device);
    message = i2cal_read(odorboard.device, 1);
    commit_result = i2cal_commit();
    if (commit_result < 1) {
        return commit_result;
    }

    // Parse packet
    odorboard.voc.integration = i2cal_readdata_uint8(message, 0);
    return commit_result;
}

int odorboard_sht11_get() {
    int commit_result;
    struct i2c_msg *message;

    // Request packet
    i2cal_start();
    i2cal_writedata_uint8(0x98);
    i2cal_write(odorboard.device);
    message = i2cal_read(odorboard.device, 14);
    commit_result = i2cal_commit();
    if (commit_result < 1) {
        return commit_result;
    }

    // Parse packet
    odorboard.sht11.last.temperature = i2cal_readdata_float(message, 0);
    odorboard.sht11.last.humidity = i2cal_readdata_float(message, 4);
    odorboard.sht11.last.dewpoint_temperature = i2cal_readdata_float(message, 8);
    odorboard.sht11.last.timestamp = i2cal_readdata_uint16(message, 12);
    return commit_result;
}

int odorboard_sht11_set_interval(unsigned int interval) {
    i2cal_start();
    i2cal_writedata_uint8(0x9a);
    i2cal_writedata_uint8(interval);
    i2cal_write(odorboard.device);
    return i2cal_commit();
}

int odorboard_pump_speed_set(unsigned int pump_id, unsigned int speed) {
    i2cal_start();
    i2cal_writedata_uint8(0x8c);
    i2cal_writedata_uint16(pump_id);
    i2cal_writedata_uint16(speed);
    i2cal_write(odorboard.device);
    return i2cal_commit();
}

int odorboard_pump_speed_get() {
    int commit_result;
    struct i2c_msg *message;

    // Request packet
    i2cal_start();
    i2cal_writedata_uint8(0x8c);
    i2cal_write(odorboard.device);
    message = i2cal_read(odorboard.device, 6);
    commit_result = i2cal_commit();
    if (commit_result < 1) {
        return commit_result;
    }

    // Parse packet
    odorboard.pump.speed[0] = i2cal_readdata_uint16(message, 0);
    odorboard.pump.speed[1] = i2cal_readdata_uint16(message, 2);
    odorboard.pump.speed[2] = i2cal_readdata_uint16(message, 4);
    return commit_result;
}

int odorboard_led_mode_set() {
    i2cal_start();
    i2cal_writedata_uint8(0xa0);
    i2cal_writedata_uint8(odorboard.led.mode);
    i2cal_write(odorboard.device);
    return i2cal_commit();
}

int odorboard_led_mode_get() {
    int commit_result;
    struct i2c_msg *message;

    // Request packet
    i2cal_start();
    i2cal_writedata_uint8(0xa0);
    i2cal_write(odorboard.device);
    message = i2cal_read(odorboard.device, 1);
    commit_result = i2cal_commit();
    if (commit_result < 1) {
        return commit_result;
    }

    // Parse packet
    odorboard.led.mode = i2cal_readdata_uint8(message, 0);
    return commit_result;
}

int odorboard_led_external_set() {
    i2cal_start();
    i2cal_writedata_uint8(0xa1);
    i2cal_writedata_uint8(odorboard.led.external);
    i2cal_write(odorboard.device);
    return i2cal_commit();
}

int odorboard_led_external_get() {
    int commit_result;
    struct i2c_msg *message;

    // Request packet
    i2cal_start();
    i2cal_writedata_uint8(0xa1);
    i2cal_write(odorboard.device);
    message = i2cal_read(odorboard.device, 1);
    commit_result = i2cal_commit();
    if (commit_result < 1) {
        return commit_result;
    }

    // Parse packet
    odorboard.led.external = i2cal_readdata_uint8(message, 0);
    return commit_result;
}
