CFLAGS += `pkg-config gtk+-3.0 --cflags`
CFLAGS += -g -Wall
LDLIBS += `pkg-config gtk+-3.0 --libs`

all: minesweeper

minesweeper: minesweeper.o minesweeper_game.o gresource.o

minesweeper.o: minesweeper.c minesweeper_game.h

minesweeper_game.o: minesweeper_game.c minesweeper_game.h

# https://stackoverflow.com/questions/26265836/how-to-include-resources-file-in-anjuta-project
gresource.c: minesweeper.gresource.xml $(shell glib-compile-resources --generate-dependencies minesweeper.gresource.xml)
	glib-compile-resources --target=$@ --generate-source $<

clean:
	rm -f *.o gresource.c minesweeper
