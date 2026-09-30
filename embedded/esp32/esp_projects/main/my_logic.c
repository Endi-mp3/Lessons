#include "my_logic.h"
#ifdef TESTING
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# define PRINTF(...) printf(__VA_ARGS__)

static SlotInfo slots[MAX_SLOTS] = {
	{0x1, "Power On",    {0x01, 0x02, 0xDE, 0xAD} },
	{0x2, "Volume Up",   {0x10, 0x20, 0xBE, 0xAF} },
	{0x3, "Volume Down", {0x45, 0x22, 0xDE, 0xAD} },
	{0x4, "Ch +",        {0x11, 0x34, 0xAC, 0xAB} },
	{0x5, "Ch -",        {0x13, 0x55, 0xBE, 0xAD} }
};

#define SLOTS_COUNT 5

#else
# define PRINTF(...) printf(__VA_ARGS__)
#endif


/**
 * @brief Build response for slot_get command
 * Returns array of SlotInfo structures
 */
static int handle_slot_get(struct Packet* pkt)
{
	PRINTF("Command: my_sock_cmd_slot_get\n");

	// Copy slot data into response packet
	size_t slots_size = sizeof(SlotInfo) * SLOTS_COUNT;

	if (slots_size > 4096 - sizeof(struct Header)) {
		PRINTF("Error: slots data too large\n");
		return -1;
	}

	// Update packet header for response
	pkt->header.len = (uint32_t)slots_size;

	// Copy slots data into packet->data
	memcpy(pkt->data, slots, slots_size);

	PRINTF("Response: %u slots, %u bytes\n", SLOTS_COUNT, (unsigned)pkt->header.len);
	return 0;
}

/**
 * @brief Build response for slot_trigger command
 * Expects payload: uint8_t slot_id
 */
static int handle_slot_trigger(struct Packet* pkt)
{
	if (pkt->header.len < 1) {
		PRINTF("Error: slot_trigger requires slot_id\n");
		return -1;
	}

	uint8_t slot_id = pkt->data[0];
	PRINTF("Command: my_sock_cmd_slot_trigger for slot %d\n", slot_id);

	// Find slot and print info
	for (int i = 0; i < SLOTS_COUNT; i++) {
		if (slots[i].slot_id == slot_id) {
			PRINTF("  Found: %s, IR data: %02x %02x %02x %02x\n",
				   slots[i].slot_name,
				   slots[i].ir_data[0], slots[i].ir_data[1],
				   slots[i].ir_data[2], slots[i].ir_data[3]);
			break;
		}
	}

	// Response: status byte (0 = success)
	pkt->header.len = 1;
	pkt->data[0] = 0x00;  // success

	return 0;
}

/**
 * @brief Build response for slot_clean command
 */
static int handle_slot_clean(struct Packet* pkt)
{
	PRINTF("Command: my_sock_cmd_slot_clean\n");

	// Response: status byte
	pkt->header.len = 1;
	pkt->data[0] = 0x00;  // success

	return 0;
}

/**
 * @brief Build response for slot_clean_all command
 */
static int handle_slot_clean_all(struct Packet* pkt)
{
	PRINTF("Command: my_sock_cmd_slot_clean_all\n");

	// Response: status byte
	pkt->header.len = 1;
	pkt->data[0] = 0x00;  // success

	return 0;
}

/**
 * @brief Build response for slot_assign command
 * Expects payload: SlotInfo structure
 */
static int handle_slot_assign(struct Packet* pkt)
{
	if (pkt->header.len < sizeof(SlotInfo)) {
		PRINTF("Error: slot_assign requires SlotInfo structure\n");
		return -1;
	}

	PRINTF("Command: my_sock_cmd_slot_assign\n");

	SlotInfo* new_slot = (SlotInfo*)pkt->data;
	PRINTF("  New slot: id=%d, name=%s\n", new_slot->slot_id, new_slot->slot_name);

	// Find empty slot and assign
	for (int i = 0; i < MAX_SLOTS; i++) {
		if (slots[i].slot_id == 0) {
			memcpy(&slots[i], new_slot, sizeof(SlotInfo));
			PRINTF("  Assigned to slot index %d\n", i);
			break;
		}
	}

	// Response: status byte
	pkt->header.len = 1;
	pkt->data[0] = 0x00;  // success

	return 0;
}

/**
 * @brief Build response for full_reset command
 */
static int handle_full_reset(struct Packet* pkt)
{
	PRINTF("Command: my_sock_cmd_full_reset\n");

	// Clear all slots
	memset(slots, 0, sizeof(slots));

	// Response: status byte
	pkt->header.len = 1;
	pkt->data[0] = 0x00;  // success

	return 0;
}

/**
 * @brief Build response for watcher_settings command
 */
static int handle_watcher_settings(struct Packet* pkt)
{
	PRINTF("Command: my_sock_cmd_watcher_settings\n");

	// Response: status byte
	pkt->header.len = 1;
	pkt->data[0] = 0x00;  // success

	return 0;
}

/**
 * @brief Build response for wifi_settings command
 */
static int handle_wifi_settings(struct Packet* pkt)
{
	PRINTF("Command: my_sock_cmd_wifi_settings\n");

	// Response: status byte
	pkt->header.len = 1;
	pkt->data[0] = 0x00;  // success

	return 0;
}

/**
 * @brief Build response for update_slot command
 */
static int handle_update_slot(struct Packet* pkt)
{
	PRINTF("Command: my_sock_cmd_update_slot\n");

	// Response: status byte
	pkt->header.len = 1;
	pkt->data[0] = 0x00;  // success

	return 0;
}

/**
 * @brief Main packet handler
 * Modifies packet in-place: reads from pkt->data (input), writes response to pkt->data (output)
 */
int handle_package(struct Packet* pkt)
{
	PRINTF("=== Got package: cmd=%d, id=0x%04x, len=%u ===\n",
		   pkt->header.cmd, pkt->header.id, pkt->header.len);

	switch (pkt->header.cmd)
	{
		case my_sock_cmd_slot_get:
			return handle_slot_get(pkt);

		case my_sock_cmd_slot_trigger:
			return handle_slot_trigger(pkt);

		case my_sock_cmd_slot_clean:
			return handle_slot_clean(pkt);

		case my_sock_cmd_slot_clean_all:
			return handle_slot_clean_all(pkt);

		case my_sock_cmd_slot_assign:
			return handle_slot_assign(pkt);

		case my_sock_cmd_watcher_settings:
			return handle_watcher_settings(pkt);

		case my_sock_cmd_wifi_settings:
			return handle_wifi_settings(pkt);

		case my_sock_cmd_update_slot:
			return handle_update_slot(pkt);

		case my_sock_cmd_full_reset:
			return handle_full_reset(pkt);

		default:
			PRINTF("Error: Unknown command %d\n", pkt->header.cmd);
			pkt->header.len = 1;
			pkt->data[0] = 0xFF;  // error
			return -1;
	}

	return 0;
}
