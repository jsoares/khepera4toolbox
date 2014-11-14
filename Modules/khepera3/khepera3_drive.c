/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include <assert.h>
#include <math.h>
#include <string.h>
#include "khepera3_drive.h"
#include "khepera3.h"

/*** Module initialization ***/

void khepera3_drive_init() {
    khepera3.motor_left.direction = -1;
    khepera3.motor_right.direction = 1;
}

/*** Functions to set/get values on both motors ***/

void khepera3_drive_stop() {
    khepera3_drive_set_speed(0,0);
}

void khepera3_drive_start() {
    /khepera3_drive_set_pid(10, 5, 1);
    khepera3_drive_set_profile(3, 0, 20, 1);
    khepera3_drive_set_position_control(20, 400);
    khepera_dspic_write_register8(I2C_MOT_CONTROL_TYPE, kh4RegIdle); // set mode idle
}

void khepera3_drive_idle() {
	khepera_dspic_write_register8(I2C_MOT_CONTROL_TYPE, kh4RegIdle);
}

void khepera3_drive_set_pid(int kp, int ki, int kd) {
    khepera_dspic_write_register8(I2C_MOT_KP, kp);
    khepera_dspic_write_register8(I2C_MOT_KI, ki);
    khepera_dspic_write_register8(I2C_MOT_KD, kd);
}

void khepera3_drive_set_profile(int acc_inc, int acc_div, int min_speed_acc, int min_speed_dec) {
    khepera_dspic_write_register8(I2C_MOT_ACC_INC, acc_inc);
    khepera_dspic_write_register8(I2C_MOT_ACC_DIV, acc_div);
    khepera_dspic_write_register8(I2C_MOT_MIN_SPEED_ACC, min_speed_acc);
    khepera_dspic_write_register8(I2C_MOT_MIN_SPEED_DEC, min_speed_dec);
}

void khepera3_drive_set_position_control(int pos_margin, int pos_speed) {
    khepera_dspic_write_register16(I2C_MOT_POS_MAR_L, pos_margin);
    khepera_dspic_write_register16(I2C_MOT_SPEED_POS_L, pos_speed);
}

void khepera3_drive_set_speed(int speed_left, int speed_right) {
    unsigned char buf[4];

    khepera_dspic_write_register8(I2C_MOT_CONTROL_TYPE, kh4RegSpeed);

    buf[0] = speed_left & 0xff;
    buf[1] = (speed_left >> 8) & 0xff;
    buf[2] = speed_right & 0xff;
    buf[3] = (speed_right >> 8) & 0xff;

    khepera_dspic_write_register(I2C_MOT_LEFT_SPEED_CONSIGN_L,buf,4);
}

void khepera3_drive_set_speed_differential(float speed, float forward_coefficient, float differential_coefficient) {
	float speed_left;
	float speed_right;

	speed_left = speed * (forward_coefficient + differential_coefficient);
	speed_right = speed * (forward_coefficient - differential_coefficient);
	//printf("$SPEED,%f,%f\n", speed_left, speed_right);
	khepera3_drive_set_speed((int)floorf(speed_left + 0.5), (int)floorf(speed_right + 0.5));
}

void khepera3_drive_set_speed_differential_bounded(float speed, float forward_coefficient, float forward_coefficient_max, float differential_coefficient, float differential_coefficient_max) {
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

	khepera3_drive_set_speed_differential(speed, forward_coefficient, differential_coefficient);
}

void khepera3_drive_set_speed_using_profile(int speed_left, int speed_right) {
    unsigned char buf[4];

    khepera_dspic_write_register8(I2C_MOT_CONTROL_TYPE, kh4RegSpeedProfile);

    buf[0] = speed_left & 0xff;
    buf[1] = (speed_left >> 8) & 0xff;
    buf[2] = speed_right & 0xff;
    buf[3] = (speed_right >> 8) & 0xff;

    khepera_dspic_write_register(I2C_MOT_LEFT_SPEED_CONSIGN_L,buf,4);
}

void khepera3_drive_goto_position(int position_left, int position_right) {
    unsigned char buf[8];

    khepera_dspic_write_register8(I2C_MOT_CONTROL_TYPE, kh4RegPosition);

    buf[0] = position_left & 0xff;
    buf[1] = (position_left >> 8) & 0xff;
    buf[2] = (position_left >> 16) & 0xff;
    buf[3] = (position_left >> 24) & 0xff;
    buf[4] = position_right & 0xff;
    buf[5] = (position_right >> 8) & 0xff;
    buf[6] = (position_right >> 16) & 0xff;
    buf[7] = (position_right >> 24) & 0xff;

    khepera_dspic_write_register(I2C_MOT_LEFT_POS_CONSIGN_LL,buf,8);
}

void khepera3_drive_reset_position() {
    khepera_dspic_write_register8(I2C_MOT_POSITION_RESET,1);
}

void khepera3_drive_get_current_speed() {
    unsigned char buf[4];
    khepera_dspic_read_register(I2C_MOT_LEFT_SPEED_L, buf, 4);

    khepera3.motor_left.current_speed = i2cal_readbuffer_uint16(buf, 0);
    khepera3.motor_right.current_speed = i2cal_readbuffer_uint16(buf, 2);
}

void khepera3_drive_get_current_position() {
    unsigned char buf[8];
    khepera_dspic_read_register(I2C_MOT_LEFT_POS_LL, buf, 8);

    khepera3.motor_left.current_position = i2cal_readbuffer_uint32(buf, 0);
    khepera3.motor_right.current_position = i2cal_readbuffer_uint32(buf, 4);
}
