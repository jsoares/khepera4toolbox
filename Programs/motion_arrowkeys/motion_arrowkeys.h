/*!
 * (c) 2006-2007 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

struct sMotionArrowKeys {
	float speed;
	float direction;
	float speedstep;
	float speedlimit;
	float directionstep;
	int directiontime[16];
	int directiontime_write;
};

void motion_arrowkeys_init(struct sMotionArrowKeys * motion_arrowkeys);
void motion_arrowkeys_update(struct sMotionArrowKeys * motion_arrowkeys, int key, int timediff);
int motion_arrowkeys_getspeedleft(struct sMotionArrowKeys * motion_arrowkeys);
int motion_arrowkeys_getspeedright(struct sMotionArrowKeys * motion_arrowkeys);
