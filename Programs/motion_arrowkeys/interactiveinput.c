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


#include "interactiveinput.h"
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

void interactiveinput_init() {
    struct termios tty;

    // Set icanon on stdin to disable line buffering
    tcgetattr(0, &tty);
    tty.c_lflag &= ~ICANON;
    tcsetattr(0, TCSAFLUSH, &tty);
}

int interactiveinput_waitkey(int timeout_usec) {
    fd_set fhset;
    int maxfh = 0;
    struct timeval timeout;
    int res;
    int len;
    char buffer;

    // Select
    FD_ZERO(&fhset);
    FD_SET(STDIN_FILENO, &fhset);
    if (maxfh < STDIN_FILENO) {
        maxfh = STDIN_FILENO;
    }
    timeout.tv_sec = 0;
    timeout.tv_usec = timeout_usec;
    maxfh++;
    res = select(maxfh, &fhset, NULL, NULL, &timeout);
    if (!res) {
        return -1;
    }

    // Read char
    len = read(STDIN_FILENO, &buffer, 1);
    if (len) {
        return (int) buffer;
    }
    return -1;
}
