/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "i2cal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stropts.h>

// I2C filehandle
int i2cal_fd = -1;

void i2cal_init() {
	i2cal_open();
	i2cal_start();
}

int i2cal_open() {
	// If already open, do nothing
	if (i2cal_fd >= 0) {
		return 2;
	}

	// Try to open
	i2cal_fd = open("/dev/i2c-3", O_RDWR);
	if (i2cal_fd < 0) {
		printf("Fatal error: unable to open the I2C bus (/dev/i2c-3): res=%d\n", i2cal_fd);
		exit(255);
		//return 0;
	}

	return 1;
}

void i2cal_close() {
	if (i2cal_fd < 0) {
		return;
	}
	close(i2cal_fd);
	i2cal_fd = -1;
}

void i2cal_start() {
	// Initialize variables for the next transfer
	i2cal.messages_nw = 0;
	i2cal.write_buffer_nw = 0;
	i2cal.write_buffer_start = 0;
	i2cal.read_buffer_nw = 0;
}

struct i2c_msg *i2cal_read_buffer(int i2c_address, unsigned char *buffer, int len) {
	struct i2c_msg *msg;

	// If our message buffer is full, give up
	if (i2cal.messages_nw > I2CAL_MESSAGES_LEN) {
		return 0;
	}

	// Create and add a new read message
	msg = &(i2cal.messages[i2cal.messages_nw]);
	i2cal.messages_nw++;

	msg->addr  = i2c_address;
	msg->flags = I2C_M_RD;
	msg->buf   = buffer;
	msg->len   = len;
	//i2cal.read_buffer_nw+=len;

	return msg;
}

struct i2c_msg *i2cal_write_buffer(int i2c_address, unsigned char *buffer, int len) {
	struct i2c_msg *msg;

	// If our message buffer is full, give up
	if (i2cal.messages_nw > I2CAL_MESSAGES_LEN) {
		return 0;
	}

	// Create and add a new write message
	msg = &(i2cal.messages[i2cal.messages_nw]);
	i2cal.messages_nw++;

	msg->addr  = i2c_address;
	msg->flags = 0;  // TODO: add proper flag
	msg->buf   = buffer;
	msg->len   = len;
	//i2cal.write_buffer_start=i2cal.write_buffer_nw;

	return msg;
}

struct i2c_msg *i2cal_read(int i2c_address, int len) {
	struct i2c_msg *msg;

	// If our read buffer would be full, give up
	if (i2cal.read_buffer_nw + len > I2CAL_READ_BUFFER_LEN) {
		return 0;
	}

	// Add a new read message and reserve the necessary space in the read buffer
	msg = i2cal_read_buffer(i2c_address, &(i2cal.read_buffer[i2cal.read_buffer_nw]), len);
	if (msg == 0) {
		return 0;
	}
	i2cal.read_buffer_nw += len;

	return msg;
}

struct i2c_msg *i2cal_write(int i2c_address) {
	struct i2c_msg *msg;

	// If our write buffer would be full, give up
	if (i2cal.write_buffer_nw > I2CAL_WRITE_BUFFER_LEN) {
		return 0;
	}

	// Add a new write message with the content of the write buffer
	msg = i2cal_write_buffer(i2c_address, &(i2cal.write_buffer[i2cal.write_buffer_start]), i2cal.write_buffer_nw - i2cal.write_buffer_start);
	if (msg == 0) {
		return 0;
	}
	i2cal.write_buffer_start = i2cal.write_buffer_nw;

	return msg;
}

unsigned char *i2cal_writedata_uint8(unsigned char value) {
	unsigned char *buf = i2cal_writedata_buffer(1);

	if (buf == 0) {
		return 0;
	}
	*buf = value;

    //printf("i2c: %hhu\n", *buf);

	return buf;
}

unsigned char *i2cal_writedata_int16(int value) {
	unsigned char *arr = (unsigned char *)(&value);
	unsigned char *buf = i2cal_writedata_buffer(2);

	if (buf == 0) {
		return 0;
	}
	buf[0] = arr[0];
	buf[1] = arr[1];
	return buf;
}

unsigned char *i2cal_writedata_uint16(unsigned int value) {
	unsigned char *arr = (unsigned char *)(&value);
	unsigned char *buf = i2cal_writedata_buffer(2);

	if (buf == 0) {
		return 0;
	}
	buf[0] = arr[0];
	buf[1] = arr[1];
	return buf;
}

unsigned char *i2cal_writedata_int32(int value) {
	unsigned char *arr = (unsigned char *)(&value);
	unsigned char *buf = i2cal_writedata_buffer(4);

	if (buf == 0) {
		return 0;
	}
	buf[0] = arr[0];
	buf[1] = arr[1];
	buf[2] = arr[2];
	buf[3] = arr[3];
	return buf;
}

unsigned char *i2cal_writedata_uint32(unsigned int value) {
	unsigned char *arr = (unsigned char *)(&value);
	unsigned char *buf = i2cal_writedata_buffer(4);

	if (buf == 0) {
		return 0;
	}
	buf[0] = arr[0];
	buf[1] = arr[1];
	buf[2] = arr[2];
	buf[3] = arr[3];
	return buf;
}

unsigned char *i2cal_writedata_float(float value) {
	unsigned char *arr = (unsigned char *)(&value);
	unsigned char *buf = i2cal_writedata_buffer(4);

	if (buf == 0) {
		return 0;
	}
	buf[0] = arr[0];
	buf[1] = arr[1];
	buf[2] = arr[2];
	buf[3] = arr[3];
	return buf;
}

unsigned char *i2cal_writedata_buffer(int len) {
	unsigned char *buf;

	// Reserve bytes in the write buffer (which can then be filled by other functions)
	if (i2cal.write_buffer_nw + len > I2CAL_WRITE_BUFFER_LEN) {
		i2cal.write_buffer_nw = I2CAL_WRITE_BUFFER_LEN + 1;
		return 0;
	}
	buf = &(i2cal.write_buffer[i2cal.write_buffer_nw]);
	i2cal.write_buffer_nw += len;
	return buf;
}

int i2cal_commit() {
	struct i2c_rdwr_ioctl_data msgset;

	// Create a message set
	msgset.msgs = i2cal.messages;
	msgset.nmsgs = i2cal.messages_nw;

	// Execute that message set
	return ioctl(i2cal_fd, I2C_RDWR, &msgset);
}

// Helper functions to read formatted data from result buffer

unsigned char i2cal_readdata_uint8(struct i2c_msg *message, int offset) {
	return i2cal_readbuffer_uint8(message->buf,offset);
}

int i2cal_readdata_int16(struct i2c_msg *message, int offset) {
	return i2cal_readbuffer_int16(message->buf,offset);
}

unsigned int i2cal_readdata_uint16(struct i2c_msg *message, int offset) {
	return i2cal_readbuffer_uint16(message->buf,offset);
}

int i2cal_readdata_int32(struct i2c_msg *message, int offset) {
	return i2cal_readbuffer_int32(message->buf,offset);
}

unsigned int i2cal_readdata_uint32(struct i2c_msg *message, int offset) {
	return i2cal_readbuffer_uint32(message->buf,offset);
}

float i2cal_readdata_float(struct i2c_msg *message, int offset) {
    return i2cal_readbuffer_float(message->buf,offset);
}

// Helper functions to read formatted data from generic buffer

unsigned char i2cal_readbuffer_uint8(unsigned char *buf, int offset) {
	return buf[offset];
}

int i2cal_readbuffer_int16(unsigned char *buf, int offset) {
	unsigned char arr[2];
	short int *value = (short int *)arr;

	arr[0] = buf[offset];
	arr[1] = buf[++offset];
	return (int)(*value);
}

unsigned int i2cal_readbuffer_uint16(unsigned char *buf, int offset) {
	unsigned char arr[4];
	unsigned int *value = (unsigned int *)arr;

	arr[0] = buf[offset];
	arr[1] = buf[++offset];
	arr[2] = 0;
	arr[3] = 0;
	return *value;
}

int i2cal_readbuffer_int32(unsigned char *buf, int offset) {
	unsigned char arr[4];
	int *value = (int *)arr;

	arr[0] = buf[offset];
	arr[1] = buf[++offset];
	arr[2] = buf[++offset];
	arr[3] = buf[++offset];
	return *value;
}

unsigned int i2cal_readbuffer_uint32(unsigned char *buf, int offset) {
	unsigned char arr[4];
	unsigned int *value = (unsigned int *)arr;

	arr[0] = buf[offset];
	arr[1] = buf[++offset];
	arr[2] = buf[++offset];
	arr[3] = buf[++offset];
	return *value;
}

float i2cal_readbuffer_float(unsigned char *buf, int offset) {
	unsigned char arr[4];
	float *value = (float *)arr;

	arr[0] = buf[offset];
	arr[1] = buf[++offset];
	arr[2] = buf[++offset];
	arr[3] = buf[++offset];
	return *value;
}

/*** dsPIC function wrappers ***/

int khepera_dspic_read_register(int reg, unsigned char * buf, int len) {
	struct i2c_msg *msg_read;

    // Send register and read value
	i2cal_start();
	i2cal_writedata_uint8((unsigned int)reg);
	i2cal_write(DSPIC_I2C_ADDRESS);
	msg_read = i2cal_read(DSPIC_I2C_ADDRESS, len);
	if (i2cal_commit() < 0) {
		return 0;
	}

    // Copy return to buffer
    memcpy((void *) buf, (void *) msg_read->buf, (size_t) len);

	// Return success
	return -1;
}

int khepera_dspic_read_register8(int reg, unsigned int *result) {
	struct i2c_msg *msg_read;

	// Send register and read value
	i2cal_start();
	i2cal_writedata_uint8((unsigned int)reg);
	i2cal_write(DSPIC_I2C_ADDRESS);
	msg_read = i2cal_read(DSPIC_I2C_ADDRESS, 1);
	if (i2cal_commit() < 0) {
		return 0;
	}

	// Parse value and return
	//printf("motor read %d\n", msg_read->buf[0]);
	*result = msg_read->buf[0];
	return -1;
}

int khepera_dspic_read_register16(int reg, unsigned int *result) {
	struct i2c_msg *msg_read;
	unsigned int value;

    // Send register and read value (for HH register)
	i2cal_start();
	i2cal_writedata_uint8((unsigned int)reg);
	i2cal_write(DSPIC_I2C_ADDRESS);
	msg_read = i2cal_read(DSPIC_I2C_ADDRESS, 2);
	if (i2cal_commit() < 0) {
		return 0;
	}
	value = i2cal_readdata_uint16(msg_read, 0);

	// Return success
	*result = value;
	return -1;
}

int khepera_dspic_read_register32(int reg, unsigned int *result) {
	struct i2c_msg *msg_read;
	unsigned int value;

    // Send register and read value (for HH register)
	i2cal_start();
	i2cal_writedata_uint8((unsigned int)reg);
	i2cal_write(DSPIC_I2C_ADDRESS);
	msg_read = i2cal_read(DSPIC_I2C_ADDRESS, 4);
	if (i2cal_commit() < 0) {
		return 0;
	}
	value = i2cal_readdata_uint32(msg_read, 0);

	// Return success
	*result = value;
	return -1;
}

int khepera_dspic_write_register(int reg, unsigned char * buf, int len) {
    int i;

    i2cal_start();
	i2cal_writedata_uint8((unsigned int)reg);

    for(i=0; i<len; i++)
        i2cal_writedata_uint8(buf[i]);

	i2cal_write(DSPIC_I2C_ADDRESS);
	if (i2cal_commit() < 0) {
		return 0;
	}

	return -1;
}

int khepera_dspic_write_register8(int reg, unsigned int value) {
    unsigned char buf[1];
    buf[0] = value & 0xff;

    return khepera_dspic_write_register(reg, buf, 1);
}

int khepera_dspic_write_register16(int reg, unsigned int value) {
    unsigned char buf[2];
    buf[0] = value & 0xff;
    buf[1] = (value >> 8) & 0xff;

    return khepera_dspic_write_register(reg, buf, 2);
}

int khepera_dspic_write_register32(int reg, unsigned int value) {
    unsigned char buf[4];
    buf[0] = value & 0xff;
    buf[1] = (value >> 8) & 0xff;
    buf[2] = (value >> 16) & 0xff;
    buf[3] = (value >> 24) & 0xff;

    return khepera_dspic_write_register(reg, buf, 4);
}