#ifndef MINESWEEPER_GAME_H
#define MINESWEEPER_GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MINESWEEPER_BOMB -1

#define MAX_DIMENSION 50

// Represents a game of minesweeper
typedef struct minesweeper_game
{
	int height;
	int width;
	int num_bombs;
	signed char** board;
} minesweeper_game;

// List of coordinates of neighbors
extern int to_check[][2];
extern int to_check_len;

/**
 * A loop that sets (x, y) equal to the coordinates of each neighbor of (x_orig, y_orig)
 * Note that x and y are declared int, so only a name is needed, not a variable
 *
 * x_orig		X coordinate of cell
 * y_orig		Y coordinate of cell
 * x			X coordinate of neighbor
 * y			Y coordinate of neighbor
 */
#define CHECK_NEIGHBORS(x_orig, y_orig, x, y) \
	for(int neighbor_iterator = 0, \
			/* These initial values are important to avoid some nasty memory stuff*/ \
			x = x_orig + to_check[to_check_len - 1][0], \
			y = y_orig + to_check[to_check_len - 1][1]; \
			neighbor_iterator < to_check_len; \
			x = x_orig + to_check[neighbor_iterator][0], \
			y = y_orig + to_check[neighbor_iterator][1], \
			++neighbor_iterator)

/**
 * Check if coordinates point to a valid game cell
 *
 * x			X coordinate of cell
 * y			Y coordinate of cell
 * game			Game to check
 */
#define VALID_CELL(x, y, game) \
	(x >= 0 && y >= 0 && x < game->width && y < game->height)


/**
 * Create a new minesweeper game with the specified properties
 *
 * height		The height of the board (rows)
 * width		The width of the board (cols)
 * num_bombs		The number of bombs on the board
 */
minesweeper_game* minesweeper_create_game(int height, int width, int num_bombs);

/**
 * Remove all existing bombs and replace them
 *
 * game			The game to restart
 */
void minesweeper_restart_game(minesweeper_game* game);

/**
 * Free a minesweeper game and associated memory
 *
 * game			The game to free
 */
void minesweeper_free_game(minesweeper_game* game);

#endif
