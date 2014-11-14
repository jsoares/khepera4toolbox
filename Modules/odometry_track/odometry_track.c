/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "odometry_track.h"
#include "khepera3.h"
#include "nmea.h"
#include "errno.h"
		
#define PI 3.14159265358979

void odometry_track_init() {
}

int odometry_track_start(struct sOdometryTrack * ot) {
	khepera3_drive_get_current_position();
	return odometry_track_start_pos(ot, -khepera3.motor_left.current_position, khepera3.motor_right.current_position);
}

int odometry_track_start_pos(struct sOdometryTrack * ot, int pos_left, int pos_right) {
	ot->result.x = 0;
	ot->result.y = 0;
	ot->result.theta = 0;

	ot->state.pos_left_prev = pos_left;
	ot->state.pos_right_prev = pos_right;

	// Odometry: default values
	ot->configuration.is_default = 1;
	ot->configuration.wheel_distance = 0.08841;
	ot->configuration.wheel_conversion_left = 2 * PI * 0.021 / 2764.8;
	ot->configuration.wheel_conversion_right = 2 * PI * 0.021 / 2764.8;

	// Read the odometry configuration file
	odometry_track_read_configuration(ot, "/etc/khepera/odometry");
	return 1;
}

struct sOdometryTrack * odometry_track_read_configuration_odometry_track = 0;

void odometry_track_read_configuration_process_message(struct sNMEAMessage *m, int withchecksum) {
	if ((strcmp(m->command, "ODOMETRY") == 0) && (m->argument_count >= 3)) {
		odometry_track_read_configuration_odometry_track->configuration.is_default = 0;
		odometry_track_read_configuration_odometry_track->configuration.wheel_distance = atof(m->argument[0]);
		odometry_track_read_configuration_odometry_track->configuration.wheel_conversion_left = atof(m->argument[1]);
		odometry_track_read_configuration_odometry_track->configuration.wheel_conversion_right = atof(m->argument[2]);
	}
}

void odometry_track_read_configuration(struct sOdometryTrack * ot, const char *filename) {
	struct sNMEAParser parser;
	FILE *file;
	char buffer[128];
	int len;

	// Initialize the NMEA parser
	nmea_parser_init(&parser);
	odometry_track_read_configuration_odometry_track = ot;
	parser.hook_process_message = odometry_track_read_configuration_process_message;

	// Open the file
	file = fopen(filename, "r");
	if (file == NULL) {
		return;
	}

	// Read the whole file
	while ((len = fread(buffer, 1, 128, file)) > 0) {
		nmea_parser_process_data(&parser, buffer, len);
	}

	// Close the file
	fclose(file);
}

void odometry_track_step(struct sOdometryTrack * ot) {
	khepera3_drive_get_current_position();
	odometry_track_step_pos(ot, -khepera3.motor_left.current_position, khepera3.motor_right.current_position);
}

void odometry_track_step_pos(struct sOdometryTrack * ot, int pos_left, int pos_right) {
	long delta_pos_left, delta_pos_right;
	float delta_left, delta_right, delta_theta, theta2;
	float delta_x, delta_y;

	delta_pos_left = pos_left - ot->state.pos_left_prev;
	delta_pos_right = pos_right - ot->state.pos_right_prev;
	delta_left = delta_pos_left * ot->configuration.wheel_conversion_left;
	delta_right = delta_pos_right * ot->configuration.wheel_conversion_right;
	delta_theta = (delta_left - delta_right) / ot->configuration.wheel_distance;
	theta2 = ot->result.theta + delta_theta * 0.5;
	delta_x = (delta_left + delta_right) * 0.5 * cosf(theta2);
	delta_y = (delta_left + delta_right) * 0.5 * sinf(theta2);

	ot->result.x += delta_x;
	ot->result.y += delta_y;
	ot->result.theta += delta_theta;
	ot->state.pos_left_prev = pos_left;
	ot->state.pos_right_prev = pos_right;
}
