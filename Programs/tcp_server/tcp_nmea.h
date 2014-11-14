/*!
 * (c) 2006-2009 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef TCP_NMEA
#define TCP_NMEA

#include "nmea.h"
#include <stdio.h>

// TCP NMEA structure
struct sTCPNMEA {
	int server_filehandle;					// The filehandle of the listener socket.
	int connection_filehandle;				// The filehandle of the connection. (Note that only one connection is accepted, i.e. any new connection closes the current one.)
	FILE *connection_file;
	struct sNMEAParser parser;		// The parser.
	void (*hook_process_message)(struct sNMEAMessage *m, int withchecksum);	// The process message hook.
};

// Public instance of the structure
struct sTCPNMEA tcp_nmea;

// Initializes the module.
void tcp_nmea_init();
// Opens a TCP listener socket.
int tcp_nmea_start(int port);
// Checks for incoming connections or incoming data.
int tcp_nmea_receive();

#endif
