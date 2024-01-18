
#include <stdio.h>
#include <gtk/gtk.h>
GtkWidget *window;
GtkWidget *grid;
GtkWidget *button;
GtkWidget *edit_Field;
GtkWidget *label;
GtkWidget *list_view;
GtkListStore *list_store;


static void print_hello (GtkWidget *widget, gpointer data)
{
  gtk_label_set_text((GtkLabel *) label,"Hello World!");
}

static void print_text ()
{
  gtk_label_set_text((GtkLabel *) label, gtk_editable_get_text(GTK_EDITABLE(edit_Field)));
}


static void activate (GtkApplication *app, gpointer user_data)
{
  /* create a new window, and set its title */
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Recipe Organizer");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 150);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();
  gtk_grid_set_row_spacing (GTK_GRID (grid), 6);
  gtk_widget_set_margin_top (grid, 10);
  gtk_widget_set_margin_start (grid, 10);
  gtk_widget_set_margin_bottom(grid, 10);

  /* Pack the container in the window */
  gtk_window_set_child (GTK_WINDOW (window), grid);

  /* Place the first button in the grid cell (0, 0), and make it fill
  * just 1 cell horizontally and vertically (ie no spanning)*/
  button = gtk_button_new_with_label ("Button 1");
  g_signal_connect (button, "clicked", G_CALLBACK (print_text), NULL);
  gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

  /* Place the second button in the grid cell (1, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)*/
  button = gtk_button_new_with_label ("Button 2");
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  gtk_grid_attach (GTK_GRID(grid), button, 1, 0, 1, 1);

  /* Place the Quit button in the grid cell (0, 1), and make it
   * span 2 columns.*/
  button = gtk_button_new_with_label ("Quit");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_window_destroy), window);
  gtk_grid_attach (GTK_GRID(grid), button, 0, 1, 2, 1);

  /* Entry field */
  edit_Field = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid), edit_Field, 0, 2, 1, 1);

  /* label */
  label = gtk_label_new("Hej Verden!");
  gtk_grid_attach(GTK_GRID(grid), label, 0, 3 , 1, 1);

  GtkWidget *hpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);

  label = gtk_label_new("Whatdupp");

  GtkWidget *frame1 = gtk_frame_new (NULL);
  GtkWidget *frame2 = gtk_frame_new (NULL);

  gtk_widget_set_size_request (hpaned, 200, -1);

  gtk_paned_set_start_child (GTK_PANED (hpaned), frame1);
  gtk_paned_set_resize_start_child (GTK_PANED (hpaned), TRUE);
  gtk_paned_set_shrink_start_child (GTK_PANED (hpaned), FALSE);
  gtk_widget_set_size_request (frame1, 50, 50);

  gtk_paned_set_end_child (GTK_PANED (hpaned), frame2);
  gtk_paned_set_resize_end_child (GTK_PANED (hpaned), FALSE);
  gtk_paned_set_shrink_end_child (GTK_PANED (hpaned), FALSE);
  gtk_widget_set_size_request (frame2, 50, 50);

  gtk_grid_attach(GTK_GRID(grid), hpaned, 0, 5 , 1, 1);

  gtk_window_present (GTK_WINDOW (window));
}

int main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("gtk4_demo.htg.unord", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

