#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "termit.h"

/// ===== Portable fmemopen fallback =====
/// On systems without fmemopen, use a tmpfile and write the buffer into it.

static FILE* trmt_fmemopen_ro(const void* buf, size_t size)
{
#if defined(__APPLE__) || defined(__MACH__) || defined(__sun) || defined(_WIN32)
    FILE* f = tmpfile();
    if (!f) return NULL;
    if (size) {
        if (fwrite(buf, 1, size, f) != size) {
            fclose(f);
            return NULL;
        }
    }
    rewind(f);
    return f;
#else
    // POSIX/glibc has fmemopen
    return fmemopen((void*)buf, size, "r");
#endif
}

/// ===== Helpers =====
static TermitCmdListItem_t* trmt_cmd_item_create(const char* cmd)
{
    if (!cmd)
		return NULL;
    TermitCmdListItem_t* item = (TermitCmdListItem_t*)malloc(sizeof(TermitCmdListItem_t));
    if (!item)
		return NULL;
    item->len = strlen(cmd);
    item->cmd = (char*)malloc(item->len + 1);
    if (!item->cmd) {
        free(item);
        return NULL;
    }
    memcpy(item->cmd, cmd, item->len + 1);
    item->next = NULL;
    return item;
}

static void trmt_cmd_list_free(TermitCmdListItem_t* head)
{
    while (head) {
        TermitCmdListItem_t* tmp = head->next;
        free(head->cmd);
        free(head);
        head = tmp;
    }
}

static int trmt_parse_first_line(const char* line, int* x, int* y)
{
    // Accept "010 012", "10 12", with or without leading zeros
    if (!line || !x || !y)
		return -1;
    int tx = 0, ty = 0;
    if (sscanf(line, "%d %d", &tx, &ty) != 2)
		return -1;
    *x = tx;
    *y = ty;
    return 0;
}

/// ===== Core parser =====

static Termit_t* trmt_parse_cfg(FILE* f)
{
    if (!f)
		return NULL;

    char line[512];
    if (!fgets(line, sizeof(line), f))
		return NULL;

    int startX = 0, startY = 0;
    if (trmt_parse_first_line(line, &startX, &startY) != 0) {
        return NULL;
    }

    Termit_t* trmt = trmt_create();
    if (!trmt)
		return NULL;

    trmt->startX = startX;
    trmt->startY = startY;

    trmt->trmt_game = (TermitGame_t*)calloc(1, sizeof(TermitGame_t));
    if (!trmt->trmt_game) {
        trmt_delete(trmt);
        return NULL;
    }
    trmt->trmt_game->x = trmt->startX;
    trmt->trmt_game->y = trmt->startY;
    trmt->trmt_game->cur_cmd_idx = 0;

    TermitCmdListItem_t* head = NULL;
    TermitCmdListItem_t* tail = NULL;

    while (fgets(line, sizeof(line), f)) {
        // strip trailing newline
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "EOF") == 0)
			break;
        if (line[0] == '\0')
			continue; // skip empty lines

        TermitCmdListItem_t* item = trmt_cmd_item_create(line);
        if (!item) {
            trmt_delete(trmt);
            return NULL;
        }
        if (!head) {
            head = tail = item;
        } else {
            tail->next = item;
            tail = item;
        }
    }

    trmt->trmt_list_cmds = head;
    return trmt;
}

/// ===== API =====
Termit_t* trmt_create()
{
    Termit_t* trmt = (Termit_t*)calloc(1, sizeof(Termit_t));
    if (!trmt)
		return NULL;
    trmt->startX = 0;
    trmt->startY = 0;
    trmt->trmt_game = NULL;
    trmt->trmt_list_cmds = NULL;
    return trmt;
}

Termit_t* trmt_createf(const char* cfg)
{
    if (!cfg)
		return NULL;
    size_t n = strlen(cfg);
    FILE* f = trmt_fmemopen_ro(cfg, n);
    if (!f)
		return NULL;
    Termit_t* trmt = trmt_parse_cfg(f);
    fclose(f);
    return trmt;
}

Termit_t* trmt_createp(const char* path2cfg)
{
    if (!path2cfg)
		return NULL;
    FILE* f = fopen(path2cfg, "r");
    if (!f)
		return NULL;
    Termit_t* trmt = trmt_parse_cfg(f);
    fclose(f);
    return trmt;
}

void trmt_delete(Termit_t* trmt)
{
    if (!trmt)
		return;
    if (trmt->trmt_game)
		free(trmt->trmt_game);
    if (trmt->trmt_list_cmds)
		trmt_cmd_list_free(trmt->trmt_list_cmds);
    free(trmt);
}

const char* trmt_get_next_cmd(Termit_t* trmt)
{
    if (!trmt || !trmt->trmt_game)
		return NULL;

    TermitCmdListItem_t* item = trmt->trmt_list_cmds;
    int idx = 0;
    while (item) {
        if (idx == trmt->trmt_game->cur_cmd_idx) {
            trmt->trmt_game->cur_cmd_idx++;
            return item->cmd;
        }
        item = item->next;
        idx++;
    }
    return NULL; // no more commands
}

int trmt_get_game_info(Termit_t* trmt, TermitGame_t** gameinfoPtr)
{
    if (!trmt || !trmt->trmt_game || !gameinfoPtr)
		return -1;
    *gameinfoPtr = trmt->trmt_game;
    return 0;
}

int trmt_get_cords(Termit_t* trmt, int *x, int *y)
{
    if (!trmt || !trmt->trmt_game || !x || !y)
		return -1;
    *x = trmt->trmt_game->x;
    *y = trmt->trmt_game->y;
    return 0;
}

int trmt_reset(Termit_t* trmt)
{
    if (!trmt || !trmt->trmt_game)
		return -1;
    trmt->trmt_game->x = trmt->startX;
    trmt->trmt_game->y = trmt->startY;
    trmt->trmt_game->cur_cmd_idx = 0;
    return 0;
}

int trmt_set_cords(Termit_t* trmt, int x, int y)
{
    if (!trmt || !trmt->trmt_game)
		return -1;
	trmt->trmt_game->x = x;
	trmt->trmt_game->y = y;
	return 0;
}
int trmt_set_color(Termit_t* trmt, TermitGameColor_t color)
{
    if (!trmt || !trmt->trmt_game)
		return -1;
	trmt->trmt_game->color = color;
	return 0;
}

TermitGameState_t trmt_get_state(Termit_t* trmt)
{
    if (!trmt || !trmt->trmt_game)
		return trmt_state_get_state_error;

	return trmt->trmt_game->state;
}

int trmt_game_new(Termit_t *trmt)
{
	if (!trmt)
		return -1;

	if (trmt->trmt_game)
		return 1;

	trmt->trmt_game = (TermitGame_t*)malloc(sizeof(TermitGame_t));
	memset(trmt->trmt_game, 0, sizeof(TermitGame_t));
	trmt->trmt_game->color = trmt_color_red + rand() % 16;
	return 0;
}

int trmt_game_stop(Termit_t *trmt)
{
	if (!trmt || !trmt->trmt_game)
		return -1;

	free(trmt->trmt_game);
	trmt->trmt_game = NULL;
	return 0;
}

int trmt_game_reset(Termit_t *trmt)
{
	if (!trmt || !trmt->trmt_game)
		return -1;

	memset(trmt->trmt_game, 0, sizeof(TermitGame_t));
	trmt->trmt_game->color = trmt_color_red + rand() % 16;
	return 0;
}

int trmt_set_live(Termit_t* trmt, int live)
{
	if (!trmt || !trmt->trmt_game)
		return -1;
	trmt->trmt_game->live = live;
	return 0;
}
