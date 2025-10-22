#pragma once


typedef enum
{
	trmt_color_red = 0, // TODO set colors from ncurses
	trmt_color_blue,
	trmt_color_yellow,
	trmt_color_green
		// TODO create at least 16 colors
} TermitGameColor_t;

typedef enum
{
	trmt_state_get_state_error = -2,
	trmt_state_inactive = -1,
	trmt_state_stepDone = 0,
	trmt_state_stepWait,
	trmt_state_stepNow
} TermitGameState_t;

typedef struct TermitStructTemp
{
	int x;
	int y;
	////
	int cur_cmd_idx;
	TermitGameColot_t color; // set by random (default) or updated by API
	TermitGameState_t state;
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

int trmt_set_cords(Termit_t* trmt, int x, int y);
int trmt_set_color(Termit_t* trmt, TermitGameColor_t color);

TermitGameState_t trmt_get_state(Termit_t* trmt);

