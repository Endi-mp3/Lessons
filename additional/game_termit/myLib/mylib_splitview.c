// mylib_sv_splitview.c — split manager with bordered windows and separators
#include "mylib_splitview.h"
#include <stdlib.h>
#include <string.h>

#define MSV_MAX_NODES 128

typedef struct {
    int used;
    WINDOW *win;
    int x, y, w, h; // geometry
    int parent;     // -1 if root
    int child_a;    // first child id or -1
    int child_b;    // second child id or -1
    mylib_sv_split_dir_t dir; // direction of split that produced children (valid if has children)
} mylib_sv_node_t;

static mylib_sv_node_t nodes[MSV_MAX_NODES];
static int root_id = -1;

static int alloc_node(void) {
    for (int i = 0; i < MSV_MAX_NODES; i++) {
        if (!nodes[i].used) {
            nodes[i].used = 1;
            nodes[i].win = NULL;
            nodes[i].child_a = -1;
            nodes[i].child_b = -1;
            nodes[i].parent = -1;
            nodes[i].dir = MYLIB_SV_DIR_VERTICAL;
            return i;
        }
    }
    return -1;
}

static void draw_border(WINDOW *w) {
    if (!w) return;
    box(w, 0, 0);
    wrefresh(w);
}

static WINDOW* create_window_rect(int y, int x, int h, int w) {
    WINDOW *win = newwin(h, w, y, x);
    if (win) {
        keypad(win, TRUE);
        draw_border(win);
    }
    return win;
}

int mylib_sv_init(void)
{
    memset(nodes, 0, sizeof(nodes));

    int term_h, term_w;
    getmaxyx(stdscr, term_h, term_w);

    root_id = alloc_node();
    if (root_id < 0) return -1;

    nodes[root_id].x = 0;
    nodes[root_id].y = 0;
    nodes[root_id].w = term_w;
    nodes[root_id].h = term_h;
    nodes[root_id].win = create_window_rect(0, 0, term_h, term_w);
    if (!nodes[root_id].win) return -1;

    return 0;
}

void mylib_sv_shutdown(void)
{
    for (int i = 0; i < MSV_MAX_NODES; i++) {
        if (nodes[i].used) {
            if (nodes[i].win) {
                delwin(nodes[i].win);
            }
            nodes[i].used = 0;
        }
    }
    root_id = -1;
}

static int is_valid(int id) {
    return (id >= 0 && id < MSV_MAX_NODES && nodes[id].used);
}

int mylib_sv_clear_id(int id)
{
    if (!is_valid(id)) return -1;
    WINDOW *w = nodes[id].win;
    if (!w) return -1;
    // clear inside border: leave edges intact
    int h = nodes[id].h;
    int wcols = nodes[id].w;
    for (int r = 1; r < h - 1; r++) {
        for (int c = 1; c < wcols - 1; c++) {
            mvwaddch(w, r, c, ' ');
        }
    }
    wrefresh(w);
    return 0;
}

int mylib_sv_get_base_id(int id, mylib_sv_point_t *out)
{
    if (!is_valid(id) || !out) return -1;
    out->x = nodes[id].x;
    out->y = nodes[id].y;
    return 0;
}

int mylib_sv_get_size_id(int id, mylib_sv_size_t *out)
{
    if (!is_valid(id) || !out) return -1;
    out->w = nodes[id].w;
    out->h = nodes[id].h;
    return 0;
}

WINDOW* mylib_sv_get_win(int id)
{
    if (!is_valid(id)) return NULL;
    return nodes[id].win;
}

int mylib_sv_create_split(int parent_id, mylib_sv_split_dir_t dir)
{
    if (!is_valid(parent_id)) return -1;
    mylib_sv_node_t *p = &nodes[parent_id];

    // Cannot split if already has children
    if (p->child_a != -1 || p->child_b != -1) return -1;

    int a_id = alloc_node();
    int b_id = alloc_node();
    if (a_id < 0 || b_id < 0) return -1;

    nodes[a_id].parent = parent_id;
    nodes[b_id].parent = parent_id;

    int x = p->x, y = p->y, w = p->w, h = p->h;

    if (dir == MYLIB_SV_DIR_VERTICAL) {
        // left/right; add a vertical separator column
        int left_w = w / 2;
        int sep_w = 1;
        int right_w = w - left_w - sep_w;

        nodes[a_id].x = x;
        nodes[a_id].y = y;
        nodes[a_id].w = left_w;
        nodes[a_id].h = h;

        nodes[b_id].x = x + left_w + sep_w;
        nodes[b_id].y = y;
        nodes[b_id].w = right_w;
        nodes[b_id].h = h;

        // render separator in parent region
        // We keep parent window as a container, children get their own windows
        // Draw separator line using stdscr for global coordinates
        for (int r = y; r < y + h; r++) {
            mvaddch(r, x + left_w, ACS_VLINE);
        }
    } else {
        // horizontal: top/bottom; add a horizontal separator row
        int top_h = h / 2;
        int sep_h = 1;
        int bottom_h = h - top_h - sep_h;

        nodes[a_id].x = x;
        nodes[a_id].y = y;
        nodes[a_id].w = w;
        nodes[a_id].h = top_h;

        nodes[b_id].x = x;
        nodes[b_id].y = y + top_h + sep_h;
        nodes[b_id].w = w;
        nodes[b_id].h = bottom_h;

        for (int c = x; c < x + w; c++) {
            mvaddch(y + top_h, c, ACS_HLINE);
        }
    }

    refresh();

    nodes[a_id].win = create_window_rect(nodes[a_id].y, nodes[a_id].x, nodes[a_id].h, nodes[a_id].w);
    nodes[b_id].win = create_window_rect(nodes[b_id].y, nodes[b_id].x, nodes[b_id].h, nodes[b_id].w);

    p->child_a = a_id;
    p->child_b = b_id;
    p->dir = dir;

    // Return ID of the second (sibling) region so caller can use both: parent_id and returned id.
    return b_id;
}

void mylib_sv_redraw_all(void)
{
    // Redraw borders for all nodes
    for (int i = 0; i < MSV_MAX_NODES; i++) {
        if (nodes[i].used && nodes[i].win) {
            box(nodes[i].win, 0, 0);
            wrefresh(nodes[i].win);
        }
    }
    // Redraw separators by re-walking parent-child splits
    for (int i = 0; i < MSV_MAX_NODES; i++) {
        if (!nodes[i].used) continue;
        mylib_sv_node_t *p = &nodes[i];
        if (p->child_a != -1 && p->child_b != -1) {
            int x = p->x, y = p->y, w = p->w, h = p->h;
            if (p->dir == MYLIB_SV_DIR_VERTICAL) {
                int left_w = nodes[p->child_a].w;
                for (int r = y; r < y + h; r++) mvaddch(r, x + left_w, ACS_VLINE);
            } else {
                int top_h = nodes[p->child_a].h;
                for (int c = x; c < x + w; c++) mvaddch(y + top_h, c, ACS_HLINE);
            }
        }
    }
    refresh();
}

