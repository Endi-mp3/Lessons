// mylib_console.c — single-threaded input-output console bound to a split
#include "mylib_console.h"
#include "mylib_splitview.h"
#include "mylib_io.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define MSV_CON_MAX_LINES 256
#define MSV_CON_MAX_INPUT 256

static int con_split_id = -1;
static char *lines[MSV_CON_MAX_LINES];
static int line_count = 0;

static void redraw_console(void)
{
    if (con_split_id < 0) return;
    mylib_io_clear(con_split_id);
    // draw from bottom up if overflow
    int row = 1;
    for (int i = (line_count > MSV_CON_MAX_LINES ? line_count - MSV_CON_MAX_LINES : 0);
         i < line_count; i++) {
        mylib_io_print_at(con_split_id, row, 1, "%s", lines[i]);
        row++;
    }
    // prompt line
    mylib_io_print_at(con_split_id, row, 1, "> ");
}

int mylib_cli_init(int split_id)
{
    con_split_id = split_id;
    for (int i = 0; i < MSV_CON_MAX_LINES; i++) lines[i] = NULL;
    line_count = 0;
    redraw_console();
    return 0;
}

int mylib_cli_run(mylib_cli_on_command_fn on_command)
{
    if (con_split_id < 0) return -1;
    WINDOW *w = mylib_sv_get_win(con_split_id);
    if (!w) return -1;

    char buf[MSV_CON_MAX_INPUT];
    while (1) {
        // Find prompt row = last printed row + 1
        mylib_sv_size_t sz;
        mylib_sv_get_size_id(con_split_id, &sz);
        int prompt_row = (line_count < sz.h - 2) ? (line_count + 1) : (sz.h - 2);
        mylib_io_print_at(con_split_id, prompt_row, 1, "> ");
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

#define CLI_MAX_LINES 100
#define CLI_MAX_LEN   256

static char cli_lines[CLI_MAX_LINES][CLI_MAX_LEN];
static int cli_line_count = 0;

int mylib_cli_write(const char *line)
{
    if (cli_line_count < CLI_MAX_LINES) {
        snprintf(cli_lines[cli_line_count], CLI_MAX_LEN, "%s", line);
        cli_line_count++;
    } else {
        // сдвигаем буфер вверх
        for (int i = 1; i < CLI_MAX_LINES; i++)
            strcpy(cli_lines[i-1], cli_lines[i]);
        snprintf(cli_lines[CLI_MAX_LINES-1], CLI_MAX_LEN, "%s", line);
    }
    return 0;
}

int mylib_cli_step(int split_id, mylib_cli_on_command_fn cb)
{
    WINDOW *w = (split_id == -1) ? stdscr : mylib_sv_get_win(split_id);
    if (!w) return -1;
    keypad(w, TRUE);
    nodelay(w, TRUE);

    static char input[CLI_MAX_LEN];
    static int pos = 0;

    int ch = wgetch(w);
    if (ch != ERR) {
        if (ch == '\n') {
            input[pos] = '\0';
            if (pos > 0 && cb) cb(input);
            pos = 0;
        } else if ((ch == KEY_BACKSPACE || ch == 127) && pos > 0) {
            pos--;
        } else if (ch >= 32 && ch < 127 && pos < CLI_MAX_LEN-1) {
            input[pos++] = (char)ch;
        }
    }

    // --- Отрисовка ---
    werase(w);
    box(w, 0, 0);

    int max_y, max_x;
    getmaxyx(w, max_y, max_x);

    // выводим последние строки истории
    int start = (cli_line_count > max_y-2) ? cli_line_count - (max_y-2) : 0;
    int row = 1;
    for (int i = start; i < cli_line_count; i++, row++) {
        mvwprintw(w, row, 1, "%s", cli_lines[i]);
    }

    // строка ввода
    mvwprintw(w, max_y-2, 1, "> %.*s", pos, input);

    wnoutrefresh(w);
    return 0;
}
