/*!
 * (c) 2007 - 2007 EPFL, Lausanne, Switzerland
 * Jim Pugh, Sven Gowal, Thomas Lochmatter
 */

#include "relativepositionboard.h"
#include "i2cal.h"
#include "nmea.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define PI 3.14159265358979

struct sRelativePositionBoard relativepositionboard;

/* Read Calibration */

// (private) Read the calibration.
void relativepositionboard_read_calibration(const char *filename) {
  // Open the file
  int s;
  int found[8];
  float sl, sc, sr, alpha, beta, gamma, u, v;
  int rid, sindex, n;
  char line[256];

  for (s = 0; s < 8; s++) {
    relativepositionboard.calibration.rssi[s].scaling_left = 2.18f;
    relativepositionboard.calibration.rssi[s].scaling_center = 1.0f;
    relativepositionboard.calibration.rssi[s].scaling_right = 2.08f;
    relativepositionboard.calibration.rssi[s].alpha = 2.66f;
    relativepositionboard.calibration.rssi[s].beta = 2105.36f;
    relativepositionboard.calibration.rssi[s].gamma = 2.58f;
    relativepositionboard.calibration.rssi[s].u = 0.62f;
    relativepositionboard.calibration.rssi[s].v = 21887.61f;
  }

  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Calibration file not found.\nPlease create /etc/khepera/relativepositionboard.\n"
            "To calibrate the board you can use the suite of scripts\n"
            "available with this code. Please read the README file.\n");
    return;
  }

  // Keep track of which sector is found
  memset(found, 0, sizeof(found));

  // Each line of the file is as follows: robot_id sector_index scaling_left scaling_right alpha beta gamma u v
  do{
    fgets(line, 256, file);
    n = sscanf(line, "%d %d %f %f %f %f %f %f %f %f", &rid, &sindex, &sl, &sc, &sr, &alpha, &beta, &gamma, &u, &v);

    if (n != 10) continue;

    if (rid == 0) {
      for (s = 0; s < 8; s++) {
        if (found[s]) continue;
        relativepositionboard.calibration.rssi[s].scaling_left = sl;
        relativepositionboard.calibration.rssi[s].scaling_center = sc;
        relativepositionboard.calibration.rssi[s].scaling_right = sr;
        relativepositionboard.calibration.rssi[s].alpha = alpha;
        relativepositionboard.calibration.rssi[s].beta = beta;
        relativepositionboard.calibration.rssi[s].gamma = gamma;
        relativepositionboard.calibration.rssi[s].u = u;
        relativepositionboard.calibration.rssi[s].v = v;
      }
    } else if (rid == relativepositionboard.robot_id) {
      relativepositionboard.calibration.rssi[sindex].scaling_left = sl;
      relativepositionboard.calibration.rssi[sindex].scaling_center = sc;
      relativepositionboard.calibration.rssi[sindex].scaling_right = sr;
      relativepositionboard.calibration.rssi[sindex].alpha = alpha;
      relativepositionboard.calibration.rssi[sindex].beta = beta;
      relativepositionboard.calibration.rssi[sindex].gamma = gamma;
      relativepositionboard.calibration.rssi[sindex].u = u;
      relativepositionboard.calibration.rssi[sindex].v = v;
      found[sindex] = 1;
    }

  } while (!feof(file));

  // Close the file
  fclose(file);
}

/* Calculation */

// (private) Computes the mapping between RSSI range and true range
float relativepositionboard_calculate_range(int sindex, float rssi_range) {
  float a = relativepositionboard.calibration.rssi[sindex].alpha;
  float b = relativepositionboard.calibration.rssi[sindex].beta;
  float c = relativepositionboard.calibration.rssi[sindex].gamma;
  float u = relativepositionboard.calibration.rssi[sindex].u;
  float v = relativepositionboard.calibration.rssi[sindex].v;
  return a*expf(-powf(rssi_range/b, c)) + u - rssi_range/v;
}

// (private) Update the range and bearing values for one robot.
void relativepositionboard_calculate(int robot_id) {
  // Find hemisphere with highest total RSSI
  float hemisphere_max_value = 0;
  int hemisphere_max_index = 0;
  int s;

  for (s = 0; s < 8; s++) {
    float value = 0;
    int k;

    // Ignore weird I2C corrupt data
    if (relativepositionboard.robot[robot_id].rssi[s] == 0.0f) return;
    if (fabs(relativepositionboard.robot[robot_id].rssi[s]) > 17000.0f) return;

    for (k = -1; k <= 1; k++) {
      int index = (s + k) & 0x7;
      value += relativepositionboard.robot[robot_id].rssi[index];
    }
    if (value > hemisphere_max_value) {
      hemisphere_max_index = s;
      hemisphere_max_value = value;
    }
  }

  // Get relevant RSSI values
  float good_rssi[3];
  int k;
  for (k = -1; k <= 1; k++) {
    int index = (hemisphere_max_index + k) & 0x7;
    good_rssi[k + 1] = relativepositionboard.robot[robot_id].rssi[index];
  }

  // Calculate b and c terms
  const float c_beta = 0.70711;
  const float s_beta = 0.70711;
  float sl = relativepositionboard.calibration.rssi[hemisphere_max_index].scaling_left;
  float sc = relativepositionboard.calibration.rssi[hemisphere_max_index].scaling_center;
  float sr = relativepositionboard.calibration.rssi[hemisphere_max_index].scaling_right;
  float b = (sl*good_rssi[0] + sc*2.0*good_rssi[1] + sr*good_rssi[2]) / (2.0 * c_beta + 2.0);
  float c = (sl*good_rssi[0] - sr*good_rssi[2]) / (2.0 * s_beta);

  // Calculate range and bearing
  float bearing = atan2(c, b) - (2.0 * PI / 8) * (float)hemisphere_max_index;
  float rssi_range = sqrtf(b * b + c * c);

  // Update the robot values (account for robot radius)
  relativepositionboard.robot[robot_id].range = relativepositionboard_calculate_range(hemisphere_max_index, rssi_range);
  while (bearing > PI) bearing -= 2.0*PI;
  while (bearing < -PI) bearing += 2.0*PI;
  relativepositionboard.robot[robot_id].bearing = bearing;
  relativepositionboard.robot[robot_id].age = 0;
}


/* Initialization */
void relativepositionboard_init() {
  // Start the I2C module
  i2cal_init();
  relativepositionboard.device = 0x56;
  relativepositionboard.stream.configuration.device = relativepositionboard.device;
  relativepositionboard.stream.configuration.reg = 0x88;
  relativepositionboard.stream.configuration.block_size = 18;
  relativepositionboard.stream.configuration.blocks_count_max = 4;

  // Set the ID and calibration values
  relativepositionboard.robot_id = 0;
  FILE *file = fopen("/etc/khepera/robotid", "r");
  if (file) {
    fscanf(file, "%d", &relativepositionboard.robot_id);
    fclose(file);
  }
  relativepositionboard_read_calibration("/etc/khepera/relativepositionboard");
}


/* Start / Stop Transmitter and Receiver */
void relativepositionboard_start(unsigned int robot_id) {
  // Enable emitter and receiver
  relativepositionboard.configuration.rx_enabled = 1;
  relativepositionboard.configuration.tx_enabled = 1;
  relativepositionboard.configuration.tx_data = robot_id;

  // If an invalid robot id was provided, disable the emitter
  if (robot_id >= 32) {
    printf("Relative position board: invalid robot_id. The emitter has been switched off!\n");
    relativepositionboard.configuration.tx_enabled = 0;
    relativepositionboard.configuration.tx_data = 0;
  }

  // Set the configuration on the board
  relativepositionboard_configuration_rx_enabled_set();
  relativepositionboard_configuration_tx_enabled_set();
  relativepositionboard_configuration_tx_data_set();
}

void relativepositionboard_stop() {
  relativepositionboard.configuration.tx_enabled = 0;
  relativepositionboard.configuration.rx_enabled = 0;
  relativepositionboard_configuration_tx_enabled_set();
  relativepositionboard_configuration_rx_enabled_set();
}

/* Stream Reading */

void relativepositionboard_stream_start() {
  // Start the stream
  i2c_stream_read_start(&relativepositionboard.stream);
}

void relativepositionboard_stream_step() {
  // Increment the age counter
  int r;
  for (r = 0; r < 32; r++) {
    relativepositionboard.robot[r].age++;
  }

  // Read the next samples from the stream
  i2c_stream_read_next(&relativepositionboard.stream);

  // Process all (non-empty) blocks
  int i;
  for (i = 0; i < relativepositionboard.stream.result.blocks_count; i++) {
    int block_status = i2c_stream_readdata_blockstatus(&relativepositionboard.stream, i);
    if (block_status == I2C_STREAM_BLOCKSTATUS_BUFFER_OVERRUN) {
      // Buffer overrun should not happen here, ignore and go on
      printf("Relative position board: buffer overrun!\n");
    } else if (block_status == I2C_STREAM_BLOCKSTATUS_EMPTY) {
      // Empty block? Do nothing ...
    } else {
      unsigned int robot_id = i2c_stream_readdata_uint8(&relativepositionboard.stream, i, 1);
      if (robot_id < 32) {
        int s;
        for (s = 0; s < 8; s++) {
          int value = i2c_stream_readdata_int16(&relativepositionboard.stream, i, s * 2 + 2);
          relativepositionboard.robot[robot_id].rssi[s] = (float)value;
        }
        relativepositionboard_calculate(robot_id);
      }
    }
  }
}

/* Write Configuration */

// (private) Set one configuration value.
void relativepositionboard_configuration_set8(unsigned char cmd, int data) {
  i2cal_start();
  i2cal_writedata_uint8(cmd);
  i2cal_writedata_uint8(data);
  i2cal_write(relativepositionboard.device);
  int result = i2cal_commit();
  if (result < 0) {
    printf("Warning: I2C write error.\n");
  }
}

void relativepositionboard_configuration_set16(unsigned char cmd, int data) {
  i2cal_start();
  i2cal_writedata_uint8(cmd);
  i2cal_writedata_uint16(data);
  i2cal_write(relativepositionboard.device);
  int result = i2cal_commit();
  if (result < 0) {
    printf("Warning: I2C write error.\n");
  }
}

void relativepositionboard_configuration_rx_enabled_set() {
  relativepositionboard_configuration_set8(0x80, relativepositionboard.configuration.rx_enabled & 0x1);
}

void relativepositionboard_configuration_tx_enabled_set() {
  relativepositionboard_configuration_set8(0x90, relativepositionboard.configuration.tx_enabled & 0x1);
}

void relativepositionboard_configuration_tx_data_set() {
  relativepositionboard_configuration_set16(0x91, relativepositionboard.configuration.tx_data);
}

void relativepositionboard_configuration_tx_power_set() {
  relativepositionboard_configuration_set16(0x92, relativepositionboard.configuration.tx_power);
}

void relativepositionboard_configuration_tx_sectors_set() {
  relativepositionboard_configuration_set16(0x93, relativepositionboard.configuration.tx_sectors);
}

void relativepositionboard_configuration_tx_interval_set() {
  relativepositionboard_configuration_set16(0x94, relativepositionboard.configuration.tx_interval);
}


/* Read Configuration */

// (private) Read one configuration value.
int relativepositionboard_configuration_get8(unsigned char cmd) {
  int commit_result;
  struct i2c_msg *message;

  // Request packet
  i2cal_start();
  i2cal_writedata_uint8(cmd);
  i2cal_write(relativepositionboard.device);
  message = i2cal_read(relativepositionboard.device, 1);
  commit_result = i2cal_commit();
  if (commit_result < 1) {
    printf("Warning: I2C read error.\n");
    return 0;
  }

  // Parse packet
  return i2cal_readdata_uint8(message, 0);
}

// (private) Read one configuration value.
int relativepositionboard_configuration_get16(unsigned char cmd) {
  int commit_result;
  struct i2c_msg *message;

  // Request packet
  i2cal_start();
  i2cal_writedata_uint8(cmd);
  i2cal_write(relativepositionboard.device);
  message = i2cal_read(relativepositionboard.device, 2);
  commit_result = i2cal_commit();
  if (commit_result < 1) {
    printf("Warning: I2C read error.\n");
    return 0;
  }

  // Parse packet
  return i2cal_readdata_uint16(message, 0);
}

void relativepositionboard_configuration_rx_enabled_get() {
  relativepositionboard.configuration.rx_enabled = relativepositionboard_configuration_get8(0x80);
}

void relativepositionboard_configuration_tx_enabled_get() {
  relativepositionboard.configuration.tx_enabled = relativepositionboard_configuration_get8(0x90);
}

void relativepositionboard_configuration_tx_data_get() {
  relativepositionboard.configuration.tx_data = relativepositionboard_configuration_get16(0x91);
}

void relativepositionboard_configuration_tx_power_get() {
  relativepositionboard.configuration.tx_power = relativepositionboard_configuration_get16(0x92);
}

void relativepositionboard_configuration_tx_sectors_get() {
  relativepositionboard.configuration.tx_sectors = relativepositionboard_configuration_get16(0x93);
}

void relativepositionboard_configuration_tx_interval_get() {
  relativepositionboard.configuration.tx_interval = relativepositionboard_configuration_get16(0x94);
}


/* Calibration */

void relativepositionboard_calibrate(float threshold_factor) {
  // Start calibration
  i2cal_start();
  i2cal_writedata_uint8(0x82);
  i2cal_writedata_float(threshold_factor);
  i2cal_write(relativepositionboard.device);
  int result = i2cal_commit();
  if (result < 0) {
    printf("Warning: I2C write error.\n");
  }
}

void relativepositionboard_calibration_set() {
  i2cal_start();
  i2cal_writedata_uint8(0x81);
  int s, c;
  for (c = 0; c < 2; c++) {
    for (s = 0; s < 8; s++) {
      i2cal_writedata_uint16(relativepositionboard.calibration.sector[s][c].mean);
      i2cal_writedata_uint16(relativepositionboard.calibration.sector[s][c].threshold);
    }
  }
  i2cal_write(relativepositionboard.device);
  int result = i2cal_commit();
  if (result < 0) {
    printf("Warning: I2C write error.\n");
  }
}

int relativepositionboard_calibration_get() {
  int commit_result;
  struct i2c_msg *message;

  // Request packet
  i2cal_start();
  i2cal_writedata_uint8(0x81);
  i2cal_write(relativepositionboard.device);
  message = i2cal_read(relativepositionboard.device, 64);
  commit_result = i2cal_commit();
  if (commit_result < 1) {
    printf("Warning: I2C read error.\n");
    return 0;
  }

  // Parse packet
  int s, c;
  for (c = 0; c < 2; c++) {
    for (s = 0; s < 8; s++) {
      relativepositionboard.calibration.sector[s][c].mean = i2cal_readdata_uint16(message, s * 4 + 0 + 32*c);
      relativepositionboard.calibration.sector[s][c].threshold = i2cal_readdata_uint16(message, s * 4 + 2 + 32*c);
    }
  }
  return 1;
}

void relativepositionboard_calibration_save() {
  i2cal_start();
  i2cal_writedata_uint8(0x83);
  i2cal_writedata_uint8(0);
  i2cal_write(relativepositionboard.device);
  int result = i2cal_commit();
  if (result < 0) {
    printf("Warning: I2C write error.\n");
  }
}

void relativepositionboard_calibration_load() {
  i2cal_start();
  i2cal_writedata_uint8(0x84);
  i2cal_writedata_uint8(0);
  i2cal_write(relativepositionboard.device);
  int result = i2cal_commit();
  if (result < 0) {
    printf("Warning: I2C write error.\n");
  }
}
