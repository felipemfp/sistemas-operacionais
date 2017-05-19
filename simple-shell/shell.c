#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <history.h>
#include <shell.h>
#include <history.h>

void error(char * message) {
  printf("error: %s\n", message);
}

void print_command(struct Command * cmd) {
  int i;
  for (i = 0; i < cmd->argc; i++) {
    printf("%s ", cmd->argv[i]);
  }
  printf("\n");
}

void read_command(struct Command * cmd) {
  printf("$ ");
  fgets(cmd->command, COMMAND_SIZE, stdin);
  sscanf(cmd->command, "%[^\n]", cmd->command);

  int i = 0;
  char * token = strtok(cmd->command, " ");
  while (token != 0) {
    cmd->argv[i++] = token;
    if (i == ARGV_SIZE) break;
    token = strtok(0, " ");
  }

  cmd->argv[i] = NULL;
  cmd->argc = i;
}

int handle_command(struct Command * cmd, struct History * hist) {

  if (strcmp(cmd->argv[0], EXIT_COMMAND) == 0) return BREAK;
  else if (strcmp(cmd->argv[0], CD_COMMAND) == 0) {
    if (chdir(cmd->argv[1]) == -1) {
      error(strerror(errno));
    }
  } else if (strcmp(cmd->argv[0], HISTORY_COMMAND) == 0) {
    if (cmd->argc > 1) {
      if (strcmp(cmd->argv[1], "-c") == 0) {
        history_clear(hist);
      } else if (cmd->argv[1] != NULL) {
        int offset = atoi(cmd->argv[1]);
        if ((offset == 0 && strcmp(cmd->argv[1], "0") != 0) || (offset < 0) || (offset < 29)) {
          error("offset invalid");
        } else if (offset > history_length(hist)) {
          error("offset out of bounds");
        } else {
          return handle_command(&(hist->history[history_rank(hist, offset)]), hist);
        }
      }
    } else {
      int i;
      int start = history_start(hist);
      int length = history_length(hist);
      for (i = 0; i < length; i++) {
        printf("%2d. ", start);
        print_command(&(hist->history[start]));
        start = (start + 1) % HISTORY_SIZE;
      }
    }
  } else {
    pid_t child_pid = fork();
    if (child_pid < 0) {
      error("can't fork");
    } else if (child_pid == 0) {
      _exit(execvp(cmd->argv[0], cmd->argv));
    } else {
      int status;
      if (waitpid(child_pid, &status, 0) == -1) {
        error(strerror(errno));
      } else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        error("last command return a nonzero result");
      }
    }
  }

  return 0;
}

int main() {
  struct History history;
  history_init(&history);

  while (1) {
    int rank = history_end(&history);
    read_command(&(history.history[rank]));
    if (handle_command(&(history.history[rank]), &history) == BREAK) break;
    history_push(&history);
  }

  return(0);
}
