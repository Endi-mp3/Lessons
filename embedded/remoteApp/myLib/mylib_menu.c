#include "mylib_splitview.h"
#include "mylib_menu.h"
#include "mylib_io.h"

#define DEFAULT_PRIORITY (0)

static int global_id_counter = 1;
static int global_max_prio = 0;

static MyLibMenuItem* s_create_libMenuItem(MyLibMenu* parentPtr, const char* title, int id, int prio, mylib_menu_item_type_t type)
{
	MyLibMenuItem* item = calloc(1, sizeof(MyLibMenuItem));
    item->type = type;
	item->id = id;
	item->prio = prio;
    item->title = strdup(title);
    item->next = parentPtr->items;
    parentPtr->items = item;
	return item;
}

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
	MyLibMenuItem* item = s_create_libMenuItem(parentPtr, title, global_id_counter++, DEFAULT_PRIORITY, MYLIB_MENU_ITEM_SUBMENU);
    item->data.submenu = submenu;
	submenu->self = item;
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
    if (!parentPtr) return MYLIB_MENU_RET_ERROR;
	MyLibMenuItem* item = s_create_libMenuItem(parentPtr,
												title,
												global_id_counter++,
												DEFAULT_PRIORITY,
												MYLIB_MENU_ITEM_INT);
    item->data.intValue = defaultValue;
    return item->id;
}

int mylib_menu_create_checkbox(MyLibMenu* parentPtr, const char* title, bool defaultValue)
{
    if (!parentPtr) return MYLIB_MENU_RET_ERROR;
	MyLibMenuItem* item = s_create_libMenuItem(parentPtr,
												title,
												global_id_counter++,
												DEFAULT_PRIORITY,
												MYLIB_MENU_ITEM_CHECKBOX);
    item->data.boolValue = defaultValue;
    return item->id;
}

int mylib_menu_create_string(MyLibMenu* parentPtr, const char* title, const char* defaultValue)
{
    if (!parentPtr) return MYLIB_MENU_RET_ERROR;
	MyLibMenuItem* item = s_create_libMenuItem(parentPtr,
												title,
												global_id_counter++,
												DEFAULT_PRIORITY,
												MYLIB_MENU_ITEM_STRING);
    item->data.strValue = strdup(defaultValue ? defaultValue : "");
    return item->id;
}

int mylib_menu_create_exit_button(MyLibMenu* parentPtr, const char* title)
{
    if (!parentPtr) return MYLIB_MENU_RET_ERROR;
	MyLibMenuItem* item = s_create_libMenuItem(parentPtr,
												title ? title : "Quit",
												MYLIB_MENU_RET_BTN_QUIT,
												DEFAULT_PRIORITY,
												MYLIB_MENU_ITEM_BUTTON);
    return item->id;
}

int mylib_menu_create_start_button(MyLibMenu* parentPtr, const char* title)
{
    if (!parentPtr) return MYLIB_MENU_RET_ERROR;
	MyLibMenuItem* item = s_create_libMenuItem(parentPtr,
												title ? title : "Start",
												MYLIB_MENU_RET_BTN_START,
												DEFAULT_PRIORITY,
												MYLIB_MENU_ITEM_BUTTON);
    return item->id;
}

int mylib_menu_create_button(MyLibMenu* parentPtr, const char* title, mylib_meny_button_callback_t cb)
{
    if (!parentPtr) return MYLIB_MENU_RET_ERROR;
	MyLibMenuItem* item = s_create_libMenuItem(parentPtr,
												title,
												global_id_counter++,
												DEFAULT_PRIORITY,
												MYLIB_MENU_ITEM_BUTTON);
    item->data.callback = cb;
    return item->id;
}

// ---------------- Config retrieval ----------------

int mylib_menu_get_config(MyLibMenu* rootPtr, int id, void* resultPtr)
{
    if (!rootPtr) return MYLIB_MENU_RET_ERROR;
    MyLibMenuItem* it = rootPtr->items;
    while (it) {
        if (it->id == id) {
            switch (it->type) {
                case MYLIB_MENU_ITEM_INT:
                    *(int*)resultPtr = it->data.intValue;
                    return MYLIB_MENU_RET_OK;
                case MYLIB_MENU_ITEM_CHECKBOX:
                    *(bool*)resultPtr = it->data.boolValue;
                    return MYLIB_MENU_RET_OK;
                case MYLIB_MENU_ITEM_STRING:
                    *(char**)resultPtr = strdup(it->data.strValue ? it->data.strValue : "");
                    return MYLIB_MENU_RET_OK;
                default:
                    return MYLIB_MENU_RET_ERROR;
            }
        }
        if (it->type == MYLIB_MENU_ITEM_SUBMENU) {
            if (mylib_menu_get_config(it->data.submenu, id, resultPtr) == 0)
                return MYLIB_MENU_RET_OK;
        }
        it = it->next;
    }
    return MYLIB_MENU_RET_ERROR;
}

WINDOW* mylib_menu_prepare_step(int split_id, bool isBlocking)
{
#ifdef MYLIB_SPLITVIEW_USED
    WINDOW *w = (split_id == -1) ? stdscr : mylib_sv_get_win(split_id);
#else
    WINDOW *w = (split_id == -1) ? stdscr : NULL;
#endif
	keypad(w, TRUE);
	nodelay(w, !isBlocking); // неблокирующий ввод
	return w;
}
// ---------------- Show menu (blocking) ----------------
int mylib_menu_show(MyLibMenu* root, int split_id)
{
	WINDOW *w = mylib_menu_prepare_step(split_id, TRUE);
    if (!root || !w)
		return MYLIB_MENU_RET_ERROR;

	MyLibMenu *ppCurrent = root;
	while(1) {
		int st = mylib_menu_step(w, &ppCurrent, split_id);
		if (st == MYLIB_MENU_RET_OK) {
			continue;
		} else {
			return st;
		}
	}
}

int mylib_menu_step(WINDOW* w, MyLibMenu **ppCurrent, int split_id)
{
    if (!ppCurrent || !*ppCurrent)
        return MYLIB_MENU_RET_ERROR;

    MyLibMenu *current = *ppCurrent;
    static int choice = 0;

    // state for inline edit mode
    static int edit_mode = 0;
    static int edit_id = -1;
    static bool edit_is_string = false;
    static char edit_buf[256];
    static int edit_pos = 0;

    if (!w)
        return MYLIB_MENU_RET_ERROR;

    // --- Edit mode: capture characters without blocking ---
    if (edit_mode) {
        if (split_id == -1) {
            clear();
            mvprintw(0, 0, "Menu: %s", current->title);
            mvprintw(LINES - 2, 0, "Input: %s", edit_buf);
        } else {
            werase(w);
            box(w, 0, 0);
            mvwprintw(w, 0, 2, " %s ", current->title);

            // draw current menu again to keep UI stable
            int idx = 0;
            int currentLastPosition = 0;
            for (int prio = 0; prio <= global_max_prio; prio++) {
                for (MyLibMenuItem* it = current->items; it; it = it->next) {
                    if (it->prio < 0 || it->prio != prio) continue;
                    it->position = currentLastPosition++;
                    if (idx == choice) wattron(w, A_REVERSE);
                    switch (it->type) {
                        case MYLIB_MENU_ITEM_CHECKBOX:
                            mylib_io_print_at(split_id, idx + 2, 2, "[%c] %s",
                                             it->data.boolValue ? 'X' : ' ', it->title);
                            break;
                        case MYLIB_MENU_ITEM_INT:
                            mylib_io_print_at(split_id, idx + 2, 2, "%s: %d",
                                             it->title, it->data.intValue);
                            break;
                        case MYLIB_MENU_ITEM_STRING:
                            mylib_io_print_at(split_id, idx + 2, 2, "%s: %s",
                                             it->title, it->data.strValue ? it->data.strValue : "");
                            break;
                        default:
                            mylib_io_print_at(split_id, idx + 2, 2, "%s", it->title);
                            break;
                    }
                    if (idx == choice) wattroff(w, A_REVERSE);
                    idx++;
                }
            }

            mylib_sv_size_t sz;
            mylib_sv_get_size_id(split_id, &sz);
            int prompt_row = (sz.h - 3 > 1) ? sz.h - 3 : 1;
            mylib_io_print_at(split_id, prompt_row, 2, "Input: %s", edit_buf);
        }

        int ch = wgetch(w);
        if (ch == ERR) {
            return MYLIB_MENU_RET_OK;
        }

        if (ch == 27) { // ESC cancel edit
            edit_mode = 0;
            edit_buf[0] = '\0';
            edit_pos = 0;
            return MYLIB_MENU_RET_OK;
        }

        if (ch == '\n' || ch == KEY_ENTER) {
            MyLibMenuItem* item = NULL;
            for (MyLibMenuItem* it = current->items; it; it = it->next) {
                if (it->id == edit_id) {
                    item = it;
                    break;
                }
            }

            if (item) {
                if (item->type == MYLIB_MENU_ITEM_INT) {
                    item->data.intValue = atoi(edit_buf);
                } else if (item->type == MYLIB_MENU_ITEM_STRING) {
                    free(item->data.strValue);
                    item->data.strValue = strdup(edit_buf);
                }
            }

            edit_mode = 0;
            edit_id = -1;
            edit_is_string = false;
            edit_buf[0] = '\0';
            edit_pos = 0;
            return MYLIB_MENU_RET_OK;
        }

        if ((ch == KEY_BACKSPACE || ch == 127) && edit_pos > 0) {
            edit_buf[--edit_pos] = '\0';
            return MYLIB_MENU_RET_OK;
        }

        if (edit_is_string) {
            if (ch >= 32 && ch < 127 && edit_pos < 255) {
                edit_buf[edit_pos++] = (char)ch;
                edit_buf[edit_pos] = '\0';
            }
        } else {
            if (((ch >= '0' && ch <= '9') || ch == '-' || ch == '+') &&
                edit_pos < 31) {
                edit_buf[edit_pos++] = (char)ch;
                edit_buf[edit_pos] = '\0';
            }
        }

        return MYLIB_MENU_RET_OK;
    }

    // --- Normal menu drawing ---
    if (split_id == -1) {
        clear();
        mvprintw(0, 0, "Menu: %s", current->title);
    } else {
        werase(w);
        box(w, 0, 0);
        mvwprintw(w, 0, 2, " %s ", current->title);
        mylib_io_clear(split_id);
        mylib_io_print_at(split_id, 0, 1, "Menu: %s", current->title);
    }

    int idx = 0;
    int currentLastPosition = 0;

    for (int prio = 0; prio <= global_max_prio; prio++) {
        for (MyLibMenuItem* it = current->items; it; it = it->next) {
            if (it->prio < 0 || it->prio != prio)
                continue;

            if (idx == choice) wattron(w, A_REVERSE);

            it->position = currentLastPosition++;

            if (split_id == -1) {
                switch (it->type) {
                    case MYLIB_MENU_ITEM_CHECKBOX:
                        mvprintw(idx + 2, 2, "[%c] %s", it->data.boolValue ? 'X' : ' ', it->title);
                        break;
                    case MYLIB_MENU_ITEM_INT:
                        mvprintw(idx + 2, 2, "%s: %d", it->title, it->data.intValue);
                        break;
                    case MYLIB_MENU_ITEM_STRING:
                        mvprintw(idx + 2, 2, "%s: %s", it->title,
                                 it->data.strValue ? it->data.strValue : "");
                        break;
                    default:
                        mvprintw(idx + 2, 2, "%s", it->title);
                        break;
                }
            } else {
                switch (it->type) {
                    case MYLIB_MENU_ITEM_CHECKBOX:
                        mylib_io_print_at(split_id, idx + 2, 2, "[%c] %s",
                                         it->data.boolValue ? 'X' : ' ', it->title);
                        break;
                    case MYLIB_MENU_ITEM_INT:
                        mylib_io_print_at(split_id, idx + 2, 2, "%s: %d",
                                         it->title, it->data.intValue);
                        break;
                    case MYLIB_MENU_ITEM_STRING:
                        mylib_io_print_at(split_id, idx + 2, 2, "%s: %s",
                                         it->title, it->data.strValue ? it->data.strValue : "");
                        break;
                    default:
                        mylib_io_print_at(split_id, idx + 2, 2, "%s", it->title);
                        break;
                }
            }

            if (idx == choice) wattroff(w, A_REVERSE);
            idx++;
        }
    }

    wrefresh(w);

    // --- Handle input ---
    int ch = wgetch(w);
    if (ch == ERR)
        return MYLIB_MENU_RET_OK;

    int itemCount = 0;
    for (MyLibMenuItem* it = current->items; it; it = it->next)
        itemCount++;

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
            for (MyLibMenuItem* it = current->items; it; it = it->next) {
                if (it->position == choice && it->type == MYLIB_MENU_ITEM_INT) {
                    it->data.intValue -= 1;
                    break;
                }
            }
            break;

        case KEY_RIGHT:
            for (MyLibMenuItem* it = current->items; it; it = it->next) {
                if (it->position == choice && it->type == MYLIB_MENU_ITEM_INT) {
                    it->data.intValue += 1;
                    break;
                }
            }
            break;

        case 10:
        case ' ':
        {
            for (MyLibMenuItem* it = current->items; it; it = it->next) {
                if (it->position == choice) {
                    if (it->type == MYLIB_MENU_ITEM_SUBMENU) {
                        current = it->data.submenu;
                        choice = 0;
                        break;
                    } else if (it->id == MYLIB_MENU_RET_BTN_QUIT) {
                        return MYLIB_MENU_RET_BTN_QUIT;
                    } else if (it->id == MYLIB_MENU_RET_BTN_START) {
                        return MYLIB_MENU_RET_BTN_START;
                    } else if (it->id == MYLIB_MENU_RET_BTN_BACK) {
                        if (current->parent) {
                            current = current->parent;
                            choice = 0;
                            break;
                        }
                    } else if (it->type == MYLIB_MENU_ITEM_BUTTON) {
                        if (it->data.callback != NULL)
                            return it->data.callback(it);
                        return it->id;
                    } else if (it->type == MYLIB_MENU_ITEM_CHECKBOX) {
                        it->data.boolValue = !it->data.boolValue;
                        break;
                    } else if (it->type == MYLIB_MENU_ITEM_INT || it->type == MYLIB_MENU_ITEM_STRING) {
                        edit_mode = 1;
                        edit_id = it->id;
                        edit_is_string = (it->type == MYLIB_MENU_ITEM_STRING);
                        edit_pos = 0;
                        memset(edit_buf, 0, sizeof(edit_buf));

                        if (edit_is_string) {
                            strncpy(edit_buf, it->data.strValue ? it->data.strValue : "", sizeof(edit_buf) - 1);
                            edit_pos = (int)strlen(edit_buf);
                        } else {
                            snprintf(edit_buf, sizeof(edit_buf), "%d", it->data.intValue);
                            edit_pos = (int)strlen(edit_buf);
                        }
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
                return MYLIB_MENU_RET_BTN_QUIT;
            }
            break;
    }

    if (current) {
        *ppCurrent = current;
    }

    return MYLIB_MENU_RET_OK;
}

int mylib_menu_set_item_priority(MyLibMenu* rootPtr, int id, int prio)
{
	MyLibMenuItem* it = rootPtr->items;
	while (it) {

		if (it->id == id) {
			if (prio > global_max_prio)
				global_max_prio = prio;
			it->prio = prio;
			return 0;
		}

		if (it->type == MYLIB_MENU_ITEM_SUBMENU &&
			mylib_menu_set_item_priority(it->data.submenu, id, prio) == 0)
			return 0;

		it = it->next;
	}
	return -1;
}

int mylib_menu_get_item_priority(MyLibMenu* rootPtr, int id, int *prioPtr)
{
	MyLibMenuItem* it = rootPtr->items;
	while (it) {

		if (it->id == id) {
			*prioPtr = it->prio;
			return 0;
		}

		if (it->type == MYLIB_MENU_ITEM_SUBMENU &&
			mylib_menu_get_item_priority(it->data.submenu, id, prioPtr) == 0)
			return 0;

		it = it->next;
	}
	return -1;
}

int mylib_menu_id(MyLibMenu* menuPtr)
{
	if (menuPtr && menuPtr->self) {
		return menuPtr->self->id;
	}
	return -1;
}

int mylib_menu_set_menu_priority(MyLibMenu* menuPtr, int prio)
{
	if (menuPtr && menuPtr->self) {
		if (prio > global_max_prio)
			global_max_prio = prio;
		menuPtr->self->prio = prio;
		return 0;
	}
	return -1;
}

int mylib_menu_get_menu_priority(MyLibMenu* menuPtr)
{
	if (menuPtr && menuPtr->self) {
		return menuPtr->self->prio;
	}
	return -2;
}


