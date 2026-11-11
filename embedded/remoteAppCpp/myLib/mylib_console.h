#ifndef MSV_CONSOLE_H
#define MSV_CONSOLE_H

#include <stdio.h>
#include <string>
#include <vector>
#include "mylib_splitview.h"

namespace MyLib
{
	typedef void (*mylib_cli_on_command_fn)(const char *cmd);

	class CLI
	{
		public:
			const unsigned int max_lines = 100;
			const unsigned int max_length = 256;

			static CLI* instance();
			void init(SplitViewNode* sv);
			void clear();
			void write(const std::string &line);
			void write(const char* line);
			int render();
			int run(mylib_cli_on_command_fn on_command);
			int step(mylib_cli_on_command_fn on_command);
			int output_step();
		protected:
			CLI();
		private:
			SplitViewNode* _sv;
			static CLI* _instance;
			std::vector<std::string> _lines;

			void redraw_console();
	};

#define MYLIB_CLI_PRINT(...) \
	{   \
		char MYLIB_CLI_PRINT_BUF[4096];	\
		snprintf(MYLIB_CLI_PRINT_BUF, 4096, __VA_ARGS__); \
		CLI::instance()->write(MYLIB_CLI_PRINT_BUF); \
		CLI::instance()->render(); \
	}


};
#endif

