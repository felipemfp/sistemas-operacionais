#ifndef SHELL_H
#define SHELL_H

#include "history.h"

#define EXIT_COMMAND "exit"
#define CD_COMMAND "cd"
#define HISTORY_COMMAND "history"

#define BREAK 127

void error(char * message);
void print_command(struct Command * cmd);
void read_command(struct Command * cmd);
int handle_command(struct Command * cmd, struct History * hist);

#endif
