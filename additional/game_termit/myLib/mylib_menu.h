#ifndef MYLIB_MENU_H
#define MYLIB_MENU_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MyLibMenu MyLibMenu;
typedef struct MyLibMenuItem MyLibMenuItem;

typedef int (*mymeny_button_callback_t)(void* pvPtr);

enum {
    MYLIB_MENU_RET_BTN_BACK  = -4,
    MYLIB_MENU_RET_BTN_START = -3,
    MYLIB_MENU_RET_BTN_QUIT  = -2,
    MYLIB_MENU_RET_ERROR     = -1,
};

typedef enum {
    MYLIB_MENU_ITEM_BUTTON,
    MYLIB_MENU_ITEM_SUBMENU,
    MYLIB_MENU_ITEM_INT,
    MYLIB_MENU_ITEM_CHECKBOX,
    MYLIB_MENU_ITEM_STRING
} mylib_menu_item_type_t;

struct MyLibMenuItem
{
    mylib_menu_item_type_t type;
    char *title;
    int id;
    union {
        int intValue;
        bool boolValue;
        char *strValue;
        MyLibMenu *submenu;
        mymeny_button_callback_t callback;
    } data;
    MyLibMenuItem *next;
};

struct MyLibMenu
{
    char *title;
    MyLibMenuItem *items;
    MyLibMenu *parent;
};

// Creation / deletion
MyLibMenu *mylib_menu_create(const char* title);
MyLibMenu *mylib_menu_create_submenu(MyLibMenu* parentPtr, const char* title);
void mylib_menu_delete(MyLibMenu* ptr);

// Config items
int mylib_menu_create_int_config(MyLibMenu* parentPtr, const char* title, int defaultValue);
int mylib_menu_create_checkbox(MyLibMenu* parentPtr, const char* title, bool defaultValue);
int mylib_menu_create_string(MyLibMenu* parentPtr, const char* title, const char* defaultValue);

// Special buttons
int mylib_menu_create_exit_button(MyLibMenu* parentPtr, const char* title);
int mylib_menu_create_start_button(MyLibMenu* parentPtr, const char* title);
int mylib_menu_create_button(MyLibMenu* parentPtr, const char* title, mymeny_button_callback_t cb);

// Get config value by ID
int mylib_menu_get_config(MyLibMenu* rootPtr, int id, void* resultPtr);

// Show menu in split (blocking)
int mylib_menu_show(MyLibMenu* root, int split_id);

int mylib_menu_step(MyLibMenu **ppCurrent, int split_id);
#ifdef __cplusplus
}
#endif

#endif

