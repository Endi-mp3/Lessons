#ifndef MSV_SPLITVIEW_H
#define MSV_SPLITVIEW_H

#include <stdint.h>
#include <ncurses.h>


#ifdef __cplusplus
extern "C" {
#endif

namespace MyLib
{

	typedef struct
	{
		size_t x;  // origin column inside terminal
		size_t y;  // origin row inside terminal
	} sv_point_t;

	typedef struct
	{
		size_t w;  // width in columns
		size_t h;  // height in rows
	} sv_size_t;

	/**
	 * Initialize root split tree covering the entire terminal screen.
	 * Creates a single root window with a border.
	 * Must be called after initscr().
	 * Returns 0 on success, -1 on failure.
	 */

	class SplitViewNode
	{
		public:
			typedef enum
			{
				HORIZONTAL = 0,  // horizontal line; splits top/bottom
				VERTICAL   = 1   // vertical line; splits left/right
			} dir_t;

			SplitViewNode(int x = 0, int y = 0, int w = 0, int h = 0, SplitViewNode::dir_t dir = VERTICAL);
			SplitViewNode* split(SplitViewNode::dir_t dir);

			~SplitViewNode();

			int clear();
			void redraw();

			WINDOW* win() const;
			const sv_point_t& point() const;
			const sv_size_t& size() const;
		private:
			WINDOW *_win;
			sv_size_t _size;
			sv_point_t _point;
			dir_t _dir;
			/// Functions
			void  _draw_border();
			WINDOW* _create_window_rect(int y, int x, int h, int w);
	};

	class SplitView
	{
	private:
		SplitViewNode* _root_node;
	public:
		SplitView();
		~SplitView();
		SplitViewNode* rootNode()
		{
			return _root_node;
		}
	};
}
#ifdef __cplusplus
}
#endif

#endif

