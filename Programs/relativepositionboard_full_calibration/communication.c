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

#include "communication.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

static void *communication_thread(void *p);
static int create_socket(communication_t *c);

int communication_send_message(communication_t *c, int dest, char *data, int len) {
    struct sockaddr_in RemAddr; // Address of other robot
    int n; // Size of message

    int AddrLen = sizeof (RemAddr); // Address size
    RemAddr.sin_family = AF_INET;
    if (dest >= 200 && dest <= 231)
        RemAddr.sin_port = htons(ROBOT_PORT);
    else
        RemAddr.sin_port = htons(ROBOT_PORT + c->robot_id); // Send on a separate port (to not confuse netcat)

    inet_aton(IP_ADDRESS, &RemAddr.sin_addr);
    RemAddr.sin_addr.s_addr += dest * 0x1000000; // 192.168.141.dest

    n = sendto(c->send_socket, data, len, 0, (struct sockaddr *) &RemAddr, AddrLen);

    if (n < 0) {
        printf("Error sending message \"%s\" to %d\n", data, dest);
        return -1;
    }

    return 0;
}

int communication_broadcast_message(communication_t *c, char *data, int len) {
    return communication_send_message(c, -1, data, len);
}

int communication_init(communication_t *c, int robot_id, struct gengetopt_args_info *options) {
    c->robot_id = robot_id;
    c->recv_port = ROBOT_PORT;
    c->msg_rdy = 0;
    c->finish = 0;

    c->msg = malloc(BUFFER_LENGTH);

    printf("Initializing communication of robot %d on port %d...\n", robot_id, c->recv_port);
    if (create_socket(c) != 0) return -1;
    printf("UDP socket initialized\n");

    // Create remote communication thread
    pthread_create(&c->robot_communication_thread, NULL, &communication_thread, (void *) c);

    return 0;
}

void communication_destroy(communication_t *c) {
    c->finish = 1;
    // Wait for the thread to finish
    pthread_join(c->robot_communication_thread, NULL);
    close(c->recv_socket);
    close(c->send_socket);
    free(c->msg);
    printf("UDP socket closed\n");
}

void *communication_get_message(communication_t *c, int *sender_id, int *len) {
    if (!c->msg_rdy) return NULL;

    *sender_id = c->msg_sender_id;
    *len = c->msg_len;
    return c->msg;
}

void communication_next_message(communication_t *c) {
    c->msg_rdy = 0;
    usleep(1000);
    return;
}

static void *communication_thread(void *p) {
    communication_t *c = (communication_t *) p;

    struct sockaddr_in RemAddr; // Address of other robot
    char msg_buf[BUFFER_LENGTH]; // Message from socket
    int len; // Number of received bytes
    int src; // ID of message sender
    unsigned int AddrLen = sizeof (RemAddr); // Address size

    while (!c->finish) {
        // Attempt to read
        if ((len = recvfrom(c->recv_socket, msg_buf, BUFFER_LENGTH, 0, (struct sockaddr *) &RemAddr, &AddrLen)) <= 0) {
            // Error unless nothing received
            if (errno != EWOULDBLOCK) {
                printf("Reading from UDP socket failed: %d, %d\n", errno, c->recv_socket);
                return NULL;
            }
            usleep(10000);

        } else {
            // Get sender ID
            src = RemAddr.sin_addr.s_addr / 0x1000000; // Take highest byte

            // Get message from other robot

            // Wait until main thread processes previous message
            // One should really do this with a mutex!!!
            while (c->msg_rdy) {
                usleep(500);
                if (c->finish) return NULL;
            }

            // Get message info
            memcpy(c->msg, msg_buf, len);
            c->msg_len = len;
            c->msg_sender_id = src;
            c->msg_rdy = 1;
            usleep(500);
        }
    }

    return NULL;
}

static int create_socket(communication_t *c) {
    struct sockaddr_in LocalAddr; // Address of robot
    struct linger TimeOut; // Timeout structure for socket

    int AddrLen = sizeof (LocalAddr); // Address size
    int one = 1; // Needed for ioctl()

    /* Create sockets */
    c->recv_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (c->recv_socket < 0) {
        printf("Could not create UDP socket\n");
        return -1;
    }
    c->send_socket = socket(AF_INET, SOCK_DGRAM, 0);

    /* Allow broadcasting */
    setsockopt(c->send_socket, SOL_SOCKET, SO_BROADCAST, (void *) &one, sizeof (one));

    /* Initialize local address */
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    LocalAddr.sin_port = htons(c->recv_port);

    /* Bind local address to socket */
    if (bind(c->recv_socket, (struct sockaddr *) &LocalAddr, AddrLen) < 0) {
        printf("Could not bind address to UDP socket\n");
        close(c->recv_socket);
        close(c->send_socket);
        return -1;
    }

    /* Set socket to non-blocking mode */
    if (ioctl(c->recv_socket, FIONBIO, (char *) &one))
        printf("Could not set to non-block mode\n");

    /* Abort if timeout period exceeded */
    TimeOut.l_onoff = 1;
    TimeOut.l_linger = 0;
    if (setsockopt(c->recv_socket, SOL_SOCKET, SO_LINGER, (char *) &TimeOut, sizeof (TimeOut)))
        printf("Timeout enable failed\n");

    return 0;
}
