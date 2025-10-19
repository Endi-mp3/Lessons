#include "MyMenu.h"
#include <stdio.h>

#include "mgvmgmt.h"
#include <ncurses.h>
#include <stdlib.h>

int on_hello(void* __attribute((unused))__pv)
{
    printf("Hello from callback %s!\n", __FUNCTION__);
    // Инициализация поля 256x256
    mgvmgmt_init(256, 256);

    // Закрасим диагональ для наглядности
    for (int i = 0; i < 50; i++) {
        mgvmgmt_set_cell(i, i, 1, (i % 7) + 2);
    }

    // Добавим игрока 0 в точку (5,5)
    mgvmgmt_add_player(0, 5, 5, 3);

    // Основной цикл
    int ch;
    while ((ch = getch()) != 'q') {
        switch (ch) {
            // Движение игрока стрелками
            case KEY_UP:    mgvmgmt_move_player(0, 0, -1); break;
            case KEY_DOWN:  mgvmgmt_move_player(0, 0,  1); break;
            case KEY_LEFT:  mgvmgmt_move_player(0, -1, 0); break;
            case KEY_RIGHT: mgvmgmt_move_player(0,  1, 0); break;

            // Скроллинг viewport клавишами WAS
			case 'w': mgvmgmt_scroll_viewport(0, -1); break;
			case 's': mgvmgmt_scroll_viewport(0,  1); break;
			case 'a': mgvmgmt_scroll_viewport(-1, 0); break;
			case 'd': mgvmgmt_scroll_viewport( 1, 0); break;
        }

        // Перерисовка
        mgvmgmt_draw_field();
    }

    mgvmgmt_shutdown();
    return 0;
}

int main()
{
    MyMenu* root = mymenu_create("Main Menu");

    int id_volume = mymenu_create_int_config(root, "Volume", 5);
    int id_music  = mymenu_create_checkbox(root, "Music", true);

    MyMenu* settings = mymenu_create_submenu(root, "Settings");
    int id_name = mymenu_create_string(settings, "Player Name", "Alex");
    mymenu_create_button(settings, "Start test game!", on_hello);
	mymenu_show(root);

    int volume;
    mymenu_get_config(root, id_volume, &volume);
    printf("Current volume: %d\n", volume);

    mymenu_delete(root);
    return 0;
}

