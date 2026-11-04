#include "mylib_game_field.h"
#include "mylib_splitview.h"
#include "mylib_io.h"
#include <stdlib.h>
#include <string.h>

static int cell_split_id = -1;
static int grid_w = 0, grid_h = 0;
static mylib_gmfld_t **grid = NULL;

int mylib_gmfld_init(int split_id, int width, int height)
{
    cell_split_id = split_id;
    grid_w = width;
    grid_h = height;

    grid = malloc(height * sizeof(mylib_gmfld_t*));
    for (int y = 0; y < height; y++) {
        grid[y] = malloc(width * sizeof(mylib_gmfld_t));
        for (int x = 0; x < width; x++) {
            grid[y][x].filled = 0;
            grid[y][x].symbol = ' ';
            grid[y][x].color_pair = 1;
        }
    }
    return 0;
}

int mylib_gmfld_set(int x, int y, int filled, chtype symbol, int color_pair)
{

    if (x < 0 || x >= grid_w || y < 0 || y >= grid_h) return -1;
    grid[y][x].filled = filled;
    grid[y][x].symbol = symbol;
    grid[y][x].color_pair = color_pair;
    return 0;
}

int mylib_gmfld_get(int x, int y, mylib_gmfld_t *out)
{

    if (x < 0 || x >= grid_w || y < 0 || y >= grid_h) return -1;
    if (!out) return -1;
    *out = grid[y][x];
    return 0;
}

int mylib_gmfld_draw(void)
{
    WINDOW *w = mylib_sv_get_win(cell_split_id);
    if (!w) return -1;
    werase(w);
    box(w, 0, 0);

    // compute cell size in characters
    int cell_w = 2;
    int cell_h = 1;

    for (int y = 0; y < grid_h; y++) {
        for (int x = 0; x < grid_w; x++) {
            int scr_y = 1 + y * cell_h;
            int scr_x = 1 + x * cell_w;
            mylib_gmfld_t *c = &grid[y][x];
            if (c->filled) {
                wattron(w, COLOR_PAIR(c->color_pair));
                mvwaddch(w, scr_y, scr_x, c->symbol);
                wattroff(w, COLOR_PAIR(c->color_pair));
            } else {
                mvwaddch(w, scr_y, scr_x, '.'); // empty cell marker
            }
        }
    }
    wrefresh(w);
    return 0;
}

int mylib_gmfld_step(int split_id)
{

    WINDOW *w = (split_id == -1) ? stdscr : mylib_sv_get_win(split_id);
    if (!w) return -1;

    // Очистка только своей области
    werase(w);
    box(w, 0, 0);

    // Пример отрисовки клеточного поля
    for (int y = 0; y < grid_h; y++) {
        for (int x = 0; x < grid_w; x++) {
            mylib_gmfld_t *c = &grid[y][x];
            int scr_y = 1 + y;
            int scr_x = 1 + x * 2;

            if (c->filled) {
                wattron(w, COLOR_PAIR(c->color_pair));
                mvwaddch(w, scr_y, scr_x, c->symbol);
                wattroff(w, COLOR_PAIR(c->color_pair));
            } else {
                mvwaddch(w, scr_y, scr_x, '.');
            }
        }
    }

    wnoutrefresh(w);
    return 0;
}

