/*!
 * (c) 2006-2009 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "tcp_nmea.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

void tcp_nmea_init() {
    tcp_nmea.hook_process_message = 0;
    tcp_nmea.server_filehandle = -1;
    tcp_nmea.connection_filehandle = -1;
    nmea_parser_init(&tcp_nmea.parser);
}

int tcp_nmea_start(int port) {
    struct sockaddr_in sa;
    struct hostent *host;
    int bindres, listenres;

    // Don't touch anything if the port is zero
    if (port == 0) {
        return 0;
    }

    // Initialize
    tcp_nmea.server_filehandle = -1;
    tcp_nmea.connection_filehandle = -1;
    tcp_nmea.connection_file = 0;

    // Create socket handle
    tcp_nmea.server_filehandle = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_nmea.server_filehandle < 0) {
        fprintf(stderr, "Unable to create socket: Error %d.\n", tcp_nmea.server_filehandle);
        tcp_nmea.server_filehandle = -1;
        return 0;
    }

    // Bind (on all local addresses)
    host = gethostbyname("0.0.0.0");
    memcpy((char *) &sa.sin_addr, (char *) host->h_addr, host->h_length);
    sa.sin_family = host->h_addrtype;
    sa.sin_port = htons(port);

    bindres = bind(tcp_nmea.server_filehandle, (struct sockaddr *) & sa, sizeof (sa));
    if (bindres < 0) {
        if (bindres == EACCES) {
            fprintf(stderr, "Bind failed: You do not have enough access rights to listen on port %d.\n", port);
        } else {
            fprintf(stderr, "Bind failed: Error %d.\n", bindres);
        }
        tcp_nmea.server_filehandle = -1;
        return 0;
    }

    // Listen
    listenres = listen(tcp_nmea.server_filehandle, SOMAXCONN);
    if (listenres < 0) {
        if (listenres == EADDRINUSE) {
            fprintf(stderr, "Listen failed: Port %d is already in use.\n", port);
        } else {
            fprintf(stderr, "Listen failed: Error %d.\n", listenres);
        }
        tcp_nmea.server_filehandle = -1;
        return 0;
    }

    // Set the non-blocking flag (important!), because we ask the kernel regularly whether a new connection is there (polling)
    fcntl(tcp_nmea.server_filehandle, F_SETFL, O_NONBLOCK);

    // Success!
    //fprintf(stderr, "Listening on TCP port %d.\n", port);
    return 1;
}

int tcp_nmea_accept() {
    struct sockaddr_in sc;
    socklen_t sclen;
    int fh;

    // If there is no server, do nothing
    if (tcp_nmea.server_filehandle < 0) {
        return 0;
    }

    // Accept
    sclen = sizeof (sc);
    fh = accept(tcp_nmea.server_filehandle, (struct sockaddr *) & sc, &sclen);
    if (fh < 0) {
        if ((errno != EINTR) && (errno != EAGAIN)) {
            fprintf(stderr, "Accept failed (Error %d): connection request ignored.\n", errno);
        }
        return 0;
    }

    // Set non-blocking mode (important!), because our main loop shall run all the time and check if new data is available (polling)
    fcntl(fh, F_SETFL, O_NONBLOCK);

    // If we have a connection already, close it
    if (tcp_nmea.connection_filehandle > -1) {
        fclose(tcp_nmea.connection_file);
        tcp_nmea.connection_filehandle = -1;
        tcp_nmea.connection_file = 0;
    }

    // Set the new connection and initialize the parser
    fprintf(stderr, "Connection accepted.\n");
    tcp_nmea.connection_filehandle = fh;
    tcp_nmea.connection_file = fdopen(tcp_nmea.connection_filehandle, "a");
    nmea_parser_init(&tcp_nmea.parser);
    tcp_nmea.parser.hook_process_message = tcp_nmea.hook_process_message;
    return 1;
}

int tcp_nmea_receive() {
    char buffer[128];
    int len;

    // Check for new connections
    tcp_nmea_accept();

    // Only go on if there is a connection
    if (tcp_nmea.connection_filehandle < 0) {
        return 0;
    }

    // Read and parse until there is nothing available any more
    while (1) {
        len = read(tcp_nmea.connection_filehandle, buffer, 128);
        if (len < 1) {
            return 0;
        }
        nmea_parser_process_data(&tcp_nmea.parser, buffer, len);
    }

    return 1;
}
