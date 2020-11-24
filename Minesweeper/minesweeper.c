#include <gtk/gtk.h>

// Same but for the CSS file
extern char _binary_minesweeper_css_min_start[];
extern char _binary_minesweeper_css_min_end[];

// Symbols exported by the linker so we can package the UI file with the
// executable
extern char _binary_minesweeper_ui_min_start[];
extern char _binary_minesweeper_ui_min_end[];

static void print_hello (GtkWidget *widget, gpointer data)
{
	g_print ("Hello World\n");
}

/**
 * Load css from CSS_FILE and apply it to the current screen
 */
void load_css()
{
	// Load CSS styling from packaged data
	size_t css_size = _binary_minesweeper_css_min_end - _binary_minesweeper_css_min_start;
	GtkCssProvider* css = gtk_css_provider_new();
	gtk_css_provider_load_from_data(css, _binary_minesweeper_css_min_start, css_size, NULL);

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
	GObject *main_grid;

	gtk_init (&argc, &argv);

	// Load UI definition from packaged data
	size_t ui_size = _binary_minesweeper_ui_min_end - _binary_minesweeper_ui_min_start;
	GtkBuilder* builder = gtk_builder_new_from_string(_binary_minesweeper_ui_min_start, ui_size);

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
	g_clear_object(&builder);
	return 0;
}
