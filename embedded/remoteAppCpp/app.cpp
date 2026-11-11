#include <iostream>
#include "myLib/mylib_io.h"
#include "myLib/mylib_console.h"
#include "myLib/mylib_splitview.h"
#include <unistd.h>
using namespace MyLib;

int main(void)
{
	SplitView sv;
	SplitViewNode* svn = sv.rootNode();
	SplitViewNode* svn2 = svn->split(SplitViewNode::VERTICAL);
	SplitViewNode* svn3 = svn->split(SplitViewNode::HORIZONTAL);
	svn->redraw();
	svn2->redraw();
	svn3->redraw();
	CLI::instance()->init(svn);
	CLI::instance()->write("Hello world in console\n");
	CLI::instance()->output_step();
	CLI::instance()->render();
	svn->redraw();
	svn2->redraw();
	svn3->redraw();
	usleep(5000000);
	delete CLI::instance();
	delete svn;
	delete svn2;
	delete svn3;
	return 0;
}
