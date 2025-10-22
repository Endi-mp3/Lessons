#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "mylib_menu.h"
#include "mylib_splitview.h"
#include "mylib_io.h"

static int global_id_counter = 1;

// ---------------- Creation / deletion ----------------

MyLibMenu *mylib_menu_create(const char* title)
{
    MyLibMenu* menu = calloc(1, sizeof(MyLibMenu));
    if (!menu) return NULL;
    menu->title = strdup(title);
    menu->items = NULL;
    menu->parent = NULL;
    return menu;
}

MyLibMenu *mylib_menu_create_submenu(MyLibMenu* parentPtr, const char* title)
{
    if (!parentPtr) return NULL;
    MyLibMenu* submenu = calloc(1, sizeof(MyLibMenu));
    submenu->title = strdup(title);
    submenu->parent = parentPtr;

    // Back button
    MyLibMenuItem* backBtn = calloc(1, sizeof(MyLibMenuItem));
    backBtn->type = MYLIB_MENU_ITEM_BUTTON;
    backBtn->title = strdup("Back");
    backBtn->id = MYLIB_MENU_RET_BTN_BACK;
    submenu->items = backBtn;

    // Add link in parent
    MyLibMenuItem* item = calloc(1, sizeof(MyLibMenuItem));
    item->type = MYLIB_MENU_ITEM_SUBMENU;
    item->title = strdup(title);
    item->data.submenu = submenu;
    item->next = parentPtr->items;
    parentPtr->items = item;

    return submenu;
}

void mylib_menu_delete(MyLibMenu* ptr)
{
    if (!ptr) return;
    MyLibMenuItem* it = ptr->items;
    while (it) {
        MyLibMenuItem* next = it->next;
        free(it->title);
        if (it->type == MYLIB_MENU_ITEM_STRING && it->data.strValue)
            free(it->data.strValue);
        if (it->type == MYLIB_MENU_ITEM_SUBMENU)
            mylib_menu_delete(it->data.submenu);
        free(it);
        it = next;
    }
    free(ptr->title);
    free(ptr);
}

// ---------------- Item creation ----------------

int mylib_menu_create_int_config(MyLibMenu* parentPtr, const char* title, int defaultValue)
{
    if (!parentPtr) return -1;
    MyLibMenuItem* item = calloc(1, sizeof(MyLibMenuItem));
    item->type = MYLIB_MENU_ITEM_INT;
    item->title = strdup(title);
    item->id = global_id_counter++;
    item->data.intValue = defaultValue;
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

int mylib_menu_create_checkbox(MyLibMenu* parentPtr, const char* title, bool defaultValue)
{
    if (!parentPtr) return -1;
    MyLibMenuItem* item = calloc(1, sizeof(MyLibMenuItem));
    item->type = MYLIB_MENU_ITEM_CHECKBOX;
    item->title = strdup(title);
    item->id = global_id_counter++;
    item->data.boolValue = defaultValue;
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

int mylib_menu_create_string(MyLibMenu* parentPtr, const char* title, const char* defaultValue)
{
    if (!parentPtr) return -1;
    MyLibMenuItem* item = calloc(1, sizeof(MyLibMenuItem));
    item->type = MYLIB_MENU_ITEM_STRING;
    item->title = strdup(title);
    item->id = global_id_counter++;
    item->data.strValue = strdup(defaultValue ? defaultValue : "");
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

int mylib_menu_create_exit_button(MyLibMenu* parentPtr, const char* title)
{
    if (!parentPtr) return -1;
    MyLibMenuItem* item = calloc(1, sizeof(MyLibMenuItem));
    item->type = MYLIB_MENU_ITEM_BUTTON;
    item->title = strdup(title ? title : "Quit");
    item->id = MYLIB_MENU_RET_BTN_QUIT;
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

int mylib_menu_create_start_button(MyLibMenu* parentPtr, const char* title)
{
    if (!parentPtr) return -1;
    MyLibMenuItem* item = calloc(1, sizeof(MyLibMenuItem));
    item->type = MYLIB_MENU_ITEM_BUTTON;
    item->title = strdup(title ? title : "Start");
    item->id = MYLIB_MENU_RET_BTN_START;
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

int mylib_menu_create_button(MyLibMenu* parentPtr, const char* title, mymeny_button_callback_t cb)
{
    if (!parentPtr) return -1;
    MyLibMenuItem* item = calloc(1, sizeof(MyLibMenuItem));
    item->type = MYLIB_MENU_ITEM_BUTTON;
    item->title = strdup(title);
    item->id = global_id_counter++;
    item->data.callback = cb;
    item->next = parentPtr->items;
    parentPtr->items = item;
    return item->id;
}

// ---------------- Config retrieval ----------------

int mylib_menu_get_config(MyLibMenu* rootPtr, int id, void* resultPtr)
{
    if (!rootPtr) return -1;
    MyLibMenuItem* it = rootPtr->items;
    while (it) {
        if (it->id == id) {
            switch (it->type) {
                case MYLIB_MENU_ITEM_INT:
                    *(int*)resultPtr = it->data.intValue;
                    return 0;
                case MYLIB_MENU_ITEM_CHECKBOX:
                    *(bool*)resultPtr = it->data.boolValue;
                    return 0;
                case MYLIB_MENU_ITEM_STRING:
                    *(char**)resultPtr = strdup(it->data.strValue ? it->data.strValue : "");
                    return 0;
                default:
                    return -1;
            }
        }
        if (it->type == MYLIB_MENU_ITEM_SUBMENU) {
            if (mylib_menu_get_config(it->data.submenu, id, resultPtr) == 0)
                return 0;
        }
        it = it->next;
    }
    return -1;
}

// ---------------- Show menu (blocking) ----------------
int mylib_menu_show(MyLibMenu* root, int split_id)
{
    if (!root) return -1;

    MyLibMenu* current = root;
    int choice = 0;
    int ch;

    WINDOW *w = (split_id == -1) ? stdscr : mylib_sv_get_win(split_id);
    if (!w) return -1;
    keypad(w, TRUE);
    nodelay(w, FALSE);

    while (1) {
        // Очистка и заголовок
        if (split_id == -1) {
            clear();
            mvprintw(0, 0, "Menu: %s", current->title);
        } else {
            mylib_io_clear(split_id);
            mylib_io_print_at(split_id, 0, 1, "Menu: %s", current->title);
        }

        // Отрисовка пунктов
        int idx = 0;
        MyLibMenuItem* it = current->items;
        while (it) {
            if (idx == choice) wattron(w, A_REVERSE);

            if (split_id == -1) {
                switch (it->type) {
                    case MYLIB_MENU_ITEM_CHECKBOX:
                        mvprintw(idx+2, 2, "[%c] %s", it->data.boolValue?'X':' ', it->title);
                        break;
                    case MYLIB_MENU_ITEM_INT:
                        mvprintw(idx+2, 2, "%s: %d", it->title, it->data.intValue);
                        break;
                    case MYLIB_MENU_ITEM_STRING:
                        mvprintw(idx+2, 2, "%s: %s", it->title,
                                 it->data.strValue?it->data.strValue:"");
                        break;
                    default:
                        mvprintw(idx+2, 2, "%s", it->title);
                        break;
                }
            } else {
                switch (it->type) {
                    case MYLIB_MENU_ITEM_CHECKBOX:
                        mylib_io_print_at(split_id, idx+2, 2, "[%c] %s",
                                        it->data.boolValue?'X':' ', it->title);
                        break;
                    case MYLIB_MENU_ITEM_INT:
                        mylib_io_print_at(split_id, idx+2, 2, "%s: %d",
                                        it->title, it->data.intValue);
                        break;
                    case MYLIB_MENU_ITEM_STRING:
                        mylib_io_print_at(split_id, idx+2, 2, "%s: %s",
                                        it->title, it->data.strValue?it->data.strValue:"");
                        break;
                    default:
                        mylib_io_print_at(split_id, idx+2, 2, "%s", it->title);
                        break;
                }
            }

            if (idx == choice) wattroff(w, A_REVERSE);
            it = it->next;
            idx++;
        }

        wrefresh(w);

        // Ввод
        ch = wgetch(w);
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
                {
                    int i = 0;
                    for (it = current->items; it; it = it->next, i++) {
                        if (i == choice && it->type == MYLIB_MENU_ITEM_INT) {
                            it->data.intValue -= 1;
                            break;
                        }
                    }
                }
                break;
            case KEY_RIGHT:
                {
                    int i = 0;
                    for (it = current->items; it; it = it->next, i++) {
                        if (i == choice && it->type == MYLIB_MENU_ITEM_INT) {
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
                            if (it->type == MYLIB_MENU_ITEM_SUBMENU) {
                                current = it->data.submenu;
                                choice = 0;
                                break;
                            } else if (it->id == MYLIB_MENU_RET_BTN_QUIT) {
                                return 1;
                            } else if (it->id == MYLIB_MENU_RET_BTN_START) {
                                return 0;
                            } else if (it->id == MYLIB_MENU_RET_BTN_BACK) {
                                if (current->parent) {
                                    current = current->parent;
                                    choice = 0;
                                    break;
                                }
                            } else if (it->type == MYLIB_MENU_ITEM_BUTTON && it->data.callback) {
                                it->data.callback(NULL);
                                break;
                            } else if (it->type == MYLIB_MENU_ITEM_CHECKBOX) {
                                it->data.boolValue = !it->data.boolValue;
                                break;
                            } else if (it->type == MYLIB_MENU_ITEM_INT) {
                                echo();
                                curs_set(1);
                                char buf[32];
                                if (split_id == -1) {
                                    mvprintw(LINES-2, 0, "Input new number: ");
                                    getnstr(buf, sizeof(buf)-1);
                                } else {
                                    mylib_io_print_at(split_id, idx+4, 2, "Input new number: ");
                                    wgetnstr(w, buf, sizeof(buf)-1);
                                }
                                it->data.intValue = atoi(buf);
                                noecho();
                                curs_set(0);
                                break;
                            } else if (it->type == MYLIB_MENU_ITEM_STRING) {
                                echo();
                                curs_set(1);
                                char buf[256];
                                if (split_id == -1) {
                                    mvprintw(LINES-2, 0, "Input new string: ");
                                    getnstr(buf, sizeof(buf)-1);
                                } else {
                                    mylib_io_print_at(split_id, idx+4, 2, "Input new string: ");
                                    wgetnstr(w, buf, sizeof(buf)-1);
                                }
                                free(it->data.strValue);
                                it->data.strValue = strdup(buf);
                                noecho();
                                curs_set(0);
                                break;
                            }
                        }
                    }
                }
                break;
            case 27: // ESC
                if (current->parent) {
                    current = current->parent;
                    choice = 0;
                } else {
                    return 1;
                }
                break;
            default:
                break;
        }
    }
    return -1;
}

int mylib_menu_step(MyLibMenu **ppCurrent, int split_id)
{
    if (!ppCurrent || !*ppCurrent) return -1;
    MyLibMenu *current = *ppCurrent;
    static int choice = 0;

    WINDOW *w = (split_id == -1) ? stdscr : mylib_sv_get_win(split_id);
    if (!w) return -1;
    keypad(w, TRUE);
    nodelay(w, TRUE); // неблокирующий ввод

    // --- Отрисовка ---
    if (split_id == -1) {
        clear();
        mvprintw(0, 0, "Menu: %s", current->title);
    } else {
		werase(w);
		box(w, 0, 0);
		mvwprintw(w, 0, 2, " %s ", current->title);
        // mylib_io_clear(split_id);
        mylib_io_print_at(split_id, 0, 1, "Menu: %s", current->title);
    }

    int idx = 0;
    for (MyLibMenuItem* it = current->items; it; it = it->next, idx++) {
        if (idx == choice) wattron(w, A_REVERSE);

        if (split_id == -1) {
            switch (it->type) {
                case MYLIB_MENU_ITEM_CHECKBOX:
                    mvprintw(idx+2, 2, "[%c] %s", it->data.boolValue?'X':' ', it->title);
                    break;
                case MYLIB_MENU_ITEM_INT:
                    mvprintw(idx+2, 2, "%s: %d", it->title, it->data.intValue);
                    break;
                case MYLIB_MENU_ITEM_STRING:
                    mvprintw(idx+2, 2, "%s: %s", it->title,
                             it->data.strValue?it->data.strValue:"");
                    break;
                default:
                    mvprintw(idx+2, 2, "%s", it->title);
                    break;
            }
        } else {
            switch (it->type) {
                case MYLIB_MENU_ITEM_CHECKBOX:
                    mylib_io_print_at(split_id, idx+2, 2, "[%c] %s",
                                    it->data.boolValue?'X':' ', it->title);
                    break;
                case MYLIB_MENU_ITEM_INT:
                    mylib_io_print_at(split_id, idx+2, 2, "%s: %d",
                                    it->title, it->data.intValue);
                    break;
                case MYLIB_MENU_ITEM_STRING:
                    mylib_io_print_at(split_id, idx+2, 2, "%s: %s",
                                    it->title, it->data.strValue?it->data.strValue:"");
                    break;
                default:
                    mylib_io_print_at(split_id, idx+2, 2, "%s", it->title);
                    break;
            }
        }

        if (idx == choice) wattroff(w, A_REVERSE);
    }
    wrefresh(w);

    // --- Обработка ввода ---
    int ch = wgetch(w);
    if (ch == ERR) return 0; // нет ввода

    int itemCount = 0;
    for (MyLibMenuItem* it = current->items; it; it = it->next) itemCount++;

    switch (ch) {
        case KEY_UP: case 'k':
            choice = (choice - 1 + itemCount) % itemCount;
            break;
        case KEY_DOWN: case 'j':
            choice = (choice + 1) % itemCount;
            break;
        case 10: case ' ': // Enter/Space
        {
            int i=0;
            for (MyLibMenuItem* it=current->items; it; it=it->next,i++) {
                if (i==choice) {
                    if (it->id == MYLIB_MENU_RET_BTN_QUIT)
						return 1;
                    if (it->id == MYLIB_MENU_RET_BTN_START)
						return 2;
                    if (it->id == MYLIB_MENU_RET_BTN_BACK && current->parent) {
                        *ppCurrent = current->parent;
                        choice = 0;
                        return 0;
                    }
                    if (it->type == MYLIB_MENU_ITEM_SUBMENU) {
                        *ppCurrent = it->data.submenu;
                        choice = 0;
                        return 0;
                    }
                    if (it->type == MYLIB_MENU_ITEM_CHECKBOX)
                        it->data.boolValue = !it->data.boolValue;
                }
            }
        }
        break;
        case 27: // ESC
            if (current->parent) {
                *ppCurrent = current->parent;
                choice = 0;
            } else return 1;
            break;
    }
    return 0;
}

