/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef KHEPERA4_DRIVE
#define KHEPERA4_DRIVE

#include "i2cal.h"

// Regulation modes

enum kh4RegType {
    kh4RegIdle = 0,
    kh4RegSpeed = 1,
    kh4RegSpeedProfile = 2,
    kh4RegPosition = 3,
    kh4RegSOpenLoop = 4,
};

struct sKhepera4Motor {
    int i2c_address; //!< I2C bus address
    int direction; //!< -1 to invert the direction, otherwise +1
    unsigned int firmware_version; //!< Firmware version of the motor controller (updated by khepera4_motor_firmware_version, example: 104 = 0x68 => version 6, revision 8)
    int current_speed; //!< Current speed (updated by khepera4_motor_get_current_speed)
    int current_position; //!< Current position (updated by khepera4_motor_get_current_position)
    int current_torque; //!< Current torque (updated by khepera4_motor_get_current_torque)
};

//! (private) Initializes this submodule.
void khepera4_drive_init();

//! Stops both motors immediately. Transfer on I2C bus: 6 bytes.
void khepera4_drive_stop();
//! Puts both motors in normal (control) mode. Transfer on I2C bus: 6 bytes.
void khepera4_drive_start();
//! Puts both motors in idle mode. Transfer on I2C bus: 6 bytes.
void khepera4_drive_idle();

//! Configure the PID
void khepera4_drive_set_pid(int kp, int ki, int kd);
//! Configure the speed profile
void khepera4_drive_set_profile(int acc_inc, int acc_div, int min_speed_acc, int min_speed_dec);
//! Configure the position control
void khepera4_drive_set_position_control(int pos_margin, int pos_speed);

//! Sets the motor speeds. Transfer on I2C bus: 24 - 30 bytes.
void khepera4_drive_set_speed(int speed_left, int speed_right);
//! Sets the motor speeds using forward and a differential coefficient. The speed of the left wheel is set to speed * (forward_coefficient + differential_coefficient), whereas the right wheel speed is set to speed * (forward_coefficient - differential_coefficient). Transfer on I2C bus: 24 - 30 bytes.
void khepera4_drive_set_speed_differential(float speed, float forward_coefficient, float differential_coefficient);
//! Same as khepera4_drive_set_speed_differential, but the coefficients are bounded by -*_coefficient_max and *_coefficient_max. Transfer on I2C bus: 24 - 30 bytes.
void khepera4_drive_set_speed_differential_bounded(float speed, float forward_coefficient, float forward_coefficient_max, float differential_coefficient, float differential_coefficient_max);
//! Sets the motor speeds and uses the acceleration profile to reach that speed. Transfer on I2C bus: 24 - 30 bytes.
void khepera4_drive_set_speed_using_profile(int speed_left, int speed_right);

//! Goes to a specific motor position. Transfer on I2C bus: 24 - 30 bytes.
void khepera4_drive_goto_position(int position_left, int position_right);

//! Resets the encoder position. Transfer on I2C bus: 16 bytes
void khepera4_drive_reset_position();

//! Reads the current speed and updates the corresponding value the motor structures. Transfer on I2C bus: 32 bytes.
void khepera4_drive_get_current_speed();
//! Reads the current position and updates the corresponding value the motor structures. Transfer on I2C bus: 32 bytes.
void khepera4_drive_get_current_position();

#endif
