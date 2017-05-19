#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define COMMAND_SIZE 132
#define EXIT_COMMAND "exit"

void error(char * message) {
  printf("error: %s\n", message);
}

void read_command(char * command) {
  printf("$ ");
  fgets(command, COMMAND_SIZE, stdin);
  sscanf(command, "%[^\n]", command);
}

int main() {
  char command[COMMAND_SIZE + 1];
  pid_t child_pid;

  while (1) {
    read_command(command);

    /*
     * handle
     */
    if (strcmp(command, EXIT_COMMAND) == 0) break;
    else {
      child_pid = fork();
      if (child_pid < 0) {
        error("can't fork");
      } else if (child_pid == 0) {
        char *argv[] = { command, NULL };
        _exit(execvp(command, argv));
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
