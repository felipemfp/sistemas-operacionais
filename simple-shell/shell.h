#ifndef SHELL_H
#define SHELL_H

#include "history.h"

#define EXIT_COMMAND       "exit"
#define CD_COMMAND         "cd"
#define HISTORY_COMMAND    "history"
#define SKIP_HISTORY       126
#define BREAK              127

#define error(fmt, args ...)    printf("error: "); printf(fmt, ## args); printf("\n");

void print_command(struct Command * cmd);
void read_command_from_string(struct Command * cmd, char command[]);
void read_command(struct Command * cmd);
int handle_command(struct Command * cmd, struct History * hist);
int handle_internal_command_cd(struct Command * cmd, struct History * hist);
int handle_internal_command_history(struct Command * cmd, struct History * hist);
int handle_external_command(struct Command * cmd, struct History * hist);

#endif
