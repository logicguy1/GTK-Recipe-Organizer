#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <sqlite3.h>

GtkWidget *window;
GtkWidget *grid;
GtkWidget *button;
GtkWidget *edit_Field;
GtkWidget *label;
GtkWidget *list_view;
GtkListStore *list_store;

// Setup tree variables
GtkTreeStore *store;
GtkWidget *tree;
GtkTreeViewColumn *column;
GtkCellRenderer *renderer;

GtkTreeIter iter1;  /* Parent iter */
GtkTreeIter iter2;  /* Child iter  */

enum
{
   TITLE_COLUMN,
   CATEGORY_COLUMN,
   DIETARRY_COLUMN,
   N_COLUMNS
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 *  DB SETUP
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

sqlite3 *db;
char *zErrMsg = 0;
int rc;
char *sql;
const char* data = "Callback function called";

static void closeDB()
{
  sqlite3_close(db);
  printf("Database closed succesfully!");
}

static int getRecipesCallback(void *data, int argc, char **argv, char **azColName){
  for (int i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }

  gtk_tree_store_append (store, &iter2, &iter1);  /* Acquire a child iterator */
  gtk_tree_store_set (store, &iter2,
                      TITLE_COLUMN, argv[1],
                      DIETARRY_COLUMN, argv[2],
                      -1);
}

static int getCatsCallback(void *data, int argc, char **argv, char **azColName){

  gtk_tree_store_append (store, &iter1, NULL);  /* Acquire a top-level iterator */
  gtk_tree_store_set (store, &iter1,
                      TITLE_COLUMN, "",
                      CATEGORY_COLUMN, argv[1],
                      DIETARRY_COLUMN, "",
                      -1);

  const char *staticSQL = "SELECT \
  recipe.id, \
  recipe.name, \
  IFNULL( \
    group_concat(dietarry_requirements.name, ', '), '' \
  ) AS req_names \
FROM \
  recipe \
  LEFT JOIN recipe_requirements_association ON recipe.id = recipe_requirements_association.recipe_id \
  LEFT JOIN dietarry_requirements ON recipe_requirements_association.requirement_id = dietarry_requirements.id \
WHERE \
	recipe.category = %s \
GROUP BY \
  recipe.id, recipe.name;";

  size_t resultLength = strlen(staticSQL) + strlen(argv[0]);
  char *sql = malloc(resultLength + 1); // +1 for the null terminator
  if (sql != NULL) {
    sprintf(sql, staticSQL, argv[0]);
  } else {
    printf("Memory allocation failed\n");
  }
  rc = sqlite3_exec(db, sql, getRecipesCallback, (void*)data, &zErrMsg);

  if( rc != SQLITE_OK ) {
    sqlite3_free(zErrMsg);
  } else {
    printf("Data retrived succesfully!\n");
  }
  free(sql);

  return 0;
}

static void getCats()
{
  sql = "SELECT id, name FROM categories;";
  rc = sqlite3_exec(db, sql, getCatsCallback, (void*)data, &zErrMsg);

  if( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    printf("Data retrived succesfully!\n");
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 *  LIST VIEW SETUP 
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

static void populate_tree_model (GtkTreeStore *store)
{

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


/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 *  MAIN ACTIVATE LOOP SETUP 
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

static void activate (GtkApplication *app, gpointer user_data)
{

  printf("Getting DB");
  rc = sqlite3_open("src/database.db", &db);

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

  /* Create a model.  We are using the store model for now, though we
   * could use any other GtkTreeModel */
  store = gtk_tree_store_new (N_COLUMNS,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING);

  /* custom function to fill the model with data */
  //populate_tree_model (store);
  getCats();

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
  printf("Helllo world");
  GtkApplication *app;
  int status;

  app = gtk_application_new ("gtk4_demo.htg.unord", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

