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


#ifndef ODOMETRY_GOTO
#define ODOMETRY_GOTO

#include "odometry_track.h"

struct sOdometryGoto {

    struct {
        int acceleration_step;
        float speed_max;
        float speed_min;
    } configuration;
    struct sOdometryTrack * track;

    struct {
        float goal_x;
        float goal_y;
        int speed_left_internal;
        int speed_right_internal;
    } state;

    struct {
        int speed_left;
        int speed_right;
        int closetogoal;
        int veryclosetogoal;
        int atgoal;
    } result;
};

//! Initializes this module.
void odometry_goto_init();

//! Initializes a goto structure.
void odometry_goto_start(struct sOdometryGoto * og, struct sOdometryTrack * ot);
//! Sets a new target position.
void odometry_goto_set_goal(struct sOdometryGoto * og, float goalx, float goaly);
//! Calculates the new motor speeds.
void odometry_goto_step(struct sOdometryGoto * og);

#endif
