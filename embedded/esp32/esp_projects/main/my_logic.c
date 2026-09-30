#include "my_logic.h"

int handle_package(struct Packet* pkt)
{
	switch (pkt->header.cmd)
	{
	case my_sock_cmd_slot_assign:

		// pseudo code
		// form success answer package
		// form error answer package
		//
		break;
	default:
		break;
	}

	return 0;
}
