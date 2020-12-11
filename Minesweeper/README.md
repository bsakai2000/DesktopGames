# Minesweeper

A simple GTK Minesweeper game based off of Windows XP Minesweeper. For
information on GTK file layout and compilation, see top-level README.md. The
Minesweeper game state is in `minesweeper_game.c`, and the GTK wrapper for
actually playing the game is in `minesweeper.c`. Image resources are in the
`images/` directory, and the manual page is `minesweeper.6`.

## TODO
 - Make the images look less bad
 - Ensure the game can be properly resized, or disable resizing
 - Remove coordinate callback system, as it seems too convoluted to be the correct solution
 - Move some general-purpose GTK functions like `load_css` to a separate module for use in other games
