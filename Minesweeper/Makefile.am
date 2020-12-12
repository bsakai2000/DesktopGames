bin_PROGRAMS = minesweeper
EXTRA_DIST = minesweeper.6 images minesweeper.gresource.xml minesweeper.ui minesweeper.css

AM_CFLAGS = -Wall -Wno-format-truncation -Werror @GTK_CFLAGS@

minesweeper_SOURCES = minesweeper.c minesweeper_game.c minesweeper_game.h gresource.c
minesweeper_LDADD = @GTK_LIBS@

minesweeper_MANS = minesweeper.6

# https://stackoverflow.com/questions/26265836/how-to-include-resources-file-in-anjuta-project
gresource.c: minesweeper.gresource.xml $(shell glib-compile-resources --generate-dependencies minesweeper.gresource.xml)
	glib-compile-resources --target=$@ --generate $<

minesweeper.6: minesweeper.6.txt
	a2x --doctype manpage --format manpage $<

clean-local:
	rm -f gresource.c
