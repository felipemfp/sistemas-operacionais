CC=gcc
CFLAGS=-Wall -ansi -I.
DEPS=shell.h history.h
OBJ=shell.o history.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
