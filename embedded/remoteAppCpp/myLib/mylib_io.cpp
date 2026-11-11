// mylib_io.c — strict rendering helpers bound to MySplitView windows
#include "mylib_io.h"
#include "mylib_splitview.h"
#include <stdarg.h>

int MyLib::IO::inside_bounds(const MyLib::sv_size_t& sz, size_t row, size_t col)
{
    // inside area is (1..h-2, 1..w-2)
    return (row >= 1 && row <= sz.h - 2 && col >= 1 && col <= sz.w - 2);
}

int MyLib::IO::print_at(SplitViewNode* sv, size_t row, size_t col, const char *fmt, ...)
{
    WINDOW *w = sv->win();
    if (!w) return -1;
    if (!inside_bounds(sv->size(), row, col)) return -1;

    va_list ap;
    va_start(ap, fmt);
    wmove(w, row, col);
    vw_printw(w, fmt, ap);
    va_end(ap);
    wrefresh(w);
    return 0;
}

int MyLib::IO::fill_rect(SplitViewNode* sv, size_t row, size_t col, int height, int width, chtype ch, int color_pair)
{
    WINDOW *w = sv->win();
    if (!w) return -1;

    wattron(w, COLOR_PAIR(color_pair));
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            if (!inside_bounds(sv->size(), row + r, col + c)) continue;
            mvwaddch(w, row + r, col + c, ch);
        }
    }
    wattroff(w, COLOR_PAIR(color_pair));
    wrefresh(w);
    return 0;
}

int MyLib::IO::clear(SplitViewNode* sv)
{
    return sv->clear();
}

