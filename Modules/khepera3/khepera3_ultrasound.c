/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "khepera3_ultrasound.h"
#include "khepera3.h"

/*** Module initialization ***/

void khepera3_ultrasound_init() {
	khepera3.ultrasound.distance_per_increment = 0.01;
}

/*** Low-level functions ***/

int khepera3_ultrasound_p(struct sKhepera3SensorsUltrasound *result) {
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

int khepera3_ultrasound() {
	return khepera3_ultrasound_p(&(khepera3.ultrasound));
}

int khepera3_ultrasound_enable(enum eKhepera3SensorsUltrasoundBit bitmask) {
	return khepera_dspic_write_register8(I2C_US_ACTIVE, bitmask);
}
