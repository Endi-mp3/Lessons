// mylib_io.c — strict rendering helpers bound to MySplitView windows
#include "mylib_io.h"
#include "mylib_splitview.h"
#include <stdarg.h>

static int inside_bounds(int split_id, int row, int col) {
    mylib_sv_size_t sz;
    if (mylib_sv_get_size_id(split_id, &sz) != 0) return 0;
    // inside area is (1..h-2, 1..w-2)
    return (row >= 1 && row <= sz.h - 2 && col >= 1 && col <= sz.w - 2);
}

int mylib_io_print_at(int split_id, int row, int col, const char *fmt, ...)
{
    WINDOW *w = mylib_sv_get_win(split_id);
    if (!w) return -1;
    if (!inside_bounds(split_id, row, col)) return -1;

    va_list ap;
    va_start(ap, fmt);
    wmove(w, row, col);
    vw_printw(w, fmt, ap);
    va_end(ap);
    wrefresh(w);
    return 0;
}

int mylib_io_fill_rect(int split_id, int row, int col, int height, int width, chtype ch, int color_pair)
{
    WINDOW *w = mylib_sv_get_win(split_id);
    if (!w) return -1;

    wattron(w, COLOR_PAIR(color_pair));
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            if (!inside_bounds(split_id, row + r, col + c)) continue;
            mvwaddch(w, row + r, col + c, ch);
        }
    }
    wattroff(w, COLOR_PAIR(color_pair));
    wrefresh(w);
    return 0;
}

int mylib_io_clear(int split_id)
{
    return mylib_sv_clear_id(split_id);
}

