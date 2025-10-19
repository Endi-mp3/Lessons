#ifndef MSV_MENU_H
#define MSV_MENU_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MsvMenu MsvMenu;
typedef struct MsvMenuItem MsvMenuItem;

typedef int (*mymeny_button_callback_t)(void* pvPtr);

enum {
    MSV_MENU_RET_BTN_BACK  = -4,
    MSV_MENU_RET_BTN_START = -3,
    MSV_MENU_RET_BTN_QUIT  = -2,
    MSV_MENU_RET_ERROR     = -1,
};

typedef enum {
    MSV_MENU_ITEM_BUTTON,
    MSV_MENU_ITEM_SUBMENU,
    MSV_MENU_ITEM_INT,
    MSV_MENU_ITEM_CHECKBOX,
    MSV_MENU_ITEM_STRING
} msv_menu_item_type_t;

struct MsvMenuItem {
    msv_menu_item_type_t type;
    char *title;
    int id;
    union {
        int intValue;
        bool boolValue;
        char *strValue;
        MsvMenu *submenu;
        mymeny_button_callback_t callback;
    } data;
    MsvMenuItem *next;
};

struct MsvMenu {
    char *title;
    MsvMenuItem *items;
    MsvMenu *parent;
};

// Creation / deletion
MsvMenu *msv_menu_create(const char* title);
MsvMenu *msv_menu_create_submenu(MsvMenu* parentPtr, const char* title);
void msv_menu_delete(MsvMenu* ptr);

// Config items
int msv_menu_create_int_config(MsvMenu* parentPtr, const char* title, int defaultValue);
int msv_menu_create_checkbox(MsvMenu* parentPtr, const char* title, bool defaultValue);
int msv_menu_create_string(MsvMenu* parentPtr, const char* title, const char* defaultValue);

// Special buttons
int msv_menu_create_exit_button(MsvMenu* parentPtr, const char* title);
int msv_menu_create_start_button(MsvMenu* parentPtr, const char* title);
int msv_menu_create_button(MsvMenu* parentPtr, const char* title, mymeny_button_callback_t cb);

// Get config value by ID
int msv_menu_get_config(MsvMenu* rootPtr, int id, void* resultPtr);

// Show menu in split (blocking)
int msv_menu_show(MsvMenu* root, int split_id);

int msv_menu_step(MsvMenu **ppCurrent, int split_id);
#ifdef __cplusplus
}
#endif

#endif

