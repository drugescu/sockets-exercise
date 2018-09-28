/*
 * server.c - Practice 3
 *
 * 2018 drugescu <drugescu@drugescu-VirtualBox>
 *
 */

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <mqueue.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>

#include "utils.h"


// Interrupt signal handler - perform cleanup
void sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("\nReceived SIGINT\n");

        // Cleanup
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    // Initialization
    printf("Main pid is %d\n", getpid());
    srand(time(NULL));
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("Could not register sig_handler, can't catch SIGINT\n'");

    return 0;
}
