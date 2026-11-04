#ifndef MSV_IO_H
#define MSV_IO_H

#include <ncurses.h>
namespace MyLib
{
	class IO
	{
		static int inside_bounds(int split_id, size_t row, size_t col);
	public:
		/**
		 * Print formatted text inside the content area of a target split window.
		 * Coordinates are relative to the inside (exclude border): (1,1) is first cell inside.
		 * Returns 0 on success, -1 on error.
		 */
		static int print_at(int split_id, size_t row, size_t col, const char *fmt, ...);
		/**
		 * Fill a rectangular area inside the target window with a character and optional color pair.
		 * row/col and height/width are inside-content coordinates.
		 * Returns 0 on success, -1 on error.
		 */
		static int fill_rect(int split_id, size_t row, size_t col, int height, int width, chtype ch, int color_pair);
		/**
		 *  Clear the inside content of the target split.
		 **/
		static int clear(int split_id);
	};

};

#endif

