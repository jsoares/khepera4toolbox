/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

// Algorithm parameters and results
struct sAlgorithm {
	struct {
		int port;
		int binary;
		int raw;
		int verbosity;
	} configuration;
};

// Algorithm instance
struct sAlgorithm algorithm;

// Prints the help text.
void help() {
	printf("Listens on a UDP port and prints all received datagrams. By default, port 3000 is used.\n");
	printf("\n");
	printf("Usage: udp_receiver [OPTIONS]\n");
	printf("\n");
	printf("Options:\n");
	printf("  -p --port PORT        Use port PORT (default: 3000)\n");
	printf("  -b --binary           Switches to binary message display (see output)\n");
	printf("  --raw                 Prints only the raw packets (not encapsulated into NMEA messages)\n");
	printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=default, 2=verbose, 3=very verbose, ...)\n");
	printf("\n");
	printf("Output:\n");
	printf("  Default mode:\n");
	printf("    $UDP, sender [host:port], data [text]\n");
	printf("  Binary mode:\n");
	printf("    $UDP, sender [host:port], data [hex notation]\n");
	printf("  In raw mode, only the data is printed (as text or in hex notation).\n");
	printf("\n");
}

// Initializes the algorithm.
void algorithm_init() {
	// Read command line options
	algorithm.configuration.port = commandline_option_value_int("-p", "--port", 3000);
	algorithm.configuration.binary = commandline_option_provided("-b", "--binary");
	algorithm.configuration.raw = commandline_option_provided(0, "--raw");
	algorithm.configuration.verbosity = commandline_option_value_int("-v", "--verbosity", 1);
}

// Runs the algorithm by calling the appropriate state function.
void algorithm_run() {
	int sock;
	struct sockaddr_in address;
	struct sockaddr_in sender;
	char recv_buffer[1024];
	int recv_len;
	socklen_t sender_len = sizeof(struct sockaddr_in);
	int res;
	int i;

	// Initialize the address
	address.sin_family = AF_INET;
	address.sin_port = htons(algorithm.configuration.port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	// Create socket and bind it
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock<0) {
		printf("Could not create socket: Error %d\n", errno);
	}
	res=bind(sock, (struct sockaddr *)&address, sizeof(address));
	if (res) {
		printf("Could not bind socket to port %d: Error %d\n", algorithm.configuration.port, errno);
	}

	// Process datagrams
	while (1) {
		// Wait for next datagram		
		recv_len = recvfrom(sock, recv_buffer, 1023, 0, (struct sockaddr *)&sender, &sender_len);

		// Print header information
		if (! algorithm.configuration.raw) {
			printf("$UDP,%s:%d,", inet_ntoa(sender.sin_addr), ntohs(sender.sin_port));
		}

		// Print data
		if (algorithm.configuration.binary) {
			if (recv_len>0) {
				printf("%02x", recv_buffer[0]);
				for (i=1; i<recv_len; i++) {
					printf(" %02x", recv_buffer[i]);
				}
			}
		} else {
			recv_buffer[recv_len]=0;
			printf("%s", recv_buffer);
		}

		// Print newline and flush
		printf("\n");
		fflush(stdout);
	}

	// Close socket (this statement is never reached)
	//close(sock);
}

// Main program.
int main(int argc, char *argv[]) {
	// Command line parsing
	commandline_init();
	commandline_option_register("-b", "--binary", cCommandLine_Option);
	commandline_option_register(0, "--raw", cCommandLine_Option);
	commandline_parse(argc, argv);

	// Help
	if (commandline_option_provided("-h", "--help")) {
		help();
		exit(1);
	}

	// Initialization
	algorithm_init();

	// Run
	algorithm_run();
	return 0;
}
