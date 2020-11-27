/**********************************************************************
 * ----- callback functions for GTK interface
 *       all other functions are in other files
 ******************************************************************* */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
//#include <gtksourceview/gtksourcelanguage.h>
#include <gtkspell/gtkspell.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "globals.h"
#include "my_file.h"

#include <libintl.h>
#define _(Text) gettext(Text)

/**********************************************************************
 * ----- MAIN Window Events -----
 ******************************************************************* */
gboolean main_window_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	g_printf("*** In Kill Event *** %d %d\n", file_changed, text_changed);
	GtkMenuItem *menu = GTK_MENU_ITEM(lookup_widget(GTK_WIDGET(main_window), "main_mnu_new"));
	main_mnu_quit_activate(menu, user_data);
}

gboolean
main_lbl_extern_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_lbl_extern_button_press_event\n");
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_exlinks");
	main_mnu_exlinks_activate(GTK_MENU_ITEM(temp), user_data);
}

gboolean 
main_image_extern_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	main_lbl_extern_button_press_event(widget, event, user_data);
}

void main_cbtn_inactive_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	int active;
	active=gtk_toggle_button_get_active (togglebutton);
//	g_printf("+++ in main_cbtn_inactive_toggled: %d\n", active);
}


void main_cbtn_stichview_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	int active;
	active=gtk_toggle_button_get_active (togglebutton);
//	g_printf("+++ in main_cbtn_stichview_toggled: %d\n", active);

	show_zettel(main_window);
}


gboolean
main_txt_stichwort_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	g_print("+++ in main_txt_stichwort_button_press_event\n");
	GtkTextBuffer *buffer;
	GtkTextIter where;
	GtkTextIter start, end;
	char *line;
	int ret, line_no;
	char head[130];

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
    gtk_text_buffer_get_iter_at_mark (buffer, &where, gtk_text_buffer_get_insert (buffer));
	line_no=gtk_text_iter_get_line(&where);

	//g_printf("    event type=%d - button=%d\n", event->type, event->button);
	if (event->button == 1) // linke maustaste
	{
		//g_printf("Event = 1 = links\n");
	}
	if (event->button == 3) // rechte maustaste
	{
		g_printf("    Index-Event = 3 = rechts\n");
		//mindex = -1; // warum ist das hier?
		GtkWidget *button;
		popup_mnu = create_pu_mnu_swort ();
		gtk_menu_popup (GTK_MENU (popup_mnu), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
//		button = lookup_widget (popup_mnu, "pu_mnu_delete");
//		gtk_widget_set_sensitive(button, FALSE);
//		button = lookup_widget (popup_mnu, "pu_mnu_goto");
//		gtk_widget_set_sensitive(button, FALSE);
		return TRUE;  // permits further processing
	}
	if (event->type == 5) // double click
	{
		g_printf("    Index-Event = 5 = doppel\n");
		// geht so nicht :-(
		gtk_text_buffer_get_iter_at_line(buffer, &start, line_no);
		gtk_text_buffer_get_iter_at_line(buffer, &end, line_no);
		gtk_text_iter_forward_to_line_end(&end);
		line = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);
		strip_stichwort(line);
		mystr_cut_at(line, '/');
		rm_whitespace(line);
		g_printf("    Stichwort=%s\n", line);
		ret=fill_indexz_stich(line);
		g_printf("    ready %d catchwords - line: %s\n", ret, line);
		sprintf(head, _("1. Stichwort = %s"),  line);
		g_ov_active=TRUE;
		show_index_titel(head);
	}
	
	return FALSE;
}

/**********************************************************************
 * ----- MAIN Window: Link Liste Events -----
 ******************************************************************* */
void main_link_list_row_activated      (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
/* --- double click events ---*/
{
	g_printf("+++ in main_link_list_row_activated\n");
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	char *l_titel;
	char *l_zettel;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_get_selected(selection,&model,&iter);

	gtk_tree_model_get(model, &iter, COL_TITEL, &l_titel, -1);
	gtk_tree_model_get(model, &iter, COL_ZETTEL, &l_zettel, -1);
//	g_print ("The row: '%s' Zettel '%d' has been selected.\n", l_titel, l_zettel);

	zindex=(int) l_zettel;
	show_zettel(main_window);
}


/* enter here only on valid lines */
void main_link_list_cursor_changed     (GtkTreeView     *treeview,
                                        gpointer         user_data)
{
	g_printf("+++ in main_link_list_cursor_changed\n");
	GdkEvent *event;
	event = gtk_get_current_event();
	int x;
	x = event->button.button;	
	// x=1 links, x=2 mitte, x=3 rechts

	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkWidget *text, *label;
	GtkWidget *button;
	GtkTextBuffer *text_buffer;
	gboolean VALID;
	char *l_titel;
	char *l_zettel;
	int i=1;
	static int last_zettel;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	VALID=gtk_tree_selection_get_selected(selection,&model,&iter);
	if (!VALID) return;
	gtk_tree_model_get(model, &iter, COL_ZETTEL, &i, -1);
//	gtk_tree_model_get(model, &iter, COL_TITEL, &l_titel, -1);
//	g_print ("    The row: '%s' Zettel '%d' has been selected.\n", l_titel, i);

	if (x == 3){ // mouse klick right
		//g_printf("   right mouse click - mindex=%d i=%d x=%d\n", mindex, i, x);
		mindex = i;
		
		/* popup_mnu is activated in main_link_list_button_press_event */
		button = lookup_widget (popup_mnu, "pu_mnu_delete");
		gtk_widget_set_sensitive(button, TRUE);
		button = lookup_widget (popup_mnu, "pu_mnu_goto");
		gtk_widget_set_sensitive(button, TRUE);
	} else if (VALID == TRUE) {
		// --- other events ---
		text = lookup_widget (GTK_WIDGET(main_window), "main_txt_text");
		button = lookup_widget (GTK_WIDGET(main_window), "main_btn_edit");
		gtk_tree_model_get(model, &iter, COL_ZETTEL, &i, -1);
		if(i != last_zettel) {
			last_zettel = i;
			zettel_toggle = TRUE;
			gtk_widget_set_sensitive(text, FALSE);
			gtk_widget_set_sensitive(button, FALSE);
		} else {
			if(zettel_toggle){
				i = zindex;
				zettel_toggle = FALSE;
				gtk_widget_set_sensitive(text, TRUE);
				gtk_widget_set_sensitive(button, TRUE);
			} else {
				zettel_toggle = TRUE;
				gtk_widget_set_sensitive(text, FALSE);
				gtk_widget_set_sensitive(button, FALSE);
			}
		}
		g_ptr_data = zettel[i] [0];
		get_g_data();
		text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		/* encodet? */
		if (g_zeile[0]=='#') {
//			g_print("    Text is encoded\n");
			int ret=strdecode(g_zeile);
			if(!ret) {
				mystr_cut_vorn(g_zeile, 1);	// strip #	
				fill_text(text_buffer);
			} else {
				gtk_text_buffer_set_text (text_buffer, _("*** Text ist verschlüsselt ***"), -1);
			}
		} else {
			fill_text(text_buffer);
		}
		/*-----*/
		//fill_text(text_buffer);
		g_ptr_data = zettel[i] [7];
		get_g_data();
		label = lookup_widget (GTK_WIDGET(main_window), "main_lbl_head");
		gtk_label_set_text (GTK_LABEL(label), g_zeile);
	}

}

/* all (also empty) line mouse click enters here */
gboolean
main_link_list_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	/* this is processed before "main_link_list_cursor_changed"
	 * but each mouse click activates both when this returns false */
	int x;
	x = event->button;	
	g_printf("+++ in main_link_list_button_press_event %d - %d\n", x, zindex);
	
	if (x == 3){ // mouse click right
		mindex = -1;
		GtkWidget *button;
		popup_mnu = create_pu_mnu_liste ();
		gtk_menu_popup (GTK_MENU (popup_mnu), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
		button = lookup_widget (popup_mnu, "pu_mnu_delete");
		gtk_widget_set_sensitive(button, FALSE);
		button = lookup_widget (popup_mnu, "pu_mnu_goto");
		gtk_widget_set_sensitive(button, FALSE);
		//return TRUE;  // would permit further processing
	}
	/* return needs to be false - otherwise 
	 * main_link_list_cursor_changed is not processed */
	return FALSE;
}

/**********************************************************************
 * MAIN Window Button Events
 *   will be rerouted to equivalent Menu events	
 ******************************************************************* */
void main_btn_open_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_open");
	main_mnu_open_activate(GTK_MENU_ITEM(temp), user_data);
}

void main_btn_save_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_save");
	main_mnu_save_activate(GTK_MENU_ITEM(temp), user_data);
}

void main_btn_new_clicked              (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_zneu");
	main_mnu_zneu_activate(GTK_MENU_ITEM(temp), user_data);
}

void main_btn_edit_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_zedit");
	main_mnu_zedit_activate(GTK_MENU_ITEM(temp), user_data);
}

void main_btn_first_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_zfirst");
	main_mnu_zfirst_activate(GTK_MENU_ITEM(temp), user_data);
}

void main_btn_back_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_zback");
	main_mnu_zback_activate(GTK_MENU_ITEM(temp), user_data);
}

void main_btn_forward_clicked          (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_zforward");
	main_mnu_zforward_activate(GTK_MENU_ITEM(temp), user_data);
}

void main_btn_last_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_zlast");
	main_mnu_zlast_activate(GTK_MENU_ITEM(temp), user_data);

}

void main_btn_goto_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_zgoto");
	main_mnu_zgoto_activate(GTK_MENU_ITEM(temp), user_data);
}

void main_btn_undo_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_undo");
	main_mnu_undo_activate(GTK_MENU_ITEM(temp), user_data);
}

void main_btn_index_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_index");
	main_mnu_index_activate(GTK_MENU_ITEM(temp), user_data);
}


void main_btn_autor_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_autor");
	main_mnu_autor_activate(GTK_MENU_ITEM(temp), user_data);

}


void main_btn_stich_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_stichw");
	main_mnu_stichw_activate(GTK_MENU_ITEM(temp), user_data);

}


void main_btn_pdf_clicked              (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_zv_pdf");
	main_mnu_zv_pdf_activate(GTK_MENU_ITEM(temp), user_data);
}

void main_btn_copy_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(main_window), "main_mnu_zv_copy");
	main_mnu_zv_copy_activate(GTK_MENU_ITEM(temp), user_data);
}


/**********************************************************************
 * MAIN Window Menu Events
 *   Button events will be rerouted to here	
 ******************************************************************* */
void main_mnu_new_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_new_activate\n");
	if (current_filename) rm_lock_file(current_filename);		// unlock "old" file
	new_file ();
}

void main_mnu_open_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_open_activate\n");
	if (file_changed == TRUE) {
		g_printf("*** File not saved ***\n");
		strcpy(last_action, "open_file");
		//on_Achtung_activate (menuitem, "Zettelkasten nicht gespeichert\nÄnderungen verwerfen?");
		on_Achtung_activate (_("Zettelkasten nicht gespeichert\nÄnderungen verwerfen?"));
	} else {
		strcpy(last_action, "open_file");
		open_file();
	}
}

void main_mnu_z_import_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_print("+++ in main_mnu_z_import_activate\n");
  	strcpy(last_action, "import_xml");
	open_file();
}

void main_mnu_save_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_save_activate\n");
	if (current_filename == NULL) {
  		strcpy(last_action, "save_as");
    	save_as ();
	} else {
    	real_save_file (current_filename);
	}
}

void main_mnu_save_as_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_save_as_activate\n");
  	strcpy(last_action, "save_as");
	save_as("Save File as");
}

void main_mnu_z_export_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_print("+++ in main_mnu_z_export_activate\n");
  	strcpy(last_action, "export_as");
	save_as();
}

void main_mnu_file_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_file_activate\n");
	my_file_info();
}

void main_mnu_quit_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	//file_changed = TRUE;
	
	g_printf("*** In Exit *** %d %d\n", file_changed, text_changed);
	int ret = get_i_config("lock_file");
	if (ret == 1 && readonly == FALSE) {	// Lock file support
		if (current_filename) rm_lock_file(current_filename);
	}
	if (file_changed == TRUE) {
		g_printf("*** File not saved ***\n");
		//on_Achtung_activate (menuitem, "Zettelkasten nicht gespeichert\nÄnderungen verwerfen?");
		on_Achtung_activate (_("Zettelkasten nicht gespeichert\nÄnderungen verwerfen?"));
	} else {
  		exit (0);	// --- Top Level Programm exit
	}
	return;
}

void main_mnu_zfirst_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_mnu_zfirst_activate\n");
	strcpy(last_action,"");
	zindex=1;
	cursor_pos=0;
	show_zettel(main_window);
}

void main_mnu_zback_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GdkEvent *event;
	event = gtk_get_current_event();
	int x, newlink;
	char sstring[50], status[70];
	x = event->button.button;	
//	g_printf("+++ in main_mnu_zback_activate: event=%d\n",x);
	/* x=0 if keyboard pressed and 1 for mouse click */

	int loop, size, active;
	GtkWidget *button, *statusbar;
	button=lookup_widget(GTK_WIDGET(main_window), "main_cbtn_inactive");
	active=gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button));

	cursor_pos=0;
	if (x==0 && !(strncmp(last_action, "zfind", 5))) {
		strcpy(sstring, last_action);
		mystr_cut_vorn(sstring, 7);
		newlink=find_in_header(sstring, 0); //suche rückwärts
		if (newlink >= 0) {
			sprintf(last_action,"zfind: %s", sstring);
			zindex = newlink;
			show_zettel(main_window);
		} else {
			statusbar = lookup_widget (GTK_WIDGET(main_window), "main_appbar");
			sprintf(status, _("Am Beginn: \"%s\" nicht mehr gefunden"), sstring); 
 			gnome_appbar_set_status (GNOME_APPBAR (statusbar), status);
		}
	} else {
		strcpy(last_action,"");
		loop=1;
		while (loop) {
			if (active) loop=0;
			zindex--;
			if (zindex < 1) zindex = satz;
			size = g_ptr_data = zettel[zindex] [1]; // Adressiert Stichwort
			size = get_g_data();  // holt Stichworte nach g_zeile
			if (g_zeile[0]!='#' && g_zeile[0]!='$') {
				loop=0;
			}
		}
		show_zettel(main_window);
	}
}


void main_mnu_znext_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_znext_activate\n");
	/* maybe deleted */
}


void main_mnu_zforward_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GdkEvent *event;
	event = gtk_get_current_event();
	int x, newlink;
	char sstring[50], status[70];
	x = event->button.button;	
//	g_printf("+++ in main_mnu_zforward_activate: event=%d\n",x);
	/* x=0 if keyboard pressed and 1 for mouse click */

	int loop, size, active;
	GtkWidget *button, *statusbar;
	button=lookup_widget(GTK_WIDGET(main_window), "main_cbtn_inactive");
	active=gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button));

	cursor_pos=0;
	if (x==0 && !(strncmp(last_action, "zfind", 5))) { // keypress && search
		strcpy(sstring, last_action);
		mystr_cut_vorn(sstring, 7);
		newlink=find_in_header(sstring, 1); //suche vorwärts
		if (newlink >= 0) {
			sprintf(last_action,"zfind: %s", sstring);
			zindex = newlink;
			show_zettel(main_window);
		} else {
			statusbar = lookup_widget (GTK_WIDGET(main_window), "main_appbar");
			sprintf(status, _("Am Ende: \"%s\" nicht mehr gefunden"), sstring); 
 			gnome_appbar_set_status (GNOME_APPBAR (statusbar), status);
		}
	} else { // mouse click && normal action
		strcpy(last_action,"");
		loop=1;
		while (loop) {
			if (active) loop=0;
			zindex++;
			if (zindex > satz) zindex = 1;
			size = g_ptr_data = zettel[zindex] [1]; // Adressiert Stichwort
			size = get_g_data();  // holt Stichworte nach g_zeile
			if (g_zeile[0]!='#' && g_zeile[0]!='$') {
				loop=0;
			}
		}
		show_zettel(main_window);
	}
}


void main_mnu_zlast_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_mnu_zlast_activate\n");
	strcpy(last_action,"");
	zindex=satz;
	cursor_pos=0;
	show_zettel(main_window);
}

void main_mnu_elast_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_elast_activate\n");
	int ret=last_edited();
	if (ret!=0) {
		zindex=ret;
		show_zettel(main_window);
	}
}

void main_mnu_zgoto_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	strcpy(last_action,"");
	on_input_activate(menuitem, _("Goto: Bitte Zettelnummer eingeben"));
}


void main_mnu_undo_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_undo_activate\n");
	pop_zindex();
	cursor_pos=0;
	show_zettel(main_window);
	//pop_zindex(); // zindex will be pushed again in show_zettel
}


void main_mnu_zfind_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_mnu_zfind_activate\n");
	strcpy(last_action, "zfind");
	on_input_activate(menuitem, _("Finde in Zettelüberschrift\nBitte Suchbegriff eingeben"));

}


void main_mnu_oaut_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_mnu_oaut_activate\n");
	int i,j=1;
	int size;
	GtkWidget *label;
 
 	clear_indexz();
	for (i=1; i<=satz; i++) {
    	size = g_ptr_data = zettel[i] [2]; // Adressiert Autor
    	size = get_g_data(); // holt Autor nach g_zeile
//    	g_printf("%s = %d\n", g_zeile, size);
    	if (size < 2) {
    		indexz[j]=i;
    		j++;
    	}
	}
	g_ov_active=TRUE;
	gchar *msg=_("Alle Zettel ohne Autor");
	show_index_titel(msg);
//	label=lookup_widget(GTK_WIDGET(index_window), "index_lbl_info");
//	gtk_label_set_text (GTK_LABEL(label), "Alle Zettel ohne Autor");
//	gtk_list_store_clear(store_index);
//	fill_tree_view_index();
}


void main_mnu_ostw_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_mnu_ostw_activate\n");
	int i,j=1;
	int size;
	GtkWidget *label;
 
 	clear_indexz();
	for (i=1; i<=satz; i++) {
    	size = g_ptr_data = zettel[i] [1]; // Adressiert Stichwort
    	size = get_g_data();
//    	g_printf("%s = %d\n", g_zeile, size);
    	if (size < 2) {
    		indexz[j]=i;
    		j++;
    	}
	}
	g_ov_active=TRUE;
	gchar *msg=_("Alle Zettel ohne Stichworte");
	show_index_titel(msg);
//	show_index_titel(_("Alle Zettel ohne Stichworte"));
//	label=lookup_widget(GTK_WIDGET(index_window), "index_lbl_info");
//	gtk_label_set_text (GTK_LABEL(label), "Alle Zettel ohne Stichworte");
//	gtk_list_store_clear(store_index);
//	fill_tree_view_index();
}


void main_mnu_olink_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_mnu_olink_activate\n");
	int i,j=1;
	int size;
	GtkWidget *label;
 
 	clear_indexz();
	for (i=1; i<=satz; i++) {
    	size = g_ptr_data = zettel[i] [3]; // Adressiert Links
    	size = get_g_data();
//    	g_printf("%s = %d\n", g_zeile, size);
    	if (size < 2) {
    		indexz[j]=i;
    		j++;
    	}
	}
	g_ov_active=TRUE;
	show_index_titel(_("Alle Zettel ohne Links"));
}


void main_mnu_ohead_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_mnu_ohead_activate\n");
	int i,j=1;
	int size;
	GtkWidget *label;
 
 	clear_indexz();
	for (i=1; i<=satz; i++) {
    	size = g_ptr_data = zettel[i] [7]; // Adressiert Zetteltitel
    	size = get_g_data(); // holt Zetteltitel nach g_zeile
//    	g_printf("%s = %d\n", g_zeile, size);
    	if (size < 2) {
    		indexz[j]=i;
    		j++;
    	}
	}
	g_ov_active=TRUE;
	show_index_titel(_("Alle Zettel ohne Titel/Überschrift"));
//	label=lookup_widget(GTK_WIDGET(index_window), "index_lbl_info");
//	gtk_label_set_text (GTK_LABEL(label), "Alle Zettel ohne Titel/Überschrift");
//	gtk_list_store_clear(store_index);
//	fill_tree_view_index();
}


void main_mnu_zneu_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_zneu_activate %d\n", zindex);
	/* work needed */
	neuer_zettel = TRUE;

	if (edit_window == NULL) {
		create_myedit_window();
	} else {
		/* clean edit buffer */
		GtkTextBuffer *buffer;
		GtkWidget *text;
		//gtk_text_buffer_delete(GTK_TEXT_BUFFER(sBuf), 0, 0);
		gtk_text_buffer_set_text (GTK_TEXT_BUFFER(sBuf), "", -1);
		text = lookup_widget (GTK_WIDGET(edit_window), "edit_txt_stichwort");
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		gtk_text_buffer_set_text (buffer, "", -1);
		text = lookup_widget (GTK_WIDGET(edit_window), "edit_txt_autor");
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		gtk_text_buffer_set_text (buffer, "", -1);
		text = lookup_widget (GTK_WIDGET(edit_window), "edit_txt_link");
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		gtk_text_buffer_set_text (buffer, "", -1);
		text = lookup_widget (GTK_WIDGET(edit_window), "edit_txt_vermerk");
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		gtk_text_buffer_set_text (buffer, "", -1);
		text = lookup_widget (GTK_WIDGET(edit_window), "edit_txt_extern");
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		gtk_text_buffer_set_text (buffer, "", -1);
		text = lookup_widget (GTK_WIDGET(edit_window), "edit_entry_titel");
		gtk_entry_set_text (GTK_ENTRY(text), "");

	}
	/* Make sure the window is visible. */
	gtk_window_present (GTK_WINDOW (edit_window));
	show_edit_zettel(edit_window);
}


void main_mnu_zneu_f_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_zneu_f_activate %d\n", zindex);
	if (link_window == NULL) 
	{
		link_window = GTK_WINDOW(create_ext_link_window ());
		/* set the widget pointer to NULL when the widget is destroyed */
		g_signal_connect (G_OBJECT (link_window), "destroy",
                      G_CALLBACK (gtk_widget_destroyed), &link_window);

		/* Make sure the window doesn't disappear behind the main window. */
		gtk_window_set_transient_for (GTK_WINDOW (link_window), GTK_WINDOW (main_window));
		gtk_window_set_resizable (GTK_WINDOW (link_window), TRUE);
	}
	/* Make sure the window is visible. */
	gtk_window_present (GTK_WINDOW (link_window));
	GtkWidget *label;
	label = lookup_widget (GTK_WIDGET(link_window), "ext_link_lbl");
	//gtk_label_set_text (GTK_LABEL(label), "");
	gtk_widget_hide(label);	
	/* fill list with template names */
	show_zneu_temp(link_window);
	return;
}


void main_mnu_zneu_x_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_zneu_x_activate: zindex=%d\n", zindex);
	//strcpy(last_action, "newz_x");
	g_edit_backlink=TRUE;
	mindex=zindex;	// save current position vor backlinking
	g_print("    g_edit_backlink=%d - mindex=%d\n", g_edit_backlink, mindex);
	main_mnu_zneu_activate(menuitem, user_data);
}


void main_mnu_zedit_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_zedit_activate\n");
	/* work needed */
	neuer_zettel = FALSE;
	strcpy(last_action,"");

	if (edit_window == NULL) {
		create_myedit_window();
	}
	/* Make sure the window is visible. */
	gtk_window_present (GTK_WINDOW (edit_window));
	show_edit_zettel(edit_window);
}


/* show zettel as PDF */
void main_mnu_zv_pdf_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	export_file ("/tmp/zettel.tex");
	system("pdflatex -interaction batchmode -output-directory /tmp /tmp/zettel.tex");
	system("gnome-open /tmp/zettel.pdf");

}

/* view a read-only copy of current zettel */
void main_mnu_zv_copy_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_print("+++ in main_mnu_zv_copy_activate\n");
//	g_print("    filename: %s\n", current_filename);
	char string[200];
//	sprintf(string, "/home/bernd/Projekte/ZMzettelkasten2/src/ZMzettelkasten -r -z %d %s &", zindex, current_filename);
	sprintf(string, "/usr/local/bin/ZMzettelkasten -r -z %d %s &", zindex, current_filename);
	system(string);
}

void  main_mnu_hilfe_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_mnu_hilfe_activate\n");
//	system("/home/bernd/Projekte/ZMzettelkasten2/src/ZMzettelkasten -r /usr/local/share/applications/ZMzettelkasten/hilfe &");
	system("/usr/local/bin/ZMzettelkasten -r /usr/local/share/applications/ZMzettelkasten/hilfe &");

}

void main_mnu_about_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_about_activate\n");
	static GtkWidget *about = NULL;

	if (about == NULL) {
		GtkWidget *label;
  		GtkImage *about_image;

		about = create_about_dialog ();
		/* set the widget pointer to NULL when the widget is destroyed */
		g_signal_connect (G_OBJECT (about), "destroy",
			G_CALLBACK (gtk_widget_destroyed), &about);
		/* Make sure the dialog doesn't disappear behind the main window. */
		gtk_window_set_transient_for (GTK_WINDOW (about), GTK_WINDOW (main_window));
		/* Do not allow user to resize the dialog */
		gtk_window_set_resizable (GTK_WINDOW (about), FALSE);
		label = lookup_widget (about, "about_txt_label");
		gtk_label_set_text (GTK_LABEL(label), g_version);
		//--- if pixmaps not shown hardlink to: usr/local/share/ZMzettelkasten/pixmaps
		//image = lookup_ (about, "about_image");
		//about_image = create_pixmap (about, "/usr/local/share/ZMzettelkasten/pixmaps/zmaus.png");
		//gtk_widget_show (about_image);
		//gtk_box_pack_start (GTK_BOX (dialog_vbox), about_image, TRUE, TRUE, 0);
	}
	/* Make sure the dialog is visible. */
	gtk_window_present (GTK_WINDOW (about));
}


void main_mnu_web_seite_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_mnu_web_seite_activate\n");
	system("/etc/alternatives/mozilla http://www.cc-c.de/german/download/zettelkasten.php");
}

/* --- save main window sizes an position --- */
void main_mnu_save_view_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in main_mnu_save_view_activate\n");
	GtkWidget *paned;
	int i;
	int left, top, width, hight;
	
	paned = lookup_widget (GTK_WIDGET (main_window), "main_vpaned1");
	i = gtk_paned_get_position (GTK_PANED (paned));
 	write_i_config(i, "m_vpan1");
	g_printf("Location vpaned1=%d\n", i);
	paned = lookup_widget (GTK_WIDGET (main_window), "main_hpaned1");
	i = gtk_paned_get_position(GTK_PANED (paned));
 	write_i_config(i, "m_hpan1");
	g_printf("Location hpaned1=%d\n", i);
	paned = lookup_widget (GTK_WIDGET (main_window), "main_vpaned2");
	i = gtk_paned_get_position(GTK_PANED (paned));
 	write_i_config(i, "m_vpan2");
	g_printf("Location vpaned3=%d\n", i);
	
	gtk_window_get_position(GTK_WINDOW (main_window), &left, &top);	
	gtk_window_get_size(GTK_WINDOW (main_window), &width, &hight);
 	write_i_config(left, "m_left");
 	write_i_config(top, "m_top");
 	write_i_config(width, "m_width");
 	write_i_config(hight, "m_hight");
	g_printf("Main_Window left=%d top=%d width=%d hight=%d\n", left, top, width, hight);

	/* save Toggle Buttons */
	int active;
	GtkWidget *button;
	button=lookup_widget(GTK_WIDGET(main_window), "main_cbtn_stichview");
	active=gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button));
 	write_i_config(active, "m_sview");
	button=lookup_widget(GTK_WIDGET(main_window), "main_cbtn_inactive");
	active=gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button));
 	write_i_config(active, "m_iactive");
}


void main_mnu_index_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_index_activate\n");
	/* work needed */

	show_index_titel("Zettelkasten - Index"); //translation?
  }


void main_mnu_v_back_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_v_back_activate, zindex=%d\n", zindex);
	int i, ret;
	gboolean show_liste=FALSE;
	
	clear_indexz();
    int j=0;
	for (i=0; i<satz; i++) {
		ret = exist_in_links(i);
		if(ret) {
			//g_print("    found backlink in %d\n", i+1);
			j++;
        	indexz[j]=i+1;
			show_liste=TRUE;
		}
	}
	char line[50];
	sprintf(line, _("Zettel, die auf Zettel %d verweisen"), zindex);
	if(show_liste) show_index_titel(line);

}

void main_mnu_z_key_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_z_key_activate\n");

	/* get Zettel text into g_zeile */
	int size;
	size = g_ptr_data = zettel[zindex] [0];
	size = get_g_data();

	/* crypting - encoding? */
	if (g_zeile[0]=='#') {
  		g_print("    Zettel is already encoded\n");
  		return;
	}
	if (strlen(g_password)==0) {	// password not set
		g_print("    *** ask for password ***\n");
		dialog_password(g_password);
	}
	if (strlen(g_password)>0) {	//  only if password is set password not set
		int ret=strencode(g_zeile);
		if(ret) { // encoding failed
			g_print("    *** encoding failed - save as normal ***\n");
			mystr_cut_vorn(g_zeile, 1); // save without encoding
		}
	}
	save_text(zindex, g_zeile);
	show_zettel(main_window);
}


void main_mnu_z_delete_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_z_delete_activate\n");
	int i, ret;
	char line[100];
	//gboolean show_liste=FALSE;
	long b_end, b_length;
	char *tmp;
	
	/*** Pop-up  OK - CANCEL ***/
	sprintf(line, _("Wollen Sie den Zettel %d wirklich löschen?\nInhalt und Links werden komplett entfernt"), zindex);
	GtkWidget *dialog = gtk_dialog_new_with_buttons(_("Delete Zettel"), main_window,
							GTK_DIALOG_MODAL,
							GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							GTK_STOCK_OK, GTK_RESPONSE_OK,
							NULL);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
//	GtkWidget *label = gtk_label_new(_("Wollen Sie wirklich den Zettel löschen?"));
	GtkWidget *label = gtk_label_new(line);
	GtkWidget *image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
	GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
	gtk_container_set_border_width (hbox, 10);
	gtk_box_pack_start_defaults (GTK_BOX(hbox), image);
	gtk_box_pack_start_defaults (GTK_BOX(hbox), label);
	gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG(dialog)->vbox), hbox);
	gtk_widget_show_all(dialog);
											
	ret = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	if (ret==GTK_RESPONSE_CANCEL) {
		g_print("*** delete Zettel abborted ***\n");
		return;
	}
//	if (ret==GTK_RESPONSE_OK) {
	/*** search and delete backlinks ***/
	clear_indexz();
    int j=0;
	for (i=0; i<satz; i++) {
		ret = exist_in_links(i);
		if(ret) {
			g_print("    found backlink to delete in %d - delete %d\n", i, zindex);
			delete_link(i+1, zindex);
		}
	}
	/* allocate Memory - save (disattach) Data behind change */ 
	g_ptr_data = zettel[zindex+1] [0]; // aktueller Eintrag plus 1
	b_end = zettel[satz+1] [0];        // Ende des Speichers
	b_length = b_end - g_ptr_data +1;  // ermittle Länge der Verschiebung
	tmp = malloc(b_length);
	if (b_length != 0 ) {
		/* --- hinteren Pufferbereich verschieben ---*/
		/* g_ptr_data zeigt auf beginnende 00 des ersten zu verschiebenden Satzes */
		g_printf ("*** Verschiebe ab Satz %d - Begin=%d Lenght=%d\n", zindex+1, g_ptr_data, b_length);
		memcpy (tmp, &g_data[g_ptr_data], b_length);
	}

	sprintf(line, _("Zettel gelöscht: %s"), g_datum_c);
	g_ptr_data = zettel[zindex] [0]; // aktueller Eintrag zum Überschreiben
	move2g_data("gelöscht"); // text
	move2g_data("##");	// stichwort
	move2g_data("");	// autor	
	move2g_data("");	// links	
	move2g_data("");	// anmerkung	
	move2g_data(line);	// date	
	move2g_data("");	// ext. Links	
	move2g_data("- none -");	// title	

	/* restore (attach) the rest of data-file and free up memory */
	/* letzte Speicheroperation setzt g_ptr_data */
	if (b_length != 0) {   
		g_printf ("Verschiebe zurück %d - Begin=%d Lenght=%d\n", zindex+1, g_ptr_data, b_length);
		memcpy (&g_data[g_ptr_data], tmp, b_length);
	}
	free(tmp);
	reindex(zindex);

	file_changed = TRUE;
	/* enable Save-Button */
	GtkWidget *button = lookup_widget (GTK_WIDGET (main_window), "main_btn_save"); //save-button
	gtk_widget_set_sensitive(button, TRUE);

	show_zettel(main_window);
}


/**********************************************************************
 * ----- Main Menu - set Zettel Type
 ******************************************************************* */
void main_mnu_norm_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
//	g_printf("+++ in main_mnu_norm_activate, %d\n", active);
	if (checkmenu==TRUE || active==0) { // will be set by show_zettel before changing the active state
		return;
	}
	set_ztype("");
}


void main_mnu_todo_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
//	g_printf("+++ in main_mnu_todo_activate, %d\n", active);
	if (checkmenu==TRUE || active==0) {
		return;
	}
	set_ztype("!");
}


void main_mnu_sz_idea_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
//	g_printf("+++ in main_mnu_sz_idea_activate, %d\n", active);
	if (checkmenu==TRUE || active==0) {
		return;
	}
	set_ztype("*");
}


void main_mnu_sz_info_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
//	g_printf("+++ in main_mnu_sz_info_activate, %d\n", active);
	if (checkmenu==TRUE || active==0) {
		return;
	}
	set_ztype("?");
}


void main_mnu_sz_list_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
//	g_printf("+++ in main_mnu_sz_list_activate, %d\n", active);
	if (checkmenu==TRUE || active==0) {
		return;
	}
	set_ztype("%");
}


void main_mnu_sz_people_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
//	g_printf("+++ in main_mnu_sz_people_activate, %d\n", active);
	if (checkmenu==TRUE || active==0) {
		return;
	}
	set_ztype("&");
}


void main_mnu_sz_book_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
//	g_printf("+++ in main_mnu_sz_book_activate, %d\n", active);
	if (checkmenu==TRUE || active==0) {
		return;
	}
	set_ztype("[");
}

void main_mnu_sz_text_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
//	g_printf("+++ in main_mnu_sz_text_activate, %d\n", active);
	if (checkmenu==TRUE || active==0) {
		return;
	}
	set_ztype("]");
}

void main_mnu_done_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
//	g_printf("+++ in main_mnu_done_activate, %d\n", active);
	if (checkmenu==TRUE || active==0) {
		return;
	}
	set_ztype("#");
}


void main_mnu_temp_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
//	g_printf("+++ in main_mnu_temp_activate, %d\n", active);
	if (checkmenu==TRUE || active==0) {
		return;
	}
		GtkWidget *menu;
		menu = lookup_widget (GTK_WIDGET (main_window), "main_mnu_zneu_f");
		gtk_widget_set_sensitive(menu, TRUE);
	
	set_ztype("$");
}

void main_mnu_exlinks_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (link_window == NULL) 
	{
		link_window = GTK_WINDOW(create_ext_link_window ());
		/* set the widget pointer to NULL when the widget is destroyed */
		g_signal_connect (G_OBJECT (link_window), "destroy",
                      G_CALLBACK (gtk_widget_destroyed), &link_window);

		/* Make sure the window doesn't disappear behind the main window. */
		gtk_window_set_transient_for (GTK_WINDOW (link_window), GTK_WINDOW (main_window));
		gtk_window_set_resizable (GTK_WINDOW (link_window), TRUE);
	}
	/* Make sure the window is visible. */
	gtk_window_present (GTK_WINDOW (link_window));
	/* load window content */
	show_ext_links(link_window);
}


/**********************************************************************
 * ----- About Window Events -----
 ******************************************************************* */
void
about_btn_close_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
//	g_printf("+++ in about_btn_close_clicked\n");
	gtk_widget_destroy (gtk_widget_get_toplevel (GTK_WIDGET (button)));
}


/**********************************************************************
 * ----- Open File Chooser Events -----
 ******************************************************************* */
gboolean
open_file_chooser_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_widget_hide (gtk_widget_get_toplevel (widget));
	return TRUE;
}

void open_file_btn_cancel_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
	g_printf("+++ in open_file_btn_cancel_clicked\n");
	gtk_widget_hide (gtk_widget_get_toplevel (GTK_WIDGET (button)));
}


void open_file_btn_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	g_printf("+++ in open_file_btn_ok_clicked\n");
	GtkWidget *filesel;
	const gchar *filename;
	//char *bakfile;
	int ret;

	filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));
	gtk_widget_hide (filesel);
	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filesel));
	if (current_filename) rm_lock_file(current_filename);  // unlock "old" file
	if (!strcmp(last_action, "open_file")) {
		if(!test4lock (filename)) {
			g_print("    will test for %s\n", filename);
			real_open_file (filename);
		}
	}
	if (!strcmp(last_action, "import_xml")) {
		import_xml (filename);
	}
	strcpy(last_action, "");
}


/**********************************************************************
 * ----- Save File Chooser Events -----
 ******************************************************************* */
gboolean
save_file_chooser_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_widget_destroy (gtk_widget_get_toplevel (widget));
	return TRUE;
}

void save_file_btn_cancel_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
	g_printf("+++ in save_file_btn_cancel_clicked\n");
	gtk_widget_destroy (gtk_widget_get_toplevel (GTK_WIDGET (button)));
	//current_filename=FALSE;
}


void save_file_btn_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	g_printf("+++ in save_file_btn_ok_clicked\n");
	GtkWidget *filesel;
	const gchar *filename;
	char line[300];

	filesel = gtk_widget_get_toplevel (GTK_WIDGET (button));
	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filesel));
	gtk_widget_destroy (filesel);
	if (!strcmp(last_action, "save_as")) {
		// existing file will be replaced 
		// => more work todo here?
		if (test_file_exist(filename)) {
			g_printf("    file %s already exist\n", filename);
			GtkWidget *dialog = gtk_dialog_new_with_buttons(_("File already exist"), main_window,
									GTK_DIALOG_MODAL,
									GTK_STOCK_OK, GTK_RESPONSE_OK,
									GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
									NULL);
			gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
			sprintf(line, _("Datei bereits vorhanden\nin %s\nwerden alle Daten überschrieben\n\nDatei wirklich anlegen?"), filename);
			GtkWidget *label = gtk_label_new(line);
			GtkWidget *image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
			GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
			gtk_container_set_border_width (hbox, 10);
			gtk_box_pack_start_defaults (GTK_BOX(hbox), image);
			gtk_box_pack_start_defaults (GTK_BOX(hbox), label);
			gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG(dialog)->vbox), hbox);
			gtk_widget_show_all(dialog);
													
			int ret = gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			g_print("    Antwort = %d\n", ret);
			if (ret==GTK_RESPONSE_OK) {
				real_save_file (filename);
			}
		} else { // file is new
			real_save_file (filename);
		}
	}
	if (!strcmp(last_action, "export_as")) {
		export_file (filename);
	}
	strcpy(last_action, "");
}

/**********************************************************************
 * ----- PopUp Menu (Main) Liste Events -----
 ******************************************************************* */
void
pu_mnu_liste_gehe_zu_zettel_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in pu_mnu_liste_gehe_zu_zettel_activate\n");
	zindex = mindex;
	cursor_pos=0;
	show_zettel(main_window);
}


void
pu_mnu_liste_link_anlegen_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in pu_mnu_liste_link_anlegen_activate\n");
	int i, j, anzahl;
	int x = (int) mindex-1;
	anzahl = get_links(zindex);	

	on_input_activate(menuitem, _("Link anlegen: Bitte Zettelnummer eingeben"));
}


void 
pu_mnu_liste_x_link_anlegen_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in pu_mnu_liste_x_link_anlegen_activate\n");
	int i, j, anzahl;
	int x = (int) mindex-1;
	anzahl = get_links(zindex);	

	on_input_activate(menuitem, _("X-Link anlegen: Bitte Zettelnummer eingeben"));
}


void
pu_mnu_liste_link_loeschen_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in pu_mnu_liste_link_loeschen_activate\n");
	int i, j, anzahl;
	int x = (int) mindex-1;
	g_printf("in on_link_loeschen_activate: user_data=%d - x=%d - pos=%d\n", user_data, x, cursor_pos);
	anzahl = get_links(zindex);
	g_printf("zu löschen ist Element %d\n", x+1);
	int new_links[anzahl-1];
	j = 0;
	for (i = 0; i <= anzahl; i++) {
		if (g_links[i] != x) {
			new_links[j] = g_links[i];
			j++;
		}		
	}
	j--;
	save_links(zindex, new_links, j);
	cursor_pos=0;
	show_zettel(main_window);
}


void pu_mnu_liste_newz_x_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in pu_mnu_liste_newz_x_activate\n");
	main_mnu_zneu_x_activate(menuitem, user_data);
}


/**********************************************************************
 * ----- PopUp Menu (Main) Stichwort Events
 ******************************************************************* */
void pu_mnu_sw_new_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in pu_mnu_sw_new_activate\n");
	stichwort_new();
}


void pu_mnu_sw_edit_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in pu_mnu_sw_edit_activate\n");
	stichwort_edit();
}


void pu_mnu_sw_delete_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in pu_mnu_sw_delete_activate\n");
	stichwort_delete();
}


/**********************************************************************
 * ----- PopUp Message Box Events -----
 ******************************************************************* */
gboolean pu_msg_box_delete_event       (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  	/* work needed */
return FALSE;
}


void pu_msg_btn_ok_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	g_printf("+++ in pu_msg_btn_ok_clicked - file:%d text:%d\n", file_changed, text_changed);
	if (text_changed == TRUE) {
		gtk_widget_hide (gtk_widget_get_toplevel GTK_WIDGET(edit_window));
		text_changed = FALSE;
	} else if (file_changed == TRUE) {
		exit(0);
	}
	gtk_widget_destroy (gtk_widget_get_toplevel (GTK_WIDGET (button)));
}


void pu_msg_btn_save_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
	g_printf("+++ in pu_msg_btn_save_clicked - file_changed:%d text_changed:%d\n", file_changed, text_changed);
	/* needs to be asked first ... save_data changes flags */
	if (file_changed == TRUE && text_changed == FALSE) {
		if (current_filename) {
			real_save_file (current_filename);
			gtk_widget_hide (gtk_widget_get_toplevel GTK_WIDGET(edit_window));
			if (strcmp(last_action, "open_file")) exit(0);
		} else {
  			strcpy(last_action, "save_as");
			save_as("Save File as");
		}
	}
	if (text_changed == TRUE) {
		save_data (edit_window);
		show_zettel (main_window);
		gtk_widget_hide (gtk_widget_get_toplevel GTK_WIDGET(edit_window));
	}
	gtk_widget_destroy (gtk_widget_get_toplevel (GTK_WIDGET (button)));
}


/**********************************************************************
 * ----- PopUp Input Box Events -----
 ******************************************************************* */
void pu_input_box_btn_cancel_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy (gtk_widget_get_toplevel (GTK_WIDGET (button)));
	strcpy(last_action, "");
}


void pu_input_box_btn_ok_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
	static GtkWidget *input_box, *statusbar;
	GtkWidget *label, *entry;
	const gchar *data;
	int newlink;
	char status[70];
	
	input_box = lookup_widget (GTK_WIDGET (button), "pu_input_box");
	label = lookup_widget (input_box, "pu_input_box_label");
	entry = lookup_widget (input_box, "pu_input_box_entry");
	data = gtk_entry_get_text(GTK_ENTRY(entry));
	newlink = atoi(data);

	data = gtk_label_get_text(GTK_LABEL (label));
	g_printf("on_input_ok_clicked Label=%s - Nummer=%d\n", data, newlink);
	/* --- standard (forward) Link --- */
	if (!strncmp(data, "Link", 4)) {
		g_printf("found LINK\n"); 
		// --- check fehlerhafte Eingaben ---
		if (newlink < 1) {
			gtk_label_set_text (GTK_LABEL(label), _("Link anlegen: Nummer zu klein - bitte neu eingeben" ));
			return;
		}
		if (newlink > satz) {
			gtk_label_set_text (GTK_LABEL(label), _("Link anlegen: Nummer zu groß - bitte neu eingeben" ));
			return;
		}
		// --- Link hinzufügen ---
		add_link(zindex, newlink);
		show_zettel(main_window);
	}
	/* --- Xross (bidirectional) Link --- */
	if (!strncmp(data, "X-Link", 6)) {
		g_printf("found X-LINK\n"); 
		// --- check fehlerhafte Eingaben ---
		if (newlink < 1) {
			gtk_label_set_text (GTK_LABEL(label), _("X-Link anlegen: Nummer zu klein - bitte neu eingeben" ));
			return;
		}
		if (newlink > satz) {
			gtk_label_set_text (GTK_LABEL(label), _("X-Link anlegen: Nummer zu groß - bitte neu eingeben" ));
			return;
		}
		// --- Link hinzufügen ---
		add_link(zindex, newlink);
		add_link(newlink, zindex);
		show_zettel(main_window);
	}

	/* --- Goto zettel nummer --- */
	if (!strncmp(data, "Goto", 4)) {
		g_printf("found Goto\n"); 
		// --- check fehlerhafte Eingaben ---
		if (newlink < 1) {
			gtk_label_set_text (GTK_LABEL(label), _("Goto: Nummer zu klein - bitte neu eingeben" ));
			return;
		}
		if (newlink > satz) {
			gtk_label_set_text (GTK_LABEL(label), _("Goto: Nummer zu groß - bitte neu eingeben" ));
			return;
		}
		// --- goto zettel ---
		zindex = newlink;
		cursor_pos=0;
		show_zettel(main_window);
	}
	
	/* --- find zettel header --- */
	if (!strncmp(data, "Find", 4)) {
//		g_printf("    found Find\n");
		data = gtk_entry_get_text(GTK_ENTRY(entry));
		newlink=find_in_header(data, 2); //suche (2x) vorwärts
		if (newlink >= 0) {
//			g_printf("    found Zettel %d\n", newlink);
			sprintf(last_action,"zfind: %s", data); // save for use with + and - Key
			zindex = newlink;
			cursor_pos=0;
			show_zettel(main_window);
		} else {
			statusbar = lookup_widget(GTK_WIDGET(main_window), "main_appbar");
			sprintf(status, _("Fehler: \"%s\" nicht gefunden"), data); 
 			gnome_appbar_set_status (GNOME_APPBAR (statusbar), status);
 			strcpy(last_action,"");
		}
	}
	// --- input box schließen ---
	gtk_widget_destroy (gtk_widget_get_toplevel (GTK_WIDGET (button)));	
}


/**********************************************************************
 * ----- PopUp list window Events -----
 ******************************************************************* */
gboolean ext_link_window_delete_event  (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	store_ext_val = FALSE;
	store_temp_val = FALSE;
	gtk_widget_destroy (gtk_widget_get_toplevel GTK_WIDGET(widget));
}


/* doubble click events */
void ext_link_liste_row_activated      (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{
	g_printf("+++ in ext_link_liste_row_activated\n");
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	char *line;
	char cmd[200];

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_get_selected(selection, &model, &iter);
	gtk_tree_model_get(model, &iter, COL_TITEL, &line, -1);
	//g_print ("The row: '%s' has been double clicked.\n", line);

	if (strncmp(line, "http", 4)==0) {
		//g_printf("http found\n");
		sprintf (cmd, "/etc/alternatives/mozilla %s &", line);
	} else if (strncmp(line, "-z", 2)==0) {
		mystr_cut_at(line, '=');
		sprintf (cmd, "/usr/local/bin/ZMzettelkasten -o %s &", line);
	} else if (strncmp(line, "zms", 3)==0) {
		mystr_cut_vorn(line, 4);
		mystr_cut_at(line, '=');
		sprintf (cmd, "/usr/local/bin/ZMmanuskript %s &", line);
	} else if (strncmp(line, "!", 1)==0) {
		sprintf (cmd, "%s &", line);
	} else if (strncmp(line, "/", 1 )==0 || (strncmp(line, "file", 4)==0)) {
		sprintf(cmd, "/usr/bin/gnome-open \"%s\" &", line);
	} else {
		sprintf(cmd, "/usr/bin/gnome-open \"%s/%s\" &", g_path, line);
	}
	g_printf("--> ext-open: %s\n", cmd);
	system(cmd);

//	gtk_widget_hide(gtk_widget_get_toplevel GTK_WIDGET(treeview));
	store_ext_val = FALSE;
	gtk_widget_destroy(gtk_widget_get_toplevel GTK_WIDGET(treeview));
}


void ext_link_liste_cursor_changed     (GtkTreeView     *treeview,
                                        gpointer         user_data)
{
	/* single clicks come to here */
	g_printf("+++ in ext_link_liste_cursor_changed\n");
	g_printf("    last_action: %s\n", last_action);
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gint i;
	
	if (!strcmp(last_action, "template")) {
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
		gtk_tree_selection_get_selected(selection, &model, &iter);
		gtk_tree_model_get(model, &iter, COL_ZETTEL, &i, -1);
		g_print ("    The template: '%d' has been selected.\n", i);

		//strcpy(last_action, "zneu_f");
		g_edit_template = TRUE;
		zindex=i;	// copy Zettel i
		mindex=i;	// save for backlink
		neuer_zettel = FALSE;
		if (edit_window == NULL) {
			create_myedit_window();
		}


		/* Make sure the window is visible. */
		gtk_window_present (GTK_WINDOW (edit_window));
		show_edit_zettel(edit_window);

		gtk_widget_hide (gtk_widget_get_toplevel GTK_WIDGET(treeview));
	}
	if (!strcmp(last_action, "chk_ext")) {
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
		gtk_tree_selection_get_selected(selection, &model, &iter);
		gtk_tree_model_get(model, &iter, COL_ZETTEL, &i, -1);
		g_print ("    The Zettel: '%d' has been selected.\n", i);
		zindex=i;
		cursor_pos=0;
		show_zettel(main_window);
	}
}


/**********************************************************************
 * ----- Edit Window Events -----
 ******************************************************************* */
gboolean edit_window_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	if (text_changed == TRUE) {
		g_printf("*** Zettel not saved ***\n");
		//on_Achtung_activate (menuitem, "Zettel nicht gespeichert\nÄnderungen verwerfen?");
		on_Achtung_activate (_("Zettel nicht gespeichert\nÄnderungen verwerfen?"));
	} else {
		gtk_widget_destroy (GTK_WIDGET(edit_window));
		edit_window=NULL;
	}
	strcpy(last_action,""); // unset last_action
}

void edit_entry_titel_changed          (GtkEditable     *editable,
                                        gpointer         user_data)
{
//	g_printf("+++ in edit_entry_titel_changed\n");
	GtkWidget *button;
	text_changed = TRUE;
	button = lookup_widget (GTK_WIDGET (edit_window), "edit_btn_save");
	gtk_widget_set_sensitive(button, TRUE);
}


void edit_mnu_save_close_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in edit_mnu_save_close_activate\n");
	save_data (edit_window);
	show_zettel (main_window);
	gtk_widget_hide (GTK_WIDGET(edit_window));
}


void edit_mnu_save_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in edit_mnu_save_activate\n");
	save_data (edit_window);
	show_zettel (main_window);
}


void edit_mnu_close_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in edit_mnu_close_activate\n");
	text_changed = FALSE;
	strcpy(last_action,""); // unset last_action
	gtk_widget_destroy (GTK_WIDGET(edit_window));
	edit_window=NULL;
}

void edit_mnu_undu_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in edit_mnu_undu_activate\n");
	int undo, redo;
	GtkWidget *button, *menu;
	undo = gtk_source_buffer_can_undo (sBuf);
//	g_printf("    can undo %d\n", undo);
	button = lookup_widget (GTK_WIDGET (edit_window), "edit_btn_undo");
	//gtk_widget_set_sensitive(button, undo);
	menu = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_undo");
	//gtk_widget_set_sensitive(menu, undo);
	if (undo) gtk_source_buffer_undo (sBuf);
	undo = gtk_source_buffer_can_undo (sBuf);
	gtk_widget_set_sensitive(button, undo);
	gtk_widget_set_sensitive(menu, undo);
	redo = gtk_source_buffer_can_undo (sBuf);
	button = lookup_widget (GTK_WIDGET (edit_window), "edit_btn_redo");
	menu = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_redo");
	gtk_widget_set_sensitive(button, redo);
	gtk_widget_set_sensitive(menu, redo);
}


void edit_mnu_redo_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("+++ in edit_mnu_redu_activate\n");
	int redo;
	GtkWidget *button, *menu;
	redo = gtk_source_buffer_can_redo (sBuf);
//	g_printf("    can redo %d\n", redo);
	button = lookup_widget (GTK_WIDGET (edit_window), "edit_btn_redo");
	menu = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_redo");
	//gtk_widget_set_sensitive(button, redo);
	//gtk_widget_set_sensitive(menu, undo);
	if (redo) gtk_source_buffer_redo (sBuf);
	redo = gtk_source_buffer_can_redo (sBuf);
//	g_printf("    can redo %d\n", redo);
	gtk_widget_set_sensitive(button, redo);
	gtk_widget_set_sensitive(menu, redo);
}


void edit_mnu_vsave_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in edit_mnu_vsave_activate\n");
	GtkWidget *paned;
	int i;
	int left, top, width, hight;
	
	paned = lookup_widget (GTK_WIDGET(edit_window), "edit_vpaned1");
	i = gtk_paned_get_position (GTK_PANED (paned));
 	write_i_config(i, "e_vpan1");
	g_printf("Location vpaned1=%d\n", i);
	paned = lookup_widget (GTK_WIDGET(edit_window), "edit_hpaned1");
	i = gtk_paned_get_position(GTK_PANED (paned));
 	write_i_config(i, "e_hpan1");
	g_printf("Location hpaned1=%d\n", i);
	
	gtk_window_get_position(GTK_WINDOW(edit_window), &left, &top);	
	gtk_window_get_size(GTK_WINDOW(edit_window), &width, &hight);
 	write_i_config(left, "e_left");
 	write_i_config(top, "e_top");
 	write_i_config(width, "e_width");
 	write_i_config(hight, "e_hight");
	g_printf("Edit_Window left=%d top=%d width=%d hight=%d\n", left, top, width, hight);
	
	GtkCheckMenuItem *item;
	item = GTK_CHECK_MENU_ITEM(lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_lineno"));
	i=gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(item));
 	write_i_config(i, "e_lineno");
 	g_printf("Edit_Window lineno=%d\n", i);
}


void edit_mnu_lineno_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(menuitem));
	g_printf("+++ in edit_mnu_lineno_activate: %d\n", active);
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(sView), active);
 	write_i_config(active, "e_lineno");
}


void edit_mnu_syntax_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int active;
	active=gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM(menuitem));
	g_printf("+++ in edit_mnu_syntax_activate: %d\n", active);
	//gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(sView), active);
    g_object_set (G_OBJECT (sBuf), "highlight", active, NULL);
 	write_i_config(active, "e_syntax");

}


void edit_mnu_search_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_print("+++ in edit_mnu_search_activate\n");
	GtkTextIter start, end;

	gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(sBuf), &start, &end);
	gchar *word="";  
	if (gtk_text_iter_compare (&start, &end) != 0) {
		word = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(sBuf), &start, &end, TRUE);
		g_print("   selected = %s\n", word);
	}
	my_search_dialog (word);
}


void on_edit_mnu_replace_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_print("+++ in edit_mnu_replace_activate\n");
	GtkTextIter start, end;

	gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(sBuf), &start, &end);
	gchar *word="";  
	if (gtk_text_iter_compare (&start, &end) != 0) {
		word = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(sBuf), &start, &end, TRUE);
		g_print("   selected = %s\n", word);
	}
	my_replace_dialog (word);
}

void edit_mnu_timestamp_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	char timestamp[80];
	time_stamp(timestamp);
	
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(sBuf), timestamp, strlen(timestamp));
}

void edit_mnu_autor_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	char help[80];
	GtkWidget *text = lookup_widget (edit_window, "edit_txt_autor");
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	sprintf(help, "%s, %s", g_autor, g_month);
	//gtk_text_buffer_set_text (text_buffer, help, -1);
	gtk_text_buffer_insert_at_cursor(text_buffer, help, strlen(help));
}


void edit_mnu_ins_dual_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkTextIter start, end;
	char *help="»«";

	gtk_text_buffer_insert_at_cursor(sBuf, help, -1);
	gtk_text_buffer_get_selection_bounds (sBuf, &start, &end);
  
	gtk_text_iter_backward_char (&start); // advance start+1
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(sBuf), &start);
}

void edit_mnu_ins_quote_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkTextIter start, end;
	char *help="„“";

	gtk_text_buffer_insert_at_cursor(sBuf, help, -1);
	gtk_text_buffer_get_selection_bounds (sBuf, &start, &end);
  
	gtk_text_iter_backward_char (&start); // advance start+1
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(sBuf), &start);
}

void edit_mnu_ins_check_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkTextIter start, end;
	char *help="✔";

	gtk_text_buffer_insert_at_cursor(sBuf, help, -1);
	gtk_text_buffer_get_selection_bounds (sBuf, &start, &end);
}

void edit_mnu_ins_arrow_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkTextIter start, end;
	char *help="⇨";

	gtk_text_buffer_insert_at_cursor(sBuf, help, -1);
	gtk_text_buffer_get_selection_bounds (sBuf, &start, &end);
}

/**********************************************************************
 * edit window buttons
 ******************************************************************* */
void edit_btn_search_clicked           (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_search");
	edit_mnu_search_activate(GTK_MENU_ITEM(temp), user_data);
}

void edit_btn_save_close_clicked       (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_save_close");
	edit_mnu_save_close_activate(GTK_MENU_ITEM(temp), user_data);
}


void edit_btn_close_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_close");
	edit_mnu_close_activate(GTK_MENU_ITEM(temp), user_data);
}


void edit_btn_undo_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_undo");
	edit_mnu_undu_activate(GTK_MENU_ITEM(temp), user_data);
}


void edit_btn_redo_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_redo");
	edit_mnu_redo_activate(GTK_MENU_ITEM(temp), user_data);
}


void edit_btn_save_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *button;
	button = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_save");
	edit_mnu_save_activate(GTK_MENU_ITEM(button), user_data);
	button = lookup_widget (GTK_WIDGET(edit_window), "edit_btn_save");
	gtk_widget_set_sensitive(button, FALSE);
}


/**********************************************************************
 * ----- Index Window Events
 ******************************************************************* */
gboolean index_window_delete_event     (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	store_index_val = FALSE;
	gtk_widget_destroy (gtk_widget_get_toplevel (widget));

	return TRUE;
}

void index_mnu_quit_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_widget_hide (GTK_WIDGET(index_window));
}


void index_mnu_note_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in index_mnu_note_activate\n");
	GtkWidget *entry;
	GtkWidget *label;

	entry = lookup_widget (GTK_WIDGET (menuitem), "index_entry_note");
	label = lookup_widget (GTK_WIDGET (menuitem), "index_lbl_note");
	show_note = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
	if (show_note) {
		gtk_widget_show (GTK_WIDGET (entry));
		gtk_widget_show (GTK_WIDGET (label));
		write_config("1", "i_note");
	} else {
		gtk_widget_hide (GTK_WIDGET (entry));
		gtk_widget_hide (GTK_WIDGET (label));
  		write_config("0", "i_note");
	}
	g_printf("    show_note=%d\n", show_text);
}


void index_mnu_autor_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in index_mnu_autor_activate\n");
	GtkWidget *entry;
	GtkWidget *label;

	entry = lookup_widget (GTK_WIDGET (menuitem), "index_entry_autor");
	label = lookup_widget (GTK_WIDGET (menuitem), "index_lbl_autor");
	show_autor = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
	if (show_autor) {
		gtk_widget_show (GTK_WIDGET (entry));
		gtk_widget_show (GTK_WIDGET (label));
		write_config("1", "i_autor");
	} else {
		gtk_widget_hide (GTK_WIDGET (entry));
		gtk_widget_hide (GTK_WIDGET (label));
  		write_config("0", "i_autor");
	}
	g_printf("    show_text=%d\n", show_text);
}


void index_mnu_text_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in index_mnu_text_activate\n");
	GtkWidget *entry;
	GtkWidget *label;

	entry = lookup_widget (GTK_WIDGET (menuitem), "index_entry_text");
	label = lookup_widget (GTK_WIDGET (menuitem), "index_lbl_text");
	show_text = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
	if (show_text) {
		gtk_widget_show (GTK_WIDGET (entry));
		gtk_widget_show (GTK_WIDGET (label));
		write_config("1", "i_text");
	} else {
		gtk_widget_hide (GTK_WIDGET (entry));
		gtk_widget_hide (GTK_WIDGET (label));
  		write_config("0", "i_text");
	}
	g_printf("    show_text=%d\n", show_text);
}


void index_mnu_titel_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in index_mnu_titel_activate\n");
	GtkWidget *entry;
	GtkWidget *label;

	entry = lookup_widget (GTK_WIDGET (menuitem), "index_entry_titel");
	label = lookup_widget (GTK_WIDGET (menuitem), "index_lbl_titel");
	show_titel = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
	if (show_titel) {
		gtk_widget_show (GTK_WIDGET (entry));
		gtk_widget_show (GTK_WIDGET (label));
		write_config("1", "i_titel");
	} else {
		gtk_widget_hide (GTK_WIDGET (entry));
		gtk_widget_hide (GTK_WIDGET (label));
  		write_config("0", "i_titel");
	}
	g_printf("    show_titel=%d\n", show_titel);
}


void index_mnu_vsave_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in index_mnu_vsave_activate\n");
	int left, top, width, hight;
	
	gtk_window_get_position(GTK_WINDOW(index_window), &left, &top);	
	gtk_window_get_size(GTK_WINDOW(index_window), &width, &hight);
 	write_i_config(left, "i_left");
 	write_i_config(top, "i_top");
 	write_i_config(width, "i_width");
 	write_i_config(hight, "i_hight");
	g_printf("    Index Window left=%d top=%d width=%d hight=%d\n", left, top, width, hight);
}


void index_mnu_links_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in index_mnu_links_activate\n");
	g_printf("--- seems to work, needs more testing ---\n");
	GtkWidget *index_list;
	index_list = lookup_widget (GTK_WIDGET(index_window), "index_list");
	GtkTreeIter iter;
	GtkTreeView *treeview = (GtkTreeView *)index_list;
	GtkTreeModel *list_store = gtk_tree_view_get_model (treeview);
	gboolean valid;
	int row_count = 0;
	int i, j, k, l, anzahl, index;
	int found[10000];
	int new_links[10000];

	/*** Pop-up  OK -CANCEL ***/
	GtkWidget *dialog = gtk_dialog_new_with_buttons(_("X-Links"), main_window,
							GTK_DIALOG_MODAL,
							GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							GTK_STOCK_OK, GTK_RESPONSE_OK,
							NULL);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
	GtkWidget *label = gtk_label_new(_("Wollen Sie wirklich alle sichtbaren Zettel verlinken?"));
	GtkWidget *image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
	GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
	gtk_container_set_border_width (hbox, 10);
	gtk_box_pack_start_defaults (GTK_BOX(hbox), image);
	gtk_box_pack_start_defaults (GTK_BOX(hbox), label);
	gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG(dialog)->vbox), hbox);
	gtk_widget_show_all(dialog);
											
	int ret = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	if (ret==GTK_RESPONSE_CANCEL) {
		g_print("*** make X-links abborted ***\n");
		return;
	}
//	if (ret==GTK_RESPONSE_OK) {
	
	
	/*** set x-links ***/
	/* Get the first iter in the list */
	valid = gtk_tree_model_get_iter_first (list_store, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;
		gint   int_data;
		// Make sure you terminate calls to gtk_tree_model_get() with a '-1' value
		gtk_tree_model_get (list_store, &iter, COL_TITEL, &str_data, COL_ZETTEL, &int_data, -1);
		/* Write int_data (=Zettelref-nr.) in found array */
		//g_print ("Row %d: (%s,%d)\n", row_count, str_data, int_data);
		g_free (str_data);
		found[row_count] = int_data-1; // Zettelindex startet mit 0
		row_count ++;
		valid = gtk_tree_model_iter_next (list_store, &iter);
	}
	// --- für jeden Eintrag in found Array ---
	for (i = 0; i < row_count; i++) {
		index = found[i]+1; // must be corrected
		anzahl = get_links(index);
		g_printf("Zettel %d hat %d Links g_links[0]%d\n", index, anzahl, g_links[0]);
		anzahl+=row_count;
		
		j = l = k = 0;
		while (anzahl != 0) {
//			g_printf("i=%d j=%d l=%d k=%d anzahl=%d g_links[l]=%d found[k]=%d\n", i, j, l, k, anzahl, g_links[l], found[k]);	
			if (g_links[l] == found[k]) {
			// --- Zettel in found und g_links vorhanden ---
//				g_printf("in g==f\n");
				new_links[j] = found[k];
				j++;
				l++;
				k++;
				anzahl--;
			} else if (g_links[l] > found[k]) {
			// --- Zettel existiert noch nicht in g_links ---
//				g_printf("in g>f\n");
				// --- und kein Verweis auf sich selbst --- 
				if (found[k] != index-1) {
//				g_printf("in g>f && != index\n");
					new_links[j] = found[k];
					j++;
				}
				k++;
			} else if (g_links[l] < found[k]) {
			// --- Zettel Nr. aus original
				if (g_links[l] == -1) {
					// --- dann doch aus found
					if (found[k] != index-1) {
//						g_printf("in g<f k=-1 k!=index\n");
						new_links[j] = found[k];
						j++;
					}				
					k++;
				} else {
//					g_printf("in g<f k=!-1\n");
					new_links[j] = g_links[l];
					l++;
					j++;
				}
			}
			anzahl--;
		}
		g_printf("Zettel %d: neue Anzahl %d\n", index, j);
		save_links(index, new_links, j);
/*		char line[1000];
		char zahl[10];
		strcpy(line, "");
		for (l = 0; l < j; l++)
		{
			sprintf(zahl, "%d ", new_links[l]);
			strcat(line, zahl);
		}
		g_print("%s\n", line);
*/
	}
	cursor_pos=0;
  show_zettel(GTK_WIDGET(main_window));
}


void index_mnu_ffilter_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in index_mnu_ffilter_activate\n");
	GtkWidget *entry;
		
	/* Such Window Text Feld */
	entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_text");
	strcpy(g_s_text, gtk_entry_get_text(GTK_ENTRY(entry)));
	/* Such Window Stichwort Feld */
	entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_stichwort");
	strcpy(g_s_stich, gtk_entry_get_text(GTK_ENTRY(entry)));
	/* Such Window Autor/Quelle Feld */
	entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_autor");
	strcpy(g_s_autor, gtk_entry_get_text(GTK_ENTRY(entry)));
	/* Such Window Anmerkung Feld */
	entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_note");
	strcpy(g_s_note, gtk_entry_get_text(GTK_ENTRY(entry)));
	/* Such Window Titel Feld */
	entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_titel");
	strcpy(g_s_titel, gtk_entry_get_text(GTK_ENTRY(entry)));
	g_printf("    Filter clicked: titel=%s text=%s stichwort=%s autor=%s note=%s\n", g_s_titel, g_s_text, g_s_stich, g_s_autor, g_s_note);
	
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_mnu_fclear_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in index_mnu_fclear_activate\n");
	GtkWidget *entry;

	entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_text");
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	strcpy(g_s_text, "");
	entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_stichwort");
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	strcpy(g_s_stich, "");
	entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_titel");
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	strcpy(g_s_titel, "");
	entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_note");
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	strcpy(g_s_note, "");
	entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_autor");
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	strcpy(g_s_autor, "");
	g_printf("    Filter clicked: text=%s stichwort=%s autor=%s\n", g_s_text, g_s_stich, g_s_autor);

	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void
index_btn_quit_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(index_window), "index_mnu_quit");
	index_mnu_quit_activate(GTK_MENU_ITEM(temp), user_data);
}


void
index_button_filter_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(index_window), "index_mnu_ffilter");
	index_mnu_ffilter_activate(GTK_MENU_ITEM(temp), user_data);
}


void
index_button_clear_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(index_window), "index_mnu_fclear");
	index_mnu_fclear_activate(GTK_MENU_ITEM(temp), user_data);
}

void index_btn_xlink_clicked           (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
//	g_printf("+++ in index_btn_xlink_clicked\n");
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(index_window), "index_mnu_links");
	index_mnu_links_activate(GTK_MENU_ITEM(temp), user_data);
}

void index_list_cursor_changed         (GtkTreeView     *treeview,
                                        gpointer         user_data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gboolean VALID;

	char *l_titel;
	char *l_zettel;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	VALID=gtk_tree_selection_get_selected(selection,&model,&iter);

	if (VALID == TRUE) {
		gtk_tree_model_get(model, &iter, COL_TITEL, &l_titel, -1);
		gtk_tree_model_get(model, &iter, COL_ZETTEL, &l_zettel, -1);
		g_print ("The row: '%s' Zettel '%d' has been selected.\n", l_titel, l_zettel);

		zindex= (int) l_zettel;
		cursor_pos=0;
		show_zettel(main_window);
		if (strlen(g_s_text) != 0)
		{
			mark_text(main_window, g_s_text, 'y');
		}
	}
}


/**********************************************************************
 * ----- File Info Box Events
 ******************************************************************* */
void file_info_btn_cancel_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy (gtk_widget_get_toplevel (GTK_WIDGET (button)));
}


void file_info_btn_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	g_printf("+++ in file_info_btn_ok_clicked %s\n", user_data);
	GtkWidget *window, *text;	
	GtkTextBuffer *text_buffer;
	GtkTextIter start, end;
	window = gtk_widget_get_toplevel (GTK_WIDGET (button));
	text = lookup_widget (window, "file_info_txt_note");
	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_text_buffer_get_bounds (text_buffer, &start, &end);
//	g_printf("head=%s\n", gtk_text_buffer_get_text (text_buffer, &start, &end, TRUE));
	strcpy(description, gtk_text_buffer_get_text (text_buffer, &start, &end, TRUE));

	file_changed = TRUE;
	text = lookup_widget (GTK_WIDGET (main_window), "main_btn_save"); //save-button
	gtk_widget_set_sensitive(text, TRUE);
	gtk_widget_destroy(window);

}




void main_mnu_autor_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_autor_activate\n");
	strcpy(last_action, "");
	list_autor();
}


void main_mnu_stichw_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_stichw_activate\n");
	strcpy(last_action, "");
	list_stichwort();
}


void main_mnu_password_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_print("+++ in main_mnu_password_activate - old password='%s'\n", g_password);
	dialog_password(g_password);
//	g_print("    new password='%s'\n", g_password);
	show_zettel(main_window);
}


void main_mnu_exlink_check_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_exlink_check_activate\n");
	check_exlinks();
}


void main_mnu_setup_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in main_mnu_setup_activate\n");
	my_option_window();
}


/**********************************************************************
 * ----- File Info Box Events
 ******************************************************************* */
void list_mnu_quit_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	strcpy(list_action, "");
	//store_liste_val = FALSE;
	g_printf("+++ in list_mnu_quit_activate: last_action='%s'\n", last_action);
	gtk_widget_hide (GTK_WIDGET(list_window));
}


gboolean list_window_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	strcpy(list_action, "");
	store_liste_val = FALSE;
	gtk_widget_destroy (gtk_widget_get_toplevel (widget));
}


void list_btn_close_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp = lookup_widget(GTK_WIDGET(list_window), "list_mnu_quit");
	list_mnu_quit_activate(GTK_MENU_ITEM(temp), user_data);
}


void list_mnu_vsave_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in list_mnu_vsave_activate\n");
	int left, top, width, hight;
	
	gtk_window_get_position(GTK_WINDOW(list_window), &left, &top);	
	gtk_window_get_size(GTK_WINDOW(list_window), &width, &hight);
 	write_i_config(left, "l_left");
 	write_i_config(top, "l_top");
 	write_i_config(width, "l_width");
 	write_i_config(hight, "l_hight");
	g_printf("    List Window left=%d top=%d width=%d hight=%d\n", left, top, width, hight);
}




void list_treeview_row_activated       (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{
	g_printf("+++ in liste_treeview_row_activated\n");
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	char *line, *line1;
	char head[130];
	int i, len, ret;
	char token[]=",";

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_get_selected(selection, &model, &iter);
	gtk_tree_model_get(model, &iter, COL_TITEL, &line, -1);
	g_print ("    The row: '%s' has been double clicked.\n", line);
	g_printf("    list_action: %s\n", list_action);


 	clear_indexz();
 	len=strlen(line);
 	int j=1;
 	g_ov_active=TRUE;
   	if(!strcmp(list_action, "autor")) {
 		for (i=1; i<=satz; i++) {
    		ret = g_ptr_data = zettel[i] [2]; // Adressiert Autor
    		ret = get_g_data(); // holt data nach g_zeile
    		//g_printf("len=%d - ret=%d | %s\n",len, ret, g_zeile);
			if (ret>=len && strstr(g_zeile, line)) {
    		//if (ret>=len && !strncmp(line, g_zeile, len)) {
    			indexz[j]=i;
    			j++;
    		}
		}
		g_printf("    ready %d authors - line: %s\n", j, line);
		sprintf(head, _("Autor = %s"), line);
		show_index_titel(head);
	}
 	j=1;
   	if(!strcmp(list_action, "stich")) {
		j=fill_indexz_stich(line);
/* 		for (i=1; i<=satz; i++) {
    		ret = g_ptr_data = zettel[i] [1]; // Adressiert stichwort
    		ret = get_g_data(); // holt data nach g_zeile
			if (g_zeile[0]=='!' || g_zeile[0]=='#' || g_zeile[0]=='$' || g_zeile[0]=='%') {
				mystr_cut_vorn(g_zeile, 1);
			}
    		//g_printf("len=%d - ret=%d | %s\n",len, ret, g_zeile);
     		line1 = strtok(g_zeile, token);
 			while (line1 != NULL) { // Zerlege bei , in Zeilen
     			//g_printf("    Stichwort=%s\n", line1);
     			mystr_cut_at(line1, '/');
    			if (!strcmp(line1, line)) {
    				indexz[j]=i;
    				j++;
				}
    			line1 = strtok(NULL, token);
    		}
		}
*/		g_printf("    ready: %d catchwords - line: %s\n", j, line);
		sprintf(head, _("1. Stichwort = %s"), line);
		show_index_titel(head);
	}

}



void edit_mnu_sp_aus_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
//	g_printf("in edit_mnu_sp_aus_activate\n");
	if (gtkspell_get_from_text_view(GTK_TEXT_VIEW(sView))!=NULL) {
// 		g_printf("    in !gtkpell\n");
		GtkSpell *spell=gtkspell_get_from_text_view(GTK_TEXT_VIEW(sView));
		gtkspell_detach(spell);
	}
}


void edit_mnu_sp_default_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (gtkspell_get_from_text_view(GTK_TEXT_VIEW(sView))!=NULL) {
// 		g_printf("    in !gtkpell\n");
  		GtkSpell *spell=gtkspell_get_from_text_view(GTK_TEXT_VIEW(sView));
  		gtkspell_detach(spell);
		spell = gtkspell_new_attach(GTK_TEXT_VIEW(sView), NULL, NULL);
	} else {
		GtkSpell *spell = gtkspell_new_attach(GTK_TEXT_VIEW(sView), NULL, NULL);
	}
}


void edit_mnu_sp_german_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (gtkspell_get_from_text_view(GTK_TEXT_VIEW(sView))!=NULL) {
// 		g_printf("    in !gtkpell\n");
  		GtkSpell *spell=gtkspell_get_from_text_view(GTK_TEXT_VIEW(sView));
  		gtkspell_detach(spell);
		spell = gtkspell_new_attach(GTK_TEXT_VIEW(sView), NULL, NULL);
	gtkspell_set_language(spell, "de_DE", NULL);
	} else {
		GtkSpell *spell = gtkspell_new_attach(GTK_TEXT_VIEW(sView), NULL, NULL);
	gtkspell_set_language(spell, "de_DE", NULL);
	}

}


void edit_mnu_sp_english_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (gtkspell_get_from_text_view(GTK_TEXT_VIEW(sView))!=NULL) {
// 		g_printf("    in !gtkpell\n");
  		GtkSpell *spell=gtkspell_get_from_text_view(GTK_TEXT_VIEW(sView));
  		gtkspell_detach(spell);
		spell = gtkspell_new_attach(GTK_TEXT_VIEW(sView), NULL, NULL);
	gtkspell_set_language(spell, "en_EN", NULL);
	} else {
		GtkSpell *spell = gtkspell_new_attach(GTK_TEXT_VIEW(sView), NULL, NULL);
	gtkspell_set_language(spell, "en_EN", NULL);
	}
}


/**********************************************************************
 * ----- Option Window Events
 ******************************************************************* */
void option_btn_seldir_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
	g_print("+++ in option_btn_seldir_clicked\n");
	GtkWidget *dialog, *field;
	gchar *dirname;
	
	//window = lookup_widget(option_window, "option_window");
	dialog=gtk_file_chooser_dialog_new ("Select Directoy ...", option_window,
										GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
										GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
										GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
										NULL);
	gint result = gtk_dialog_run (GTK_DIALOG(dialog));
	if (result == GTK_RESPONSE_ACCEPT) {
		dirname = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));
		field = lookup_widget(GTK_WIDGET(option_window), "option_entry_path");
		gtk_entry_set_text(GTK_ENTRY(field), dirname);

	}
	gtk_widget_destroy(dialog);
}


void option_btn_close_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	g_print("+++ option_btn_close_clicked\n");
	GtkWidget *field;
	char string[50];
	/* Allgemein */
	field = lookup_widget(GTK_WIDGET(option_window), "option_entry_path");
	write_config(gtk_entry_get_text(GTK_ENTRY(field)), "path");
	field = lookup_widget(GTK_WIDGET(option_window), "option_cbtn_autoload");
	int val=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(field));
	write_i_config(val, "auto_load");
	field = lookup_widget(GTK_WIDGET(option_window), "option_cbtn_autobackup");
	val=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(field));
	write_i_config(val, "auto_backup");
	field = lookup_widget(GTK_WIDGET(option_window), "option_rbtn_backup_std");
	val=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(field));
	write_i_config(val, "a_backup_std");
	field = lookup_widget(GTK_WIDGET(option_window), "option_cbtn_autoconvert");
	val=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(field));
	write_i_config(val, "auto_convert");
	field = lookup_widget(GTK_WIDGET(option_window), "option_cbtn_lock_file");
	val=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(field));
	write_i_config(val, "lock_file");
	/* Ansicht */
	field = lookup_widget(GTK_WIDGET(option_window), "option_combo_toolbar");
	val=gtk_combo_box_get_active(GTK_COMBO_BOX(field));
	g_print("    combo=%d\n", val);
	write_i_config(val, "toolbar");
	/* Editor */
	field = lookup_widget(GTK_WIDGET(option_window), "option_cbtn_lineno");
	val=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(field));
	write_i_config(val, "e_lineno");
	field = lookup_widget(GTK_WIDGET(option_window), "option_cbtn_syntax");
	val=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(field));
	write_i_config(val, "e_syntax");
	field = lookup_widget(GTK_WIDGET(option_window), "option_entry_font");
	sprintf(string, "\"%s\"", gtk_entry_get_text(GTK_ENTRY(field)));
	write_config(string, "e_font");
	field = lookup_widget(GTK_WIDGET(option_window), "option_cbtn_margin");
	val=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(field));
	if (val == 0) {
		write_i_config(val, "e_margin");
	} else {
		field = lookup_widget(GTK_WIDGET(option_window), "option_spin_margin");
		val=gtk_spin_button_get_value(GTK_SPIN_BUTTON(field));
		write_i_config(val, "e_margin");
	}
	g_print("    rand=%d\n", val);
	field = lookup_widget(GTK_WIDGET(option_window), "option_combo_spell");
	val=gtk_combo_box_get_active(GTK_COMBO_BOX(field));
	g_print("    spell=%d\n", val);
	write_i_config(val, "e_spell");
	field = lookup_widget(GTK_WIDGET(option_window), "option_cbtn_auto_save");
	val=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(field));
	write_i_config(val, "auto_save");
	/* autor */
	field = lookup_widget(GTK_WIDGET(option_window), "option_entry_autor");
	strcpy (g_autor, gtk_entry_get_text(GTK_ENTRY(field)));
	sprintf(string, "\"%s\"", gtk_entry_get_text(GTK_ENTRY(field)));
	write_config(string, "autor");
	field = lookup_widget(GTK_WIDGET(option_window), "option_entry_token");
	sprintf(string, "\"%s\"", gtk_entry_get_text(GTK_ENTRY(field)));
	write_config(string, "token");
	field = lookup_widget(GTK_WIDGET(option_window), "option_cbtn_autor");
	val=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(field));
	write_i_config(val, "e_autor");
	

	gtk_widget_destroy(option_window);
	option_window=FALSE;
}


void
option_cbtn_margin_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *field = lookup_widget(GTK_WIDGET(option_window), "option_spin_margin");
	gtk_widget_set_sensitive(field, gtk_toggle_button_get_active(togglebutton));
}

void option_cbtn_autobackup_toggled    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *field = lookup_widget(GTK_WIDGET(option_window), "option_rbtn_backup_std");
	gtk_widget_set_sensitive(field, gtk_toggle_button_get_active(togglebutton));
	field = lookup_widget(GTK_WIDGET(option_window), "option_rbtn_backup_zkb");
	gtk_widget_set_sensitive(field, gtk_toggle_button_get_active(togglebutton));
}

void option_font_dialog_response       (GtkFontSelectionDialog *dialog,
										gint response, gpointer data)
{
	g_print("+++ in option_font_dialog_response\n");
	gchar *font;

	//gint result = gtk_dialog_run (GTK_DIALOG(dialog));
	g_print("    response=%d - %d\n", response, GTK_RESPONSE_OK);
	if (response==GTK_RESPONSE_OK || response==GTK_RESPONSE_APPLY) {
		font = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(dialog));
		g_print("    font='%s'\n", font);
		GtkWidget *field = lookup_widget(GTK_WIDGET(option_window), "option_entry_font");
		gtk_entry_set_text(GTK_ENTRY(field), font);

	}

	gtk_widget_destroy(dialog);
}
										
void option_btn_selfont_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	g_print("+++ in option_btn_selfont_clicked\n");
	GtkWidget *dialog, *field;
	
	//window = lookup_widget(option_window, "option_window");
	dialog=gtk_font_selection_dialog_new("Select a Font");
	gtk_font_selection_dialog_set_font_name(dialog, "monospace 10");
	gtk_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(option_font_dialog_response), NULL);
	gtk_widget_show_all(dialog);
}


void option_btn_default_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *field = lookup_widget(option_window, "option_entry_token");
	gtk_entry_set_text(field, ":,;");
}


/**********************************************************************
 * ----- Search Dialog Events
 ******************************************************************* */
gboolean search_dialog_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_widget_destroy (widget);
	return FALSE;
}


void search_btn_close_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy (gtk_widget_get_toplevel (GTK_WIDGET (button)));
}


void search_btn_back_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *field = lookup_widget(GTK_WIDGET(search_dialoge), "search_entry_text");
	search_e_text(gtk_entry_get_text(GTK_ENTRY(field)), 1);
}


void search_btn_vor_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *field = lookup_widget(GTK_WIDGET(search_dialoge), "search_entry_text");
	search_e_text(gtk_entry_get_text(GTK_ENTRY(field)), 0);
}



gboolean
replace_dialog_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_widget_destroy (widget);
	return FALSE;
}


void
replace_btn_close_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy (gtk_widget_get_toplevel (GTK_WIDGET (button)));
}


void
replace_btn_search_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *field = lookup_widget(GTK_WIDGET(search_dialoge), "replace_entry_old");
	search_e_text(gtk_entry_get_text(GTK_ENTRY(field)), 0);
}


void
replace_btn_replace_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *field = lookup_widget(GTK_WIDGET(search_dialoge), "replace_entry_old");
	replace_e_text(gtk_entry_get_text(GTK_ENTRY(field)), 0);
}


void
replace_btn_all_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *field = lookup_widget(GTK_WIDGET(search_dialoge), "replace_entry_old");
	replace_e_text(gtk_entry_get_text(GTK_ENTRY(field)), 1);
}





void main_mnu_zbookmark_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_print("+++ in main_mnu_zbookmark_activate\n");
	g_print("    *** programming needed ***\n");
	error_not_exist();
}



void
on_index_mnu_fhidden_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void index_mnu_ftodo_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_list_todo=TRUE;
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_mnu_fhidden_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_list_hidden=TRUE;
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_mnu_fkeyed_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_list_keyed=TRUE;
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_mnu_fidea_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_list_idea=TRUE;
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_mnu_finfo_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_list_info=TRUE;
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_mnu_flist_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_list_list=TRUE;
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_mnu_fpeople_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_list_people=TRUE;
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_mnu_fbook_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_list_book=TRUE;
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_mnu_ftext_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_list_text=TRUE;
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_mnu_ftemplate_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_list_template=TRUE;
	gtk_list_store_clear(store_index);
	fill_tree_view_index();
}


void index_button_todo_clicked         (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(index_window), "index_mnu_ftodo");
	index_mnu_ftodo_activate(GTK_MENU_ITEM(temp), user_data);
}


void index_button_info_clicked         (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(index_window), "index_mnu_finfo");
	index_mnu_finfo_activate(GTK_MENU_ITEM(temp), user_data);

}


void index_button_idea_clicked         (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *temp;
	temp = lookup_widget(GTK_WIDGET(index_window), "index_mnu_fidea");
	index_mnu_fidea_activate(GTK_MENU_ITEM(temp), user_data);

}





