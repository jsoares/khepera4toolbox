/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "i2c_stream.h"

#define I2C_STREAM_RESET 1

void i2c_stream_init() {
    i2cal_init();
}

int i2c_stream_read_start(struct sI2CStreamRead *stream) {
    i2cal_start();
    i2cal_writedata_uint8(stream->configuration.reg);
    i2cal_writedata_uint8(I2C_STREAM_RESET);
    i2cal_write(stream->configuration.device);
    i2c_stream_read_start_without_reset(stream);
    return i2cal_commit();
}

int i2c_stream_read_start_without_reset(struct sI2CStreamRead *stream) {
    stream->result.blockstatus_last = 0;
    stream->result.blocks_count = 0;
    stream->result.message = 0;
    return 1;
}

int i2c_stream_read_next(struct sI2CStreamRead *stream) {
    int res;

    // If there was an overrun before, don't do anything
    if (stream->result.blockstatus_last == I2C_STREAM_BLOCKSTATUS_BUFFER_OVERRUN) {
        return 0;
    }

    // Decide how many blocks to read
    stream->result.blocks_count = stream->result.blockstatus_last;
    if (stream->result.blocks_count > stream->configuration.blocks_count_max) {
        stream->result.blocks_count = stream->configuration.blocks_count_max;
    }
    if (stream->result.blocks_count < 1) {
        stream->result.blocks_count = 1;
    }

    // Read that many blocks
    i2cal_start();
    i2cal_writedata_uint8(stream->configuration.reg);
    i2cal_write(stream->configuration.device);
    stream->result.message = i2cal_read(stream->configuration.device, stream->configuration.block_size * stream->result.blocks_count);
    res = i2cal_commit();
    if (res <= 0) {
        return res;
    }

    // Memorize the block status of the last block
    stream->result.blockstatus_last = i2c_stream_readdata_blockstatus(stream, stream->result.blocks_count - 1);
    return res;
}

unsigned char i2c_stream_readdata_blockstatus(struct sI2CStreamRead *stream, int block) {
    return i2cal_readdata_uint8(stream->result.message, stream->configuration.block_size * block);
}

unsigned char i2c_stream_readdata_uint8(struct sI2CStreamRead *stream, int block, int offset) {
    return i2cal_readdata_uint8(stream->result.message, stream->configuration.block_size * block + offset);
}

int i2c_stream_readdata_int16(struct sI2CStreamRead *stream, int block, int offset) {
    return i2cal_readdata_int16(stream->result.message, stream->configuration.block_size * block + offset);
}

unsigned int i2c_stream_readdata_uint16(struct sI2CStreamRead *stream, int block, int offset) {
    return i2cal_readdata_uint16(stream->result.message, stream->configuration.block_size * block + offset);
}

int i2c_stream_readdata_int32(struct sI2CStreamRead *stream, int block, int offset) {
    return i2cal_readdata_int32(stream->result.message, stream->configuration.block_size * block + offset);
}

unsigned int i2c_stream_readdata_uint32(struct sI2CStreamRead *stream, int block, int offset) {
    return i2cal_readdata_uint32(stream->result.message, stream->configuration.block_size * block + offset);
}
