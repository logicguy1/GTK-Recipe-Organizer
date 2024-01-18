#include <stdio.h>
#include <gtk/gtk.h>
GtkWidget *window;
GtkWidget *grid;
GtkWidget *button;
GtkWidget *edit_Field;
GtkWidget *label;
GtkWidget *list_view;
GtkListStore *list_store;

enum
{
   TITLE_COLUMN,
   CATEGORY_COLUMN,
   DIETARRY_COLUMN,
   N_COLUMNS
};

static void populate_tree_model (GtkTreeStore *store)
{
  GtkTreeIter iter1;  /* Parent iter */
  GtkTreeIter iter2;  /* Child iter  */

  gtk_tree_store_append (store, &iter1, NULL);  /* Acquire a top-level iterator */
  gtk_tree_store_set (store, &iter1,
                      TITLE_COLUMN, "A course fit for the main plate on the table",
                      CATEGORY_COLUMN, "Main Courses",
                      DIETARRY_COLUMN, "",
                      -1);

  gtk_tree_store_append (store, &iter2, &iter1);  /* Acquire a child iterator */
  gtk_tree_store_set (store, &iter2,
                      TITLE_COLUMN, "Lasagna",
                      -1);

  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      TITLE_COLUMN, "Veggie Lasanga",
                      DIETARRY_COLUMN, "Vegitarian",
                      -1);

  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      TITLE_COLUMN, "Pita bread with falaffel",
                      DIETARRY_COLUMN, "Vegitarian",
                      -1);



  gtk_tree_store_append (store, &iter1, NULL);  /* Acquire a top-level iterator */
  gtk_tree_store_set (store, &iter1,
                      TITLE_COLUMN, "A bit for the sweet tooth",
                      CATEGORY_COLUMN, "Desserts",
                      DIETARRY_COLUMN, "",
                      -1);

  gtk_tree_store_append (store, &iter2, &iter1);  /* Acquire a child iterator */
  gtk_tree_store_set (store, &iter2,
                      TITLE_COLUMN, "Crêpes",
                      DIETARRY_COLUMN, "",
                      -1);

  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      TITLE_COLUMN, "Cookies",
                      DIETARRY_COLUMN, "",
                      -1);

  gtk_tree_store_append (store, &iter2, &iter1);
  gtk_tree_store_set (store, &iter2,
                      TITLE_COLUMN, "Tiramisu",
                      DIETARRY_COLUMN, "",
                      -1);
}


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

   GtkTreeStore *store;
   GtkWidget *tree;
   GtkTreeViewColumn *column;
   GtkCellRenderer *renderer;

   /* Create a model.  We are using the store model for now, though we
    * could use any other GtkTreeModel */
   store = gtk_tree_store_new (N_COLUMNS,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING);

   /* custom function to fill the model with data */
   populate_tree_model (store);

   /* Create a view */
   tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));

   /* The view now holds a reference.  We can get rid of our own
    * reference */
   g_object_unref (G_OBJECT (store));

   /* Create a cell render and arbitrarily make it red for demonstration
    * purposes */
   renderer = gtk_cell_renderer_text_new ();
   g_object_set (G_OBJECT (renderer),
                 "foreground", "red",
                 NULL);

   /* Create a column, associating the "text" attribute of the
    * cell_renderer to the first column of the model */
   column = gtk_tree_view_column_new_with_attributes ("Category", renderer,
                                                      "text", CATEGORY_COLUMN,
                                                      NULL);

   /* Add the column to the view. */
   gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

   /* Second column.. title of the book. */
   renderer = gtk_cell_renderer_text_new ();
   column = gtk_tree_view_column_new_with_attributes ("Recipe",
                                                      renderer,
                                                      "text", TITLE_COLUMN,
                                                      NULL);
   gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

   /* Last column.. whether a book is checked out. */
   renderer = gtk_cell_renderer_text_new ();
   column = gtk_tree_view_column_new_with_attributes ("Dietarry Requirement",
                                                      renderer,
                                                      "text", DIETARRY_COLUMN,
                                                      NULL);
   gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

  gtk_grid_attach(GTK_GRID(grid), tree, 0, 0 , 5, 5);

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

