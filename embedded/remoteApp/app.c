#include "my_socket_lib.h"
#include "my_socket_proto.h"
#include "mylib_menu.h"
#include "mylib_splitview.h"
#include "app_types.h"
#include "mylib_console.h"

static MyLibMenu *menu;
static int port = 44004;

struct AppScreens
{
	int appscr_menu;
	int appscr_cli;
};

static struct AppScreens appScreens = { -1, -1 };
static struct MenuTrigerSlotCheckBox menuTrigerChBx = {-1, {-1} };
static struct MenuTrigerSlot menuTrigSlt = {NULL, -1, -1};
static struct MenuNetwork menuNetCtx = { -1, -1, NULL, NULL, NULL, -1, -1 };
static struct MenuButtonSlot menuBtnSlt = {NULL, -1, -1};
static struct MenuResetingDevice menuResetDev = {NULL, -1, -1};
static struct MenuSettingsPayload menuSettingsPayload = {NULL, -1, -1, -1};
static int menuButtonSlotAdd;
static int menuButtonSlotStart;

static SlotInfo slots[MAX_SLOTS];
static int slots_count = 0;
static int selected_slot = -1;

///---------------- Definitions ----------------
int cb_device_clean_slot(void* __attribute((unused)) pvPtr);
int cb_device_full_reset(void* __attribute((unused)) pvPtr);
int cb_monitoring(void* __attribute((unused)) pvPtr);
int cb_slot_selected(void* __attribute((unused)) pvPtr);
int default_callback(void* __attribute((unused)) pvPtr);
int handle_clnt(const char* server_ip, int cmd, const char* payload);
void s_init_menu(void);
void s_init_splitView(void);
void s_show_trigger_slot_menu(const char* server_ip);
int s_fetch_slots_from_device(const char* server_ip, int server_port);
int s_send_slot_trigger(const char* server_ip, int server_port, int slot_id);

///---------------- Implementation ----------------

int main(int __attribute((unused)) argc, char* __attribute((unused)) argv[])
{
	// init menu
	// start
	// selection client / server
	// check params client / server
	// run client / server

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	start_color();

	s_init_splitView();
	mylib_cli_init(appScreens.appscr_cli);
	s_init_menu();

	MyLibMenu *current_menu = menu;
	MyLibMenuReturnCode_t showResult = MYLIB_MENU_RET_OK;
	WINDOW* menuWindow = mylib_menu_prepare_step(appScreens.appscr_menu, FALSE);
	MYLIB_CLI_PRINT("Application started\n");

	// Main menu loop
	while(showResult == MYLIB_MENU_RET_OK) {
		showResult = mylib_menu_step(menuWindow, &current_menu, appScreens.appscr_menu);
		mylib_cli_output_step(appScreens.appscr_cli);

		switch(showResult) {
			case MYLIB_MENU_RET_BTN_QUIT:
				endwin();
				mylib_sv_shutdown();
				return 0;

			case MYLIB_MENU_RET_ERROR:
				MYLIB_CLI_PRINT("ERROR in menu processing\n");
				endwin();
				mylib_sv_shutdown();
				return 0;

			default:
				// Check if it's a slot trigger
				if (showResult == menuTrigSlt.TriggerBtn) {
					char* ip;
					mylib_menu_get_config(menu, menuNetCtx.IP, &ip);
					mylib_menu_get_config(menu, menuNetCtx.Port, &port);

					MYLIB_CLI_PRINT("Fetching slots from device...\n");
					if (s_fetch_slots_from_device(ip, port) == 0) {
						s_show_trigger_slot_menu(ip);
					} else {
						MYLIB_CLI_PRINT("Failed to fetch slots\n");
					}
					free(ip);
					showResult = MYLIB_MENU_RET_OK;  // Return to main menu
				}
				// Check if it's payload send
				else if (showResult == menuSettingsPayload.SettingSend) {
					uint8_t *payload;
					int cmd;
					char* ip;

					mylib_menu_get_config(menu, menuSettingsPayload.SettingCmd, &cmd);
					mylib_menu_get_config(menu, menuSettingsPayload.SettingData, &payload);
					mylib_menu_get_config(menu, menuNetCtx.IP, &ip);

					handle_clnt(ip, cmd, (const char*)payload);
					free(payload);
					free(ip);
					showResult = MYLIB_MENU_RET_OK;
				}
				break;
		}

		usleep(16000);
		doupdate();
	}

	endwin();
	mylib_sv_shutdown();
	return 0;
}

///---------------- Callbacks & Internal functions ----------------

/**
 * Fetch slots list from device via socket
 * Returns 0 on success, -1 on failure
 */
int s_fetch_slots_from_device(const char* server_ip, int server_port)
{
	struct Packet *pkt = NULL;
	char* ip = (char*)server_ip;

	int sock = my_sock_init_client(ip, server_port);
	if (sock < 0) {
		MYLIB_CLI_PRINT("Failed to connect to device\n");
		return -1;
	}

	// Send request for slot list
	my_sock_send(sock, 0x04, my_sock_cmd_slot_get, 0, NULL);
	pkt = my_sock_recv(sock, 4096);
	my_close(sock, "fetch slots");

	if (!pkt) {
		MYLIB_CLI_PRINT("No response from device\n");
		return -1;
	}

	// Parse slot data from packet
	// Expected format: array of SlotInfo structures
	// SlotInfo = { int slot_id(4), char slot_name[32], char ir_data[4] }

	slots_count = 0;
	uint32_t data_len = pkt->header.len;
	uint32_t slot_size = sizeof(SlotInfo);

	// Calculate number of slots
	int num_slots = data_len / slot_size;
	if (num_slots > MAX_SLOTS) {
		num_slots = MAX_SLOTS;
	}

	// Copy SlotInfo structures directly
	SlotInfo *src_slots = (SlotInfo *)pkt->data;
	for (int i = 0; i < num_slots; i++) {
		memcpy(&slots[i], &src_slots[i], sizeof(SlotInfo));
		slots_count++;

		MYLIB_CLI_PRINT("Slot %d: %s\n", slots[i].slot_id, slots[i].slot_name);
	}

	my_free(pkt, 0, "pkt");
	MYLIB_CLI_PRINT("Fetched %d slots from device\n", slots_count);
	return 0;
}


/**
 * Send trigger command for selected slot
 */
int s_send_slot_trigger(const char* server_ip, int server_port, int slot_id)
{
	struct Packet *pkt = NULL;
	char* ip = (char*)server_ip;

	int sock = my_sock_init_client(ip, server_port);
	if (sock < 0) {
		MYLIB_CLI_PRINT("Failed to connect to device\n");
		return -1;
	}

	// Send trigger command: slot_id(1)
	uint8_t payload[1] = { (uint8_t)slot_id };
	my_sock_send(sock, 0x04, 0x03, 1, payload);  // 0x03 = trigger slot command

	pkt = my_sock_recv(sock, 4096);
	my_close(sock, "trigger slot");

	if (!pkt) {
		MYLIB_CLI_PRINT("Device did not respond to trigger\n");
		return -1;
	}

	MYLIB_CLI_PRINT("Slot %d triggered successfully\n", slot_id);
	my_free(pkt, 0, "pkt");
	return 0;
}

/**
 * Slot selection callback
 */
int cb_slot_selected(void* pvPtr)
{
	MyLibMenuItem* item = (MyLibMenuItem*)pvPtr;
	if (!item) return -1;

	// The item data is stored as intValue (slot index)
	int slot_idx = item->data.intValue;
	if (slot_idx < 0 || slot_idx >= slots_count) return -1;

	char* ip;
	mylib_menu_get_config(menu, menuNetCtx.IP, &ip);

	MYLIB_CLI_PRINT("Sending trigger for slot: %s\n", slots[slot_idx].slot_name);
	s_send_slot_trigger(ip, port, slots[slot_idx].slot_id);

	free(ip);
	return 0;
}

/**
 * Create and show the trigger slot menu dynamically
 */
void s_show_trigger_slot_menu(const char* server_ip)
{
	if (slots_count == 0) {
		MYLIB_CLI_PRINT("No slots available\n");
		return;
	}

	// Create submenu for slots
	MyLibMenu *slot_menu = mylib_menu_create("Select Slot to Trigger");

	// Add slot items as buttons
	for (int i = 0; i < slots_count; i++) {
		char slot_label[512];
		snprintf(slot_label, sizeof(slot_label), "[%d] %s %s",
				 slots[i].slot_id, slots[i].slot_name, slots[i].ir_data);

		int btn_id = mylib_menu_create_button(slot_menu, slot_label, cb_slot_selected);

		// Store slot index as intValue for retrieval in callback
		MyLibMenuItem* item = slot_menu->items;
		while (item && item->id != btn_id) item = item->next;
		if (item) item->data.intValue = i;
	}

	// Add back button
	mylib_menu_create_button(slot_menu, "Back", NULL);

	// Show the menu
	WINDOW* slot_window = mylib_menu_prepare_step(appScreens.appscr_menu, FALSE);
	int result = mylib_menu_show(slot_menu, appScreens.appscr_menu);

	mylib_menu_delete(slot_menu);
}

int cb_monitoring(void* __attribute((unused)) pvPtr)
{
	char* ip;
	mylib_menu_get_config(menu, menuNetCtx.Port, &port);
	mylib_menu_get_config(menu, menuNetCtx.IP, &ip);
	uint8_t buf[4] = {0x1, 0x10, 0x20, 0x30};
	int sock = my_sock_init_client(ip, port);
	my_sock_send(sock, 0x1, my_sock_cmd_watcher_settings, 4, (void*)buf);
	free(ip);
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
	free(ip);
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
	free(ip);
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

	MYLIB_CLI_PRINT("Send to server...");
	my_sock_send(sock, 0xDEAD, cmd, strlen(payload), (void*)payload);
	MYLIB_CLI_PRINT(".done\n");

	MYLIB_CLI_PRINT("Recv from server...");
	struct Packet *pkt = my_sock_recv(sock, 4096);
	if (pkt == NULL) {
		my_close(sock, "sock");
		MYLIB_CLI_PRINT(".failed\n");
		return my_sock_err_recv;
	}

	MYLIB_CLI_PRINT(".done\n");
	switch (pkt->header.cmd) {
		case my_sock_cmd_err:
			MYLIB_CLI_PRINT("Got failed response: %02x\n", pkt->data[0]);
			res = my_sock_err_error;
			break;
		default:
			my_sock_print_package(pkt, pkt->data);
			break;
	}
	my_free(pkt, 0, "pkt");
	my_close(sock, "client sock");
	return res;
}

void s_init_menu(void)
{
	menu = mylib_menu_create("Menu Setting");

	menuNetCtx.Network = mylib_menu_create_submenu(menu, "Setting WIFI/BLE");
	menuNetCtx.WIFI = mylib_menu_create_submenu(menuNetCtx.Network, "Setting WIFI");
	menuNetCtx.WIFIconnect = mylib_menu_create_submenu(menuNetCtx.WIFI, "WIFI");
	menuNetCtx.WifiName = mylib_menu_create_string(menuNetCtx.WIFIconnect, "name", "HUAWEI-D8Yk");
	menuNetCtx.WifiSid = mylib_menu_create_string(menuNetCtx.WIFIconnect, "pasword", "1111111");

	menuBtnSlt.ButtonSlot = mylib_menu_create_submenu(menu, "Slot Setting");
	menuBtnSlt.SlotName = mylib_menu_create_string(menuBtnSlt.ButtonSlot, "name slot", "new name");
	menuBtnSlt.SlotBtnStart = mylib_menu_create_button(menuBtnSlt.ButtonSlot, "Start listen new IR signal", NULL);

	menuTrigSlt.TriggerBtn = mylib_menu_create_button(menu, "Trigger Slot", NULL);

	menuResetDev.ResetDevice = mylib_menu_create_submenu(menu, "resseting the device");
	menuResetDev.FullReset = mylib_menu_create_button(menuResetDev.ResetDevice, "full reset", cb_device_full_reset);
	menuResetDev.CleanSlot = mylib_menu_create_button(menuResetDev.ResetDevice, "cleane slot", cb_device_clean_slot);

	int menuMonitoring = mylib_menu_create_button(menu, "Monitoring", cb_monitoring);

	MyLibMenu *menuConnectionSettings = mylib_menu_create_submenu(menu, "connection setting");
	menuNetCtx.IP = mylib_menu_create_string(menuConnectionSettings, "IP", "127.0.0.1");
	menuNetCtx.Port = mylib_menu_create_int_config(menuConnectionSettings, "Port", 9111);

	menuSettingsPayload.SettingsPayload = mylib_menu_create_submenu(menu, "Payload setting");
	menuSettingsPayload.SettingCmd = mylib_menu_create_int_config(menuSettingsPayload.SettingsPayload, "Command code:", 01);
	menuSettingsPayload.SettingData = mylib_menu_create_string(menuSettingsPayload.SettingsPayload, "Data:", "0102030405060");
	menuSettingsPayload.SettingSend = mylib_menu_create_button(menuSettingsPayload.SettingsPayload, "[ SEND PACKET ]", NULL);

	int menuButtonQuit = mylib_menu_create_exit_button(menu, "Quit");

	mylib_menu_set_item_priority(menu, menuButtonQuit, 7);
	mylib_menu_set_item_priority(menu, menuMonitoring, 4);
	mylib_menu_set_menu_priority(menuNetCtx.Network, 0);
	mylib_menu_set_menu_priority(menuBtnSlt.ButtonSlot, 1);
	mylib_menu_set_menu_priority(menuResetDev.ResetDevice, 3);
	mylib_menu_set_menu_priority(menuConnectionSettings, 5);
	mylib_menu_set_menu_priority(menuSettingsPayload.SettingsPayload, 6);
}

void s_init_splitView(void)
{
	if (mylib_sv_init() != 0) {
		perror("mylib_sv_init failed");
		return;
	}
	appScreens.appscr_menu = 0;
	appScreens.appscr_cli = mylib_sv_create_split(appScreens.appscr_menu, MYLIB_SV_DIR_HORIZONTAL);
}
