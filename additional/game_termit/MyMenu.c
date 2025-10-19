#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "MyMenu_Types.h"
#include "MyMenu.h"

static int global_id_counter = 1;

enum {
    mymenu_ret_btn_back  = -4,
    mymenu_ret_btn_start = -3,
    mymenu_ret_btn_quit  = -2,
    mymenu_ret_error     = -1,
};

// Создание корневого меню
struct MyMenu* mymenu_create(const char* title) {
    MyMenu* menu = calloc(1, sizeof(MyMenu));
    if (!menu) return NULL;
    menu->title = strdup(title);
    menu->items = NULL;
    menu->parent = NULL;

    // Добавляем кнопку "Выход"
    MyMenuItem* exitBtn = calloc(1, sizeof(MyMenuItem));
    exitBtn->type = MENU_ITEM_BUTTON;
    exitBtn->title = strdup("Quit");
    exitBtn->id = mymenu_ret_btn_quit;
    exitBtn->next = NULL;
    menu->items = exitBtn;

    return menu;
}

// Создание подменю
struct MyMenu* mymenu_create_submenu(struct MyMenu* parentPtr, const char* title) {
    if (!parentPtr) return NULL;
    MyMenu* submenu = calloc(1, sizeof(MyMenu));
    submenu->title = strdup(title);
    submenu->parent = parentPtr;

    // Добавляем кнопку "Назад"
    MyMenuItem* backBtn = calloc(1, sizeof(MyMenuItem));
    backBtn->type = MENU_ITEM_BUTTON;
    backBtn->title = strdup("Back");
    backBtn->id = mymenu_ret_btn_back;
    submenu->items = backBtn;

    // Добавляем в родительское меню ссылку на это подменю
    MyMenuItem* item = calloc(1, sizeof(MyMenuItem));
    item->type = MENU_ITEM_SUBMENU;
    item->title = strdup(title);
    item->data.submenu = submenu;
    item->next = parentPtr->items;
    parentPtr->items = item;

    return submenu;
}

// Удаление меню
void mymenu_delete(struct MyMenu* ptr) {
    if (!ptr) return;
    MyMenuItem* it = ptr->items;
    while (it) {
        MyMenuItem* next = it->next;
        free(it->title);
        if (it->type == MENU_ITEM_STRING && it->data.strValue)
            free(it->data.strValue);
        if (it->type == MENU_ITEM_SUBMENU)
            mymenu_delete(it->data.submenu);
        free(it);
        it = next;
    }
    free(ptr->title);
    free(ptr);
}

// Создание конфигурационных опций
int mymenu_create_int_config(struct MyMenu* parentPtr, const char* title, int defaultValue) {
    if (!parentPtr) return -1;
    MyMenuItem* item = calloc(1, sizeof(MyMenuItem));
    item->type = MENU_ITEM_INT;
    item->title = strdup(title);
    item->id = global_id_counter++;
    item->data.intValue = defaultValue;
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

int mymenu_create_checkbox(struct MyMenu* parentPtr, const char* title, bool defaultValue) {
    if (!parentPtr) return -1;
    MyMenuItem* item = calloc(1, sizeof(MyMenuItem));
    item->type = MENU_ITEM_CHECKBOX;
    item->title = strdup(title);
    item->id = global_id_counter++;
    item->data.boolValue = defaultValue;
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

int mymenu_create_string(struct MyMenu* parentPtr, const char* title, const char* defaultValue) {
    if (!parentPtr) return -1;
    MyMenuItem* item = calloc(1, sizeof(MyMenuItem));
    item->type = MENU_ITEM_STRING;
    item->title = strdup(title);
    item->id = global_id_counter++;
    item->data.strValue = strdup(defaultValue ? defaultValue : "");
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

// Получение значения по ID
int mymenu_get_config(struct MyMenu* rootPtr, int id, void* resultPtr) {
    if (!rootPtr) return -1;
    MyMenuItem* it = rootPtr->items;
    while (it) {
        if (it->id == id) {
            switch (it->type) {
                case MENU_ITEM_INT:
                    *(int*)resultPtr = it->data.intValue;
                    return 0;
                case MENU_ITEM_CHECKBOX:
                    *(bool*)resultPtr = it->data.boolValue;
                    return 0;
                case MENU_ITEM_STRING:
                    *(char**)resultPtr = strdup(it->data.strValue ? it->data.strValue : "");
                    return 0;
                default:
                    return -1;
            }
        }
        if (it->type == MENU_ITEM_SUBMENU) {
            if (mymenu_get_config(it->data.submenu, id, resultPtr) == 0)
                return 0;
        }
        it = it->next;
    }
    return -1;
}

// Главное отображение меню
int mymenu_show(struct MyMenu* root) {
    if (!root) return -1;

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    MyMenu* current = root;
    int choice = 0;
    int ch;

    while (1) {
        clear();
        mvprintw(0, 0, "Menu: %s", current->title);

        // вывод пунктов меню (с текущими значениями)
        int idx = 0;
        MyMenuItem* it = current->items;
        while (it) {
            if (idx == choice) attron(A_REVERSE);

            switch (it->type) {
                case MENU_ITEM_CHECKBOX:
                    mvprintw(idx + 2, 2, "[%c] %s", it->data.boolValue ? 'X' : ' ', it->title);
                    break;
                case MENU_ITEM_INT:
                    mvprintw(idx + 2, 2, "%s: %d", it->title, it->data.intValue);
                    break;
                case MENU_ITEM_STRING:
                    mvprintw(idx + 2, 2, "%s: %s", it->title, it->data.strValue ? it->data.strValue : "");
                    break;
                default:
                    mvprintw(idx + 2, 2, "%s", it->title);
                    break;
            }

            if (idx == choice) attroff(A_REVERSE);
            it = it->next;
            idx++;
        }
        refresh();

        ch = getch();
        int itemCount = 0;
        for (it = current->items; it; it = it->next) itemCount++;

        switch (ch) {
            case KEY_UP:
            case 'k':
                choice = (choice - 1 + itemCount) % itemCount;
                break;
            case KEY_DOWN:
            case 'j':
                choice = (choice + 1) % itemCount;
                break;
            case KEY_LEFT:
                // Для INT уменьшаем значение, если выбрано
                {
                    int i = 0;
                    for (it = current->items; it; it = it->next, i++) {
                        if (i == choice && it->type == MENU_ITEM_INT) {
                            it->data.intValue -= 1;
                            break;
                        }
                    }
                }
                break;
            case KEY_RIGHT:
                // Для INT увеличиваем значение, если выбрано
                {
                    int i = 0;
                    for (it = current->items; it; it = it->next, i++) {
                        if (i == choice && it->type == MENU_ITEM_INT) {
                            it->data.intValue += 1;
                            break;
                        }
                    }
                }
                break;
            case 10: // Enter
            case ' ':
            {
                int i = 0;
                for (it = current->items; it; it = it->next, i++) {
                    if (i == choice) {
                        if (it->type == MENU_ITEM_SUBMENU) {
                            current = it->data.submenu;
                            choice = 0;
                            break;
                        } else if (it->id == mymenu_ret_btn_quit) {
                            endwin();
                            return 1;
                        } else if (it->id == mymenu_ret_btn_start) {
                            endwin();
                            return 0;
                        } else if (it->id == mymenu_ret_btn_back) {
                            if (current->parent) {
                                current = current->parent;
                                choice = 0;
                                break;
                            }
                        } else if (it->type == MENU_ITEM_BUTTON && it->data.callback) {
                            it->data.callback(NULL);
                            break;
                        } else if (it->type == MENU_ITEM_CHECKBOX) {
                            it->data.boolValue = !it->data.boolValue;
                            break;
                        } else if (it->type == MENU_ITEM_INT) {
                            echo();
                            curs_set(1);
                            char buf[32];
                            mvprintw(LINES-2, 0, "Input new number: ");
                            clrtoeol();
                            getnstr(buf, sizeof(buf)-1);
                            it->data.intValue = atoi(buf);
                            noecho();
                            curs_set(0);
                            break;
                        } else if (it->type == MENU_ITEM_STRING) {
                            echo();
                            curs_set(1);
                            char buf[256];
                            mvprintw(LINES-2, 0, "Input new string: ");
                            clrtoeol();
                            getnstr(buf, sizeof(buf)-1);
                            free(it->data.strValue);
                            it->data.strValue = strdup(buf);
                            noecho();
                            curs_set(0);
                            break;
                        }
                    }
                }
                break;
            }
            case 27: // ESC
                if (current->parent) {
                    current = current->parent;
                    choice = 0;
                } else {
                    // Если корень — трактуем ESC как выход
                    endwin();
                    return 1;
                }
                break;
            default:
                break;
        }
    }

    endwin();
    return -1;
}

int mymenu_create_button(struct MyMenu* parentPtr, const char* title, mymenu_button_callback_t cb) {
    if (!parentPtr) return -1;
    MyMenuItem* item = calloc(1, sizeof(MyMenuItem));
    if (!item) return -1;

    item->type = MENU_ITEM_BUTTON;
    item->title = strdup(title);
    item->id = global_id_counter++;
    item->data.callback = cb;
    item->next = parentPtr->items;
    parentPtr->items = item;

    return item->id;
}

int mymenu_create_exit_button(struct MyMenu* parentPtr, const char* title) {
    if (!parentPtr) return -1;
    MyMenuItem* item = calloc(1, sizeof(MyMenuItem));
    item->type = MENU_ITEM_BUTTON;
    item->title = strdup(title ? title : "Quit");
    item->id = mymenu_ret_btn_quit; // специальный ID для выхода
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

int mymenu_create_start_button(struct MyMenu* parentPtr, const char* title) {
    if (!parentPtr) return -1;
    MyMenuItem* item = calloc(1, sizeof(MyMenuItem));
    item->type = MENU_ITEM_BUTTON;
    item->title = strdup(title ? title : "Start");
    item->id = mymenu_ret_btn_start; // специальный ID для старта
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

