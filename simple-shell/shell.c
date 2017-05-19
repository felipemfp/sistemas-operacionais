#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define COMMAND_SIZE 132
#define ARGV_SIZE 10
#define EXIT_COMMAND "exit"
#define CD_COMMAND "cd"

struct COMMAND {
  char command[COMMAND_SIZE + 1];
  char * argv[ARGV_SIZE + 1];
  int argc;
};

void error(char * message) {
  printf("error: %s\n", message);
}

void print_command(struct COMMAND cmd) {
  int i;
  for (i = 0; i < cmd.argc; i++) {
    printf("%s ", cmd.argv[i]);
  }
  printf("\n");
}

void read_command(struct COMMAND * cmd) {
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

int main() {
  struct COMMAND command;

  while (1) {
    read_command(&command);

    /*
     * handle
     */
    if (strcmp(command.argv[0], EXIT_COMMAND) == 0) break;
    else if (strcmp(command.argv[0], CD_COMMAND) == 0) {
      if (chdir(command.argv[1]) == -1) {
        error(strerror(errno));
      }
    } else {
      pid_t child_pid = fork();
      if (child_pid < 0) {
        error("can't fork");
      } else if (child_pid == 0) {
        _exit(execvp(command.argv[0], command.argv));
      } else {
        int status;
        if (waitpid(child_pid, &status, 0) == -1) {
          error(strerror(errno));
        } else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
          error("command not found");
        }
      }
    }
  }

  return(0);
}
