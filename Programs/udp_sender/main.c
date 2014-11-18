/*!
 * (c) 2006-2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#include "commandline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

// Algorithm parameters and results

struct sAlgorithm {

    struct {
        int port;
        char hostname[1024];
        int binary;
        int verbosity;
    } configuration;

    struct {
        char buffer[1024];
        int len;
    } message;
};

// Algorithm instance
struct sAlgorithm algorithm;

// Prints the help text.

void help() {
    printf("Sends a UDP packet to another computer/robot. By default, port 3000 is used.\n");
    printf("\n");
    printf("Usage: udp_sender [OPTIONS] HOST[:PORT] MESSAGE\n");
    printf("\n");
    printf("Options:\n");
    printf("  -b --binary           Switches to binary message interpretation (see examples)\n");
    printf("  -v --verbosity V      Sets the verbosity level (0=quiet, 1=default, 2=verbose, 3=very verbose, ...)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  Send a text message to port  robot 203 (192.168.141.203):\n");
    printf("    udp_sender 192.168.141.203 \"Hello world\"\n");
    printf("  Send a binary message (ABC) to same robot:\n");
    printf("    udp_sender -b 192.168.141.203 65 66 67\n");
    printf("  The same, but with hex notation:\n");
    printf("    udp_sender -b 192.168.141.203 0x41 0x42 0x43\n");
    printf("\n");
}

void parse_hostname_port() {
    char *port_str;

    // Check if there is a ':' sign in the hostname
    port_str = strchr(algorithm.configuration.hostname, ':');
    if (!port_str) {
        return;
    }

    // Terminate the hostname string there
    *port_str = 0;

    // Convert the rest of the string to a number
    port_str++;
    algorithm.configuration.port = strtol(port_str, 0, 0);
    if ((algorithm.configuration.port < 1) || (algorithm.configuration.port > 65535)) {
        printf("Invalid port: %s (interpreted as %d)\n", port_str, algorithm.configuration.port);
        exit(2);
    }
}

void parse_message() {
    int i;
    int argument_len;
    const char *argument;
    int total_length = 0;

    // Parse all but the first command line argument to generate the message
    if (algorithm.configuration.binary) {
        // In binary mode, convert each number to a character
        for (i = 1; i < commandline_argument_count(); i++) {
            algorithm.message.buffer[i - 1] = commandline_argument_int(i, 0);
        }
        algorithm.message.len = i - 1;
    } else {
        // In text mode, concatenate all arguments
        algorithm.message.buffer[0] = 0;
        for (i = 1; i < commandline_argument_count(); i++) {
            argument = commandline_argument(i, 0);
            argument_len = strlen(argument);
            total_length += argument_len;
            if (total_length > 1024) {
                printf("Maximum message length (1024) exceeded!\n");
                exit(2);
            }
            strcat(algorithm.message.buffer, argument);
        }
        algorithm.message.len = total_length;
    }
}

// Initializes the algorithm.

void algorithm_init() {
    // Read command line options
    algorithm.configuration.binary = commandline_option_provided("-b", "--binary");
    algorithm.configuration.verbosity = commandline_option_value_int("-v", "--verbosity", 1);

    // Check if the user provided enough arguments
    if (commandline_argument_count() < 2) {
        help();
        exit(1);
    }

    // Get hostname and port (first argument)
    algorithm.configuration.port = commandline_option_value_int("-p", "--port", 3000);
    strncpy(algorithm.configuration.hostname, commandline_argument(0, 0), 1023);
    parse_hostname_port();

    // Parse the message
    parse_message();
}

// Runs the algorithm by calling the appropriate state function.

void algorithm_run() {
    int sock;
    struct sockaddr_in address;
    struct hostent *host;
    int res;

    // Initialize the address
    address.sin_family = AF_INET;
    address.sin_port = htons(algorithm.configuration.port);

    // Hostname resolution
    host = gethostbyname(algorithm.configuration.hostname);
    if (!host) {
        printf("Unable to resolve hostname %s\n", algorithm.configuration.hostname);
        exit(2);
    }
    address.sin_addr.s_addr = *(unsigned long*) host->h_addr;

    // Create datagram socket and send the datagram
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    res = sendto(sock, algorithm.message.buffer, algorithm.message.len, 0, (struct sockaddr *) & address, sizeof (struct sockaddr_in));
    if (res > 0) {
        printf("UDP datagram (%d bytes) sent!\n", res);
    } else {
        printf("Error (%d) while sending UDP datagram!\n", res);
    }
}

// Main program.

int main(int argc, char *argv[]) {
    // Command line parsing
    commandline_init();
    commandline_option_register("-b", "--binary", cCommandLine_Option);
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
