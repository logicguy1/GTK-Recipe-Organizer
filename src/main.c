#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <sqlite3.h>

GtkWidget *window;
GtkWidget *grid;
GtkWidget *innerGrid;
GtkWidget *button;
GtkWidget *edit_Field;
GtkWidget *label;
GtkWidget *list_view;
GtkListStore *list_store;
GtkWidget *title;

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
  recipe.id, recipe.name \
ORDER BY recipe.name;";

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
  sql = "SELECT id, name FROM categories ORDER BY name;";
  rc = sqlite3_exec(db, sql, getCatsCallback, (void*)data, &zErrMsg);

  if( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    printf("Data retrived succesfully!\n");
  }
}

static int getSpecificRecipeCallback(void *data, int argc, char **argv, char **azColName){
  for (int i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }

  // gtk_label_set_text((GtkLabel *) title, argv[1]);
  const char *template = "<span size='x-large'>%s</span>";
  char formatted[286]; // Define buffer to hold the formatted SQL string

  // Format the SQL query string with the row and column offsets
  snprintf(formatted, sizeof(formatted), template, argv[1]);
  gtk_label_set_markup(GTK_LABEL(title), formatted);

  return 0;
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

static void
activate_cb(GtkTreeView* tree_view, gpointer user_data)
{
  printf("Working\n");
  GtkTreePath *path;
  GtkTreeViewColumn *focus_column;

  // Get the cursor path and focus column
  gtk_tree_view_get_cursor(tree_view, &path, &focus_column);

  // Check if cursor is set
  if (path != NULL) {
    gchar *path_string = gtk_tree_path_to_string(path);
    g_print("Cursor Path: %s\n", path_string);
    // Split the path string into row and column indices
    gchar **tokens = g_strsplit(path_string, ":", 2);
    if (tokens != NULL && tokens[0] != NULL && tokens[1] != NULL) {
      gint row = atoi(tokens[0]);
      gint column = atoi(tokens[1]);
      g_print("Row: %d, Column: %d\n", row, column);

      // Define the SQL query template with placeholders
      const char *sql_template = "SELECT id, name FROM recipe WHERE recipe.category = (SELECT id FROM categories ORDER BY name LIMIT 1 OFFSET %d) ORDER BY name LIMIT 1 OFFSET %d;";
      char formatted_sql[286]; // Define buffer to hold the formatted SQL string

      // Format the SQL query string with the row and column offsets
      snprintf(formatted_sql, sizeof(formatted_sql), sql_template, row, column);

      // Print the formatted SQL query
      printf("Formatted SQL Query: %s\n", formatted_sql);

      rc = sqlite3_exec(db, formatted_sql, getSpecificRecipeCallback, (void*)data, &zErrMsg);

      if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
      } else {
        printf("Data retrived succesfully!\n");
      }
    }

    g_strfreev(tokens);

    // Free the path since it's been used
    g_free(path_string);

    // Free the path since it's been used
    gtk_tree_path_free(path);
  } else {
    g_print("Cursor is not set.\n");
  }

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 *  MAIN ACTIVATE LOOP SETUP 
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

static void activate (GtkApplication *app, gpointer user_data)
{

  printf("Getting DB\n");
  rc = sqlite3_open("src/database.db", &db);

  /* create a new window, and set its title */
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Recipe Organizer");
  gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
  gtk_window_set_resizable(GTK_WINDOW(window), false);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID (grid), 6);
  gtk_widget_set_margin_top(grid, 0);
  gtk_widget_set_margin_start(grid, 0);
  gtk_widget_set_margin_bottom(grid, 0);

  /* Pack the container in the window */
  gtk_window_set_child(GTK_WINDOW (window), grid);

  /* Create a model.  We are using the store model for now, though we
   * could use any other GtkTreeModel */
  store = gtk_tree_store_new(N_COLUMNS,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING);

  /* custom function to fill the model with data */
  //populate_tree_model (store);
  getCats();

  /* Create a view */
  tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL (store));

  gtk_widget_set_size_request(GTK_TREE_VIEW(tree), 500, 800);


  /* The view now holds a reference.  We can get rid of our own
   * reference */
  g_object_unref(G_OBJECT (store));

  /* Create a cell render and arbitrarily make it red for demonstration
   * purposes */
  renderer = gtk_cell_renderer_text_new();
  g_object_set(G_OBJECT(renderer), "foreground", "red", NULL);

  /* Create a column, associating the "text" attribute of the
  * cell_renderer to the first column of the model */
  column = gtk_tree_view_column_new_with_attributes("Category", renderer, "text", CATEGORY_COLUMN, NULL);

  /* Add the column to the view. */
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  /* Second column.. title of the book. */
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes ("Recipe", renderer, "text", TITLE_COLUMN, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

  /* Last column.. whether a book is checked out. */
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes ("Dietarry Requirement", renderer, "text", DIETARRY_COLUMN, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);
  g_signal_connect(GTK_TREE_VIEW (tree), "cursor-changed", G_CALLBACK (activate_cb), NULL);

  gtk_grid_attach(GTK_GRID(grid), tree, 0, 0 , 5, 5);

  innerGrid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID (innerGrid), 6);

  gtk_grid_attach(GTK_GRID(grid), innerGrid, 5, 0, 1, 1);

  title = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large'>No recipe chosen</span>");
  gtk_grid_attach(GTK_GRID(innerGrid), title, 1, 1, 1, 1);

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

