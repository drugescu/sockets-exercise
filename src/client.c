/*
 * client.c - Practice 3
 *
 * 2018 drugescu <drugescu@drugescu-VirtualBox>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 */

#include "utils.h"

struct sockaddr_in addr;
struct sockaddr_in server_addr;
int sock = 0;
int rc;

// Sends request for a file, followed by a close connection message
void send_request(int socket) {
    send(sock, CLOSE_MESSAGE, strlen(CLOSE_MESSAGE) , 0 );
    printf("Sent message %s to server.\n", CLOSE_MESSAGE);
}

// Close all sockets
void cleanup() {
    close(sock);
}

int main(int argc, char **argv)
{

    char BUF[ BUF_SIZE ] = { 0 };
    char *msg;

    DIE(argc == 1, "No ADDR and PORT arguments given, exiting.\n");
    DIE(argc == 2, "No PORT argument given, exiting.\n");

    // Establish connection with server
    sock = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sock < 0, "Socket creation error in client.\n");

    memset(&server_addr, '0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    rc = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    DIE(rc <= 0, "Error in address convertion (inet_pton).\n");

    rc = connect(sock, (sa_t *)&server_addr, sizeof(server_addr));
    DIE (rc < 0, "Error in connection to server.\n");

    printf("Established connection with server 127.0.0.1 on port %d.\n", atoi(argv[1]));

    // Now send requests
    send_request(sock);

    // Close sockets
    cleanup();

    return 0;
}
