#ifndef MSV_SPLITVIEW_H
#define MSV_SPLITVIEW_H

#include <ncurses.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MSV_SPLIT_HORIZONTAL = 0,  // horizontal line; splits top/bottom
    MSV_SPLIT_VERTICAL   = 1   // vertical line; splits left/right
} msv_split_dir_t;

typedef struct {
    int x;  // origin column inside terminal
    int y;  // origin row inside terminal
} msv_point_t;

typedef struct {
    int w;  // width in columns
    int h;  // height in rows
} msv_size_t;

/**
 * Initialize root split tree covering the entire terminal screen.
 * Creates a single root window with a border.
 * Must be called after initscr().
 * Returns 0 on success, -1 on failure.
 */
int msv_init(void);

/**
 * Shutdown split system and destroy all windows.
 */
void msv_shutdown(void);

/**
 * Create a split of the region identified by 'parent_id' into two child windows.
 * The split direction controls the separator orientation.
 * Returns the ID of the newly created child (secondary) region, or -1 on error.
 * The original 'parent_id' becomes the first child; the function returns the ID of the second.
 *
 * Layout policy:
 * - Vertical: left gets roughly half, right gets remainder.
 * - Horizontal: top gets roughly half, bottom gets remainder.
 * - Both children get borders; separator rendered with ACS lines.
 */
int msv_create_split(int parent_id, msv_split_dir_t dir);

/**
 * Clear the content area (inside the border) of the window by ID.
 * Does not remove the border or the split lines.
 * Returns 0 on success, -1 if ID invalid.
 */
int msv_clear_id(int id);

/**
 * Get the origin (top-left) point of a window by ID.
 * Returns 0 on success, -1 if ID invalid.
 */
int msv_get_base_id(int id, msv_point_t *out);

/**
 * Get the size (width, height) of a window by ID.
 * Returns 0 on success, -1 if ID invalid.
 */
int msv_get_size_id(int id, msv_size_t *out);

/**
 * Retrieve the ncurses WINDOW* for a given split ID (read-only use).
 * Returns NULL if ID invalid.
 */
WINDOW* msv_get_win(int id);

/**
 * Redraw borders for all windows and separators.
 */
void msv_redraw_all(void);

#ifdef __cplusplus
}
#endif

#endif

