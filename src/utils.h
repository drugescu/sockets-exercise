#ifndef MAIN_UTILS_H
#define MAIN_UTILS_H    1

#include <errno.h>

#define DIE(assertion, call_description)                \
    do {                                \
        if (assertion) {                    \
            fprintf(stderr, "(%s, %d): ",           \
                    __FILE__, __LINE__);        \
            perror(call_description);           \
            exit(errno);                    \
        }                           \
    } while (0)

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>

#define BUF_SIZE        1024
#define MAX_CLIENTS       12

typedef struct sockaddr_in sai_t;
typedef struct sockaddr sa_t;

#endif
