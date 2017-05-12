#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_SIZE 132
#define EXIT_COMMAND "exit"

void error(char * message) {
  printf("error: %s\n", message);
  exit(1);
}

void read_command(char * command) {
  printf("$ ");
  fgets(command, COMMAND_SIZE, stdin);
  sscanf(command, "%[^\n]", command);
}

int main() {
  char command[COMMAND_SIZE + 2]; // \n + null

  while (1) {
    read_command(command);

    /*
     * handle
     */
    if (strcmp(command, EXIT_COMMAND) == 0) break;
    else error("Command not found");
  }

  return(0);
}
