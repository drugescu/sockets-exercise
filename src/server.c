/*
 * server.c - Practice 3
 *
 * 2018 drugescu <drugescu@drugescu-VirtualBox>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 */

#include "utils.h"

int sockfd = 0;

sai_t sv_addr;
sai_t cl_addr;

fd_set read_fds;
fd_set temp_fds;

int fdmax = 0;

// TODO: Will implement a hashmap for a custom client structure containing
//          address, port, and other details

// Interrupt signal handler - perform cleanup
void sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("\nReceived SIGINT\n");

        // Cleanup
        close(sockfd);

        printf("Server shut down, sockets closed.\n");
    }

    exit(EXIT_SUCCESS);
}

// Listen to incoming client requests
void listen_requests(int port) {

    printf("Listening to clients on port %d...\n", port);
    listen(sockfd, MAX_CLIENTS);

    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;

    while (true) { // Loop ad infinitum
        // Listen to I/O
        temp_fds = read_fds;
        if (select(fdmax + 1, &temp_fds, NULL, NULL, NULL) == -1)
            DIE(1 == 1, "Error in select.\n");

        int index;
        for (index = 0; index <= fdmax; index++) {
            if (FD_ISSET(index, &temp_fds)) {
                if (index == sockfd) { // New connection on listening socket
                    // Default action, accept connection
                    int client_length;
                    client_length = sizeof(cl_addr);

                    int newsockfd;
                    newsockfd = accept(sockfd, (sa_t *) &cl_addr, &client_length);
                    DIE (newsockfd == -1, "Error in accept.\n");

                    // Add new socket to fds
                    FD_SET(newsockfd, &read_fds);
                    if (newsockfd > fdmax)
                        fdmax = newsockfd;

                    printf("Established new connection with client %s:%d, fd:%d.\n",
                            inet_ntoa(cl_addr.sin_addr), ntohs(cl_addr.sin_port),
                            newsockfd);

                    // Insert client into our client list
                }
                else { // We've received something on one of the clients sockets
                    // Default action, recv
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    // Initialization
    printf("Main pid is %d\n", getpid());
    srand(time(NULL));
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("Could not register sig_handler, can't catch SIGINT\n'");

    // Empty descriptors
    FD_ZERO(&read_fds);

    // Open server TCP socket on port from argument 1, listen from all addresses
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "Error opening socket!\n");

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(atoi(argv[1]));
    sv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket
    if (bind(sockfd, (sa_t *) &sv_addr, sizeof(sv_addr)) != -1) {
        // Listen to requests
        listen_requests(atoi(argv[1]));
    }
    else {
        perror("Couldn't open socket!\n'");
    }

    return 0;
}
