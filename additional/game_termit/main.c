#include "MyMenu.h"
#include <stdio.h>


int on_hello(void* __attribute((unused))__pv)
{
    printf("Hello from callback %s!\n", __FUNCTION__);
    return 0;
}

int main()
{
    MyMenu* root = mymenu_create("Main Menu");

    int id_volume = mymenu_create_int_config(root, "Volume", 5);
    int id_music  = mymenu_create_checkbox(root, "Music", true);

    MyMenu* settings = mymenu_create_submenu(root, "Settings");
    int id_name = mymenu_create_string(settings, "Player Name", "Alex");
    mymenu_create_button(settings, "Say Hello!", on_hello);
    mymenu_create_checkbox(settings, "Music", true);
	mymenu_show(root);

    int volume;
    mymenu_get_config(root, id_volume, &volume);
    printf("Current volume: %d\n", volume);

    mymenu_delete(root);
    return 0;
}

