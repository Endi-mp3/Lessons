#include <unistd.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include "mylib_menu.h"
#include "mylib_splitview.h"
#include "mylib_stat_info.h"
#include "mylib_game_field.h"
#include "mylib_console.h"

typedef struct
{
    int cellular_id;
    int stats_id;
    int console_id;
} GameScreen;

bool running = true;

void on_command_cb(const char *cmd)
{
    if (!cmd || !*cmd) return;

    if (strcmp(cmd, "quit") == 0) {
        mylib_cli_write("Exiting...");
        running = false;
        return;
    }

    if (strcmp(cmd, "help") == 0) {
        mylib_cli_write("Commands:");
        mylib_cli_write("  quit           - exit program");
        mylib_cli_write("  menu <name>    - switch menu (main/settings/debug)");
        mylib_cli_write("  help           - show this help");
        return;
    }

    mylib_cli_write("Unknown command. Type 'help' for list.");
}

GameScreen setup_game_screen(void)
{
    GameScreen gs = { -1, -1, -1 };
    if (mylib_sv_init() != 0) {
        return gs;
    }
	gs.cellular_id = 0;
	gs.stats_id = mylib_sv_create_split(0, MYLIB_SV_DIR_VERTICAL);
	gs.console_id = mylib_sv_create_split(gs.stats_id, MYLIB_SV_DIR_HORIZONTAL);
    return gs;
}

int on_hello(void* __attribute((unused))__pv)
{
    printf("Hello from callback %s!\n", __FUNCTION__);
    return 0;
}

int main(void)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();

    GameScreen gs = setup_game_screen();

    // инициализация модулей
    mylib_gmfld_init(gs.cellular_id, 32, 32);
    mylib_stat_init(gs.stats_id);
    mylib_cli_init(gs.console_id);

    MyLibMenu *menu = mylib_menu_create("Main");
    mylib_menu_create_start_button(menu, "Start");
    mylib_menu_create_exit_button(menu, "Quit");
    MyLibMenu *current_menu = menu;

    while (running) {
        mylib_gmfld_step(gs.cellular_id);
        mylib_stat_step(gs.stats_id);
        mylib_cli_step(gs.console_id, on_command_cb);

        doupdate();
        usleep(16000);
    }

    mylib_sv_shutdown();
    endwin();
    return 0;
}

