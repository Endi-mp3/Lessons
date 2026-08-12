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

struct MenuButtonSlot {
	MyLibMenu *ButtonSlot;
	int SlotName;
	int SlotBtnStart;
} menuBtnSlt = {NULL, -1, -1};

struct MenuTrigerSlot {
	MyLibMenu *TrigerSlot;
	int TrigerBtn;
	int TrigerBtnStart;
} menuTrigSlt = {NULL, -1, -1};

struct MenuTrigerSlotCheckBox {
	int TrigerChkBox1;
	int TrigerChkBox2;
	int TrigerChkBox3;
	int TrigerChkBox4;
} menuTrigerChBx = {-1, -1, -1, -1};


struct MenuRessetingDevice {
	MyLibMenu *RessetDevice;
	int FullReset;
	int CleanSlot;
} menuRessetDev = {NULL, -1, -1};

struct MenuSettingsPayload {
	MyLibMenu *SettingsPayload;
	int SettingCmd;
	int SettingData;
	int SettingSend;
} menuSettingsPayload = {NULL, -1, -1, -1};

int menuButtonSlotAdd, menuButtonSlotStart;

int cb_button_triger_slot_menu(void* __attribute((unused)) pvPtr)
{
	struct Packet *pkt = NULL;
	char* ip;
	mylib_menu_get_config(menu, menuNetCtx.Port, &port);
	mylib_menu_get_config(menu, menuNetCtx.IP, &ip);
    int sock = my_sock_init_client(ip, port);
    if (sock >= 0) {
//<<<<<<< Updated upstream
		my_sock_send(sock, 0x04, 0x02, 0, NULL);  		//my_sock_cmd_triger_menu
		pkt = my_sock_recv(sock, 4096);
		my_close(sock, "triger slot");
//=======
    my_sock_send(sock, 0x04, 0x02, 0, NULL);  		//my_sock_cmd_triger_menu
	pkt = my_sock_recv(sock, 4096);
	my_close(sock, "triger slot");
	
    menuTrigSlt.TrigerSlot = mylib_menu_create("TriggerSlot");
    
    menuTrigerChBx.TrigerChkBox1 = mylib_menu_create_checkbox(menuTrigSlt.TrigerSlot, "checkbox 1", true);
    menuTrigerChBx.TrigerChkBox2 = mylib_menu_create_checkbox(menuTrigSlt.TrigerSlot, "checkbox 2", true);
    menuTrigerChBx.TrigerChkBox3 = mylib_menu_create_checkbox(menuTrigSlt.TrigerSlot, "checkbox 3", true);
    menuTrigerChBx.TrigerChkBox4 = mylib_menu_create_checkbox(menuTrigSlt.TrigerSlot, "checkbox 4", true);
   
    
    menuTrigSlt.TrigerBtnStart = mylib_menu_create_button(menuTrigSlt.TrigerSlot, "Start", NULL);
    
    mylib_menu_create_exit_button(menuTrigSlt.TrigerSlot, "[ BACK TO MAIN MENU ]");
    
    mylib_menu_show(menuTrigSlt.TrigerSlot, -1);
    
    return 0;
//>>>>>>> Stashed changes
    }
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
	fprintf(stderr,"Send to server...");
	my_sock_send(sock, 0xDEAD, cmd, strlen(payload), (void*)payload);
	fprintf(stderr,".done\n");
	// recv routine
	fprintf(stderr,"Recv from server...");
	struct Packet *pkt = my_sock_recv(sock, 4096);
	if (pkt == NULL) {
		my_close(sock, "sock");
		fprintf(stderr,".failed\n");
		return my_sock_err_recv;
	}

	fprintf(stderr,".done\n");
	switch (pkt->header.cmd) {
		case my_sock_cmd_err:
			fprintf(stderr,stderr, "Got failed response: %02x\n", pkt->data[0]);
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

	menuBtnSlt.ButtonSlot = mylib_menu_create_submenu(menu, "Slot Setting");
	menuBtnSlt.SlotName = mylib_menu_create_string(menuBtnSlt.ButtonSlot, "name slot", "new name");
	menuBtnSlt.SlotBtnStart = mylib_menu_create_button(menuBtnSlt.ButtonSlot, "Start listen new IR signal", NULL);  
	
	menuTrigSlt.TrigerBtn = mylib_menu_create_button(menu, "Triger Slot", cb_button_triger_slot_menu);
	
	
	
	menuRessetDev.RessetDevice = mylib_menu_create_submenu(menu, "resseting the device");
	menuRessetDev.FullReset = mylib_menu_create_button(menuRessetDev.RessetDevice, "full reset", cb_device_full_reset);
	menuRessetDev.CleanSlot = mylib_menu_create_button(menuRessetDev.RessetDevice, "cleane slot", cb_device_clean_slot);
	

    int menuMonitoring= mylib_menu_create_button(menu, "Monitoring", cb_monitoring);

    MyLibMenu *menuConnectionSettings = mylib_menu_create_submenu(menu, "connection setting");
    menuNetCtx.IP = mylib_menu_create_string (menuConnectionSettings, "IP", "192.168.88.24");
	menuNetCtx.Port = mylib_menu_create_int_config(menuConnectionSettings, "Port", 3344);

	menuSettingsPayload.SettingsPayload = mylib_menu_create_submenu(menu, "Payload setting");
	menuSettingsPayload.SettingCmd = mylib_menu_create_int_config(menuSettingsPayload.SettingsPayload, "Command code:", 01);
	menuSettingsPayload.SettingData = mylib_menu_create_string(menuSettingsPayload.SettingsPayload, "Data:", "0102030405060");
	menuSettingsPayload.SettingSend = mylib_menu_create_button(menuSettingsPayload.SettingsPayload, "[ SEND PACKET ]", NULL);

	
	int menuButtonQuit = mylib_menu_create_exit_button(menu, "Quit");

	mylib_menu_set_item_priority(menu, menuButtonQuit, 7);
	mylib_menu_set_item_priority(menu, menuMonitoring, 4);
	mylib_menu_set_menu_priority(menuNetCtx.Network, 0);
	mylib_menu_set_menu_priority(menuBtnSlt.ButtonSlot, 1);
//	mylib_menu_set_menu_priority(menuTrigSlt.TrigerBtn, 2);
	mylib_menu_set_menu_priority(menuRessetDev.RessetDevice, 3);
	mylib_menu_set_menu_priority(menuConnectionSettings, 5);
	mylib_menu_set_menu_priority(menuSettingsPayload.SettingsPayload, 6);

    // MyLibMenu *current_menu = menu;

	MyLibMenuReturnCode_t showResult = mylib_menu_show(menu, -1);
	switch(showResult) {
		case MYLIB_MENU_RET_BTN_QUIT:
			endwin();
			return 0;
		case MYLIB_MENU_RET_ERROR:
			fprintf(stderr,"%s %i: TODO ERROR\n", __FUNCTION__, __LINE__);
			endwin();
			return 0;
	}

	endwin();
	fprintf(stderr,"Menu finished\n");
	char* ip;

	fprintf(stderr,"showResult = %i\n", showResult);
	if (showResult == menuTrigSlt.TrigerBtn) {
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


		showResult = mylib_menu_show(menuTrigSlt.TrigerSlot, -1);
		endwin();
		fprintf(stderr,"Menu finished\n");
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
	if (showResult == menuSettingsPayload.SettingSend) {
		uint8_t *payload;
		int cmd;
		char *payloadFilePath;
		mylib_menu_get_config(menu, menuSettingsPayload.SettingCmd, &cmd);
		mylib_menu_get_config(menu, menuSettingsPayload.SettingData, &payload);
		handle_clnt(ip, cmd, payload);
		free(payload);

	} else {
		// pizdec
		return -1;
	}
	free(ip);
	return 0;
}




