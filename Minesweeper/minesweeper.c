#include <stdbool.h>
#include <gtk/gtk.h>

// Prefix for GResource files
#define GRESOURCE_PREFIX "/com/example/DesktopGames/Minesweeper/"

void print_location(GtkWidget *widget, gpointer data)
{
	int* dimensions = g_object_get_data(G_OBJECT(widget), "loc");
	printf("LEFT: %d, TOP: %d\n", dimensions[0], dimensions[1]);
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

int main (int argc, char *argv[])
{
	int height = 20;
	int width  = 20;
	
	gtk_init (&argc, &argv);

	// Load UI definition from packaged data
	GtkBuilder* main_builder = gtk_builder_new_from_resource(GRESOURCE_PREFIX "minesweeper.ui");

	GtkGrid* grid = GTK_GRID(gtk_builder_get_object(main_builder, "grid"));
	for(int i = 0; i < height; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			// Create new button
			GtkButton* new_button = GTK_BUTTON(gtk_button_new());
			gtk_widget_set_visible(GTK_WIDGET(new_button), true);
			gtk_widget_set_hexpand(GTK_WIDGET(new_button), true);
			gtk_widget_set_vexpand(GTK_WIDGET(new_button), true);
			gtk_button_set_relief(new_button, GTK_RELIEF_NONE);

			// Tell button where it is
			int* dimensions = calloc(2, sizeof(int));
			dimensions[0] = j;
			dimensions[1] = i;
			g_object_set_data(G_OBJECT(new_button), "loc", dimensions); 

			// Set callback
			g_signal_connect(new_button, "clicked", G_CALLBACK(print_location), NULL);

			// Attach button to grid
			gtk_grid_attach(grid, GTK_WIDGET(new_button), j, i, 1, 1);
		}
	}
	
	// Load and apply CSS
	load_css();


#if 0
	/* Connect signal handlers to the constructed widgets. */
	window = gtk_builder_get_object (builder, "window");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

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
	return 0;
}
