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
#include <signal.h>
#include "my_logic.h"
///----------------------------------- Types  -----------------------------------
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

struct ClntEntity
{
	int clnt_sock_fd;
	int clnt_rcv_cnt;
	int clnt_expected_len;
	int clnt_sent_back;
	uint8_t clnt_buffer[BUFFER_SIZE];
};

///----------------------------------- Variables -----------------------------------
volatile sig_atomic_t keep_running = 1;
struct sockaddr_in address;
int addrlen = sizeof(address);
///----------------------------------- Helpers -----------------------------------
void handle_sigint(int sig)
{
	printf("Got interrupt signal. Stop working\n");
    keep_running = 0;
}

void printBuffer(const uint8_t* buffer, uint16_t len)
{
	for(int i = 0; i < len; i++) {
		printf("%02x ", (uint8_t)buffer[i]);
	}
}

void disconnectClient(struct ClntEntity* clnt)
{
	if (clnt->clnt_sock_fd != -1)
		close(clnt->clnt_sock_fd);
	clnt->clnt_sock_fd = -1;
	clnt->clnt_rcv_cnt = 0;
	clnt->clnt_expected_len = 0;
	clnt->clnt_sent_back = 0;
}

int init_signal()
{
	struct sigaction sa;
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Failed to set up signal handler");
        return -1;
	}
	return 0;
}

int init_server()
{
	int server_fd;

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
	return server_fd;
}

int call_testing_function(struct ClntEntity* ent)
{
	struct Packet* pkt = (struct Packet*)ent->clnt_buffer;
	int r = handle_package(pkt);
	if (r == -1)
		return -1;
	ent->clnt_sent_back = 0;
	ent->clnt_expected_len = pkt->header.len + sizeof(struct Header);
	return 0;
}

///----------------------------------- Functions -----------------------------------
void anotherLogic(void)
{
//	printf("\n...Doint something else...\n");
}

int main(int argc, char* argv[])
{
	if (init_signal()) {
		return -1;
	}

	struct ClntEntity clnt[MAX_CLNT];
	for(int i = 0; i < MAX_CLNT; i++) {
		clnt[i].clnt_sock_fd = -1;
		disconnectClient(&clnt[i]);
	}

	int server_fd = init_server();
	if (server_fd < 0) {
		return -1;
	}

	if (listen(server_fd, MAX_PENDING_CONNECTIONS) < 0) {
        perror("listen failed");
        close(server_fd);
		return -1;
    }

	printf("Server listening on port %d...\n", PORT);

	while(keep_running) {
		int new_socket;
		new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
		if (new_socket != -1) {
			for (int i = 0; i < MAX_CLNT; i++) {
				if (clnt[i].clnt_sock_fd == -1) {
					printf("New connection, socket fd is %d\n", new_socket);
					disconnectClient(&clnt[i]);
					clnt[i].clnt_sock_fd = new_socket;
					break;
				}
			}
		}

		for (int clnt_idx = 0; clnt_idx < MAX_CLNT; clnt_idx++) {
			if (clnt[clnt_idx].clnt_sock_fd == -1) {
				continue;
			}

			int n = recv(clnt[clnt_idx].clnt_sock_fd,
					clnt[clnt_idx].clnt_buffer + clnt[clnt_idx].clnt_rcv_cnt,
					1, 0); // packet_size

			if (n == 0) {
				// nothing to recieve
			} else if (n > 0) {
				clnt[clnt_idx].clnt_rcv_cnt += n;
				if (clnt[clnt_idx].clnt_rcv_cnt == sizeof(struct Header))
					clnt[clnt_idx].clnt_expected_len = PACKET_PTR_FULL_SIZE((struct Packet*)clnt[clnt_idx].clnt_buffer);

				if (clnt[clnt_idx].clnt_rcv_cnt == clnt[clnt_idx].clnt_expected_len) {
					printf("Got full package from client %d: {", (unsigned)clnt[clnt_idx].clnt_buffer[4]);
					printBuffer(clnt[clnt_idx].clnt_buffer, clnt[clnt_idx].clnt_rcv_cnt);

					call_testing_function(&clnt[clnt_idx]);
					printf("}\nSend buffer back: {");
					printBuffer(clnt[clnt_idx].clnt_buffer, clnt[clnt_idx].clnt_expected_len);
					while(clnt[clnt_idx].clnt_sent_back != clnt[clnt_idx].clnt_expected_len) {
						int r = send(clnt[clnt_idx].clnt_sock_fd,
								clnt[clnt_idx].clnt_buffer + clnt[clnt_idx].clnt_sent_back,
								clnt[clnt_idx].clnt_expected_len - clnt[clnt_idx].clnt_sent_back, 0);
						if (r < 0) {
							// handle error
							perror("Error sent back");
							break;
						}
						clnt[clnt_idx].clnt_sent_back += r;
					}
					disconnectClient(&clnt[clnt_idx]);
					printf("}\nsent it back and close it\n");
				}

				anotherLogic(); // something else
			} else if (errno == EAGAIN) {
				continue;
			} else {
				perror("recv failed");
				disconnectClient(&clnt[clnt_idx]);
				continue;
			}
        }
	}

	for(int i = 0; i < MAX_CLNT; i++) {
		disconnectClient(&clnt[i]);
	}

	close(server_fd);
	return 0;
}

