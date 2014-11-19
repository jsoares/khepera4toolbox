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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "khepera4_ultrasound.h"
#include "khepera4.h"

/*** Module initialization ***/

void khepera4_ultrasound_init() {
    khepera4.ultrasound.distance_per_increment = 0.01;
}

/*** Low-level functions ***/

int khepera4_ultrasound_p(struct sKhepera4SensorsUltrasound *result) {
    unsigned char buf[10];
    int i;

    // Arguments/environment checks
    assert(result != 0);

    // Read sensor values
    if (!khepera_dspic_read_register(I2C_US_L_L, buf, 10)) {
        return 0;
    }

    // Decode sensor values
    for (i = 0; i < 12; i++) {
        result->sensor[i] = i2cal_readbuffer_int16(buf, i * 2);
    }

    // Decode timestamp
    result->timestamp = time(NULL);

    // Everything OK
    return -1;
}

/*** High-level functions ***/

int khepera4_ultrasound() {
    return khepera4_ultrasound_p(&(khepera4.ultrasound));
}

int khepera4_ultrasound_enable(enum eKhepera4SensorsUltrasoundBit bitmask) {
    return khepera_dspic_write_register8(I2C_US_ACTIVE, bitmask);
}
