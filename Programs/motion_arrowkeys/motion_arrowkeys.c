/*!
 * (c) 2006-2007 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "motion_arrowkeys.h"
#include <stdio.h>
#include <math.h>

void motion_arrowkeys_init(struct sMotionArrowKeys * am) {
	int i;

	am->speed = 0;
	am->direction = 0;
	am->speedstep = 10;
	am->directionstep = 1000;
	am->speedlimit = 50000;

	am->directiontime_write = 0;
	for (i = 0; i < 16; i++) {
		am->directiontime[i] = 0;
	}
}

void motion_arrowkeys_update(struct sMotionArrowKeys * am, int key, int timediff) {
	int i;

	// Update directiontime
	for (i = 0; i < 16; i++) {
		if (am->directiontime[i] > 0) {
			am->directiontime[i] += timediff;
			if (am->directiontime[i] > 1000000) {
				am->directiontime[i] = 0;
			}
		} else if (am->directiontime[i] < 0) {
			am->directiontime[i] -= timediff;
			if (am->directiontime[i] < -1000000) {
				am->directiontime[i] = 0;
			}
		}
	}

	// Adapt speed according to input key
	if ((key == 's') || (key == '5')) {
		am->speed = 0;
		for (i = 0; i < 16; i++) {
			am->directiontime[i] = 0;
		}
	} else if ((key == 'w') || (key == '8')) {
		am->speed += am->speedstep;
		if (am->speed > am->speedlimit) {
			am->speed = am->speedlimit;
		}
	} else if ((key == 'x') || (key == '2')) {
		am->speed -= am->speedstep;
		if (am->speed < -am->speedlimit) {
			am->speed = -am->speedlimit;
		}
	} else if ((key == 'q') || (key == '7')) {
	} else if ((key == 'a') || (key == '4')) {
		am->directiontime[am->directiontime_write] = -1;
		am->directiontime_write++;
		am->directiontime_write &= 15;
	} else if ((key == 'y') || (key == '1')) {
	} else if ((key == 'e') || (key == '9')) {
	} else if ((key == 'd') || (key == '6')) {
		am->directiontime[am->directiontime_write] = 1;
		am->directiontime_write++;
		am->directiontime_write &= 15;
	} else if ((key == 'c') || (key == '3')) {
	} else if (key == '+') {
		am->speedstep++;
	} else if (key == '-') {
		am->speedstep--;
	} else if (key == -1) {
	} else if (key > 0) {
		printf(" [unknown key %d]", key);
	}

	// Derive new direction
	am->direction = 0;
	for (i = 0; i < 16; i++) {
		if (am->directiontime[i] > 0) {
			am->direction += am->directionstep;
		} else if (am->directiontime[i] < 0) {
			am->direction -= am->directionstep;
		}
	}
}

int motion_arrowkeys_getspeedleft(struct sMotionArrowKeys * am) {
	float speed = am->speed;
	speed += am->direction;
	return (int)floorf(speed + 0.5);
}

int motion_arrowkeys_getspeedright(struct sMotionArrowKeys * am) {
	float speed = am->speed;
	speed -= am->direction;
	return (int)floorf(speed + 0.5);
}
