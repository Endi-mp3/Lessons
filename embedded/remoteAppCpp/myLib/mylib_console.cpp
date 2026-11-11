// mylib_console.c — single-threaded input-output console bound to a split
#include "mylib_console.h"
#include "mylib_splitview.h"
#include "mylib_io.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

using namespace MyLib;

CLI* CLI::_instance = nullptr;

CLI* CLI::instance()
{
	if (CLI::_instance == nullptr)
		_instance = new CLI();
	return _instance;
}

CLI::CLI()
{
	_sv = nullptr;
}

void CLI::init(SplitViewNode* sv)
{
	_sv = sv;
	clear();
}

void CLI::redraw_console()
{
    if (_sv != nullptr)
		return;
	IO::clear(_sv);
    // draw from bottom up if overflow
    size_t row = 1;
    for (size_t i = (_lines.size() > max_lines ? _lines.size() - max_lines : 0); i < _lines.size(); i++) {
		IO::print_at(_sv, row, 1, "%s", _lines.at(i).c_str());
        row++;
    }
    // prompt line
	IO::print_at(_sv, row, 1, "> ");
}

void CLI::clear()
{
	_lines.clear();
    redraw_console();
}

int CLI::run(mylib_cli_on_command_fn on_command)
{
    if (_sv != nullptr)
		return -1;

    WINDOW *w = _sv->win();
    if (!w)
		return -1;

    char buf[CLI::max_length];
    while (1) {
        // Find prompt row = last printed row + 1
		sv_size_t sz = _sv->size();
        int prompt_row = (_lines.size() < sz.h - 2) ? (_lines.size() + 1) : (sz.h - 2);
		IO::print_at(_sv, prompt_row, 1, "> ");
        wmove(w, prompt_row, 3);
        wclrtoeol(w);
        wrefresh(w);

        // Read input
        memset(buf, 0, sizeof(buf));
        echo();
        wgetnstr(w, buf, max_length - 1);
        noecho();

        // Dispatch
        if (buf[0] != '\0') {
            on_command(buf);
        }
    }
    return 0;
}

void CLI::write(const std::string &line)
{
    if (_lines.size() >= max_lines) {
        // сдвигаем буфер вверх
		_lines.erase(_lines.begin());
    }
	_lines.push_back(line);
}

void CLI::write(const char *line)
{
	write(std::string(line));
}

// Render stored CLI lines to the console split window.
// Called after MYLIB_CLI_WRITE or via MYLIB_CLI_PRINT macro.
int CLI::render()
{
    if (_sv != nullptr)
		return -1;

    WINDOW *w = _sv->win();
    if (!w)
		return -1;

	MyLib::sv_size_t sz = _sv->size();

    // Clear the content area
	IO::clear(_sv);

    // Display last (max_y - 2) lines from history
    size_t start = (_lines.size() > sz.h - 2) ? _lines.size() - (sz.h - 2) : 0;
    size_t row = 1;
    for (size_t i = start; i < _lines.size() && row < sz.h - 1; i++, row++) {
		IO::print_at(_sv, row, 1, "%s", _lines[i].c_str());
    }

    // Redraw border and flush to screen
    box(w, 0, 0);
    wrefresh(w);

    return 0;
}

int CLI::step(mylib_cli_on_command_fn cb)
{
    WINDOW *w = (_sv == nullptr) ? stdscr : _sv->win();
    if (!w) return -1;
    keypad(w, TRUE);
    nodelay(w, TRUE);

	std::string input;
    static size_t pos = 0;

    int ch = wgetch(w);
    if (ch != ERR) {
        if (ch == '\n') {
            if (pos > 0 && cb) cb(input.c_str());
            pos = 0;
        } else if ((ch == KEY_BACKSPACE || ch == 127) && pos > 0) {
            pos--;
        } else if (ch >= 32 && ch < 127 && pos < max_length - 1) {
			input.insert(pos++, (const char*)&ch, 1);
        }
    }

    // --- Отрисовка ---
    werase(w);
    box(w, 0, 0);

    size_t max_y, max_x;
    getmaxyx(w, max_y, max_x);

    // выводим последние строки истории
    size_t start = (_lines.size() > max_y-2) ? _lines.size() - (max_y-2) : 0;
    size_t row = 1;
    for (size_t i = start; i < _lines.size(); i++, row++) {
        mvwprintw(w, row, 1, "%s", _lines[i].c_str());
    }

    // строка ввода
    mvwprintw(w, max_y-2, 1, "> %.*s", pos, input.c_str());

    wnoutrefresh(w);
    return 0;
}

int CLI::output_step()
{
    WINDOW *w = (_sv == nullptr) ? stdscr : _sv->win();
    if (!w) return -1;
    keypad(w, TRUE);
    nodelay(w, TRUE);

    // --- Отрисовка ---
    werase(w);
    box(w, 0, 0);

    size_t max_y, max_x;
    getmaxyx(w, max_y, max_x);

    // выводим последние строки истории
    size_t start = (_lines.size() > max_y-2) ? _lines.size() - (max_y-2) : 0;
    size_t row = 1;
    for (size_t i = start; i < _lines.size(); i++, row++) {
        mvwprintw(w, row, 1, "%s", _lines[i].c_str());
    }
    wnoutrefresh(w);
    return 0;
}
