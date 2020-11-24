#ifndef MINESWEEPER_GAME_H
#define MINESWEEPER_GAME_H

#include <stdio.h>
#include <stdlib.h>

#define MINESWEEPER_BOMB -1

#define MAX_DIMENSION 50

typedef struct minesweeper_game
{
	int height;
	int width;
	int num_bombs;
	signed char** board;
} minesweeper_game;

/**
 * Create a new minesweeper game with the specified properties
 *
 * height		The height of the board (rows)
 * width		The width of the board (cols)
 * num_bombs		The number of bombs on the board
 */
minesweeper_game* minesweeper_new_game(int height, int width, int num_bombs);

/**
 * Free a minesweeper game and associated memory
 *
 * game			The game to free
 */
void minesweeper_free_game(minesweeper_game* game);

#endif
