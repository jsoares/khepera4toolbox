

/*!
 * (c) 2011 EPFL, Lausanne, Switzerland
 *  Example of usage: ./relativepositionboard --rb-rate=0.5 --pc-id=20 --id=0
 */

#include "khepera4.h"
#include "communication.h"
#include "relativepositionboard.h"
#include "windsensorboard.h"

#include "cmdline.h"
#include "kh4_utils.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* The usual */
struct gengetopt_args_info options;
int running = 1;

/* The communication */
communication_t communication;

int parseCmdLine(int argc, char *argv[]) {
    /* Parse arguments */
    cmdline_parser(argc, argv, &options);

    /* Check arguments */
    if (options.rb_rate_arg < 0.1)
        options.rb_rate_arg = 0.1;
    else if (options.rb_rate_arg > 20.0)
        options.rb_rate_arg = 20.0;

    if (options.id_arg < 0) options.id_arg = 0;
    if (options.id_arg > 31) options.id_arg = 31;

    return 0;
}

void exitProgram(int signal) {
    printf("Stopping program...\n");
    running = 0;
}

/**** Main algorithm  ****/
void run() {
    static int sign = 1;
    int i, len;
    char buffer[256];
    int speed_left = 4000;
    int speed_right = 4000;

    /* Sending received RSSI data */
    relativepositionboard_stream_step();
    for (i = 0; i < 32; i++) {
        if (i == options.id_arg) continue;
        if (relativepositionboard.robot[i].age != 0) continue;

        len = snprintf(buffer, 256, "$RSSI,%d,%d,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f\n",
                options.id_arg, i,
                relativepositionboard.robot[i].rssi[0],
                relativepositionboard.robot[i].rssi[1],
                relativepositionboard.robot[i].rssi[2],
                relativepositionboard.robot[i].rssi[3],
                relativepositionboard.robot[i].rssi[4],
                relativepositionboard.robot[i].rssi[5],
                relativepositionboard.robot[i].rssi[6],
                relativepositionboard.robot[i].rssi[7]);
        communication_send_message(&communication, options.pc_arg, buffer, len);
    }

    /* Do not go straight: generates better training data */
    speed_left += sign * speed_left / 10;
    speed_right -= sign * speed_right / 10;
    if ((rand() % 300) == 0) sign = -sign;

    braitenberg(&speed_left, &speed_right);
    khepera4_drive_set_speed(speed_left, speed_right);
}

/**** Main program *****/
int main(int argc, char *argv[]) {
    void *msg;
    int sender_id;
    int length;
    int n = 0;

    khepera4_init();

    /* Parse command line arguments */
    parseCmdLine(argc, argv);

    /* Initialize the khepera 4 */
    motor_init();
    rb_init(&options);
    tracking_init();

    /* Start communication */
    communication_init(&communication, options.id_arg, &options);

    /* Catch SIGINT (CTRL+C) */
    signal(SIGINT, exitProgram);

    /* Wait for 3 start messages */
    printf("Waiting for start...\t");
    fflush(stdout);
    msg = communication_get_message(&communication, &sender_id, &length);
    while (n < 3 && running) {
        if (!msg) {
            usleep(10000);
            communication_next_message(&communication);
            msg = communication_get_message(&communication, &sender_id, &length);
            continue;
        }

        if (sender_id != options.pc_arg) {
            printf("Received message from %d (is that normal?)\n", sender_id);
            usleep(10000);
            communication_next_message(&communication);
            msg = communication_get_message(&communication, &sender_id, &length);
            continue;
        }

        n++;
        usleep(10000);
        communication_next_message(&communication);
        msg = communication_get_message(&communication, &sender_id, &length);
    }
    if (!running) {
        printf("[FAIL]\n");
        communication_destroy(&communication);
        khepera_stop();
        return -1;
    }
    printf("[ OK ]\n");

    /* Calibrate (2 seconds to do that) */
    printf("Calibrating (offset) RB board...\t");
    fflush(stdout);
    sleep(1);
    rb_calibrate();
    ir_calibrate();
    sleep(1);
    printf("[ OK ]\n");

    /* Start transmitting */
    relativepositionboard_start(options.id_arg);

    /* Run */
    while (running) {
        run();
        usleep(1000); // do not hog the CPU
    }

    /* Stop */
    communication_destroy(&communication);
    khepera_stop();

    return 0;

}
