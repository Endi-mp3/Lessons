#ifndef MSV_CONSOLE_H
#define MSV_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mylib_cli_on_command_fn)(const char *cmd);

/**
 * Initialize console into given split ID.
 * Returns 0 on success.
 */
int mylib_cli_init(int split_id);

/**
 * Append an output line to the console view.
 */
int mylib_cli_write(const char *line);

/**
 * Run input loop; reads user input from the console split,
 * calls on_command for non-empty lines.
 * Blocks until on_command decides to exit (e.g., on "quit").
 */
int mylib_cli_run(mylib_cli_on_command_fn on_command);

int mylib_cli_step(int split_id, mylib_cli_on_command_fn on_command);
#ifdef __cplusplus
}
#endif

#endif

