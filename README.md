# Practice 3

## Sockets exercise

Server and client apps that send files through sockets.

### Server
* Listens on argv port for requests for files. A request is typically a filename.
* Sends the file through TCP, or the string message `"File not found"`.
* Can receive requests from multiple clients, multiplexed by TCP port.
* Usage: `./server.out 5001`

### Client
* Establishes connection to the server and sends requests for various files.
* Connection is requested to server IP argv[1] on TCP port argv[2].
* A request of form `"[REQUESTING_DISCONNECT]"` will trigger sv|cl disconnect.
* A request of a filename is assumed directory-less (root dir of server).
* Saves the file being sent to it by the server.
* Usage: [server_ip] [server_port] [server_file]: `./client.out 127.0.0.1 5001 file1`

### Protocol
* `'\t'` character is command separator.
* When sending a file, we first send size (unsigned long), then packets of the file.
