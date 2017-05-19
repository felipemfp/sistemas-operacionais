#ifndef HISTORY_H
#define HISTORY_H

#include <stdint.h>

#define HISTORY_SIZE 30
#define COMMAND_SIZE 132
#define ARGV_SIZE 10

struct Command {
  char command[COMMAND_SIZE + 1];
  char * argv[ARGV_SIZE + 1];
  int argc;
};

struct History
{
  struct Command history[HISTORY_SIZE];
  int end;
  int length;
};

void history_init(struct History * hist);
int history_length(struct History * hist);
int history_end(struct History * hist);
int history_start(struct History * hist);
void history_push(struct History * hist);
void history_clear(struct History * hist);
int history_rank(struct History * hist, int offset);

#endif
