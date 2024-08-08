#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_error(const char *msg, int server_fd, int client_fd) {
    perror(msg);
    if (client_fd >= 0) close(client_fd);
    if (server_fd >= 0) close(server_fd);
    exit(EXIT_FAILURE);
}

int main() {
    // Creating  socket file descriptor.
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) handle_error("socket failed", -1, -1);

    // Attaching socket to port 8080.
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        handle_error("setsockopt failed", server_fd, -1);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding the socket to the network address and port.
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        handle_error("bind failed", server_fd, -1);

    // Beginning listening for incoming connections.
    if (listen(server_fd, 3) < 0) handle_error("listen failed", server_fd, -1);
    printf("Server is listening on port %d\n", PORT);

    // Accepting incoming connections.
    int new_socket; // client_fd
    int addrlen = sizeof(address);
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
        (socklen_t *)&addrlen)) < 0) handle_error("accept failed", server_fd, -1);
    printf("Connection accepted\n");

    // Reading data from the client.
    char buffer[BUFFER_SIZE] = {0};
    int valread = read(new_socket, buffer, BUFFER_SIZE);
    if (valread < 0) handle_error("read failed", server_fd, new_socket);
    buffer[valread] = '\0';
    printf("Received: %s\n", buffer);    

    // Sending a response to the client.
    if (send(new_socket, buffer, strlen(buffer), 0) < 0)
        handle_error("send failed", server_fd, new_socket);
    printf("Message echoed to the sender\n");

    // Closing the socket.
    close(new_socket);
    close(server_fd);

    return 0;
}