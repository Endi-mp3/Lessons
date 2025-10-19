#ifndef MSV_IO_H
#define MSV_IO_H

#include <ncurses.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Print formatted text inside the content area of a target split window.
 * Coordinates are relative to the inside (exclude border): (1,1) is first cell inside.
 * Returns 0 on success, -1 on error.
 */
int msv_io_print_at(int split_id, int row, int col, const char *fmt, ...);

/**
 * Fill a rectangular area inside the target window with a character and optional color pair.
 * row/col and height/width are inside-content coordinates.
 * Returns 0 on success, -1 on error.
 */
int msv_io_fill_rect(int split_id, int row, int col, int height, int width, chtype ch, int color_pair);

/**
 * Clear the inside content of the target split.
 */
int msv_io_clear(int split_id);

#ifdef __cplusplus
}
#endif

#endif

