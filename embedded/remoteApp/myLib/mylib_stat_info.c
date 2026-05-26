#include "mylib_stat_info.h"
#include "mylib_splitview.h"
#include "mylib_io.h"
#include <string.h>

#define MSV_STAT_MAX_ITEMS 64

static int stat_split_id = -1;
static struct {
    char title[128];
    mylib_stat_getter_fn getter;
} items[MSV_STAT_MAX_ITEMS];
static int item_count = 0;

int mylib_stat_init(int split_id)
{
    stat_split_id = split_id;
    item_count = 0;
    mylib_io_clear(stat_split_id);
    return 0;
}

int mylib_stat_add(const char *title, mylib_stat_getter_fn getter)
{
    if (item_count >= MSV_STAT_MAX_ITEMS)
		return -1;
    strncpy(items[item_count].title, title, sizeof(items[item_count].title)-1);
    items[item_count].title[sizeof(items[item_count].title)-1] = '\0';
    items[item_count].getter = getter;
    item_count++;
    return 0;
}

int mylib_stat_draw(void)
{
    if (stat_split_id < 0)
		return -1;
    mylib_io_clear(stat_split_id);
    int row = 1;
    for (int i = 0; i < item_count; i++) {
        const char *val = items[i].getter ? items[i].getter() : "";
        mylib_io_print_at(stat_split_id, row, 1, "%-20s : %s", items[i].title, val);
        row++;
    }
    return 0;
}

int mylib_stat_step(int split_id)
{
    WINDOW *w = (split_id == -1) ? stdscr : mylib_sv_get_win(split_id);
    if (!w) return -1;
    werase(w);
    box(w, 0, 0);
    mylib_stat_draw();
    wnoutrefresh(w);
    return 0;
}

