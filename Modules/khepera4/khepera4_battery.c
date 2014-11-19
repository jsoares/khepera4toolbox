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


#include <assert.h>
#include "khepera4_battery.h"
#include "khepera4.h"

/*** Module initialization ***/

void khepera4_battery_init() {
}

/*** Low-level functions ***/

int khepera4_battery_p(struct sKhepera4Battery *battery) {
    unsigned char buf[12];

    if (!khepera_dspic_read_register(I2C_BAT_STATUS, buf, 12)) {
        return 0;
    }

    battery->status = i2cal_readbuffer_uint8(buf, 0);
    battery->capacity_remaining_absolute = i2cal_readbuffer_uint16(buf, 1);
    battery->capacity_remaining_relative = i2cal_readbuffer_uint8(buf, 3);
    battery->current = (int) ((float) i2cal_readbuffer_int16(buf, 4))*0.07813;
    battery->current_average = (int) ((float) i2cal_readbuffer_int16(buf, 6))*0.07813;
    battery->temperature = ((float) i2cal_readbuffer_uint16(buf, 8))*0.003906;
    battery->voltage = (int) ((float) i2cal_readbuffer_uint16(buf, 10))*9.76;

    return -1;
}

/*** High-level functions ***/

int khepera4_battery() {
    return khepera4_battery_p(&(khepera4.battery));
}
