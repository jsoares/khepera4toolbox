#ifndef KHEPERA4_DRIVE_H
#define KHEPERA4_DRIVE_H

//! Enables webots encoders
void khepera4_drive_init();
//! Sets motors speed to 0
void khepera4_drive_stop();
//! Does nothing as webots does not need to get motors enabled
void khepera4_drive_start();
//! Sets each motor speed and saves that current speed in khepera4 motor structure
void khepera4_drive_set_speed(int speed_left, int speed_right);
//! Sets differential by executing khepera4_drive_set_speed(int speed_left, int speed_right)
void khepera4_drive_set_speed_differential(float speed, float forward_coefficient, float differential_coefficient);
//! Sets differential bounded speed by executing khepera4_drive_set_speed(int speed_left, int speed_right)
void khepera4_drive_set_speed_differential_bounded(float speed, float forward_coefficient, float forward_coefficient_max, float differential_coefficient, float differential_coefficient_max);
//! Sets encoders position and saves that current position in khepera4 motor structure
void khepera4_drive_set_current_position(int position_left, int position_right);
//! Does nothing as last speed set is always saved in khepera4 motor structure
void khepera4_drive_get_current_speed();
//! Gets encoders position
void khepera4_drive_get_current_position();

#endif
