#include "my_socket_lib.h"
int port = 44004;
const char server_ip[] = "127.0.0.1";
bool flag_is_server = false;
int Max_len = 21000000;

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
		struct Packet *pkt = my_sock_recv(client_socket, Max_len);
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
	struct Packet *pkt = my_sock_recv(sock, Max_len);
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
			my_sock_print_package(pkt, pkt->data);
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


	// init menu
	// start
	// selection client / server
	// check params client / server
	// run client / server
	

    initscr(); // эти функции нужны для нормальной рисовки
    cbreak(); // настройки терминала (брейк лайн)
    noecho(); // отключаем эхо
    keypad(stdscr, TRUE); // не помню))
    curs_set(0); // устанавливаем курсор в угол экрана
    start_color(); // инициализируются цвета терминала

    MyLibMenu *menu = mylib_menu_create("Socket Settings"); 								// вложенные списки создаем, по факту меню
    MyLibMenu *menuSettings = mylib_menu_create_submenu(menu, "Settings"); 		// видишь первый параметр это тоже меню, по факту подменю это тоже самое меню, просто с родителем
	mylib_menu_create_checkbox(menuSettings, "Are you idiot sandwich?", true); 	// есть разные "формы" меню - чек бокс
	mylib_menu_create_int_config(menuSettings, "Price of your ass? $", 5); 		// цифровое значение

    mylib_menu_create_start_button(menu, "Start"); // кнопки
    mylib_menu_create_exit_button(menu, "Quit");
    MyLibMenu *current_menu = menu;
	if (mylib_menu_show(menu, -1) < 0) {
		endwin();
		return 0;
	}




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




