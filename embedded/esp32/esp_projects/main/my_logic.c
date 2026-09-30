#include "my_logic.h"
#ifdef TESTING
# include <stdio.h>
#endif


int handle_package(struct Packet* pkt)
{
#ifdef TESTING
	printf("got package\n");
#endif
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
