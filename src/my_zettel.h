/* my_zettel.h */

void add_link(long index, int newlink);
void create_myedit_window();
void fill_tree_view_m();
int find_in_header (char *search);
int get_links(long index);
//void on_Achtung_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_Achtung_activate (gpointer user_data);
void save_data (GtkWidget *edit_window);
void save_links(long index, int links[], int count);
void set_window_title (GtkWidget *main_window);
void show_ext_links (GtkWidget *list_window);

GtkWidget * create_view_and_model_m (GtkWidget *view);
