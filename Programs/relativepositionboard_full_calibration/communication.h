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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <pthread.h>
#include "cmdline.h"

#define IP_ADDRESS    "192.168.141.0"
#define ROBOT_PORT     4000
#define BUFFER_LENGTH  256

typedef struct communication {
    int recv_socket;
    int send_socket;
    int recv_port;

    int robot_id;

    pthread_t robot_communication_thread;

    // Messages
    int msg_sender_id;
    void *msg;
    int msg_len;
    int msg_rdy;

    // To kill the thread
    int finish;
} communication_t;

int communication_send_message(communication_t *c, int dest, char *data, int len);
int communication_broadcast_message(communication_t *c, char *data, int len);
int communication_init(communication_t *c, int robot_id, struct gengetopt_args_info *options);
void communication_destroy(communication_t *c);
void *communication_get_message(communication_t *c, int *sender_id, int *len);
void communication_next_message(communication_t *c);

#endif
