#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef int (*mymenu_button_callback_t)(void* pvPtr);

typedef enum
{
    MENU_ITEM_SUBMENU,
    MENU_ITEM_CHECKBOX,
    MENU_ITEM_INT,
    MENU_ITEM_STRING,
    MENU_ITEM_BUTTON
} MenuItemType;

typedef struct MyMenuItem
{
    MenuItemType type;
    char *title;
    int id;
    union {
        int intValue;
        bool boolValue;
        char *strValue;
        struct MyMenu *submenu;
        mymenu_button_callback_t callback;  // <--- добавлено
    } data;
    struct MyMenuItem *next;
} MyMenuItem;


typedef struct MyMenu
{
    char *title;
    MyMenuItem *items;
    struct MyMenu *parent;  // для возврата назад
} MyMenu;


