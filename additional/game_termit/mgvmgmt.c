// mgvmgmt.c
// MyGameViewManager — версия под обычный ncurses (ASCII/ACS символы)

#include "mgvmgmt.h"
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

// Геометрия клетки (в символах)
#define CELL_W 3
#define CELL_H 2

// --- Внутренние состояния модуля ---
static mgvmgmt_field_t field;
static mgvmgmt_viewport_t viewport;
static WINDOW *win_field = NULL;

// --- Вспомогательные функции ---
static inline int in_bounds(int x, int y) {
    return x >= 0 && x < field.width && y >= 0 && y < field.height;
}

static void draw_cell(WINDOW *win, int scr_x, int scr_y, const mgvmgmt_cell_t *c) {
    // рамка клетки
    mvwaddch(win, scr_y, scr_x, ACS_ULCORNER);
    mvwhline(win, scr_y, scr_x + 1, ACS_HLINE, CELL_W - 1);
    mvwaddch(win, scr_y, scr_x + CELL_W, ACS_URCORNER);

    mvwvline(win, scr_y + 1, scr_x, ACS_VLINE, CELL_H - 1);
    mvwvline(win, scr_y + 1, scr_x + CELL_W, ACS_VLINE, CELL_H - 1);

    mvwaddch(win, scr_y + CELL_H, scr_x, ACS_LLCORNER);
    mvwhline(win, scr_y + CELL_H, scr_x + 1, ACS_HLINE, CELL_W - 1);
    mvwaddch(win, scr_y + CELL_H, scr_x + CELL_W, ACS_LRCORNER);

    // заливка
    if (c->filled) {
        wattron(win, COLOR_PAIR(c->color_pair));
        for (int yy = 1; yy < CELL_H; yy++) {
            for (int xx = 1; xx < CELL_W; xx++) {
                mvwaddch(win, scr_y + yy, scr_x + xx, ' ');
            }
        }
        wattroff(win, COLOR_PAIR(c->color_pair));
    }

    // игрок
    if (c->player_id >= 0) {
        const mgvmgmt_player_t *p = &field.players[c->player_id];
        if (p->active) {
            wattron(win, COLOR_PAIR(p->color_pair));
            // вместо Unicode используем '@'
            mvwaddch(win, scr_y + (CELL_H / 2), scr_x + (CELL_W / 2), '@');
            wattroff(win, COLOR_PAIR(p->color_pair));
        }
    }
}

static void allocate_field(int w, int h) {
    field.width = w;
    field.height = h;
    field.cells = malloc(h * sizeof(mgvmgmt_cell_t *));
    for (int y = 0; y < h; y++) {
        field.cells[y] = malloc(w * sizeof(mgvmgmt_cell_t));
        for (int x = 0; x < w; x++) {
            field.cells[y][x].filled = 0;
            field.cells[y][x].color_pair = 1;
            field.cells[y][x].player_id = -1;
        }
    }
    for (int i = 0; i < MGVMGMT_MAX_PLAYERS; i++) {
        field.players[i].active = 0;
    }
}

static void free_field(void) {
    if (field.cells) {
        for (int y = 0; y < field.height; y++) {
            free(field.cells[y]);
        }
        free(field.cells);
        field.cells = NULL;
    }
}

// --- Публичные функции API ---
void mgvmgmt_init(int field_w, int field_h) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();

    // простые цветовые пары
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_CYAN, COLOR_BLACK);

    allocate_field(field_w, field_h);

    int term_h, term_w;
    getmaxyx(stdscr, term_h, term_w);
    int max_cells_w = (term_w - 2) / CELL_W;
    int max_cells_h = (term_h - 2) / CELL_H;
    if (max_cells_w <= 0) max_cells_w = 1;
    if (max_cells_h <= 0) max_cells_h = 1;

    viewport.x = 0;
    viewport.y = 0;
    viewport.w = (field_w < max_cells_w) ? field_w : max_cells_w;
    viewport.h = (field_h < max_cells_h) ? field_h : max_cells_h;

    win_field = newwin(viewport.h * CELL_H + 2, viewport.w * CELL_W + 2, 0, 0);
    box(win_field, 0, 0);
    wrefresh(win_field);
}

void mgvmgmt_shutdown(void) {
    free_field();
    if (win_field) {
        delwin(win_field);
        win_field = NULL;
    }
    endwin();
}

void mgvmgmt_set_cell(int x, int y, int filled, int color_pair) {
    if (!in_bounds(x, y)) return;
    mgvmgmt_cell_t *c = &field.cells[y][x];
    c->filled = filled ? 1 : 0;
    c->color_pair = color_pair;
}

void mgvmgmt_add_player(int id, int x, int y, int color_pair) {
    if (id < 0 || id >= MGVMGMT_MAX_PLAYERS) return;
    if (!in_bounds(x, y)) return;
    mgvmgmt_player_t *p = &field.players[id];
    if (p->active && in_bounds(p->x, p->y)) {
        field.cells[p->y][p->x].player_id = -1;
    }
    p->x = x;
    p->y = y;
    p->color_pair = color_pair;
    p->active = 1;
    field.cells[y][x].player_id = id;
}

void mgvmgmt_move_player(int id, int dx, int dy) {
    if (id < 0 || id >= MGVMGMT_MAX_PLAYERS) return;
    mgvmgmt_player_t *p = &field.players[id];
    if (!p->active) return;
    int nx = p->x + dx;
    int ny = p->y + dy;
    if (!in_bounds(nx, ny)) return;
    field.cells[p->y][p->x].player_id = -1;
    p->x = nx;
    p->y = ny;
    field.cells[ny][nx].player_id = id;
}

void mgvmgmt_set_viewport(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + w > field.width) w = field.width - x;
    if (y + h > field.height) h = field.height - y;
    viewport.x = x;
    viewport.y = y;
    viewport.w = w;
    viewport.h = h;
    if (win_field) {
        wresize(win_field, viewport.h * CELL_H + 2, viewport.w * CELL_W + 2);
    } else {
        win_field = newwin(viewport.h * CELL_H + 2, viewport.w * CELL_W + 2, 0, 0);
    }
    box(win_field, 0, 0);
    wrefresh(win_field);
}

void mgvmgmt_draw_field(void) {
    if (!win_field) return;
    werase(win_field);
    box(win_field, 0, 0);
    for (int vy = 0; vy < viewport.h; vy++) {
        for (int vx = 0; vx < viewport.w; vx++) {
            int gx = viewport.x + vx;
            int gy = viewport.y + vy;
            if (!in_bounds(gx, gy)) continue;
            const mgvmgmt_cell_t *c = &field.cells[gy][gx];
            int scr_x = 1 + vx * CELL_W;
            int scr_y = 1 + vy * CELL_H;
            draw_cell(win_field, scr_x, scr_y, c);
        }
    }
    wrefresh(win_field);
}

void mgvmgmt_scroll_viewport(int dx, int dy) {
    mgvmgmt_set_viewport(viewport.x + dx, viewport.y + dy, viewport.w, viewport.h);
}

