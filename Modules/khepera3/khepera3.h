/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef KHEPERA3
#define KHEPERA3

#include "i2cal.h"
#include "khepera3_drive.h"
#include "khepera3_infrared.h"
#include "khepera3_ultrasound.h"
#include "khepera3_battery.h"

// I2c Address register (from libkhepera)
#define I2C_REVISION                      0x00

#define I2C_LED                           0x01    // ON/OFF and ERR Led value

#define I2C_RGB_FL_R                      0x02    // Front Left Red led
#define I2C_RGB_FL_G                      0x03    // Front Left Green led
#define I2C_RGB_FL_B                      0x04    // Front Left Blue led
#define I2C_RGB_FR_R                      0x05    // Front Right Red led
#define I2C_RGB_FR_G                      0x06    // Front Right Green led
#define I2C_RGB_FR_B                      0x07    // Front Right Blue led
#define I2C_RGB_B_R                       0x08    // Back Red led
#define I2C_RGB_B_G                       0x09    // Back Green led
#define I2C_RGB_B_B                       0x0A    // Back Blue led

#define I2C_MOT_KP                        0x0B    // Kp value of the controller
#define I2C_MOT_KI                        0x0C    // Ki value of the controller
#define I2C_MOT_KD                        0x0D    // Kd value of the controller

#define I2C_MOT_ACC_INC                   0x0E    // Acceleration increment of the motor
#define I2C_MOT_ACC_DIV                   0x0F    // Acceleration divider of the motor

#define I2C_MOT_MIN_SPEED_ACC             0x10    // Minimum speed used during the acceleration
#define I2C_MOT_MIN_SPEED_DEC             0x11    // Minimum speed used during the deceleration

#define I2C_MOT_POS_MAR_L                 0x12    // position margin used in the position control
#define I2C_MOT_POS_MAR_H                 0x13

#define I2C_MOT_SPEED_POS_L               0x14    // Consign of speed using during the position control
#define I2C_MOT_SPEED_POS_H               0x15

#define I2C_MOT_POSITION_RESET            0x16    // register to reset the position counter of the two motors

#define I2C_MOT_CONTROL_TYPE              0x17    // Type of the control use for the motor

#define I2C_MOT_LEFT_SPEED_CONSIGN_L      0x18    // Consign for the left motor speed
#define I2C_MOT_LEFT_SPEED_CONSIGN_H      0x19

#define I2C_MOT_RIGHT_SPEED_CONSIGN_L     0x1A    // Consign for the right motor speed
#define I2C_MOT_RIGHT_SPEED_CONSIGN_H     0x1B

#define I2C_MOT_LEFT_PWM_CONSIGN_L        0x1C    // PWM Consign used in open loop for the left motor
#define I2C_MOT_LEFT_PWM_CONSIGN_H        0x1D

#define I2C_MOT_RIGHT_PWM_CONSIGN_L       0x1E    // PWM Consign used in open loop for the right motor
#define I2C_MOT_RIGHT_PWM_CONSIGN_H       0x1F

#define I2C_MOT_LEFT_POS_CONSIGN_LL       0x20    // Position Consign of the left motor
#define I2C_MOT_LEFT_POS_CONSIGN_LH       0x21
#define I2C_MOT_LEFT_POS_CONSIGN_HL       0x22
#define I2C_MOT_LEFT_POS_CONSIGN_HH       0x23

#define I2C_MOT_RIGHT_POS_CONSIGN_LL      0x24    // Position Consign of the right motor
#define I2C_MOT_RIGHT_POS_CONSIGN_LH      0x25
#define I2C_MOT_RIGHT_POS_CONSIGN_HL      0x26
#define I2C_MOT_RIGHT_POS_CONSIGN_HH      0x27

#define I2C_MOT_LEFT_SPEED_L              0x28    // Actual speed of the left motor
#define I2C_MOT_LEFT_SPEED_H              0x29

#define I2C_MOT_RIGHT_SPEED_L             0x2A    // Actual speed of the right motor
#define I2C_MOT_RIGHT_SPEED_H             0x2B

#define I2C_MOT_ON_TARGET                 0x2C    // Contain the two OnTarget flag (bit0 = left, bit1 = right)

#define I2C_MOT_LEFT_POS_LL               0x2D    // Actual position of the left motor
#define I2C_MOT_LEFT_POS_LH               0x2E
#define I2C_MOT_LEFT_POS_HL               0x2F
#define I2C_MOT_LEFT_POS_HH               0x30

#define I2C_MOT_RIGHT_POS_LL              0x31    // Actual position of the right motor
#define I2C_MOT_RIGHT_POS_LH              0x32
#define I2C_MOT_RIGHT_POS_HL              0x33
#define I2C_MOT_RIGHT_POS_HH              0x34

#define I2C_IR_AMB_BL_L                   0x40    // Ambiant value of the Back left sensor
#define I2C_IR_AMB_BL_H                   0x41
#define I2C_IR_AMB_L_L                    0x42    // Ambiant value of the Left sensor
#define I2C_IR_AMB_L_H                    0x43
#define I2C_IR_AMB_FL_L                   0x44    // Ambiant value of the Front left sensor
#define I2C_IR_AMB_FL_H                   0x45
#define I2C_IR_AMB_F_L                    0x46    // Ambiant value of the Front sensor
#define I2C_IR_AMB_F_H                    0x47
#define I2C_IR_AMB_FR_L                   0x48    // Ambiant value of the Front Right sensor
#define I2C_IR_AMB_FR_H                   0x49
#define I2C_IR_AMB_R_L                    0x4A    // Ambiant value of the Right sensor
#define I2C_IR_AMB_R_H                    0x4B
#define I2C_IR_AMB_BR_L                   0x4C    // Ambiant value of the Back Right sensor
#define I2C_IR_AMB_BR_H                   0x4D
#define I2C_IR_AMB_B_L                    0x4E    // Ambiant value of the Back sensor
#define I2C_IR_AMB_B_H                    0x4F
#define I2C_IR_AMB_GL_L                   0x50    // Ambiant value of the Ground Left sensor
#define I2C_IR_AMB_GL_H                   0x51
#define I2C_IR_AMB_GFL_L                  0x52    // Ambiant value of the Ground Front Left sensor
#define I2C_IR_AMB_GFL_H                  0x53
#define I2C_IR_AMB_GFR_L                  0x54    // Ambiant value of the Ground Front Right sensor
#define I2C_IR_AMB_GFR_H                  0x55
#define I2C_IR_AMB_GR_L                   0x56    // Ambiant value of the Ground Right sensor
#define I2C_IR_AMB_GR_H                   0x57

#define I2C_IR_DIST_BL_L                  0x58    // Distance value of the Back left sensor
#define I2C_IR_DIST_BL_H                  0x59
#define I2C_IR_DIST_L_L                   0x5A    // Distance value of the Left sensor
#define I2C_IR_DIST_L_H                   0x5B
#define I2C_IR_DIST_FL_L                  0x5C    // Distance value of the Front left sensor
#define I2C_IR_DIST_FL_H                  0x5D
#define I2C_IR_DIST_F_L                   0x5E    // Distance value of the Front sensor
#define I2C_IR_DIST_F_H                   0x5F
#define I2C_IR_DIST_FR_L                  0x60    // Distance value of the Front Right sensor
#define I2C_IR_DIST_FR_H                  0x61
#define I2C_IR_DIST_R_L                   0x62    // Distance value of the Right sensor
#define I2C_IR_DIST_R_H                   0x63
#define I2C_IR_DIST_BR_L                  0x64    // Distance value of the Back Right sensor
#define I2C_IR_DIST_BR_H                  0x65
#define I2C_IR_DIST_B_L                   0x66    // Distance value of the Back sensor
#define I2C_IR_DIST_B_H                   0x67
#define I2C_IR_DIST_GL_L                  0x68    // Distance value of the Ground Left sensor
#define I2C_IR_DIST_GL_H                  0x69
#define I2C_IR_DIST_GFL_L                 0x6A    // Distance value of the Ground Front Left sensor
#define I2C_IR_DIST_GFL_H                 0x6B
#define I2C_IR_DIST_GFR_L                 0x6C    // Distance value of the Ground Front Right sensor
#define I2C_IR_DIST_GFR_H                 0x6D
#define I2C_IR_DIST_GR_L                  0x6E    // Distance value of the Ground Right sensor
#define I2C_IR_DIST_GR_H                  0x6F

#define I2C_US_L_L                        0x70    // Distance of the Left US sensor
#define I2C_US_L_H                        0x71
#define I2C_US_FL_L                       0x72    // Distance of the Front Left US sensor
#define I2C_US_FL_H                       0x73
#define I2C_US_F_L                        0x74    // Distance of the Front US sensor
#define I2C_US_F_H                        0x75
#define I2C_US_FR_L                       0x76    // Distance of the Front Right US sensor
#define I2C_US_FR_H                       0x77
#define I2C_US_R_L                        0x78    // Distance of the Right US sensor
#define I2C_US_R_H                        0x79

#define I2C_US_ACTIVE                     0x7A	  // US activation

#define I2C_ACC_X                         0x80    // Start of the X value of the accelerometer
#define I2C_ACC_Y                         0x94    // Start of the Y value of the accelerometer
#define I2C_ACC_Z                         0xA8    // Start of the Z value of the accelerometer

#define I2C_GYR_X                         0xC0    // Start of the X value of the Gyro
#define I2C_GYR_Y                         0xD4    // Start of the X value of the Gyro
#define I2C_GYR_Z                         0xE8    // Start of the X value of the Gyro

#define I2C_BAT_STATUS			  0xFD    // Battery Status

//! dsPIC microcontroller

struct sKhepera3DSPIC {
    int i2c_address; //!< I2C bus address
    unsigned int firmware_version; //!< Firmware version
    unsigned int firmware_revision; //!< Firmware revision
};

//! The complete Khepera 3 robot

struct sKhepera3 {
    int robot_id; //!< ID of the robot
    struct sKhepera3DSPIC dspic; //!< dsPIC microcontroller
    struct sKhepera3Motor motor_left; //!< Left motor
    struct sKhepera3Motor motor_right; //!< Right motor
    struct sKhepera3SensorsInfrared infrared_ambient; //!< Infrared sensors in ambient light mode
    struct sKhepera3SensorsInfrared infrared_proximity; //!< Infrared sensors in proximity mode
    struct sKhepera3SensorsUltrasound ultrasound; //!< Ultrasound sensors
    struct sKhepera3Battery battery; //!< Battery information
};

struct sKhepera3 khepera3;

//! Initializes this module.
void khepera3_init();

//! Resets the timestamp on the dsPIC. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 3 bytes.
int khepera3_timestamp_reset();
//! Reads the firmware version and revision and updates this value in the khepera3 structure. The return value indicates success (-1) or failure (0). Transfer on I2C bus: 11 bytes.
int khepera3_dspic_firmware_version();

#endif
