#include "my_logic.h"
#ifdef TESTING
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# define PRINTF(...) printf(__VA_ARGS__)


static SlotInfo slots[MAX_SLOTS] = {
	{0x1, "TEST1", {0x1, 0x2, 0xDE, 0xAD} },
	{0x2, "TEST2", {0x1, 0x12, 0xBE, 0xAF} },
	{0x3, "TEST3", {0x45, 0x22, 0xDE, 0xAD} },
	{0x4, "TEST4", {0x11, 0x34, 0xAC, 0xAB} },
	{0x5, "TEST5", {0x13, 0x55, 0xBE, 0xAD} }
};

#else
# define PRINTF(...) printf(__VA_ARGS__)
#endif


int handle_package(struct Packet* pkt)
{
	PRINTF("got package\n");
	switch (pkt->header.cmd)
	{
	case my_sock_cmd_slot_assign:

		// pseudo code
		// form success answer package
		// form error answer package
		//
		break;
	case my_sock_cmd_slot_get:
		pkt->header.id = 1;
		pkt->header.len = sizeof(SlotInfo) * 5;
		memcpy(slots, pkt->data, sizeof(SlotInfo) * 5);
		break;
	default:
		break;
	}

	return 0;
}
