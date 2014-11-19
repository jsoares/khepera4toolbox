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


#ifndef I2C_STREAM
#define I2C_STREAM

#include "i2cal.h"

#define I2C_STREAM_BLOCKSTATUS_BUFFER_OVERRUN 255
#define I2C_STREAM_BLOCKSTATUS_EMPTY 0

struct sI2CStreamRead {

    struct {
        int device;
        int reg;
        int block_size;
        int blocks_count_max;
    } configuration;

    struct {
        int blockstatus_last;
        int blocks_count;
        struct i2c_msg *message;
    } result;
};

// Initialization
void i2c_stream_init();

// Stream reading
int i2c_stream_read_start(struct sI2CStreamRead *stream);
int i2c_stream_read_start_without_reset(struct sI2CStreamRead *stream);
int i2c_stream_read_next(struct sI2CStreamRead *stream);

// Read data from result buffer
unsigned char i2c_stream_readdata_blockstatus(struct sI2CStreamRead *stream, int block);
unsigned char i2c_stream_readdata_uint8(struct sI2CStreamRead *stream, int block, int offset);
int i2c_stream_readdata_int16(struct sI2CStreamRead *stream, int block, int offset);
unsigned int i2c_stream_readdata_uint16(struct sI2CStreamRead *stream, int block, int offset);
int i2c_stream_readdata_int32(struct sI2CStreamRead *stream, int block, int offset);
unsigned int i2c_stream_readdata_uint32(struct sI2CStreamRead *stream, int block, int offset);

// Stream writing
// Not implemented

#endif
