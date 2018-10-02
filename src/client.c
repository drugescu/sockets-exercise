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
char buffer[ BUF_SIZE ] = { 0 };
int sock = 0;
int rc;
int of; // Output fd

// Far declaration
void cleanup();

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

// Sends request for a file, followed by a close connection message
void send_request(int socket, char *filename) {
    // Send request to disconnect
    send(sock, filename, strlen(filename) , 0 );
    printf("Sent request for file \"%s\" to server.\n", filename);

    // Ensure request termination - simple protocol, \t is command separator
    send(sock, COMMAND_SEPARATOR, strlen(COMMAND_SEPARATOR) , 0 );
}

// Call this function each iteration of a work function for updating
void update_progress(int count, int size) {
    float progress = 0.0f;
    int bar_width = 70;
    int current_pos;
    int pos = 0;

    printf("[");

    // Generate new progress percentage
    progress = (float) count / size;
    pos = bar_width * progress;

    // Print progress
    for (current_pos = 0; current_pos < bar_width; current_pos++) {
        if (current_pos < pos)
            printf("=");
        else if (current_pos == pos)
            printf(">");
        else
            printf(" ");
    }

    // Flush to keep same line
    printf("] %d%%\r", (int)(progress * 100));
    fflush(stdout);
}

// Creates file, receives length, then receives until length bytes read.
void recv_file(int socket, char* filename) {
    unsigned long size; // Size of file to write
    char *ptr;
    char destination[ NAME_SIZE ]; // Destination name "received_[filename]"
    int sz; // Size of written bytes
    int count = 0; // Count of bytes received

    // Receive file length first.
    memset(buffer, 0, BUF_SIZE);
    rc = read(socket, buffer, BUF_SIZE);
    DIE (rc == 0, "Read returned 0.\n");

    // Parse file size
    char *sztoken = NULL;
    sztoken = strtok(buffer, COMMAND_SEPARATOR);

    size = strtoul(sztoken, &ptr, 10);
    count = rc - strlen(sztoken) - 1;  // Don't forget the separator
    DIE(size == 0, "File size is zero.\n");

    printf("File size is %lu.\n", size);

    // Create the new file
    sprintf(destination, "received_%s", filename);
    of = open(destination, O_CREAT | O_RDWR | O_TRUNC, 0644);
    DIE(of < 0, "Error in file creation.\n");

    printf("Buffer contains:\n%s\n", buffer);

    // Initial packet might have sent us a bit of the file
    if (count > 0) {
        char *trimmed_buffer;
        trimmed_buffer = buffer + strlen(sztoken);
        printf("Writing file part (initial)...\n");
        sz = write(of, trimmed_buffer, count);
        printf("Wrote file part (initial).\n");

        // Clear buffer
        memset(buffer, 0, BUF_SIZE);
    }

    // Receive all bytes
    while((rc > 0) && (count < size)) {
        // Receive bytes
        //printf("Reading socket for next file part...\n");
        rc = read(socket, buffer, BUF_SIZE);
        count += rc;
        //printf("Read socket for next file part.\n");

        // Write to file
        //printf("Writing file part...\n");
        sz = write(of, buffer, rc);
        //printf("Wrote file part.\n");

        // Clear buffer
        memset(buffer, 0, BUF_SIZE);

        // Update progress bar
        update_progress(count, size);

#ifdef DEBUG
        usleep(100);
#endif
    }

    // Done
    close(of);
    fflush(stdout);
    printf("\nReceived file.\n");

#ifdef DEBUG
    char *parmList[] ={ "sudo cat", destination, NULL};
    execv("/bin/cat", parmList);
#endif
}

void disconnect(int socket) {
    // Send request to disconnect
    send(sock, CLOSE_MESSAGE, strlen(CLOSE_MESSAGE) , 0 );
    printf("Sent message %s to server.\n", CLOSE_MESSAGE);
}

// Close all sockets
void cleanup() {
    close(sock);
    close(of);
}

int main(int argc, char **argv)
{

    char BUF[ BUF_SIZE ] = { 0 };
    char *msg;

    DIE(argc == 1, "No ADDR(1), PORT(2) or FILE(3) arguments given, exiting.\n");
    DIE(argc == 2, "No PORT(2) or FILE(3) arguments given, exiting.\n");
    DIE(argc == 3, "No FILE(3) argument given, exiting.\n");

    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("Could not register sig_handler, can't catch SIGINT\n'");

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

    printf("Established connection with server %s on port %d.\n", argv[1], atoi(argv[2]));

    // Now send requests
    send_request(sock, argv[3]);

    // Receive the file
    recv_file(sock, argv[3]);

    // Send disconnect message
    disconnect(sock);

    // Close sockets
    cleanup();

    return 0;
}
