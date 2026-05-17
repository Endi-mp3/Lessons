#include "my_socket_proto.h"
#include <string.h>         // memcpy
#include <stdlib.h>         // malloc, free, atoi
#include <unistd.h>         // close (Linux), sleep
#include <sys/socket.h>     // socket, connect, send, recv
#include <netinet/in.h>     // struct sockaddr_in
#include <arpa/inet.h>      // htons, htonl, inet_pton

#define DEFAULT_MSG	"vnimatelno!!!"
#define DEFAULT_CONNECTION_CNT (10u)
#define DEFAULT_MAX_LEN (4096u)

//#define MEMDEBUG

void* my_malloc_fn(size_t size, const char* __attribute((unused)) what, const char*__attribute((unused)) where, int __attribute((unused)) line)
{
	void* tmp_ptr = malloc(size);
#ifdef MEMDEBUG
	printf("%s %i: malloc called %s[%zu]. Result %p\n", where, line, what, size, tmp_ptr);
#endif
	return tmp_ptr;
}

#define my_malloc(size, what) my_malloc_fn(size, what, __FUNCTION__, __LINE__)

void my_free_fn(void* ptr, size_t __attribute((unused)) size, const char* __attribute((unused)) what, const char* __attribute((unused)) where, int __attribute((unused)) line)
{
	if (ptr == NULL)
		return;
#ifdef MEMDEBUG
	printf("%s %i: free called %s[%zu] for %p\n", where, line, what, size, ptr);
#endif
	free(ptr);
}

#define my_free(ptr, size, what) my_free_fn(ptr, size, what, __FUNCTION__, __LINE__)

#define GOT_SOCKET(sock, what) printf("%s %i: got socket: %s fd %i\n", __FUNCTION__, __LINE__, what, sock)

int my_close_fn(int sock, const char* __attribute((unused)) what, const char* __attribute((unused)) where, int __attribute((unused)) line)
{
#ifdef MEMDEBUG
	printf("%s %i: close called %s fd=%i\n", where, line, what, sock);
#endif
	return close(sock);
}

#define my_close(sock, what) my_close_fn(sock, what, __FUNCTION__, __LINE__)

int port = 44004;
const char server_ip[] = "127.0.0.1";
bool flag_is_server = false;
unsigned Max_len;

void print_package(const struct Packet* pkt, const uint8_t* pkt_data)
{
	printf("pkt->header.cmd = %x\n", pkt->header.cmd);
	printf("pkt->id = 0x%x\n", pkt->header.id);
	printf("pkt->len = %i\n", pkt->header.len);
	printf("pkt->data: ");
	for(unsigned i = 0; i < pkt->header.len; i++)
		printf("%02x ", pkt_data[i]);
	printf("\n");
}

bool my_sock_send(int sock, uint16_t id, enum MySockCommands cmd, size_t data_pkt_len, void* data)
{
    struct Packet *pkt = my_malloc(sizeof(struct Packet) + data_pkt_len, "struct Packet* pkt");
    pkt->header.cmd = cmd;
    pkt->header.id  = id;
    pkt->header.len = data_pkt_len;
    memcpy(pkt->data, data, data_pkt_len);
	int r = send(sock, pkt, sizeof(struct Packet) + pkt->header.len, 0);
	my_free(pkt, sizeof(struct Packet) + data_pkt_len, "struct Packet* pkt");
	return r != -1;
}

struct Packet* my_sock_recv(int sock)
{
	uint8_t *header_buf = (uint8_t*)malloc(sizeof(struct Header));
	ssize_t bytes_recv = -1;
	bytes_recv = recv(sock, header_buf, sizeof(struct Header), 0);
	if (bytes_recv == 0) {
		my_close(sock, "client socket");
		my_free(header_buf, 0, "buf");
		fprintf(stderr, "client socket recv errno: %i. Continue listen socket\n", errno);
		return NULL;
	} else if (bytes_recv == -1) {
		my_close(sock, "client socket");
		my_free(header_buf, 0, "buf");
		fprintf(stderr, "client socket failed errno: %i. Return with error\n", errno);
		return NULL;
	}

	struct Packet *pkt = (struct Packet *)header_buf;
	if(pkt->header.len > Max_len) {
		uint8_t errorByte = 0xFF;
		my_sock_send(sock, pkt->header.id, my_sock_cmd_err, 1, &errorByte);
		my_close(sock, "client socket");
		my_free(header_buf, 0, "buf");
		return NULL;
	}

	struct Packet *pktPtr = (struct Packet *)my_malloc(sizeof(struct Header) + pkt->header.len, "pktPtr");
	memcpy(pktPtr, header_buf, sizeof(struct Header));
	my_free(header_buf, sizeof(struct Header), "buf");

	if (pktPtr->header.len != 0) {
		bytes_recv = recv(sock, pktPtr->data, pktPtr->header.len, 0);
		if (bytes_recv == 0) {
			my_close(sock, "sock");
			my_free(pktPtr, pktPtr->header.len, "pktPtr");
			fprintf(stderr, "socket recv errno: %i. Continue listen socket\n", errno);
			return NULL;
		} else if (bytes_recv == -1) {
			my_free(pktPtr, pktPtr->header.len, "pktPtr");
			my_close(sock, "sock");
			fprintf(stderr, "socket failed errno: %i. Return with error\n", errno);
			return NULL;
		}
	}

//	print_package(pktPtr, pktPtr->data);
	return pktPtr;
}

int handle_server()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	GOT_SOCKET(sock, "server sock");
    if (sock < 0) {
        fprintf(stderr, "socket no open\n");
        return 1;
    }
    struct sockaddr_in serv_addr;

	uint32_t addrlen = sizeof(serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
	printf("Max len == %i\n", Max_len);
	if (bind(sock, (struct sockaddr*)&serv_addr, addrlen) < 0) {
        fprintf(stderr, "bind failed errno: %i\n", errno);
		my_close(sock, "server sock");
        return my_sock_err_error;
    }
	int connections_left = DEFAULT_CONNECTION_CNT;
	do {
		if (listen(sock, 1)) {
			fprintf(stderr, "listen failed errno: %i\n", errno);
			my_close(sock, "server sock");
			return my_sock_err_listen;
		}
		int client_socket = accept(sock, (struct sockaddr*)&serv_addr, (socklen_t*)&addrlen);
		GOT_SOCKET(client_socket, "client socket");
		if (client_socket < -1) {
			my_close(sock, "server sock");
			fprintf(stderr, "client socket failed errno: %i\n", errno);
			continue;
		}
		struct Packet *pkt = my_sock_recv(client_socket);
		if (!pkt) {
			my_close(client_socket, "clnt sock");
			fprintf(stderr, "client socket failed errno: %i\n", errno);
			continue;
		}
		my_sock_send(client_socket, connections_left, pkt->header.cmd, pkt->header.len, pkt->data);
		my_free(pkt, pkt->header.len , "pkt");
		my_close(client_socket, "client socket");

	} while (connections_left--);
	my_close(sock, "server sock");
	return 0;
}

int handle_clnt(const char *data_pkt)
{
	enum MySockRet res = my_sock_err_ok;
    struct sockaddr_in clnt_addr;

    clnt_addr.sin_family = AF_INET;
    clnt_addr.sin_port = htons(port);
    clnt_addr.sin_addr.s_addr = inet_addr(server_ip);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	GOT_SOCKET(sock, "sock");
	if (sock < 0)
		return -1;

    if (connect(sock, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr)) < 0) {
        fprintf(stderr, "connect failed, errno = %i\n", errno);
        my_close(sock, "client sock");
        return 1;
    }
	// send routine
	printf("Send to server...");
	my_sock_send(sock, 0xDEAD, my_sock_cmd_set, strlen(data_pkt), (void*)data_pkt);
	printf(".done\n");
	// recv routine
	printf("Recv from server...");
	struct Packet *pkt = my_sock_recv(sock);
	if (pkt == NULL) {
		my_close(sock, "sock");
		printf(".failed\n");
		return my_sock_err_recv;
	}

	printf(".done\n");

	switch (pkt->header.cmd) {
		case my_sock_cmd_err:
			fprintf(stderr, "Got failed response: %02x\n", pkt->data[0]);
			res = my_sock_err_error;
			break;
		case my_sock_cmd_get:
		case my_sock_cmd_set:
		case my_sock_cmd_info:
			print_package(pkt, pkt->data);
			break;
		default:
			// TODO print error
			break;
	}
	my_free(pkt, 0, "pkt");
	my_close(sock, "client sock");
	return res;
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
		Max_len = max_len;
		return handle_server();
	}
	else {
		printf("-------------------- Running clnt --------------------\n");
		char* msg = DEFAULT_MSG;
		Max_len = 2147483647;
		if (argc > 2)
			msg = argv[2];
		return handle_clnt(msg); // TODO параметром должно передаваться сообщение (данные в пакете)
	}
	return 0;
}




