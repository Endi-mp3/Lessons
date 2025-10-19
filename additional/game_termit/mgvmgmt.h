#ifndef MGVMGMT_H
#define MGVMGMT_H



#include <ncurses.h>
#define MGVMGMT_MAX_PLAYERS 16

// --- Типы ---

typedef struct {
    int filled;       // 0/1
    int color_pair;   // ncurses color pair
    int player_id;    // -1 если нет игрока
} mgvmgmt_cell_t;

typedef struct {
    int x, y;
    int color_pair;
    int active;
} mgvmgmt_player_t;

typedef struct {
    int width;
    int height;
    mgvmgmt_cell_t **cells;
    mgvmgmt_player_t players[MGVMGMT_MAX_PLAYERS];
} mgvmgmt_field_t;

typedef struct {
    int x, y;   // левый верхний угол
    int w, h;   // размеры в клетках
} mgvmgmt_viewport_t;

// --- API ---

// Инициализация/завершение
void mgvmgmt_init(int field_w, int field_h);
void mgvmgmt_shutdown(void);

// Работа с клетками
void mgvmgmt_set_cell(int x, int y, int filled, int color_pair);

// Работа с игроками
void mgvmgmt_add_player(int id, int x, int y, int color_pair);
void mgvmgmt_move_player(int id, int dx, int dy);

// Viewport
void mgvmgmt_set_viewport(int x, int y, int w, int h);
void mgvmgmt_draw_field(void);
void mgvmgmt_scroll_viewport(int dx, int dy);


#endif

