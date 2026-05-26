#include "my_socket_lib.h"


void* my_malloc_fn(size_t size, const char* __attribute((unused)) what, const char*__attribute((unused)) where, int __attribute((unused)) line)
{
	void* tmp_ptr = malloc(size);
#ifdef MEMDEBUG
	printf("%s %i: malloc called %s[%zu]. Result %p\n", where, line, what, size, tmp_ptr);
#endif
	return tmp_ptr;
}


void my_free_fn(void* ptr, size_t __attribute((unused)) size, const char* __attribute((unused)) what, const char* __attribute((unused)) where, int __attribute((unused)) line)
{
	if (ptr == NULL)
		return;
#ifdef MEMDEBUG
	printf("%s %i: free called %s[%zu] for %p\n", where, line, what, size, ptr);
#endif
	free(ptr);
}


int my_close_fn(int sock, const char* __attribute((unused)) what, const char* __attribute((unused)) where, int __attribute((unused)) line)
{
#ifdef MEMDEBUG
	printf("%s %i: close called %s fd=%i\n", where, line, what, sock);
#endif
	return close(sock);
}

void my_sock_print_package(const struct Packet* pkt, const uint8_t* pkt_data)
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

struct Packet* my_sock_recv(int sock, uint32_t Max_len)
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

//	my_sock_print_package(pktPtr, pktPtr->data);
	return pktPtr;
}


int my_sock_init_client(const char* ip, int port)
{
    struct sockaddr_in clnt_addr;

    clnt_addr.sin_family = AF_INET;
    clnt_addr.sin_port = htons(port);
    clnt_addr.sin_addr.s_addr = inet_addr(ip);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	GOT_SOCKET(sock, "sock");
	if (sock < 0)
		return -1;

    if (connect(sock, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr)) < 0) {
        fprintf(stderr, "connect failed, errno = %i\n", errno);
        my_close(sock, "client sock");
        return -1;
    }
	return sock;
}
