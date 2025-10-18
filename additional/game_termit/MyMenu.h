#pragma once

#include "MyMenu_Types.h"

struct MyMenu; // свободное описание
struct MyMenu *mymenu_create(const char* title);
struct MyMenu *mymenu_create_submenu(struct MyMenu* parentPtr, const char* title);
void mymenu_delete(struct MyMenu* ptr);
// если успешно создано меню, то функции mymenu_create_* возвращающие int, возвращают ID по которому можно получить значение конфигурации, иначе -1
int mymenu_create_int_config(struct MyMenu* parentPtr, const char* title, int defaultValue);
int mymenu_create_checkbox(struct MyMenu* parentPtr, const char* title, bool defaultValue);
int mymenu_create_string(struct MyMenu* parentPtr, const char* title, const char* defaultValue);
int mymenu_create_exit_button(struct MyMenu* parentPtr, const char* title);
int mymenu_create_start_button(struct MyMenu* parentPtr, const char* title);

typedef int (*mymeny_button_callback_t)(void* pvPtr);

int mymenu_create_button(struct MyMenu* parentPtr, const char* title, mymeny_button_callback_t *cb);

int mymenu_get_config(struct MyMenu* rootPtr, int id, void* resultPtr);

int mymenu_show(); // показывает меню в консоле
