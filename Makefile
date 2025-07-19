CC=gcc
CFLAGS=-Wall -Werror -Wextra
SOURCES=game_of_life.c
LIBS=-lncurses

all: game_of_life

game_of_life:
	$(CC) $(CFLAGS) $(SOURCES) -o $@ $(LIBS)

clean:
	rm game_of_life

rebuild: clean all

.PHONY = all clean rebuild
