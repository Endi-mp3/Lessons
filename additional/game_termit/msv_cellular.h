#ifndef MSV_CELLULAR_H
#define MSV_CELLULAR_H

#include <ncurses.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int filled;       // 0 = empty, 1 = filled
    chtype symbol;    // character to draw (e.g. ACS_CKBOARD, 'O', etc.)
    int color_pair;   // ncurses color pair
} msv_cell_t;

/**
 * Initialize a cellular grid inside a split.
 * width, height = number of cells (not characters).
 */
int msv_cell_init(int split_id, int width, int height);

/**
 * Set cell state.
 */
int msv_cell_set(int x, int y, int filled, chtype symbol, int color_pair);

/**
 * Get cell state.
 */
int msv_cell_get(int x, int y, msv_cell_t *out);

/**
 * Redraw the entire grid with borders and cells.
 */
int msv_cell_draw(void);

int msv_cell_step(int split_id);
#ifdef __cplusplus
}
#endif

#endif

