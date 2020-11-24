CFLAGS += `pkg-config gtk+-3.0 --cflags`
CFLAGS += -g -Wall
LDLIBS += `pkg-config gtk+-3.0 --libs`

all: minesweeper

minesweeper: minesweeper.o

minesweeper.ui.min: minesweeper.ui
	tr '\n' ' ' < minesweeper.ui | sed -e 's/\t//g' -e 's/> </></g' > minesweeper.ui.min
