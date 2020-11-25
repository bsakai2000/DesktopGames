#include <stdbool.h>
#include <gtk/gtk.h>

#include "minesweeper_game.h"

// Prefix for GResource files
#define GRESOURCE_PREFIX "/com/example/DesktopGames/Minesweeper/"
#define MOUSE_PRIMARY 1
#define MOUSE_SECONDARY 3

typedef struct coordinate
{
	int x;
	int y;
} coordinate;

minesweeper_game* game = NULL;
coordinate* coords = NULL;
bool gameOver = false;

/**
 * Replace widget with the corresponding value from game
 *
 * widget		The widget to replace
 * coord		The coordinates of the cell in the grid
 */
void replace_cell(GtkWidget* widget, coordinate* coord)
{
	if(coord->x == -1)
	{
		return;
	}

	int x = coord->x; 
	int y = coord->y;

	// Get parent grid
	GtkGrid* grid = GTK_GRID(gtk_widget_get_parent(widget));	// No unref
	// Disable the button
	GtkStyleContext* context = gtk_widget_get_style_context(widget);	// No unref
	gtk_style_context_add_class(context, "clicked");
	gtk_widget_set_can_focus(widget, false);
	gtk_widget_set_sensitive(widget, false);

	// Apply correct image style
	char style_string[5] = { 0 };
	int val = game->board[y][x];
	if(val > 0)
	{
		snprintf(style_string, sizeof(style_string) - 1, "n%d", val);
		gtk_style_context_add_class(context, style_string);
	}
	if(val == -1)
	{
		gtk_style_context_add_class(context, "bomb");
		gtk_style_context_add_class(context, "exploded");
		for(int i = 0; i < game->height; ++i)
		{
			for(int j = 0; j < game->width; ++j)
			{
				if((i != y || j != x) && game->board[i][j] == MINESWEEPER_BOMB)
				{
					GtkWidget* bomb = gtk_grid_get_child_at(grid, j, i);	// No unref
					GtkStyleContext* bomb_context = gtk_widget_get_style_context(bomb);	//no unref
					gtk_style_context_add_class(bomb_context, "bomb");
					gtk_style_context_add_class(bomb_context, "clicked");
				}
			}
		}
		gameOver = true;
	}

	// Set coordinates to (-1, -1) to signal we've been hit
	coord->x = -1;
	coord->y = -1;

#if 0
	// Create a label with the string
	GtkLabel* label = GTK_LABEL(gtk_label_new(val_string));
	gtk_widget_set_visible(GTK_WIDGET(label), true);
	gtk_widget_set_hexpand(GTK_WIDGET(label), true);
	gtk_widget_set_vexpand(GTK_WIDGET(label), true);
	gtk_widget_set_halign(GTK_WIDGET(label), GTK_ALIGN_FILL);

	// Attach the label to the grid
	gtk_grid_attach(grid, GTK_WIDGET(label), x, y, 1, 1);
#endif

	if(val == 0)
	{
		CHECK_NEIGHBORS(x, y, new_x, new_y)
		{
			if(VALID_CELL(new_x, new_y, game) && coords[new_y * game->width + new_x].x != -1)
			{
				replace_cell(
						gtk_grid_get_child_at(grid, new_x, new_y),
						coords + (new_y * game->width + new_x)
					    );
			}
		}
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
	if(gameOver)
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

			// Replace the cell with a label
			replace_cell(widget, (coordinate*) user_data);

			break;
		case MOUSE_SECONDARY:
			// Toggle flagged status
			if(gtk_style_context_has_class(context, "flagged"))
			{
				gtk_style_context_remove_class(context, "flagged");
			}
			else
			{
				gtk_style_context_add_class(context, "flagged");
			}
			break;
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

void setup_ui(GtkBuilder* main_builder, int height, int width)
{
	GtkGrid* grid = GTK_GRID(gtk_builder_get_object(main_builder, "grid"));
	// Add buttons to the grid
	for(int i = 0; i < width; ++i)
	{
		for(int j = 0; j < height; ++j)
		{
			// Create new button
			GtkButton* new_button = GTK_BUTTON(gtk_button_new());	// No unref
			gtk_widget_set_visible(GTK_WIDGET(new_button), true);
			gtk_widget_set_hexpand(GTK_WIDGET(new_button), true);
			gtk_widget_set_vexpand(GTK_WIDGET(new_button), true);
			gtk_button_set_relief(new_button, GTK_RELIEF_NORMAL);

			// Set callback
			coords[j * width + i].x = i;
			coords[j * width + i].y = j;
			g_signal_connect(new_button, "button-press-event", G_CALLBACK(get_clicks), coords + (j * width + i));

			// Attach button to grid
			gtk_grid_attach(grid, GTK_WIDGET(new_button), i, j, 1, 1);
		}
	}
}

int main (int argc, char *argv[])
{
	int height = 20;
	int width  = 20;
	int num_bombs = 50;
	
	gtk_init (&argc, &argv);

	// Setup the game
	game = minesweeper_new_game(height, width, num_bombs);

	// Initialize the coordinates
	coords = calloc(height * width, sizeof(coordinate));
	
	// Load UI definition from packaged data
	GtkBuilder* main_builder = gtk_builder_new_from_resource(GRESOURCE_PREFIX "minesweeper.ui");

	// Setup the UI
	setup_ui(main_builder, height, width);

	// Load and apply CSS
	load_css();

	// Give me back execution when window closes
	GObject* window = gtk_builder_get_object (main_builder, "window");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

#if 0
	button = gtk_builder_get_object (builder, "button1");
	g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

	button = gtk_builder_get_object (builder, "button2");
	g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

	button = gtk_builder_get_object (builder, "quit");
	g_signal_connect (button, "clicked", G_CALLBACK (gtk_main_quit), NULL);
#endif

	// Start the game
	gtk_main();

	// Cleanup and return
	g_clear_object(&main_builder);
	minesweeper_free_game(game);
	return 0;
}
