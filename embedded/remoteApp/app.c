#include "my_socket_lib.h"

int port = 44004;
bool flag_is_server = false;
int Max_len = 21000000;

int handle_clnt(const char *data_pkt, const char* server_ip)
{
	enum MySockRet res = my_sock_err_ok;
	int sock = my_sock_init_client(server_ip, port);
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

    MyLibMenu *menu = mylib_menu_create("Start Menu"); 								// вложенные списки создаем, по факту меню
    MyLibMenu *menuClient = mylib_menu_create_submenu(menu, "Client");
	MyLibMenu *menuServer = mylib_menu_create_submenu(menu, "Server");																									// видишь первый параметр это тоже меню, по факту подменю это тоже самое меню, просто с родителем
	MyLibMenu *menuClientSettingsPayload = mylib_menu_create_submenu(menuClient, "Payload setting");
	int menuClientSettingsSend = mylib_menu_create_button(menuClient, "SEND", NULL);
	int menuPaylordSettingsFile = mylib_menu_create_string(menuClientSettingsPayload, "Path to file with paylord", "");
	int menuPaylordSettingsCmd = mylib_menu_create_int_config(menuClientSettingsPayload, "Command code", 01);
	int menuPaylordSettingsData = mylib_menu_create_string(menuClientSettingsPayload, "Data srting", "xyi");
	MyLibMenu *ConectionSetings = mylib_menu_create_submenu(menu, "Connection Settings");
	int menuSettingsIP = mylib_menu_create_string (ConectionSetings, "IP", "192.168.88.24");
	int menuSettingsPort = mylib_menu_create_int_config(ConectionSetings, "Port", 3344);
    int menuServerMaxMsgLen = mylib_menu_create_int_config (menuServer, "Max Message length", 4096);
	int menuServerStartButton = mylib_menu_create_button (menuServer, "START", NULL);


	mylib_menu_create_exit_button(menu, "Quit");
    MyLibMenu *current_menu = menu;

	MyLibMenuReturnCode_t showResult = mylib_menu_show(menu, -1);
	switch(showResult) {
		case MYLIB_MENU_RET_OK:
			break;
		case MYLIB_MENU_RET_BTN_START:
			break;
		case MYLIB_MENU_RET_BTN_QUIT:
			endwin();
			return 0;
		case MYLIB_MENU_RET_ERROR:
			printf("%s %i: TODO ERROR\n", __FUNCTION__, __LINE__);
			endwin();
			return 0;
		default:
			break;
	}

	endwin();
	printf("Start\n");
	char* ip;
	mylib_menu_get_config(menu, menuSettingsPort, &port);
	mylib_menu_get_config(menu, menuSettingsIP, &ip);
	if (showResult == menuServerStartButton) {
		int maxLength;
		mylib_menu_get_config(menu, menuServerMaxMsgLen, &maxLength);
		//		handle_server(maxLength);
	} else if (showResult == menuClientSettingsSend) {
		char *payloadFilePath;
		uint8_t *payload;
		int cmd;
		mylib_menu_get_config(menu, menuPaylordSettingsFile, &payloadFilePath);
		if (strlen(payloadFilePath) > 2) {
			payload = malloc(10);
			for(int i = 0; i < 10; i++) payload[i] = i + 1;
			// TODO Read file and fill payload
			free(payloadFilePath);
		} else {
			mylib_menu_get_config(menu, menuPaylordSettingsCmd, &cmd);
			mylib_menu_get_config(menu, menuPaylordSettingsData, &payload);
		}
		handle_clnt(payload, ip);
		free(payload);
	} else {
		// pizdec
		return -1;
	}
	free(ip);
	return 0;
}




