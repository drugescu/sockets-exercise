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

char buffer[ BUF_SIZE ];

client_map_t clients;

// TODO: Will implement a hashmap for a custom client structure containing
//          address, port, and other details

void cleanup() {

    // Clear all socket flags and close them
    for (int index = 0; index <=fdmax; index++) {
        FD_CLR(index, &read_fds);
        close(index);
    }

    // Close server socket and deinit hashmap
    close(sockfd);
    map_deinit(&clients);

    printf("Server shut down, sockets closed, hashmap deinitialized.\n");
}

// Interrupt signal handler - perform cleanup
void sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("\nReceived SIGINT\n");

        // Cleanup
        cleanup();
    }

    exit(EXIT_SUCCESS);
}

void initialize() {
    map_init(&clients);
    memset(&buffer, 0, BUF_SIZE);
}

// Establish new connection with client on socket sockfd
void new_connection(int sockfd) {
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
    client_t *new_client;
    new_client = malloc(sizeof(client_t));
    DIE(new_client == NULL, "Error in client struct allocation.\n");

    memset(new_client->name, 0, NAME_SIZE);
    new_client->addr = cl_addr.sin_addr;
    new_client->port = cl_addr.sin_port;
    sprintf(new_client->name, "client%d", newsockfd);
    printf("Set attributes of new client.\n");

    map_set(&clients, new_client->name, new_client);
    printf("Inserted %s into hashmap.\n", new_client->name);

    // Now we can safely delete the temporary client_t, the hashmap makes a copy
    free(new_client);
    new_client = NULL;
}

// Treat request for file from socket sockfd
void receive_request(int sockfd) {

    // Check if we have connection to this client, if not reject
    char client_key[ NAME_SIZE ] = { 0 };
    sprintf(client_key, "client%d", sockfd);

    client_t *val;

    val = *map_get(&clients, client_key);
    printf("Map get on client key \"%s\".\n", client_key);
    if (val == NULL) {
        printf("Client NOT in hashmap.\n");

        return;
    }
    else {
        printf("Client in hashmap.\n");
    }

    // Clear buffer
    memset(&buffer, 0, BUF_SIZE);

    // Request can be for a name of a file, send the file then close connection.
    int rc;
    rc = read(sockfd, buffer, BUF_SIZE);
    if (!strlen(buffer))
        return;

    printf("Received request \"%s\" on socket %d.\n", buffer, sockfd);

    // If the request is to close the message
    if (strcmp(CLOSE_MESSAGE, buffer) == 0) {
        // Delete client from hashmap
        map_remove(&clients, client_key);

        // Now close the socket
        close(sockfd);
        FD_CLR(sockfd, &read_fds);

        printf("Removed client from records and closed its socket.\n");
    }

}

// Listen to incoming client requests
void listen_requests(int port) {

    // Initial client setup
    initialize();

    printf("Listening to clients on port %d...\n", port);
    listen(sockfd, MAX_CLIENTS);

    // Set socked in use
    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;

    while (true) { // Loop ad infinitum
        // Listen to I/O
        temp_fds = read_fds;
        if (select(fdmax + 1, &temp_fds, NULL, NULL, NULL) == -1)
            DIE(1 == 1, "Error in select.\n");

        // If we recv'd a message
        int index;
        for (index = 0; index <= fdmax; index++) {
            if (FD_ISSET(index, &temp_fds)) {
                if (index == sockfd) { // New connection on listening socket
                    new_connection(sockfd);
                }
                else { // We've received something on one of the clients sockets
                    // Default action, read
                    receive_request(index);
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

    DIE(argc == 1, "No PORT argument given. exiting");

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
