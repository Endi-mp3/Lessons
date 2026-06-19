#pragma once

#include <stdio.h>          // printf
#include <errno.h>
#include <stdint.h>         // uint8_t, uint16_t
#include <stdbool.h>        // bool

enum MySockRet
{
	my_sock_err_recv = -4,
	my_sock_err_listen = -3,
	my_sock_err_accept = -2,
	my_sock_err_error = -1,
	my_sock_err_ok = 0,
};

enum MySockCommands
{
	my_sock_cmd_err = -1,
	my_sock_cmd_slot_assign,
	my_sock_cmd_slot_trigger,
	my_sock_cmd_slot_clean,
	my_sock_cmd_slot_clean_all,
	my_sock_cmd_watcher_settings,
	my_sock_cmd_wifi_settings,
	my_sock_cmd_update_slot,
	my_sock_cmd_full_reset

};

#pragma pack(push, 1)
struct Header
{
    int8_t cmd; /// enum MySockCommands
    uint16_t id;
    uint32_t len;
//	uint32_t pack_num; // 4
//	uint32_t pack_max_count; // 255
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Packet
{
    struct Header header;
    uint8_t data[0];
};
#pragma pack(pop)


