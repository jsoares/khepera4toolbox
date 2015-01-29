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


#include "khepera4.h"
#include "commandline.h"
#include "measurement.h"
#include <stdio.h>
#include <stdlib.h>

// Buffers

struct sMeasurement {
    int sample_number;
    struct sKhepera4SensorsImu imu;
};

#define LOG_SIZE 1024
struct sMeasurement log_buffer[LOG_SIZE];
unsigned int sample_number = 0;
unsigned int accel_enabled = 1;
unsigned int gyro_enabled = 1;

// Prints the help text.

void help() {
    printf("Measures inertial sensors.\n");
    printf("\n");
    printf("Usage:\n");
    printf("  imu                    Measures one set of values\n");
    printf("  imu -r [N]             Measures N sets of values (N=infinity if not provided)\n");
    printf("  imu -r N -f            Measures N sets of values and keeps them in memory before printing (for fast acquisition)\n");
    printf("  imu -r N -F            Measures N sets of values but does not print them\n");
    printf("  imu -w US              Waits US us after each measurement\n");
    printf("  imu -na                Do not print accelerometer messages\n");
    printf("  imu -ng                Do not print gyroscope messages\n");
    printf("\n");
    printf("Output format:\n");
    printf("  $ACCEL,sample,timestamp,index,x,y,z\n");
    printf("  $GYRO,sample,timestamp,index,x,y,z\n");    
    printf("  $ACCELMEAN,sample,timestamp,x,y,z\n");
    printf("  $GYROMEAN,sample,timestamp,x,y,z\n");        
    printf("\n");
}

// Takes one measurement

void measurement_take(int i) {
    khepera4_imu_p(&(log_buffer[i].imu));
    log_buffer[i].sample_number = sample_number;
    sample_number++;
}

// Prints one measurement

void measurement_print(int i) {
    int j;
    if(accel_enabled) {
        printf("$ACCELMEAN,%d,%lld,%f,%f,%f\n", log_buffer[i].sample_number, log_buffer[i].imu.timestamp, log_buffer[i].imu.accel_mean.x, log_buffer[i].imu.accel_mean.y, log_buffer[i].imu.accel_mean.z);    
        for (j=0; j < 10; j++)
            printf("$ACCEL,%d,%lld,%d,%f,%f,%f\n", log_buffer[i].sample_number, log_buffer[i].imu.timestamp, j, log_buffer[i].imu.accel[j].x, log_buffer[i].imu.accel[j].y, log_buffer[i].imu.accel[j].z);    
    }
    if(gyro_enabled) {
        printf("$GYROMEAN,%d,%lld,%f,%f,%f\n", log_buffer[i].sample_number, log_buffer[i].imu.timestamp, log_buffer[i].imu.gyro_mean.x, log_buffer[i].imu.gyro_mean.y, log_buffer[i].imu.gyro_mean.z);    
        for (j=0; j < 10; j++)
            printf("$GYRO,%d,%lld,%d,%f,%f,%f\n", log_buffer[i].sample_number, log_buffer[i].imu.timestamp, j, log_buffer[i].imu.gyro[j].x, log_buffer[i].imu.gyro[j].y, log_buffer[i].imu.gyro[j].z);        
    }        
}

// Main program.

int main(int argc, char *argv[]) {
    // Command line parsing
    commandline_init();
    measurement_init();
    measurement_commandline_prepare();
    commandline_parse(argc, argv);

    // Help
    if (commandline_option_provided("-h", "--help")) {
        help();
        exit(1);
    }
    
    if (commandline_option_provided("-na", "--no-accel")) {
        accel_enabled = 0;
    }    
    
    if (commandline_option_provided("-ng", "--no-gyro")) {
        gyro_enabled = 0;
    }        

    // Initialization
    khepera4_init();

    // Take continuous measurements
    measurement_configuration.log_size = LOG_SIZE;
    measurement_configuration.hook_measure = &measurement_take;
    measurement_configuration.hook_print = &measurement_print;
    measurement_start();

    return 0;
}

