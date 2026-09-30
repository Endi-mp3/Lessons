#pragma once
#include "my_socket_proto.h"


/**
 * @brief	Function handle package, call proper logic function and return answer in same pkt.
 * @param	pkt - pointer to package struct. Expected that it is char buffer mapped to struct Packet, so it will be used for answer.
 * @return	0 - success. -1 in case of error.
 */
int handle_package(struct Packet* pkt);
