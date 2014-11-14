/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef ODOMETRY_TRACK
#define ODOMETRY_TRACK

struct sOdometryTrack {
	struct {
		int is_default;
		float wheel_distance;
		float wheel_conversion_left;
		float wheel_conversion_right;
	} configuration;
	struct {
		int pos_left_prev;
		int pos_right_prev;
	} state;
	struct {
		float x;
		float y;
		float theta;
	} result;
};

//! Initializes this module.
void odometry_track_init();

//! Initializes an sOdometryTrack structure using the current motor positions.
int odometry_track_start(struct sOdometryTrack * ot);
//! Initializes an sOdometryTrack structure using the given left and right position.
int odometry_track_start_pos(struct sOdometryTrack * ot, int pos_left, int pos_right);
//! Updates the position using the current motor positions.
void odometry_track_step(struct sOdometryTrack * ot);
//! Updates the position using the given left and right position.
void odometry_track_step_pos(struct sOdometryTrack * ot, int pos_left, int pos_right);

//! Reads an odometry configuration file. This function is automatically called (with /etc/khepera/odometry) by odometry_track_start and odometry_track_start_pos.
void odometry_track_read_configuration(struct sOdometryTrack * ot, const char *filename);

#endif
