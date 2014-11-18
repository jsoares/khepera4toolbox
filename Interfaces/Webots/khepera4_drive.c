#include "khepera4_drive.h"
#include "khepera4.h"

void khepera4_drive_init() {
	differential_wheels_enable_encoders(CONTROL_STEP);
}

void khepera4_drive_stop() {
	differential_wheels_set_speed(0, 0);
}

void khepera4_drive_start() {}

void khepera4_drive_set_speed(int speed_left, int speed_right) {
	differential_wheels_set_speed(speed_left, speed_right);
	khepera4.motor_left.current_speed = speed_left;
	khepera4.motor_right.current_speed = speed_right;
}

void khepera4_drive_set_speed_differential(float speed, float forward_coefficient, float differential_coefficient) {
	float speed_left;
	float speed_right;

	speed_left = speed * (forward_coefficient + differential_coefficient);
	speed_right = speed * (forward_coefficient - differential_coefficient);
	khepera4_drive_set_speed((int)(speed_left + 0.5), (int)(speed_right + 0.5));
}

void khepera4_drive_set_speed_differential_bounded(float speed, float forward_coefficient, float forward_coefficient_max, float differential_coefficient, float differential_coefficient_max) {
	if (forward_coefficient > forward_coefficient_max) {
		forward_coefficient = forward_coefficient_max;
	}
	if (forward_coefficient < -forward_coefficient_max) {
		forward_coefficient = -forward_coefficient_max;
	}

	if (differential_coefficient > differential_coefficient_max) {
		differential_coefficient = differential_coefficient_max;
	}
	if (differential_coefficient < -differential_coefficient_max) {
		differential_coefficient = -differential_coefficient_max;
	}

	khepera4_drive_set_speed_differential(speed, forward_coefficient, differential_coefficient);
}

void khepera4_drive_set_current_position(int position_left, int position_right) {
	differential_wheels_set_encoders(position_left, position_right);
	khepera4.motor_left.current_position = position_left;
	khepera4.motor_right.current_position = position_right;
}

void khepera4_drive_get_current_speed() {}

void khepera4_drive_get_current_position() {
	khepera4.motor_left.current_position = differential_wheels_get_left_encoder();
	khepera4.motor_right.current_position = differential_wheels_get_right_encoder();
}
