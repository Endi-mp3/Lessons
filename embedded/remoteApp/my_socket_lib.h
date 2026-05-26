#pragma once
#include <string.h>         // memcpy
#include <stdlib.h>         // malloc, free, atoi
#include <unistd.h>         // close (Linux), sleep
#include <sys/socket.h>     // socket, connect, send, recv
#include <netinet/in.h>     // struct sockaddr_in
#include <arpa/inet.h>      // htons, htonl, inet_pton
#include <ncurses.h>
#include "mylib_menu.h"



#include "my_socket_proto.h"
#define DEFAULT_MSG	"vnimatelno!!!"
#define DEFAULT_CONNECTION_CNT (10u)
#define DEFAULT_MAX_LEN (4096u)

//#define MEMDEBUG

void* my_malloc_fn(size_t size, const char* __attribute((unused)) what, const char*__attribute((unused)) where, int __attribute((unused)) line);
void my_free_fn(void* ptr, size_t __attribute((unused)) size, const char* __attribute((unused)) what, const char* __attribute((unused)) where, int __attribute((unused)) line);
int my_close_fn(int sock, const char* __attribute((unused)) what, const char* __attribute((unused)) where, int __attribute((unused)) line);

#define my_malloc(size, what) my_malloc_fn(size, what, __FUNCTION__, __LINE__)
#define my_free(ptr, size, what) my_free_fn(ptr, size, what, __FUNCTION__, __LINE__)
#define GOT_SOCKET(sock, what) printf("%s %i: got socket: %s fd %i\n", __FUNCTION__, __LINE__, what, sock)
#define my_close(sock, what) my_close_fn(sock, what, __FUNCTION__, __LINE__)


void my_sock_print_package(const struct Packet* pkt, const uint8_t* pkt_data);
int my_sock_init_client(const char* ip, int port);
bool my_sock_send(int sock, uint16_t id, enum MySockCommands cmd, size_t data_pkt_len, void* data);
struct Packet* my_sock_recv(int sock, uint32_t Max_len);
