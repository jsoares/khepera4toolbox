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


#include <assert.h>
#include "khepera4_battery.h"
#include "khepera4.h"

/*** Module initialization ***/

void khepera4_imu_init() {
}

/*** Low-level functions ***/

int khepera4_imu_p(struct sKhepera4SensorsImu *imu) {
    unsigned char bufAccel[60];
    unsigned char bufGyro[60];
    int i;
    
    // Read i2c data
    if (!khepera_dspic_read_register(I2C_ACC_X, bufAccel, 60)) {
        return 0;
    }
    if (!khepera_dspic_read_register(I2C_GYR_X, bufGyro, 60)) {
        return 0;
    }    
    
    // Timestamp the data
    imu->timestamp = khepera4_current_time();    

    // Process data into x,y,z coordinates
    for(i=0; i<10; i++) {
        imu->accel[i].x = ((short)(bufAccel[i*2] | bufAccel[i*2+1]<<8)>>4)/1000.0;
        imu->gyro[i].x = ((short)((bufGyro[i*2] | bufGyro[i*2+1]<<8)))*KH4_GYRO_DEG_S;
        
        imu->accel[i].y = ((short)(bufAccel[20+i*2] | bufAccel[20+i*2+1]<<8)>>4)/1000.0;
        imu->gyro[i].y = ((short)((bufGyro[20+i*2] | bufGyro[20+i*2+1]<<8)))*KH4_GYRO_DEG_S;   
        
        imu->accel[i].z = ((short)(bufAccel[40+i*2] | bufAccel[40+i*2+1]<<8)>>4)/1000.0;
        imu->gyro[i].z = ((short)((bufGyro[40+i*2] | bufGyro[40+i*2+1]<<8)))*KH4_GYRO_DEG_S;        
    }
    
    // Average last 10 measurements
    imu->accel_mean.x = 0.;
    imu->accel_mean.y = 0.;
    imu->accel_mean.z = 0.;    
    for(i=0; i<10; i++) {
        imu->accel_mean.x += imu->accel[i].x;
        imu->accel_mean.y += imu->accel[i].y;
        imu->accel_mean.z += imu->accel[i].z;
        imu->gyro_mean.x += imu->gyro[i].x;
        imu->gyro_mean.y += imu->gyro[i].y;
        imu->gyro_mean.z += imu->gyro[i].z;        
    }
    imu->accel_mean.x /= 10.;
    imu->accel_mean.y /= 10.;
    imu->accel_mean.z /= 10.;
    imu->gyro_mean.x /= 10.;
    imu->gyro_mean.y /= 10.;
    imu->gyro_mean.z /= 10.;

    return -1;
}

/*** High-level functions ***/

int khepera4_imu() {
    return khepera4_imu_p(&(khepera4.imu));
}
