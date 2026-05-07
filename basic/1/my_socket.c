#include <stdio.h>          // printf
#include <errno.h>
#include <stdlib.h>         // malloc, free
#include <stdint.h>         // uint8_t, uint16_t
#include <string.h>         // memcpy
#include <unistd.h>         // close (Linux), sleep
#include <sys/socket.h>     // socket, connect, send, recv
#include <netinet/in.h>     // struct sockaddr_in
#include <arpa/inet.h>      // htons, htonl, inet_pton

int port = 44004;
const char server_ip[] = "127.0.0.1";

#pragma pack(push, 1)
struct Header
{
    uint8_t cmd;
    uint16_t id;
//	uint32_t pack_num; // 4
//	uint32_t pack_max_count; // 255
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Packet
{
    struct Header header;
    uint32_t len;
    uint8_t data[0];
};
#pragma pack(pop)


int handle_server()
{
	struct Packet *packet= malloc(sizeof(struct Packet) + 16);
	packet->header.cmd = 0xA;
	packet->header.id = 0xDEAD;
	packet->len = 16;
	for(int i = 0; i < packet->len; i++) packet->data[i] = i & 0xFF;

	int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "socket no open\n");
		free(packet);
        return 1;
    }
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

	int addrlen = sizeof(serv_addr);
	if (bind(sock, (struct sockaddr*)&serv_addr, addrlen) < 0) {
        fprintf(stderr, "bind failed errno: %i\n", errno);
		close(sock);
		free(packet);
        return 1;
    }
	int connections_left = 100;
	do {
		if (listen(sock, 1)) {
			fprintf(stderr, "listen failed errno: %i\n", errno);
			close(sock);
			free(packet);
			return 1;
		}
		int client_socket = accept(sock, (struct sockaddr*)&serv_addr, (socklen_t*)&addrlen);
		if (client_socket < -1) {
			close(sock);
			free(packet);
			return -1;
		}
		uint8_t buf[256] = { 0 };
		ssize_t bytes_recv = -1;
		bytes_recv = recv(client_socket, buf, 255, 0);
		if (bytes_recv == 0) {
			close(client_socket);
			return -1;
		} else if (bytes_recv == -1) {
			close(client_socket);
			return -2;
		}
		printf("Full payload: ");
		for(int i = 0; i < bytes_recv; i++)
			printf("%02x ", buf[i]);
		printf("\n");

		struct Packet *pkt = (struct Packet *)buf;
		printf("pkt->header.cmd = %x\n", pkt->header.cmd);
		printf("pkt->id = 0x%x\n", pkt->header.id);
		printf("pkt->len = %i\n", pkt->len);
		printf("pkt->data: ");
		for(int i = 0; i < pkt->len; i++)
			printf("%02x ", pkt->data[i]);
		printf("\n");

		packet->header.id = connections_left;
		packet->data[0] = 0xBA;
		send(client_socket, packet, sizeof(struct Packet) + packet->len, 0);
		close(client_socket);
		printf("paket otpravil");

	} while (connections_left--);
	close(sock);
	free(packet);
	return 0;
}

int handle_clnt()
{
    struct sockaddr_in clnt_addr;

    clnt_addr.sin_family = AF_INET;
    clnt_addr.sin_port = htons(port);
    clnt_addr.sin_addr.s_addr = inet_addr(server_ip);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return -1;

    int addrlen = sizeof(clnt_addr);
    if (connect(sock, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr)) < 0) {
        fprintf(stderr, "connect failed, errno = %i\n", errno);
        close(sock);
        return 1;
    }
    int connections_left = 100;
    struct Packet *pkt = malloc(sizeof(struct Packet) + 16);
    pkt->header.cmd = 0xA;
    pkt->header.id  = 0xDEAD;
    pkt->len        = 16;
    for(int i = 0; i < 16; i++)
		pkt->data[i] = i & 0xFF;

	pkt->header.id = connections_left;
	pkt->data[0] = 0xBA;
	send(sock, pkt, sizeof(struct Packet) + pkt->len, 0);
	printf("paket otpravil \n");

	uint8_t buf[256] = { 0 };
	ssize_t bytes_recv = -1;
	bytes_recv = recv(sock, buf, 255, 0);
	if (bytes_recv == 0) {
		close(sock);
		return -1;
	} else if (bytes_recv == -1) {
		close(sock);
		return -2;
	}
	printf("Full payload: ");
	for(int i = 0; i < bytes_recv; i++)
		printf("%02x ", buf[i]);
	printf("\n");

	struct Packet *pkt = (struct Packet *)buf;
	printf("pkt->header.cmd = %x\n", pkt->header.cmd);
	printf("pkt->id = 0x%x\n", pkt->header.id);
	printf("pkt->len = %i\n", pkt->len);
	printf("pkt->data: ");
	for(int i = 0; i < pkt->len; i++)
		printf("%02x ", pkt->data[i]);
	printf("\n");
	return 0;
}

int main(int argc, char* argv[])
{
	int is_server = 0;
    if (argc > 1) {
		is_server = 1;
	}
	if (is_server) {
		printf("-------------------- Running server --------------------\n");
		return handle_server();
	}
	else {
		printf("-------------------- Running clnt --------------------\n");
		return handle_clnt();
	}
}




