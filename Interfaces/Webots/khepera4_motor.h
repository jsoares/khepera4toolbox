#ifndef KHEPERA4_MOTOR_H
#define KHEPERA4_MOTOR_H

//! One motor
struct sKhepera4Motor {
	int current_speed;					//!< Current speed (updated by khepera4_motor_get_current_speed)
	int current_position;				//!< Current position (updated by khepera4_motor_get_current_position)
	//int current_torque;				//!< Current torque (updated by khepera4_motor_get_current_torque)
};

#endif
