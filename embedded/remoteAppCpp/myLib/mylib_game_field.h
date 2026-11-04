#ifndef MSV_GAMEFIELD_H
#define MSV_GAMEFIELD_H

#include <ncurses.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int filled;       // 0 = empty, 1 = filled
    chtype symbol;    // character to draw (e.g. ACS_CKBOARD, 'O', etc.)
    int color_pair;   // ncurses color pair
} mylib_gmfld_t;

/**
 * Initialize a cellular grid inside a split.
 * width, height = number of cells (not characters).
 */
int mylib_gmfld_init(int split_id, int width, int height);

/**
 * Set cell state.
 */
int mylib_gmfld_set(int x, int y, int filled, chtype symbol, int color_pair);

/**
 * Get cell state.
 */
int mylib_gmfld_get(int x, int y, mylib_gmfld_t *out);

/**
 * Redraw the entire grid with borders and cells.
 */
int mylib_gmfld_draw(void);

int mylib_gmfld_step(int split_id);
#ifdef __cplusplus
}
#endif

#endif

