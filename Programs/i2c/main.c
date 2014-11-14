/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include "i2cal.h"
#include "measurement.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Buffer for one measurement
struct sMeasurement {
	int sample_number;
	int commit_return_value;
};

// Algorithm parameters
struct sAlgorithm {
	unsigned int i2c_address;			// The I2C address of the device to access
#define LOG_SIZE 1024
	struct sMeasurement log_buffer[LOG_SIZE];
	unsigned int sample_number;
};

// Algorithm instance
struct sAlgorithm algorithm;

// State of the sequence parser
enum eSequenceParserState {cSequenceParserState_None, cSequenceParserState_Write, cSequenceParserState_Read};

// Prints the help text.
void help() {
	printf("Sends and receives data on the I2C bus.\n");
	printf("\n");
	printf("Usage: i2c [OPTIONS] DEVICE COMMANDS\n");
	printf("\n");
	printf("Options:\n");
	printf("  -r N      Executes the same sequence N times\n");
	printf("  -f        Keeps all result in memory before printing (for fast acquisition)\n");
	printf("  -F        Does not print any results (for performance checks)\n");
	printf("  -w US     Waits US us between subsequent acquisitions\n");
	printf("\n");
	printf("Device:\n");
	printf("  The I2C address of the device to access\n");
	printf("\n");
	printf("Commands: A sequence of\n");
	printf("  w|write BYTE1 BYTE2 ...  Sends these bytes to the device\n");
	printf("  r|read COUNT             Reads COUNT bytes from the device\n");
	printf("\n");
	printf("Examples:\n");
	printf("  Writing 155 to register 5 of device 0x50 (if this device is register-based):\n");
	printf("    i2c 0x50 write 5 155\n");
	printf("  Reading register 5 of device 0x50 (if this device is register-based):\n");
	printf("    i2c 0x50 write 5 read 1\n");
	printf("  Sending a packet of 8 bytes to device 50:\n");
	printf("    i2c 0x50 write 51 23 166 255 65 0 34 66\n");
	printf("  Reading 8 bytes from device 50:\n");
	printf("    i2c 0x50 read 8\n");
	printf("  The same, but 10 times:\n");
	printf("    i2c -r 10 0x50 read 8\n");
	printf("  The same, but without any output:\n");
	printf("    i2c -q -r 10 0x50 read 8\n");
	printf("  To see what is going on, add a -v as first parameter.\n");
	printf("\n");
	printf("Output format:\n");
	printf("  $DATA,sample_number,commit_return_value[,sequence]\n");
	printf("\n");
}

// Takes one measurement
void measurement_take(int i) {
	int n;
	const char *cmd;
	int result;
	enum eSequenceParserState state = cSequenceParserState_None;

	// Start a new I2C message
	i2cal_start();

	// Parse command line arguments
	for (n = 1; n < commandline_argument_count(); n++) {
		cmd = commandline_argument(n, "");
		if ((strcmp(cmd, "w") == 0) || (strcmp(cmd, "write") == 0)) {
			if (state == cSequenceParserState_Write) {
				i2cal_write(algorithm.i2c_address);
			}
			state = cSequenceParserState_Write;
		} else if ((strcmp(cmd, "r") == 0) || (strcmp(cmd, "read") == 0)) {
			if (state == cSequenceParserState_Write) {
				i2cal_write(algorithm.i2c_address);
			}
			state = cSequenceParserState_Read;
		} else if (state == cSequenceParserState_Write) {
			i2cal_writedata_uint8(commandline_argument_int(n, 0));
		} else if (state == cSequenceParserState_Read) {
			i2cal_read(algorithm.i2c_address, commandline_argument_int(n, 1));
			state = cSequenceParserState_None;
		} else {
			printf("Ignored: %s\n", cmd);
		}
	}
	if (state == cSequenceParserState_Write) {
		i2cal_write(algorithm.i2c_address);
	}

	// Execute this I2C messages
	result = i2cal_commit();

	algorithm.log_buffer[i].sample_number = algorithm.sample_number;
	algorithm.log_buffer[i].commit_return_value = result;
	algorithm.sample_number++;
}

// Prints one measurement
void measurement_print(int i) {
	int n, k;

	// Print success
	printf("$DATA,%d,%d", algorithm.log_buffer[i].sample_number, algorithm.log_buffer[i].commit_return_value);

	// Print sequence if this is the last entry
	if (algorithm.log_buffer[i].sample_number + 1 == algorithm.sample_number) {
		for (k = 0; k < i2cal.messages_nw; k++) {
			if (i2cal.messages[k].flags == I2C_M_RD) {
				printf(",read");
			} else {
				printf(",write");
			}
			for (n = 0; n < i2cal.messages[k].len; n++) {
				printf(" %02x", i2cal.messages[k].buf[n]);
			}
		}
	}
	printf("\n");
}

// Main program.
int main(int argc, char *argv[]) {
	// General initialization
	i2cal_init();
	measurement_init();

	// Command line parsing
	commandline_init();
	measurement_commandline_prepare();
	commandline_parse(argc, argv);

	// Help
	if (commandline_option_provided("-h", "--help")) {
		help();
		exit(1);
	}

	// Check the number of arguments provided
	if (commandline_argument_count() < 2) {
		help();
		exit(1);
	}

	// Read specific command line options
	algorithm.i2c_address = commandline_argument_int(0, 0);

	// Take continuous measures.
	measurement_configuration.log_size = LOG_SIZE;
	measurement_configuration.hook_measure = &measurement_take;
	measurement_configuration.hook_print = &measurement_print;
	measurement_start();

	return 0;
}
