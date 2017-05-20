#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "history.h"

void history_init(struct History * hist)
{
  hist->end = 0;
  hist->length = 0;
}

int history_length(struct History * hist)
{
  return hist->length;
}

int history_next_rank(struct History * hist)
{
  return hist->end;
}

int history_start(struct History * hist)
{
  return hist->length == HISTORY_SIZE ? hist->end : 0;
}

void history_push(struct History * hist)
{
  hist->end = ((hist->end) + 1) % HISTORY_SIZE;
  hist->length = hist->length < HISTORY_SIZE ? hist->length + 1 : HISTORY_SIZE;
}

void history_clear(struct History * hist)
{
  hist->end = 0;
  hist->length = 0;
}

int history_rank(struct History * hist, int offset)
{
  int start = history_start(hist);
  return (start + offset) % HISTORY_SIZE;
}
