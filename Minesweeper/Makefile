CFLAGS += `pkg-config gtk+-3.0 --cflags`
CFLAGS += -g -Wall
LDLIBS += `pkg-config gtk+-3.0 --libs`

all: minesweeper

minesweeper: minesweeper.o gresource.o

gresource.c: .gresource.xml $(shell glib-compile-resources --generate-dependencies .gresource.xml)
	glib-compile-resources --target=$@ --generate-source $<

# Minify by removing newlines and tabs, and making sure XML tags are touching
minesweeper.ui.min: minesweeper.ui
	tr '\n' ' ' < minesweeper.ui | sed -e 's/\t//g' -e 's/> </></g' > minesweeper.ui.min

# Minify by removing newlines and tabs
minesweeper.css.min: minesweeper.css
	tr '\n' ' ' < minesweeper.css | sed -e 's/\t//g' > minesweeper.css.min

# Package into a linkable file so it can be packaged up with the executable
minesweeper.ui.min.o: minesweeper.ui.min
	ld -r -b binary minesweeper.ui.min -o minesweeper.ui.min.o

# Package into a linkable file so it can be packaged up with the executable
minesweeper.css.min.o: minesweeper.css.min
	ld -r -b binary minesweeper.css.min -o minesweeper.css.min.o

clean:
	rm -f *.min *.o minesweeper
