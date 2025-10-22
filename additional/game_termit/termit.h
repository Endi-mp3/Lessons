#pragma once


typedef struct TermitStructTemp
{
	int x;
	int y;
	////
	int cur_cmd_idx;
	////
	// parent?
} TermitGame_t;

typedef struct TermitCmdListItem
	char* cmd;
	size_t len;
	struct TermitCmdListItem* next;
} TermitCmdListItem_t;

typedef struct TermitStruct
{
	int startX;
	int startY;
	TermitGame_t* trmt_game;
	TermitCmdListItem_t* trmt_list_cmds;
} Termit_t;

/// Functions

/**
 * @brief	Create and fills with empty nointers Termit Struct
 * @return	Termit handler ptr
 */
Termit_t* trmt_create();
/**
 * @brief	Create and initialize with configuration from string
 * @param	cfg - Configuration in single string
 * @return	Termit handler ptr
 */
Termit_t* trmt_createf(const char* cfg);
/**
 * @brief	Create and init with config from file @param path2cfg
 * @param	path2cfg
 * @return	Termit handler ptr
 */
Termit_t* trmt_createp(const char* path2cfg);

/**
 * @brief	Deinit and destroy all existing child of Termit struct
 * @param	trmt
 */
void trmt_delete(Termit_t* trmt);

const char* trmt_get_next_cmd(Termit_t* trmt);

int trmt_get_game_info(Termit_t* trmt, TermitGame_t** gameinfoPtr);

/**
 * @brief	Return current game, if started, cords
 * @param	trmt - Termit ptr
 * @param	x - x-cord ptr
 * @param	y - y-cord ptr
 * @return	0 - success
 *			-1 - fail
 */
int trmt_get_cords(Termit_t* trmt, int *x, int *y);
