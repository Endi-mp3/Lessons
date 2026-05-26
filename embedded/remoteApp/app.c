#include "my_socket_lib.h"

static MyLibMenu *menu;
static int port = 44004;

int handle_clnt(const char* server_ip, int cmd, const char* payload)
{
	enum MySockRet res = my_sock_err_ok;
	int sock = my_sock_init_client(server_ip, port);
	// send routine
	printf("Send to server...");
	my_sock_send(sock, 0xDEAD, cmd, strlen(payload), (void*)payload);
	printf(".done\n");
	// recv routine
	printf("Recv from server...");
	struct Packet *pkt = my_sock_recv(sock, 4096);
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
			break;
		default:
			my_sock_print_package(pkt, pkt->data);
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

    menu = mylib_menu_create("Start Menu"); 								// вложенные списки создаем, по факту меню
    MyLibMenu *menuClient = mylib_menu_create_submenu(menu, "Client");
	MyLibMenu *menuClientSettingsPayload = mylib_menu_create_submenu(menuClient, "Payload setting");
	MyLibMenu *menuConnectionSettings = mylib_menu_create_submenu(menu, "Connection Settings");

	int menuClientSettingsSend = mylib_menu_create_button(menuClient, "SEND", NULL);

	int menuPayloadSettingsCmd = mylib_menu_create_int_config(menuClientSettingsPayload, "Command code:", 01);
	int menuPayloadSettingsData = mylib_menu_create_string(menuClientSettingsPayload, "Data:", "0102030405060");

	int menuSettingsIP = mylib_menu_create_string (menuConnectionSettings, "IP", "192.168.88.24");
	int menuSettingsPort = mylib_menu_create_int_config(menuConnectionSettings, "Port", 3344);

	mylib_menu_create_exit_button(menu, "Quit");
    MyLibMenu *current_menu = menu;

	MyLibMenuReturnCode_t showResult = mylib_menu_show(menu, -1);
	switch(showResult) {
		case MYLIB_MENU_RET_BTN_QUIT:
			endwin();
			return 0;
		case MYLIB_MENU_RET_ERROR:
			printf("%s %i: TODO ERROR\n", __FUNCTION__, __LINE__);
			endwin();
			return 0;
	}

	endwin();
	printf("Start\n");
	char* ip;
	mylib_menu_get_config(menu, menuSettingsPort, &port);
	mylib_menu_get_config(menu, menuSettingsIP, &ip);
	if (showResult == menuClientSettingsSend) {
		uint8_t *payload;
		int cmd;
		char *payloadFilePath;
		mylib_menu_get_config(menu, menuPayloadSettingsCmd, &cmd);
		mylib_menu_get_config(menu, menuPayloadSettingsData, &payload);
		handle_clnt(ip, cmd, payload);
		free(payload);

	} else {
		// pizdec
		return -1;
	}
	free(ip);
	return 0;
}




