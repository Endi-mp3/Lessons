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

    menu = mylib_menu_create("Menu Setting");
    MyLibMenu *menuSettingNetwork = mylib_menu_create_submenu(menu, "Setting WIFI/BLE");
    MyLibMenu *menuSettingWIFI = mylib_menu_create_submenu(menuSettingNetwork, "Setting WIFI");
    MyLibMenu *menuSettingWIFIconnect = mylib_menu_create_submenu(menuSettingWIFI, "WIFI");
    int menuSettingsWifiName = mylib_menu_create_string (menuSettingWIFIconnect, "name", "HUAWEI-D8Yk");
    int menuSettingsWifiSid = mylib_menu_create_string (menuSettingWIFIconnect, "pasword", "1111111");
    
    MyLibMenu *menuSettingBLE = mylib_menu_create_submenu(menuSettingNetwork, "Setting BLE");
    MyLibMenu *menuSettingBLEconnect = mylib_menu_create_submenu(menuSettingBLE, "BLE");

    MyLibMenu *menuButtonSlot = mylib_menu_create_submenu(menu, "Slot Setting");
	int menuButtonSlotAdd = mylib_menu_create_string (menuButtonSlot, "name slot", "new name");
	int menuButtonSlotStart = mylib_menu_create_button(menuButtonSlot, "Start", NULL);

	MyLibMenu *menuTrigerSlot = mylib_menu_create_submenu(menu, "Triger Slot");
	int menuTrigerSlotCheckBox1 = mylib_menu_create_checkbox(menuTrigerSlot, "chek box 1", true);
    int menuTrigerSlotCheckBox2 = mylib_menu_create_checkbox(menuTrigerSlot, "chek box 2", true);
    int menuTrigerSlotCheckBox3 = mylib_menu_create_checkbox(menuTrigerSlot, "chek box 3", true);
    int menuTrigerSlotCheckBox4 = mylib_menu_create_checkbox(menuTrigerSlot, "chek box 4", true);
    int menuTrigerSlotStart = mylib_menu_create_button(menuTrigerSlot, "Start", NULL);
    
    MyLibMenu *menuRessetingDevive = mylib_menu_create_submenu(menu, "resseting the device");
    int menuRessetingDeviveFull= mylib_menu_create_button(menuRessetingDevive, "full reset", NULL);
    int menuRessetingDeviveCleaneSlot= mylib_menu_create_button(menuRessetingDevive, "cleane slot", NULL);
    
    int menuMonitoring= mylib_menu_create_button(menu, "monitoring", NULL);
    
    MyLibMenu *menuConnectionSettings = mylib_menu_create_submenu(menu, "connection setting");
    int menuSettingsIP = mylib_menu_create_string (menuConnectionSettings, "IP", "192.168.88.24");
	int menuSettingsPort = mylib_menu_create_int_config(menuConnectionSettings, "Port", 3344);
    
    
    
    
    
    
	MyLibMenu *menuClientSettingsPayload = mylib_menu_create_submenu(menu, "Payload setting");
	int menuPayloadSettingsCmd = mylib_menu_create_int_config(menuClientSettingsPayload, "Command code:", 01);
	int menuPayloadSettingsData = mylib_menu_create_string(menuClientSettingsPayload, "Data:", "0102030405060");
	int menuClientSettingsSend = mylib_menu_create_button(menuClientSettingsPayload, "[ SEND PACKET ]", NULL);

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




