// mylib_sv_splitview.c — split manager with bordered windows and separators
#include "mylib_splitview.h"
#include <stdlib.h>
#include <string.h>

using namespace MyLib;

SplitViewNode::SplitViewNode(int x, int y, int w, int h, SplitViewNode::dir_t dir)
{
	_size.w = w;
	_size.h = h;
	_point.x = x;
	_point.y = y;
	_win = _create_window_rect(x, y, h, w);
	_dir = dir;
}

SplitViewNode::~SplitViewNode()
{
	delwin(_win);
}

WINDOW* MyLib::SplitViewNode::_create_window_rect(int y, int x, int h, int w)
{
    WINDOW *win = newwin(h, w, y, x);
    if (win) {
		_win = win;
        keypad(win, TRUE);
        _draw_border();
    }
    return win;
}

SplitViewNode* SplitViewNode::split(SplitViewNode::dir_t dir)
{
	int x = _point.x, y = _point.y, w = _size.w, h = _size.h;
	int new_x = _point.x, new_y = _point.y, new_w = _size.w, new_h = _size.h;
	_point.x = x;
	_point.y = y;
    if (dir == SplitViewNode::VERTICAL) {
        // left/right; add a vertical separator column
        int left_w = w / 2;
        int sep_w = 1;
        int right_w = w - left_w - sep_w;

        _size.w = left_w;
        _size.h = h;

        new_x = x + left_w + sep_w;
        new_y = y;
        new_w = right_w;
        new_h = h;
        for (int r = y; r < y + h; r++) {
            mvaddch(r, x + left_w, ACS_VLINE);
        }
    } else {
        int top_h = h / 2;
        int sep_h = 1;
        int bottom_h = h - top_h - sep_h;

        _size.w = w;
        _size.h = top_h;

        new_x = x;
        new_y = y + top_h + sep_h;
        new_w = w;
        new_h = bottom_h;

        for (int c = x; c < x + w; c++) {
            mvaddch(y + top_h, c, ACS_HLINE);
        }
    }
    refresh();
	delwin(_win);
	_dir = dir;
    _win = _create_window_rect(_point.y, _point.x, _size.h, _size.w);

	return new SplitViewNode(new_x, new_y, new_w, new_h, dir);
}

int SplitViewNode::clear()
{
    for (int r = 1; r < _size.h - 1; r++) {
        for (int c = 1; c < _size.w - 1; c++) {
            mvwaddch(_win, r, c, ' ');
        }
    }
    wrefresh(_win);
}

void SplitViewNode::redraw()
{
	box(_win, 0, 0);
	wrefresh(_win);
	if (_dir == SplitViewNode::VERTICAL) {
		int left_w = _size.w;
		for (int r = _point.y; r < _point.y + _size.h; r++)
			mvaddch(r, _point.x + left_w, ACS_VLINE);
	} else {
		int top_h = _size.h;
		for (int c = _point.x; c < _point.x + _size.w; c++)
			mvaddch(_point.y + top_h, c, ACS_HLINE);
	}
	refresh();
}

void SplitViewNode::_draw_border()
{
    box(_win, 0, 0);
    wrefresh(_win);
}

WINDOW* SplitViewNode::win() const
{
	return _win;
}

const sv_point_t& SplitViewNode::point() const
{
	return _point;
}

const sv_size_t& SplitViewNode::size() const
{
	return _size;
}

///----------------------------------------------------------------------
MyLib::SplitView::SplitView()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	start_color();

    int term_h = 0, term_w = 0;
    getmaxyx(stdscr, term_h, term_w);
	_root_node = new SplitViewNode(0, 0, term_w, term_h);
}

MyLib::SplitView::~SplitView()
{
	curs_set(1); // Возвращаем курсор
	endwin();    // Восстанавливаем терминал
}
