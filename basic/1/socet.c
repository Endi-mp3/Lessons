#include <stdio.h>          // printf
#include <stdlib.h>         // malloc, free
#include <stdint.h>         // uint8_t, uint16_t
#include <string.h>         // memcpy
#include <unistd.h>         // close (Linux)
#include <sys/socket.h>     // socket, connect, send, recv
#include <netinet/in.h>     // struct sockaddr_in
#include <arpa/inet.h>      // htons, htonl, inet_pton

struct hader
{
    uint8_t cmd;
};

struct packet
{
    struct hader header;
    uint16_t id;
    uint32_t len;
    uint8_t file_comand[];
};


int main()
{
    int server_fd, new_socket;


    uint8_t data[] ={1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    int data_len = 15;

 

    const char *server_ip = "127.0.0.1";
    int port = 8080; 



    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "socket no open\n");
        return 1;
    }
    server_fd = sock;
    

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr.s_addr) <= 0) {
        fprintf(stderr, "inet_pton не перевел ip в бинарный\n");
        close(sock);
        return 1;
    }
        int addrlen = sizeof(serv_addr);
        if (bind(server_fd, (struct sockaddr*)&serv_addr, addrlen) < 0) {
        fprintf(stderr, "bind failed\n");
        close(server_fd);
        return 1;
    }

    struct packet *pkt = malloc(sizeof(struct packet) + data_len);
    if (pkt == NULL){
        printf("память не выделилась\n");
        close(sock);
        return 1;
    }

    pkt->len = data_len;
    pkt->id = 5;
    pkt->header.cmd = 1;
    memcpy(pkt->file_comand, data, data_len);


    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "connect failed\n");
        close(sock);
        return 1;
    }


    int total_size = sizeof(pkt) + data_len;

    int sent = send(sock, pkt, total_size, 0);
    if (sent != total_size){
        printf("send не смог отправить пакет");
        close(sock);
        return 1;
    } else {
        printf("пакет отправлен");
    }
}



