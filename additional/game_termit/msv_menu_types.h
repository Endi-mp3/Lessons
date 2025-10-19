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

typedef struct MsvMenuItem
{
    MenuItemType type;
    char *title;
    int id;
    union {
        int intValue;
        bool boolValue;
        char *strValue;
        struct MsvMenu *submenu;
        mymenu_button_callback_t callback;  // <--- добавлено
    } data;
    struct MsvMenuItem *next;
} MsvMenuItem;


typedef struct MsvMenu
{
    char *title;
    MsvMenuItem *items;
    struct MsvMenu *parent;  // для возврата назад
} MsvMenu;


