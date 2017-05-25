#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <shell.h>
#include <history.h>

int main()
{
  struct History history;
  char last_command[COMMAND_SIZE + 1];
  history_init(&history);

  while (1)
  {
    int ret, rank = history_next_rank(&history);
    if (history_length(&history) == HISTORY_SIZE) {
      int i, j, l = 0, count = history.history[history_rank(&history, 0)].argc;
      for (i = 0; i < count; i++) {
        for (j = 0; j < strlen(history.history[history_rank(&history, 0)].argv[i]); j++) {
          last_command[l++] = history.history[history_rank(&history, 0)].argv[i][j];
        }
        last_command[l++] = ' ';
      }
      last_command[l] = '\0';
    }
    read_command(&(history.history[rank]));
    ret = handle_command(&(history.history[rank]), &history, last_command);
    if (ret == BREAK)
      break;
    if (ret != SKIP_HISTORY)
      history_push(&history);
  }

  return(0);
}

void print_command(struct Command * cmd)
{
  int i;
  for (i = 0; i < cmd->argc; i++)
  {
    printf("%s ", cmd->argv[i]);
  }
  printf("\n");
}

void read_command_from_string(struct Command * cmd, char command[])
{
  strcpy(cmd->command, command);
  int i = 0;
  char * token = strtok(cmd->command, " ");
  while (token != NULL)
  {
    cmd->argv[i++] = token;
    if (i == ARGV_SIZE)
      break;
    token = strtok(NULL, " ");
  }
  cmd->argv[i] = NULL;
  cmd->argc = i;
}

void read_command(struct Command * cmd)
{
  printf("$");
  fgets(cmd->command, COMMAND_SIZE, stdin);
  sscanf(cmd->command, "%[^\n]", cmd->command);
  int i = 0;
  char * token = strtok(cmd->command, " ");
  while (token != NULL)
  {
    cmd->argv[i++] = token;
    if (i == ARGV_SIZE)
      break;
    token = strtok(NULL, " ");
  }
  cmd->argv[i] = NULL;
  cmd->argc = i;
}

int handle_command(struct Command * cmd, struct History * hist, char last_command[])
{
  if (strcmp(cmd->argv[0], EXIT_COMMAND) == 0)
  {
    return BREAK;
  }
  else if (strcmp(cmd->argv[0], CD_COMMAND) == 0)
  {
    return handle_internal_command_cd(cmd, hist);
  }
  else if (strcmp(cmd->argv[0], HISTORY_COMMAND) == 0)
  {
    return handle_internal_command_history(cmd, hist, last_command);
  }
  else
  {
    return handle_external_command(cmd, hist);
  }
}


int handle_internal_command_cd(struct Command * cmd, struct History * hist)
{
  if (chdir(cmd->argv[1]) == -1)
  {
    error("%s", strerror(errno));
  }
  return 0;
}

int handle_internal_command_history(struct Command * cmd, struct History * hist, char last_command[])
{
  if (cmd->argc > 1)
  {
    if (strcmp(cmd->argv[1], "-c") == 0)
    {
      history_clear(hist);
      return SKIP_HISTORY;
    }
    else if (cmd->argv[1] != NULL)
    {
      int offset = atoi(cmd->argv[1]);
      if ((offset == 0 && strcmp(cmd->argv[1], "0") != 0) ||
          (offset < 0) ||
          (offset >= history_length(hist)))
      {
        error("%s", "offset invalid");
      }
      else
      {
        if (history_length(hist) == HISTORY_SIZE && offset == 0) {
          read_command_from_string(&(hist->history[history_next_rank(hist)]), last_command);
        }
        else {
          char command[COMMAND_SIZE + 1];
          int i, j, l = 0, count = hist->history[history_rank(hist, offset)].argc;
          for (i = 0; i < count; i++) {
            for (j = 0; j < strlen(hist->history[history_rank(hist, offset)].argv[i]); j++) {
              command[l++] = hist->history[history_rank(hist, offset)].argv[i][j];
            }
            command[l++] = ' ';
          }
          command[l] = '\0';
          read_command_from_string(&(hist->history[history_next_rank(hist)]), command);
        }

        return handle_command(&(hist->history[history_next_rank(hist)]), hist, NULL);
      }
    }
  }
  else
  {
    history_push(hist);
    int length = history_length(hist);
    if (length > 0) {
      int i;
      int start = history_start(hist);
      for (i = 0; i < length; i++)
      {
        printf("%2d ", i);
        print_command(&(hist->history[start]));
        start = (start + 1) % HISTORY_SIZE;
      }
    }
    return SKIP_HISTORY;
  }

  return 0;
}

int handle_external_command(struct Command * cmd, struct History * hist)
{
  pid_t child_pid = fork();
  if (child_pid < 0)
  {
    error("%s", "impossible to fork");
  }
  else if (child_pid == 0)
  {
    if (execvp(cmd->argv[0], cmd->argv) == -1)
      error("%s", strerror(errno));
    _exit(0);
  }
  else
  {
    int status;
    if (waitpid(child_pid, &status, 0) == -1)
    {
      error("%s", strerror(errno));
    }
    else if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
    {
      error("%s %s", cmd->argv[0], "returned a non-zero code");
    }
  }
  return 0;
}
