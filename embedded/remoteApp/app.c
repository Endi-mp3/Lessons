#include "my_socket_lib.h"
#include "my_socket_proto.h"
#include "mylib_menu.h"

static MyLibMenu *menu;
static int port = 44004;

struct MenuNetwork {
	int Port;
	int IP;
	MyLibMenu *Network;
    MyLibMenu *WIFI;
    MyLibMenu *WIFIconnect;
    int WifiName;
    int WifiSid;
} menuNetCtx = { -1, -1, NULL, NULL, NULL, -1, -1 };
int menuButtonSlotAdd, menuButtonSlotStart;

int cb_button_triger_slot_menu(void* __attribute((unused)) pvPtr)
{
   return 0;
}


int cb_monitoring(void* __attribute((unused)) pvPtr)
{
	char* ip;
	mylib_menu_get_config(menu, menuNetCtx.Port, &port);
	mylib_menu_get_config(menu, menuNetCtx.IP, &ip);
	uint8_t buf[4] = {0x1, 0x10, 0x20, 0x30};
	int sock = my_sock_init_client(ip, port);
	my_sock_send(sock, 0x1, my_sock_cmd_watcher_settings, 4, (void*)buf);
	return 0;
}

int cb_device_full_reset(void* __attribute((unused)) pvPtr)
{
	char* ip;
	mylib_menu_get_config(menu, menuNetCtx.Port, &port);
	mylib_menu_get_config(menu, menuNetCtx.IP, &ip);
    int sock = my_sock_init_client(ip, port);
    if (sock >= 0) {
        my_sock_send(sock, 0x04, my_sock_cmd_full_reset, 0, NULL); 
        my_close(sock, "reset sock");
    }
    return 0;
}

int cb_device_clean_slot(void* __attribute((unused)) pvPtr)
{
	char* ip;
	mylib_menu_get_config(menu, menuNetCtx.Port, &port);
	mylib_menu_get_config(menu, menuNetCtx.IP, &ip);
    int sock = my_sock_init_client(ip, port);
    if (sock >= 0) {
        my_sock_send(sock, 0x04, my_sock_cmd_slot_clean, 0, NULL);
    }
    return 0;
}

int default_callback(void* __attribute((unused)) pvPtr)
{
	return 0;
}

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

int main(int __attribute((unused)) argc, char* __attribute((unused)) argv[])
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

    menu = mylib_menu_create("Menu Setting");

    menuNetCtx.Network = mylib_menu_create_submenu(menu, "Setting WIFI/BLE");
    menuNetCtx.WIFI = mylib_menu_create_submenu(menuNetCtx.Network, "Setting WIFI");
    menuNetCtx.WIFIconnect = mylib_menu_create_submenu(menuNetCtx.WIFI, "WIFI");
    menuNetCtx.WifiName = mylib_menu_create_string (menuNetCtx.WIFIconnect, "name", "HUAWEI-D8Yk");
    menuNetCtx.WifiSid = mylib_menu_create_string (menuNetCtx.WIFIconnect, "pasword", "1111111");

    MyLibMenu *menuButtonSlot = mylib_menu_create_submenu(menu, "Slot Setting");
	menuButtonSlotAdd = mylib_menu_create_string (menuButtonSlot, "name slot", "new name");
	menuButtonSlotStart = mylib_menu_create_button(menuButtonSlot, "Start listen new IR signal", NULL);

	int menuButtonTriggerSlot = mylib_menu_create_button(menu, "Triger Slot", NULL);

	MyLibMenu *menuTrigerSlot = mylib_menu_create("TriggerSlot");

    int menuTrigerSlotStart = mylib_menu_create_button(menuTrigerSlot, "Start", NULL);

    MyLibMenu *menuRessetingDevive = mylib_menu_create_submenu(menu, "resseting the device");
    int menuRessetingDeviveFull= mylib_menu_create_button(menuRessetingDevive, "full reset", cb_device_full_reset);
    int menuRessetingDeviveCleaneSlot= mylib_menu_create_button(menuRessetingDevive, "cleane slot", cb_device_clean_slot);

    int menuMonitoring= mylib_menu_create_button(menu, "Monitoring", cb_monitoring);

    MyLibMenu *menuConnectionSettings = mylib_menu_create_submenu(menu, "connection setting");
    menuNetCtx.IP = mylib_menu_create_string (menuConnectionSettings, "IP", "192.168.88.24");
	menuNetCtx.Port = mylib_menu_create_int_config(menuConnectionSettings, "Port", 3344);

	MyLibMenu *menuClientSettingsPayload = mylib_menu_create_submenu(menu, "Payload setting");
	int menuPayloadSettingsCmd = mylib_menu_create_int_config(menuClientSettingsPayload, "Command code:", 01);
	int menuPayloadSettingsData = mylib_menu_create_string(menuClientSettingsPayload, "Data:", "0102030405060");
	int menuClientSettingsSend = mylib_menu_create_button(menuClientSettingsPayload, "[ SEND PACKET ]", NULL);

	int menuButtonQuit = mylib_menu_create_exit_button(menu, "Quit");

	mylib_menu_set_item_priority(menu, menuButtonQuit, 7);
	mylib_menu_set_item_priority(menu, menuMonitoring, 4);
	mylib_menu_set_menu_priority(menuNetCtx.Network, 0);
	mylib_menu_set_menu_priority(menuButtonSlot, 1);
//	mylib_menu_set_menu_priority(menuButtonTriggerSlot, 2);
	mylib_menu_set_menu_priority(menuRessetingDevive, 3);
	mylib_menu_set_menu_priority(menuConnectionSettings, 5);
	mylib_menu_set_menu_priority(menuClientSettingsPayload, 6);

    // MyLibMenu *current_menu = menu;

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
	printf("Menu finished\n");
	char* ip;

	printf("showResult = %i\n", showResult);
	if (showResult == menuButtonTriggerSlot) {
		char* ip ;
		char* slot_name = NULL;

		mylib_menu_get_config(menu, menuNetCtx.IP, &ip);
		mylib_menu_get_config(menu, menuNetCtx.Port, &port);
		mylib_menu_get_config(menu, menuButtonSlotAdd, &slot_name);

		/*
		int sock = my_sock_init_client(ip, port);
		if (sock >= 0) {
			my_sock_send(sock, 0x02, my_sock_cmd_update_slot, strlen(slot_name) + 1, (void*)slot_name);
		}

		struct Packet* pkt = my_sock_recv(sock, 4096);
		if (pkt == NULL){
			my_close(sock, "slot error");
		}
		*/


		showResult = mylib_menu_show(menuTrigerSlot, -1);
		endwin();
		printf("Menu finished\n");
		/*
		if (showResult == start) {
			// send slot triggers
			// show result (optional)
			// return back to same menu menuTriggerSlot
		}
		else (showResult == back) {
			// close menuTriggerSlot
			// clean slots info
			// return back to previous menu
		}
		*/
		return 0;
	}

	mylib_menu_get_config(menu, menuNetCtx.Port, &port);
	mylib_menu_get_config(menu, menuNetCtx.IP, &ip);
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




