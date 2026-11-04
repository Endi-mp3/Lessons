#include <iostream>
#include "myLib/mylib_io.h"
#include "myLib/mylib_console.h"
#include "myLib/mylib_splitview.h"
#include <unistd.h>
using namespace MyLib;

int main(void)
{

	int si = mylib_sv_init();
	mylib_sv_redraw_all();
	CLI::instance()->init(si);
	CLI::instance()->write("Hello world in console\n");
	CLI::instance()->output_step();
	CLI::instance()->render();
	usleep(5000000);
	delete CLI::instance();
	mylib_sv_shutdown();
	return 0;
}
