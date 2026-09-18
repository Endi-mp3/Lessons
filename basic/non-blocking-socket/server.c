#include <stdio.h>
#include <stdint.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h> /* Added for the nonblocking socket */

enum AppState
{
	wait_packet = 0,
	got_part,
	got_full_packet,
};

#define PORT 9111
#define MAX_PENDING_CONNECTIONS 5
#define BUFFER_SIZE 1024
#define MAX_CLNT 5

struct clnt {
	int socket_fd;
	uint8_t buffer[BUFFER_SIZE];
	int recv_cnt;
	int expect_len;
	
	};
	

void anotherLogic(void)
{
	printf("\n...Doint something else...\n");
}

int main(int argc, char* argv[])
{
	struct clnt clients[MAX_CLNT];
	for(int i = 0, i < MAX_CLNT, i++) {
		clients.socket_fd = -1;
	}
	
	
	
	int packet_size = 16;
	int packet_count = 4;

	int server_fd, new_socket;

	struct sockaddr_in address;
    int addrlen = sizeof(address);
	// init socket
	if ( (server_fd = socket(AF_INET, SOCK_STREAM, 0) ) == 0) {
		perror("socket creation failed");
		return -1;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
		return -1;
    }

	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl failed");
		close(server_fd);
		return -1;
	}

	if (listen(server_fd, MAX_PENDING_CONNECTIONS) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

	printf("Server listening on port %d...\n", PORT);

	while(1) {
		while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0) {
            printf("New connection, socket fd is %d\n", new_socket);
			size_t recv_cnt = 0;
			while(recv_cnt != packet_size) {
				int n = recv(new_socket, buffer + recv_cnt, 1, 0); // packet_size
				if (n == 0) {
					// nothign to recieve
				} else if (n > 0) {
					recv_cnt += n;
					printf("Got N=%d bytes, package count = %d\n", (unsigned)recv_cnt, packet_count);
					anotherLogic(); // something else
				} else if (errno == EAGAIN) {
					continue;
				} else {
					perror("recv failed");
					close(new_socket);
					close(server_fd);
					return -1;
				}
			}
			printf("Recv Data: { ");
			for(int i = 0; i < packet_size; i++) {
				printf("%02x ", buffer[i]);
			}
			printf("}\n");

			packet_count--;
			if (packet_count == 0) {
				close(new_socket);
				close(server_fd);
				return 0;
			}
            close(new_socket); // Close the connection
        }
	}

	return 0;
}

