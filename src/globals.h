/* Eigene globale Variable */
#include <gtksourceview/gtksourcebuffer.h>

/* Die gesammte Datei wird in g_data eingelesen 
	NULL-Bytes sind Delimiter zwischen einzelnen Einträgen */
/* Die Datei beginnt mit: 00 version 00 dateiinfos 00 anzahl d. zettel 00 */
gint g_maxdata;
gchar g_data[10100000];
gint g_maxzeile;
gchar g_zeile[60000]; // Zwischenspeicher zum Aufbau der Zettel
// ACHTUNG: Zettelgröße ist auch als Buffer in real_open_file in functions.c zu ändern

/* array zettel speichert Pointer auf g_data */
/* Es werden die Positionen der NULL-Bytes in g_data gespeichert
	Ein Eintrag (Satz) hat die Struktur:
	00 text 00 stichwort 00 autor 00 links 00 anmerkung 00 verweis1 00 verweis2 00 titel */
glong zettel[10000] [8];
glong indexz[10000];
/* array g_links = Zwischenspeicher für Links per zettel */
gint g_links[1000];
glong g_ptr_data;
gint zindex; // aktueller Zettel
gint eindex; // Zwischenspeicher bei Edit
gint mindex; // main-link-List Cursor changed
gint satz;   // letzter Zettel= Anzahl
gint cursor_pos; // cursor position from Edit to Main
/* g_rand=left margin für Listen*/

gint g_offset_x;
gint g_offset_y;


//gchar *g_autor;
gchar g_autor[80];
gchar g_month[40];
gchar g_password[20];

/* g_path = default directory */
gchar g_path[132];
gchar g_back[2];
gchar g_conv[2];
gchar g_s_autor[30];
gchar g_s_note[30];
gchar g_s_text[30];
gchar g_s_titel[30];
gchar g_s_stich[30];
gboolean g_ov_active;
gboolean g_edit_backlink;
gboolean g_edit_template;
gboolean g_list_todo;
gboolean g_list_idea;
gboolean g_list_info;
gboolean g_list_list;
gboolean g_list_people;
gboolean g_list_book;
gboolean g_list_text;
gboolean g_list_template;
gboolean g_list_hidden;

gboolean g_list_keyed;

/* als Zwischenspeicher für Texte */
gchar version[200];
gchar description[500];
gchar verweis1[1000];
gchar verweis2[1000];
gchar g_datum_c[30];   // datum changed = heute
gchar g_datum_e[100];  // datum created = aus datei
gchar g_version[100];  // global version string


/* --- verschobene Variable aus callbacks.c --- */
/* Definition und Handling von Globals ist Murks
   static gilt nur innerhalb eines Files :-)
   Globals kann man keine Werte z.B. NULL zuweisen */
/* This is the filename of the file currently being edited, in on-disk
   encoding (which may be UTF-8 or the locale's encoding). */
//gchar *filename;
gchar *current_filename;

/* --- Global Flags --- */
gboolean file_changed;
gboolean text_changed;
gboolean neuer_zettel;
gboolean show_autor;
gboolean show_note;
gboolean show_stich;
gboolean show_text;
gboolean show_titel;
gboolean show_last;
gboolean zettel_toggle;
gboolean readonly;		// readonly for file
gboolean readonly_c;	// readonly from command line
gboolean offset;
gboolean checkmenu;
//gboolean mark_stichwort;
//gboolean show_inactive;
gchar last_action[50]; // for find function in main and edit functions
gchar list_action[20]; // for list window header&actions

static GtkTextTag *tag_blue;

/* --- Pointer to global Window Widgets --- */
GtkWindow *main_window;
GtkWindow *edit_window;
GtkWindow *index_window;
GtkWindow *list_window;
GtkWindow *link_window;
GtkWindow *option_window;
GtkWidget *link_view, link_liste;
GtkListStore  *store_index, *store_m, *store_ext, *store_temp, *store_liste;
gboolean store_index_val, store_ext_val, store_temp_val, store_liste_val;
GtkWidget *popup_mnu;
GtkWidget *search_dialoge;
/* sView und sBuf weil Referenz in SR auf GtkSourceView & Buffer schwer/nicht anzulegen ist */
GtkWidget *sView;
GtkSourceBuffer *sBuf;

enum {
  COL_PIX,
  COL_ZETTEL,
  COL_TITEL,
  NUM_COLS,
} ;

struct words {
   int anzahl;
   char wort[50];
} worte[1000], temp[1];
