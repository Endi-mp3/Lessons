#include <stdio.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 9111
#define SERVER_ADDRESS "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {
		0xDE, 0xAD, 0xBE, 0xEF,
		0xAC, 0xAB, 0x14, 0x88,
		0xDE, 0xAD, 0xBE, 0xEF,
		0xAC, 0xAB, 0x14, 0x88,
	};

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address parameters
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr) <= 0) {
        perror("invalid address / address not supported");
        exit(EXIT_FAILURE);
    }

    // Set the socket to non-blocking mode
    if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl failed");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        // Non-blocking connect will return immediately
        // Check errno to distinguish between connection in progress and connection failed
        if (errno != EINPROGRESS) {
            perror("connection failed");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for connection to complete
    sleep(1);

    send(sock, buffer, 4, 0);
    printf("Message sent to server 4 bytes.\n");
	usleep(500000);
    send(sock, buffer+4, 4, 0);
    printf("Message sent to server 4 bytes.\n");
	usleep(500000);
    send(sock, buffer+8, 4, 0);
    printf("Message sent to server 4 bytes.\n");
	usleep(500000);
    send(sock, buffer+12, 4, 0);
    printf("Message sent to server 4 bytes.\n");
	usleep(500000);

    // Close socket
    close(sock);

    return 0;
}
