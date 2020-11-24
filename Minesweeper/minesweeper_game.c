#include "minesweeper_game.h"

/**
 * Add game->num_bombs bombs to game->board
 *
 * game		The minesweeper game to operate on
 */
static void add_bombs(minesweeper_game* game)
{
	// Fisher-Yates shuffle to generate random bomb placements in O(n)
	// https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#Modern_method
	int num_cells = game->height * game->width;
	int* to_shuffle = calloc(sizeof(int), num_cells);
	for(int i = 0; i < num_cells; ++i)
	{
		to_shuffle[i] = i;
	}
	for(int i = 0; i < num_cells; ++i)
	{
		int j = rand() % num_cells;
		int tmp = to_shuffle[i];
		to_shuffle[i] = to_shuffle[j];
		to_shuffle[j] = tmp;
	}

	// Add the bombs to the board according to our random shuffle and
	// increment its neighbors
	for(int i = 0; i < game->num_bombs; ++i)
	{
		int row = to_shuffle[i] / game->width;
		int col = to_shuffle[i] % game->width;
		game->board[row][col] = MINESWEEPER_BOMB;

		// Increment each non-bomb neighbor
		for(int y = row - 1; y <= row + 1 && y < game->height; ++y)
		{
			for(int x = col - 1; x <= col + 1 && x < game->width; ++x)
			{
				// Increment this cell if it is not a bomb
				if(x >= 0 && y >= 0 && game->board[y][x] != MINESWEEPER_BOMB)
				{
					++(game->board[y][x]);
				}
			}
		}
	}

	// Cleanup and return
	free(to_shuffle);
}

minesweeper_game* minesweeper_new_game(int height, int width, int num_bombs)
{
	// Check for reasonable bounds on the number of bombs
	if(num_bombs >= height * width || num_bombs <= 0)
	{
		printf("Invalid number of bombs!\n");
		return NULL;
	}
	// Check for reasonable bounds on dimensions
	if(height <= 0 || height > MAX_DIMENSION || width <= 0 || width > MAX_DIMENSION)
	{
		printf("Invalid dimensions!\n");
		return NULL;
	}

	// Create the game with the specified parameters
	minesweeper_game* game = calloc(1, sizeof(minesweeper_game));
	game->height = height;
	game->width = width;
	game->num_bombs = num_bombs;
	// Create the board
	game->board = calloc(height, sizeof(char*));
	for(int i = 0; i < height; ++i)
	{
		// This calloc is important - all cells MUST start with value of 0
		game->board[i] = calloc(width, sizeof(char));
	}

	// Add bombs to the game
	add_bombs(game);

	return game;
}

void minesweeper_free_game(minesweeper_game* game)
{
	for(int i = 0; i < game->height; ++i)
	{
		free(game->board[i]);
	}
	free(game->board);
	free(game);
}

#if 0
int main(int argc, char* argv[])
{
	srand(argv[1][0]);
	minesweeper_game* game = minesweeper_new_game(10, 10, 10);
	for(int i = 0; i < game->height; ++i)
	{
		for(int j = 0; j < game->width; ++j)
		printf("%d\t", game->board[i][j]);
		printf("\n");
	}

	minesweeper_free_game(game);
}
#endif
