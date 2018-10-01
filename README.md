# Practice 3

## Sockets exercise

Server and client apps that send files through sockets.

### Server
* Listens on argv port for requests for files. A request is a filename.
* Sends the file through TCP, or the string message "File not found".
* Can receive requests from multiple clients.

### Client
* Establishes connection to the server and sends requests for various files.
* Connection is requested to server TCP port argv[1].
* A request is a filename, assumed directory-less (root dir of server).
* Saves the file being sent to it by the server.
