#include <unistd.h>
#include "msv_menu.h"
#include <stdio.h>
#include "msv_splitview.h"
#include "msv_stat_info.h"
#include "msv_cellular.h"
#include <ncurses.h>
#include <string.h>

static const char* get_population(void) { return "256"; }
static const char* get_alive(void)      { return "128"; }
#include <ncurses.h>
#include <stdlib.h>

int on_hello(void* __attribute((unused))__pv)
{
    printf("Hello from callback %s!\n", __FUNCTION__);
    return 0;
}

int main()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);


	MsvMenu *menu = msv_menu_create("Main Menu");
	msv_menu_create_start_button(menu, "Start");

	int result = msv_menu_show(menu, -1);

    msv_init();

    int right = msv_create_split(0, MSV_SPLIT_VERTICAL);
    int bot = msv_create_split(right, MSV_SPLIT_HORIZONTAL);

    // stats
    msv_stat_init(right);
    msv_stat_add("Population", get_population);
    msv_stat_add("Alive", get_alive);
	msv_stat_draw();
	msv_menu_show(menu, bot);
    // cellular grid on left
    msv_cell_init(0, 10, 5);
    msv_cell_set(2, 2, 1, 'O', 2);
    msv_cell_set(3, 2, 1, 'X', 3);
	msv_cell_step(0);
	bool running = true;
	while(running) {
		msv_cell_step(0);
		msv_stat_step(right);
	    int st1 = msv_menu_step(&menu, bot);
		if (st1 == 1) running = false;
		if (st1 == 2) running = false;

		doupdate();
		usleep(16000);
	}

    getch();
    msv_shutdown();
    endwin();
    return 0;
}
