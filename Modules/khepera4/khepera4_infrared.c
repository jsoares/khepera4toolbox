/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include <assert.h>
#include <stdlib.h>
#include "khepera4_infrared.h"
#include "khepera4.h"

/*** Module initialization ***/

void khepera4_infrared_init() {
}

/*** Low-level functions ***/

int khepera4_infrared_p(struct sKhepera4SensorsInfrared *result, char infrared_command) {
    unsigned char buf[24];
    int i, reg;

    // Arguments/environment checks
    assert(result != 0);
    assert((infrared_command == 'P') || (infrared_command == 'A'));

    // Send infrared command (P or A) and read 24 bytes
    reg = (infrared_command == 'P') ? I2C_IR_DIST_BL_L : I2C_IR_AMB_BL_L;

    if (!khepera_dspic_read_register(reg, buf, 24)) {
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

int khepera4_infrared_ambient() {
    return khepera4_infrared_p(&(khepera4.infrared_ambient), 'A');
}

int khepera4_infrared_ambient_p(struct sKhepera4SensorsInfrared *result) {
    return khepera4_infrared_p(result, 'A');
}

int khepera4_infrared_proximity() {
    return khepera4_infrared_p(&(khepera4.infrared_proximity), 'P');
}

int khepera4_infrared_proximity_p(struct sKhepera4SensorsInfrared *result) {
    return khepera4_infrared_p(result, 'P');
}
