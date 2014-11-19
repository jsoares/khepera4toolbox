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
