#ifndef MYLIB_MENU_H
#define MYLIB_MENU_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @typedef MyLibMenu
 * @brief Main object of the module. It represents allocated pointer, contain information for view port and callbacks for actions
*/
typedef struct MyLibMenu MyLibMenu;

/**
 * @typedef MyLibMenuItem
 * @brief Inner item of the menu
 */
typedef struct MyLibMenuItem MyLibMenuItem;

/**
 * @typedef mymeny_button_callback_t
 * @brief The menu callback type (pointer to function)
 */
typedef int (*mymeny_button_callback_t)(void* pvPtr);


/**
 * @typedef enum
 * @brief Possible menu return values
 */
typedef enum {
    MYLIB_MENU_RET_BTN_BACK  = -4, ///< Back button pressed
    MYLIB_MENU_RET_BTN_START = -3, ///< Start button pressed
    MYLIB_MENU_RET_BTN_QUIT  = -2, ///< Quit button pressed
    MYLIB_MENU_RET_ERROR     = -1, ///< Error happened
	MYLIB_MENU_RET_OK		 = 0,  ///< Success
} MyLibMenuReturnCode_t;

typedef enum {
    MYLIB_MENU_ITEM_SUBMENU,	///< Item type is submenu
    MYLIB_MENU_ITEM_INT,		///< Item type is integer line edit box
    MYLIB_MENU_ITEM_CHECKBOX,	///< Item type is boolean check box
    MYLIB_MENU_ITEM_STRING,		///< Item type is string line edit box
    MYLIB_MENU_ITEM_BUTTON,		///< Item type is button
} mylib_menu_item_type_t;

struct MyLibMenuItem
{
    mylib_menu_item_type_t type;
    char *title;
    int id;
	int show_prio;	///< Integer value of drawing prio. Lower better. **Lower than zero means: not show!**
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
	MyLibMenuItem *self;
    MyLibMenuItem *items;
    MyLibMenu *parent;
};

// Creation / deletion

/**
 * @function mylib_menu_create
 * @brief Create menu without parent with title as text
 * @param	title - const char string pointer
 * @return	return pointer to MyLibMenu object or NULL pointer in case of errors
 */
MyLibMenu *mylib_menu_create(const char* title);

/**
 * @function mylib_menu_create
 * @brief Create menu without parent with title as text
 * @param	title - const char string pointer
 * @param	parentPtr - pointer to parent menu object
 * @return	return pointer to MyLibMenu object or NULL pointer in case of errors
 */
MyLibMenu *mylib_menu_create_submenu(MyLibMenu* parentPtr, const char* title);


/**
 * @function mylib_menu_delete
 * @brief	Properly remove MenuObject
 * @param	ptr - pointer to menu object
 */
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
// это чтобы получить значение из настроек, ты получаешь его в указатель в resultPtr, но не забываем конвертацию типов
int mylib_menu_get_config(MyLibMenu* rootPtr, int id, void* resultPtr);

int mylib_menu_id(MyLibMenu* menuPtr);

int mylib_menu_set_menu_priority(MyLibMenu* menuPtr, int prio);
/// return -2 in case of error
int mylib_menu_get_menu_priority(MyLibMenu* menuPtr);

int mylib_menu_set_item_priority(MyLibMenu* rootPtr, int id, int prio);
int mylib_menu_get_item_priority(MyLibMenu* rootPtr, int id, int *prioPtr);

// Show menu in split (blocking), -1 чтобы стартовать без сплита
int mylib_menu_show(MyLibMenu* root, int split_id);
int mylib_menu_step(MyLibMenu **ppCurrent, int split_id);


#ifdef __cplusplus
}
#endif

#endif

