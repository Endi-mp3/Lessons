#include <stdio.h>          // printf
#include <errno.h>
#include <stdlib.h>         // malloc, free, atoi
#include <stdint.h>         // uint8_t, uint16_t
#include <string.h>         // memcpy
#include <unistd.h>         // close (Linux), sleep
#include <sys/socket.h>     // socket, connect, send, recv
#include <netinet/in.h>     // struct sockaddr_in
#include <arpa/inet.h>      // htons, htonl, inet_pton
#include <stdbool.h>        // bool

#define DEFAULT_MSG	"vnimatelno!!!"
#define DEFAULT_CONNECTION_CNT (10u)
#define DEFAULT_MAX_LEN (4096u)

void* my_malloc_fn(size_t size, const char* what, const char* where, int line)
{
	void* tmp_ptr = malloc(size);
	printf("%s %i: malloc called %s[%zu]. Result %p\n", where, line, what, size, tmp_ptr);
	return tmp_ptr;
}

#define my_malloc(size, what) my_malloc_fn(size, what, __FUNCTION__, __LINE__)

void my_free_fn(void* ptr, size_t size, const char* what, const char* where, int line)
{
	printf("%s %i: free called %s[%zu] for %p\n", where, line, what, size, ptr);
	free(ptr);
}

#define my_free(ptr, size, what) my_free_fn(ptr, size, what, __FUNCTION__, __LINE__)

#define GOT_SOCKET(sock, what) printf("%s %i: got socket: %s fd %i\n", __FUNCTION__, __LINE__, what, sock)

int my_close_fn(int sock, const char* what, const char* where, int line)
{
	printf("%s %i: close called %s fd=%i\n", where, line, what, sock);
	return close(sock);
}

#define my_close(sock, what) my_close_fn(sock, what, __FUNCTION__, __LINE__)

int port = 44004;
const char server_ip[] = "127.0.0.1";
bool flag_is_server = false;

#pragma pack(push, 1)
struct Header
{
    uint8_t cmd;
    uint16_t id;
    uint32_t len;
//	uint32_t pack_num; // 4
//	uint32_t pack_max_count; // 255
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Packet
{
    struct Header header;
    uint8_t data[0];
};
#pragma pack(pop)

void print_package(const struct Packet* pkt, const uint8_t* pkt_data)
{
	printf("pkt->header.cmd = %x\n", pkt->header.cmd);
	printf("pkt->id = 0x%x\n", pkt->header.id);
	printf("pkt->len = %i\n", pkt->header.len);
	printf("pkt->data: ");
	for(int i = 0; i < pkt->header.len; i++)
		printf("%02x ", pkt_data[i]);
	printf("\n");
}

int handle_server(int Max_len)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	GOT_SOCKET(sock, "server sock");
    if (sock < 0) {
        fprintf(stderr, "socket no open\n");
        return 1;
    }
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

	int addrlen = sizeof(serv_addr);
	if (bind(sock, (struct sockaddr*)&serv_addr, addrlen) < 0) {
        fprintf(stderr, "bind failed errno: %i\n", errno);
		my_close(sock, "server sock");
        return 1;
    }
	int connections_left = DEFAULT_CONNECTION_CNT;
	do {
		if (listen(sock, 1)) {
			fprintf(stderr, "listen failed errno: %i\n", errno);
			my_close(sock, "server sock");
			return 1;
		}
		int client_socket = accept(sock, (struct sockaddr*)&serv_addr, (socklen_t*)&addrlen);
		GOT_SOCKET(client_socket, "client socket");
		if (client_socket < -1) {
			my_close(sock, "server sock");
			continue;
		}
		uint8_t buf[sizeof(struct Header)] = { 0 };
		ssize_t bytes_recv = -1;
		bytes_recv = recv(client_socket, buf, sizeof(struct Header), 0);
		if (bytes_recv == 0) {
			my_close(client_socket, "client socket");
			return -1;
		} else if (bytes_recv == -1) {
			my_close(client_socket, "client socket");
			return -2;
		}

		struct Packet *pkt = (struct Packet *)buf;
		if(pkt->header.len > Max_len) {
			send(client_socket, "1111111111111", 18, 0); 
			my_close(client_socket, "client socket");
			return -1;
		}

		uint8_t *data_buf = (uint8_t *)my_malloc(pkt->header.len, "data_buf");
		bytes_recv = recv(client_socket, data_buf, pkt->header.len, 0);
		if (bytes_recv == 0) {
			my_close(client_socket, "client socket");
			my_free(data_buf, pkt->header.len, "data_buf");
			return -1;
		} else if (bytes_recv == -1) {
			my_free(data_buf, pkt->header.len, "data_buf");
			my_close(client_socket, "client socket");
			return -2;
		}
		print_package(pkt, data_buf);
		struct Packet *p;
		p = (struct Packet*) my_malloc(sizeof(struct Header) + pkt->header.len, "client packet");

		p->header.id = connections_left;
		p->header.len = pkt->header.len;
		p->header.cmd = pkt->header.cmd;
		memcpy(p->data,	data_buf, pkt->header.len);
		send(client_socket, p, sizeof(struct Packet) + p->header.len, 0);
		my_close(client_socket, "client socket");
		printf("paket otpravil \n");
		my_free(p, sizeof(struct Header) + pkt->header.len, "client send packet");
		my_free(data_buf, pkt->header.len , "data_buf");
	
	} while (connections_left--);
	my_close(sock, "server sock");
	return 0;
}

int handle_clnt(const char *data_pkt)
{
    struct sockaddr_in clnt_addr;

    clnt_addr.sin_family = AF_INET;
    clnt_addr.sin_port = htons(port);
    clnt_addr.sin_addr.s_addr = inet_addr(server_ip);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	GOT_SOCKET(sock, "sock");
	if (sock < 0)
		return -1;

    int addrlen = sizeof(clnt_addr);
    if (connect(sock, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr)) < 0) {
        fprintf(stderr, "connect failed, errno = %i\n", errno);
        my_close(sock, "client sock");
        return 1;
    }
    int connections_left = DEFAULT_CONNECTION_CNT;
    ssize_t data_pkt_len = strlen(data_pkt);
    struct Packet *pkt = my_malloc(sizeof(struct Packet) + data_pkt_len, "struct Packet* pkt");
    pkt->header.cmd = 0xA;
    pkt->header.id  = 0xDEAD;
    pkt->header.len = data_pkt_len;
    memcpy(pkt->data, data_pkt, data_pkt_len);

	pkt->header.id = connections_left;
	pkt->data[0] = 0xBA;
	send(sock, pkt, sizeof(struct Packet) + pkt->header.len, 0);
	printf("paket otpravil \n");

	uint8_t buf[256] = { 0 };
	ssize_t bytes_recv = -1;
	bytes_recv = recv(sock, buf, 255, 0);
	if (bytes_recv == 0) {
		my_close(sock, "client sock");
		my_free(pkt, sizeof(struct Packet) + data_pkt_len, "struct Packet* pkt");
		return -1;
	} else if (bytes_recv == -1) {
		my_free(pkt, sizeof(struct Packet) + data_pkt_len, "struct Packet* pkt");
		my_close(sock, "client sock");
		return -2;
	}
	printf("Full payload: ");
	for(int i = 0; i < bytes_recv; i++)
		printf("%02x ", buf[i]);
	printf("\n");

	pkt = (struct Packet *)buf;
	print_package(pkt, pkt->data);
	my_close(sock, "client sock");
	my_free(pkt, sizeof(struct Packet) + data_pkt_len, "struct Packet* pkt");
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc > 1) {
		if (strcmp(argv[1], "server") == 0) {
			flag_is_server = true;
		} else if (strcmp(argv[1], "clnt") == 0) {
			flag_is_server = false;
		} else {
			fprintf(stderr, "Wrong args: %s\n", argv[1]);
			return -1;
		}
	}

	if (flag_is_server) {
		printf("-------------------- Running server --------------------\n");
		ssize_t max_len = DEFAULT_MAX_LEN;
		if (argc > 2)
			max_len = atoi (argv[2]);
		if (max_len <= 0)
			max_len = DEFAULT_MAX_LEN;

		return handle_server(max_len);
	}
	else {
		printf("-------------------- Running clnt --------------------\n");
		char* msg = DEFAULT_MSG;
		if (argc > 2)
			msg = argv[2];
		return handle_clnt(argv[2]); // TODO параметром должно передаваться сообщение (данные в пакете)
	}
	return 0;
}




