// msv_console.c — single-threaded input-output console bound to a split
#include "msv_console.h"
#include "msv_splitview.h"
#include "msv_io.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define MSV_CON_MAX_LINES 256
#define MSV_CON_MAX_INPUT 256

static int con_split_id = -1;
static char *lines[MSV_CON_MAX_LINES];
static int line_count = 0;

static void redraw_console(void) {
    if (con_split_id < 0) return;
    msv_io_clear(con_split_id);
    // draw from bottom up if overflow
    int row = 1;
    for (int i = (line_count > MSV_CON_MAX_LINES ? line_count - MSV_CON_MAX_LINES : 0);
         i < line_count; i++) {
        msv_io_print_at(con_split_id, row, 1, "%s", lines[i]);
        row++;
    }
    // prompt line
    msv_io_print_at(con_split_id, row, 1, "> ");
}

int msv_console_init(int split_id) {
    con_split_id = split_id;
    for (int i = 0; i < MSV_CON_MAX_LINES; i++) lines[i] = NULL;
    line_count = 0;
    redraw_console();
    return 0;
}

int msv_console_write(const char *line) {
    if (line_count < MSV_CON_MAX_LINES) {
        lines[line_count++] = strdup(line);
    } else {
        // simple scroll: free first, shift remaining
        free(lines[0]);
        memmove(&lines[0], &lines[1], sizeof(char*) * (MSV_CON_MAX_LINES - 1));
        lines[MSV_CON_MAX_LINES - 1] = strdup(line);
    }
    redraw_console();
    return 0;
}

int msv_console_run(msv_console_on_command_fn on_command) {
    if (con_split_id < 0) return -1;
    WINDOW *w = msv_get_win(con_split_id);
    if (!w) return -1;

    char buf[MSV_CON_MAX_INPUT];
    while (1) {
        // Find prompt row = last printed row + 1
        msv_size_t sz;
        msv_get_size_id(con_split_id, &sz);
        int prompt_row = (line_count < sz.h - 2) ? (line_count + 1) : (sz.h - 2);
        msv_io_print_at(con_split_id, prompt_row, 1, "> ");
        wmove(w, prompt_row, 3);
        wclrtoeol(w);
        wrefresh(w);

        // Read input
        memset(buf, 0, sizeof(buf));
        echo();
        wgetnstr(w, buf, MSV_CON_MAX_INPUT - 1);
        noecho();

        // Dispatch
        if (buf[0] != '\0') {
            on_command(buf);
        }
    }
    return 0;
}

int msv_console_step(int split_id, msv_console_on_command_fn cb) {
    WINDOW *w = (split_id == -1) ? stdscr : msv_get_win(split_id);
    if (!w) return -1;
    keypad(w, TRUE);
    nodelay(w, TRUE);

    static char buf[256];
    static int pos = 0;

    int ch = wgetch(w);
    if (ch != ERR) {
        if (ch == '\n') {
            buf[pos] = '\0';
            if (pos > 0 && cb) cb(buf);
            pos = 0;
        } else if ((ch == KEY_BACKSPACE || ch == 127) && pos > 0) {
            pos--;
        } else if (ch >= 32 && ch < 127 && pos < (int)sizeof(buf)-1) {
            buf[pos++] = (char)ch;
        }
    }

    werase(w);
    box(w, 0, 0);
    mvwprintw(w, 1, 1, "> %.*s", pos, buf);
    wnoutrefresh(w);
    return 0;
}

