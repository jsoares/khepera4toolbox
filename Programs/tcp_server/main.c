/*!
 * (c) 2006-2009 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "khepera3.h"
#include "commandline.h"
#include "tcp_nmea.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Prints the help text.
void help() {
	printf("Opens a TCP port for listening and executes motor commands of a connected TCP client.\n");
	printf("\n");
	printf("Usage: tcp_server [OPTIONS]\n");
	printf("\n");
	printf("Options:\n");
	printf("  -p --port PORT     Listens on port PORT (default: 3000)\n");
	printf("\n");
	printf("Commands:\n");
	printf("  $SPEED,left,right  Sets the motor speed\n");
	printf("  $IR_PROXIMITY      Replies with the current IR proximity values\n");
	printf("\n");
}

// This method is called each time a NMEA message is received.
void tcp_server_hook_process_message(struct sNMEAMessage *m, int withchecksum) {
	int i;

	if (strcmp(m->command, "SPEED") == 0) {
		// Set the speed
		if (m->argument_count >= 2) {
			khepera3_drive_set_speed(strtol(m->argument[0], 0, 0), strtol(m->argument[1], 0, 0));
		}
	} else if (strcmp(m->command, "IR_PROXIMITY") == 0) {
		// Report the IR proximity values
		khepera3_infrared_proximity();
		fprintf(tcp_nmea.connection_file, "$IR");
		for (i = 0; i < 11; i++) {
			fprintf(tcp_nmea.connection_file, ",%d", khepera3.infrared_proximity.sensor[i]);
		}
		fprintf(tcp_nmea.connection_file, "\n");
	}
}

// Executes the main loop.
void tcp_server_run(int port) {
	// Set up the listener socket
	tcp_nmea_init();
	tcp_nmea.hook_process_message = tcp_server_hook_process_message;
	tcp_nmea_start(port);

	// Put the wheels in normal (control) mode
	khepera3_drive_start();

	while (1) {
		// Check if new data from is available on the TCP port and process it (this will call tcp_server_nmea_receive_hook)
		tcp_nmea_receive();

		// Sleep for a while
		usleep(20000);
	}
}

// Main program.
int main(int argc, char *argv[]) {
	int port;

	// Command line parsing
	commandline_init();
	commandline_parse(argc, argv);

	// Help
	if (commandline_option_provided("-h", "--help")) {
		help();
		exit(1);
	}

	// Initialization
	khepera3_init();

	// Read arguments
	port = commandline_option_value_int("-p", "--port", 3000);

	// Run server
	tcp_server_run(port);
	return 0;
}

