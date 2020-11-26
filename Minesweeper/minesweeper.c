#include <stdbool.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "minesweeper_game.h"

// Prefix for GResource files
#define GRESOURCE_PREFIX "/com/example/DesktopGames/Minesweeper/"

// Mouseclick constants
#define MOUSE_PRIMARY 1
#define MOUSE_SECONDARY 3

#define DEFAULT_HEIGHT 20
#define DEFAULT_WIDTH 20
#define BOMB_DENSITY 0.05

typedef struct coordinate
{
	int x;
	int y;
} coordinate;

minesweeper_game* game = NULL;		// Holds game information
coordinate* coords = NULL;		// Holds coordinates of cells for callbacks
bool game_over = false;			// True if a bomb has been clicked
bool has_started = false;		// True if the user has taken an action
int timer = 0;				// Holds the number of seconds the game has been running
guint timer_timeout = 0;		// Identifies the timer timeout function
int flags = 0;				// The number of flags placed
int exposed_cells = 0;			// The number of cells the player has eposed

GtkBuilder* main_builder = NULL;	// Builder for the UI
GtkGrid* grid = NULL;			// Grid that holds cells
GtkLabel* timer_label = NULL;		// Label that shows time remaining
GtkLabel* bombs_label = NULL;		// Label that shows number of unflagged bombs
GtkWidget* smile = NULL;		// Smile status button

/**
 * Ends the current game, disabling all cells and exposing unflagged bombs
 *
 * did_win		True if the player won, false otherwise
 */
void end_game(bool did_win)
{
	game_over = true;

	for(int i = 0; i < game->height; ++i)
	{
		for(int j = 0; j < game->width; ++j)
		{
			GtkWidget* cell = gtk_grid_get_child_at(grid, j, i);	// No unref
			// Disable the cell
			gtk_widget_set_can_focus(cell, false);
			gtk_widget_set_sensitive(cell, false);
			// If we lost, expose unflagged bombs
			if(!did_win)
			{
				GtkStyleContext* cell_context = gtk_widget_get_style_context(cell);	//no unref
				if(game->board[i][j] == MINESWEEPER_BOMB)
				{
					// If this cell is a bomb, expose it
					gtk_style_context_add_class(cell_context, "bomb");
					gtk_style_context_add_class(cell_context, "clicked");
				}
				else if(gtk_style_context_has_class(cell_context, "flagged"))
				{
					// If this cell is a non-bomb flag, expose it as a false flag
					gtk_style_context_add_class(cell_context, "clicked");
					gtk_style_context_add_class(cell_context, "false_flag");
				}
			}
		}
	}

	// Set the smile
	GtkStyleContext* smile_context = gtk_widget_get_style_context(smile);	// No unref
	gtk_style_context_add_class(smile_context, did_win?"win":"lose");
}

/**
 * Update the timer label with the current time
 * Called every second, so we don't have to do anything tricky
 *
 * user_data		Ignored
 */
gboolean update_timer(gpointer user_data)
{
	// If the game ends, the timer stops
	if(game_over)
	{
		timer_timeout = 0;
		return G_SOURCE_REMOVE;
	}

	// Increment time
	++timer;

	// Get timer_label if we haven't yet
	if(!timer_label)
	{
		timer_label = GTK_LABEL(gtk_builder_get_object(main_builder, "timer"));	 // No unref
	}

	// Write the time to the label
	char timer_str[5] = { 0 };
	snprintf(timer_str, sizeof(timer_str) - 1, "%.3d", timer);
	gtk_label_set_text(timer_label, timer_str);

	return G_SOURCE_CONTINUE;
}

/**
 * Update the bombs label
 */
void update_bombs()
{
	// Get bombs label if we haven't yet
	if(!bombs_label)
	{
		bombs_label = GTK_LABEL(gtk_builder_get_object(main_builder, "bombs"));	 // No unref
	}

	// Write the number of remaining bombs to the label
	char bombs_str[5] = { 0 };
	snprintf(bombs_str, sizeof(bombs_str) - 1, "%.3d", MAX(game->num_bombs - flags, 0));
	gtk_label_set_text(bombs_label, bombs_str);
}

/**
 * Change the cell to display its value
 *
 * widget		The widget to replace
 * coord		The coordinates of the cell in the grid
 */
void click_cell(GtkWidget* widget, coordinate* coord)
{
	// Disable the button
	GtkStyleContext* context = gtk_widget_get_style_context(widget);	// No unref
	// If this cell has already been clicked, do nothing
	if(gtk_style_context_has_class(context, "clicked") ||
			gtk_style_context_has_class(context, "flagged"))
	{
		return;
	}
	gtk_style_context_add_class(context, "clicked");
	gtk_widget_set_can_focus(widget, false);
	gtk_widget_set_sensitive(widget, false);

	// Apply correct image style
	int val = game->board[coord->y][coord->x];
	char style_string[5] = { 0 };
	switch(val)
	{
		// If this cell has no bomb neighbors, we also expose all its neighbors
		// This occurs recursively
		case 0:

			// Iterate over all neighbors
			CHECK_NEIGHBORS(coord->x, coord->y, new_x, new_y)
			{
				// If this cell is valid and has not been visited, expose it
				if(VALID_CELL(new_x, new_y, game))
				{
					click_cell(gtk_grid_get_child_at(grid, new_x, new_y),
							coords + (new_y * game->width + new_x));
				}
			}
			break;

		// If this cell is a bomb, end the game
		case -1:
			gtk_style_context_add_class(context, "exploded");
			end_game(false);
			return;

		// If this cell has a normal value, just expose it
		default:
			snprintf(style_string, sizeof(style_string) - 1, "n%d", val);
			gtk_style_context_add_class(context, style_string);
			break;
	}

	// If we've exposed all non-bomb cells, we win
	++exposed_cells;
	if(exposed_cells == (game->height * game->width) - game->num_bombs)
	{
		end_game(true);
	}

}

/**
 * GCallback for clicked buttons
 *
 * widget		The clicked button
 * event		Information about the click
 * user_data		Used to pass location data
 */
gboolean get_clicks(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
	if(game_over)
	{
		return 1;
	}

	GtkStyleContext* context = gtk_widget_get_style_context(widget);	// No unref
	switch(((GdkEventButton*) event)->button)
	{
		case MOUSE_PRIMARY:
			// If this cell is flagged, don't take any action
			if(gtk_style_context_has_class(context, "flagged"))
			{
				return 1;
			}

			// Show the value of the cell
			click_cell(widget, (coordinate*) user_data);

			break;
		case MOUSE_SECONDARY:
			// Toggle flagged status
			if(gtk_style_context_has_class(context, "flagged"))
			{
				gtk_style_context_remove_class(context, "flagged");
				--flags;
			}
			else
			{
				gtk_style_context_add_class(context, "flagged");
				++flags;
			}

			update_bombs();
			break;
	}

	if(!has_started)
	{
		has_started = true;
		timer = 0;
		timer_timeout = g_timeout_add_seconds(1, G_SOURCE_FUNC(update_timer), NULL);
	}

	return 0;
}

/**
 * Load css from CSS_FILE and apply it to the current screen
 */
void load_css()
{
	// Load CSS styling from packaged GResource data
	GtkCssProvider* css = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(css, GRESOURCE_PREFIX "minesweeper.css");

	// Apply CSS styling to our window
	// I don't really understand this code or the Display/Screen model, but
	// it seems to style correctly so OK for now
	GdkDisplay* display = gdk_display_get_default();		// No unref
	GdkScreen* screen = gdk_display_get_default_screen(display);	// No unref
	gtk_style_context_add_provider_for_screen(screen,
			GTK_STYLE_PROVIDER(css),
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	// Cleanup and return
	g_clear_object(&css);
}

/**
 * Add the buttons and signals to the grid
 */
void setup_ui()
{
	// Add buttons to the grid
	for(int i = 0; i < game->width; ++i)
	{
		for(int j = 0; j < game->height; ++j)
		{
			// Create new button
			GtkButton* new_button = GTK_BUTTON(gtk_button_new());	// No unref
			gtk_widget_set_visible(GTK_WIDGET(new_button), true);
			gtk_widget_set_hexpand(GTK_WIDGET(new_button), true);
			gtk_widget_set_vexpand(GTK_WIDGET(new_button), true);
			gtk_button_set_relief(new_button, GTK_RELIEF_NORMAL);

			// Set callback
			coords[j * game->width + i].x = i;
			coords[j * game->width + i].y = j;
			g_signal_connect(new_button, "button-press-event", G_CALLBACK(get_clicks), coords + (j * game->width + i));

			// Attach button to grid
			gtk_grid_attach(grid, GTK_WIDGET(new_button), i, j, 1, 1);
		}
	}
}

/**
 * Restart the game, including the timer label, bombs label, game state, and
 * all cells
 *
 * button		Ignored
 * user_data		Ignored
 */
void restart_game(GtkButton* button, gpointer user_data)
{
	// Clear out the timer label
	if(timer_label)
		gtk_label_set_text(timer_label, "000");
	// Disable the timer if it's running
	if(timer_timeout)
	{
		g_source_remove(timer_timeout);
		timer_timeout = 0;
	}
	// Reset the underlying game
	minesweeper_restart_game(game);

	// Remove all cells
	for(int i = 0; i < game->width; ++i)
	{
		for(int j = 0; j < game->height; ++j)
		{
			GtkWidget* cell = gtk_grid_get_child_at(grid, i, j);
			gtk_widget_destroy(cell);
		}
	}

	// Reset the smile status button
	GtkStyleContext* smile_context = gtk_widget_get_style_context(smile);	// No unref
	gtk_style_context_remove_class(smile_context, "lose");
	gtk_style_context_remove_class(smile_context, "win");

	// Reset state variables
	game_over = false;
	has_started = false;
	timer = 0;
	flags = 0;
	exposed_cells = 0;

	// Re-add the cells
	setup_ui();
	// Reset the bomb counter
	update_bombs();
}

/**
 * Prints the help message
 */
void print_help(char* filename)
{
	printf("Usage: %s [OPTIONS]\n", filename);
	printf("Options:\n");
	printf("\t-b\tThe number of bombs to place\n");
	printf("\t-c\tThe number of columns (between 1 and %d)\n", MAX_DIMENSION);
	printf("\t-h\tPrint this help message\n");
	printf("\t-r\tThe number of rows (between 1 and %d)\n", MAX_DIMENSION);
	printf("\t-s\tThe seed to use\n");
}

int main (int argc, char *argv[])
{
	int height = DEFAULT_HEIGHT;
	int width  = DEFAULT_WIDTH;
	int num_bombs = height * width * BOMB_DENSITY;
	int seed = time(0);
	char c;

	// Get options from the 
	while((c = getopt(argc, argv, "hr:c:b:s:")) > 0)
	{
		switch(c)
		{
			case 'h':
				print_help(argv[0]);
				return 0;
			case 'r':
				height = atoi(optarg);
				break;
			case 'c':
				width = atoi(optarg);
				break;
			case 'b':
				num_bombs = atoi(optarg);
				break;
			case 's':
				seed = atoi(optarg);
				break;
			case '?':
				print_help(argv[0]);
				return 1;
		}
	}

	srand(seed);

	gtk_init (&argc, &argv);

	// Setup the game
	game = minesweeper_create_game(height, width, num_bombs);
	if(!game)
	{
		return 1;
	}

	// Initialize the coordinates
	coords = calloc(height * width, sizeof(coordinate));
	
	// Load UI definition from packaged data
	main_builder = gtk_builder_new_from_resource(GRESOURCE_PREFIX "minesweeper.ui");
	grid = GTK_GRID(gtk_builder_get_object(main_builder, "grid"));	// No unref

	smile = GTK_WIDGET(gtk_builder_get_object(main_builder, "smile"));
	g_signal_connect(G_OBJECT(smile), "clicked", G_CALLBACK(restart_game), NULL);

	// Give me back execution when window closes
	GObject* window = gtk_builder_get_object(main_builder, "window");	 // No unref
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Load and apply CSS
	load_css();

	// Setup the UI
	setup_ui();
	update_bombs();


	// Start the game
	gtk_main();

	// Cleanup and return
	g_clear_object(&main_builder);
	minesweeper_free_game(game);
	return 0;
}
