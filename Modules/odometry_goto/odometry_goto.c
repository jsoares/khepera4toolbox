/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "odometry_goto.h"
#include <stdlib.h>
#include <math.h>
#define PI 3.14159265358979

void odometry_goto_init() {
}

void odometry_goto_start(struct sOdometryGoto * og, struct sOdometryTrack * ot) {
	og->track = ot;
	og->configuration.acceleration_step = 500.;
	og->configuration.speed_min = 800.;
	og->configuration.speed_max = 20000.;
	og->state.goal_x = 0.;
	og->state.goal_y = 0.;
	og->state.speed_left_internal = 0;
	og->state.speed_right_internal = 0;
	og->result.speed_left = 0;
	og->result.speed_right = 0;
	og->result.atgoal = 1;
	og->result.closetogoal = 1;
	og->result.veryclosetogoal = 1;
}

void odometry_goto_set_goal(struct sOdometryGoto * og, float x, float y) {
	og->state.goal_x = x;
	og->state.goal_y = y;
	og->result.atgoal = 0;
	og->result.closetogoal = 0;
	og->result.veryclosetogoal = 0;
}

void odometry_goto_step(struct sOdometryGoto * og) {
	float dx, dy;
	float distance, goalangle, alpha;
	float speedfactor;
	long speed_left_wish, speed_right_wish;
	int atmaxspeed = 0;

	// Do nothing if we are at goal
	if (og->result.atgoal != 0) {
		return;
	}

	// Calculate new wish speeds
	dx = og->state.goal_x - og->track->result.x;
	dy = og->state.goal_y - og->track->result.y;
	distance = sqrt(dx * dx + dy * dy);
	goalangle = atan2(dy, dx);
	alpha = goalangle - og->track->result.theta;
	while (alpha > PI) {
		alpha -= 2 * PI;
	}
	while (alpha < -PI) {
		alpha += 2 * PI;
	}

	// Calculate the speed factor
	speedfactor = (distance + 0.05) * 10. * og->configuration.speed_max;
	if (speedfactor > og->configuration.speed_max) {
		speedfactor = og->configuration.speed_max;
		atmaxspeed = 1;
	}

	// Calculate the theoretical speed
	//printf("dist %f - goalangle %f - alpha %f \n", distance, goalangle, alpha);
	speed_left_wish = speedfactor * (PI - 2 * abs(alpha) + alpha) / PI + 0.5;
	speed_right_wish = speedfactor * (PI - 2 * abs(alpha) - alpha) / PI + 0.5;

	// Close to termination condition: just stop
	if (og->result.veryclosetogoal) {
		speed_left_wish = 0;
		speed_right_wish = 0;
	}

	// Limit acceleration
	if (speed_left_wish > og->state.speed_left_internal) {
		og->state.speed_left_internal += og->configuration.acceleration_step;
	}
	if (speed_left_wish < og->state.speed_left_internal) {
		og->state.speed_left_internal -= og->configuration.acceleration_step;
	}
	if (speed_right_wish > og->state.speed_right_internal) {
		og->state.speed_right_internal += og->configuration.acceleration_step;
	}
	if (speed_right_wish < og->state.speed_right_internal) {
		og->state.speed_right_internal -= og->configuration.acceleration_step;
	}

	// Don't set speeds < MIN_SPEED (for accuracy reasons)
	og->result.speed_left = og->state.speed_left_internal;
	og->result.speed_right = og->state.speed_right_internal;
	if (abs(og->result.speed_left) < og->configuration.speed_min) {
		og->result.speed_left = 0;
	}
	if (abs(og->result.speed_right) < og->configuration.speed_min) {
		og->result.speed_right = 0;
	}

	// Termination condition
	if (atmaxspeed == 0) {
		og->result.closetogoal = 1;
		if ((og->result.speed_left == 0) || (og->result.speed_right == 0)) {
			og->result.veryclosetogoal = 1;
		}
		if ((og->result.speed_left == 0) && (og->result.speed_right == 0)) {
			og->result.atgoal = 1;
		}
	}
}
