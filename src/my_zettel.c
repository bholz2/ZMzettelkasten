/**********************************************************************
 * --- Zettelkasten-Funktionen nach hier ausgelagert ---
 *     
 ******************************************************************* */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagesmanager.h>

#include "support.h"
#include "globals.h"

                                        
/* --- SR schreibt data nach g_data --- */
void move2g_data(char *line)
{
	int i=0;
	
	while(line[i] != 0) {
		g_ptr_data++;
		g_data[g_ptr_data] = line[i];
		i++;
	}
	g_ptr_data++;
	g_data[g_ptr_data] = 0x00;
}

/* --- get max count von g_zeile --- */
/*     und korrigiert um <x> Tags */
int get_len (int count, int vor)
{
	//g_printf("+++ in get_len - count=%d, vor=%d\n", count, vor);
	int i=0, n0=0, n3=0, n4=0;
	int tag=0, corr=0;
	char c;
	char UML=0xc3;			// Umlaut
	char UML1=0xc2;
	char UML3=0xe2;			// Sonderzeichen e2 80 xx

	int offset=0;
	for (i=0; i<=count+offset; i++) {
		/* korrigiere length (count) um UTF character */
		if (g_zeile[i] == UML || g_zeile[i] == UML1) { 
			offset++;
		}
		if (g_zeile[i] == UML3) { 
			offset=offset+2;
		}
		/* search for TAGs */
		if (g_zeile[i] == '<' && g_zeile[i+1] == 'i' && g_zeile[i+2] == '>') {
			i=i+2;
			while (i<=count+offset) {
				if (vor==0) count++;
				corr++;
				i++;
				if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='i' && g_zeile[i+3]=='>') {
					if (vor==0) count=count+5; // pics habe a total of 7 plus the filename
					corr=corr+5;
					break;
				}
			}
		} else if (g_zeile[i] == '<' && g_zeile[i+2] == '>') {
			tag = 3;
			//offset++;
			n3++;
			//i++;
			c = g_zeile[i+1];
		} else if (g_zeile[i] == '<' &&g_zeile[i+1] == '/' && g_zeile[i+3] == '>') {
			tag = 4;
			//offset++;
			n4++;
			//i=i+2;
			c = g_zeile[i+2];
		} else {
			tag = 0;
		} 
		if (tag != 0) {
			switch(c) {		// es dürfen nur gültige Tags gezählt werden
				case 'm':
				case 'd':
				case 'f':
				case 'k':
				case 'l':
				case 'u':
				case 'c':
				case 'h':
				case 'S':
				case 's':
				case 't':
					if (vor==0) { // correct forwards
						count=count+tag;
					}
					corr=corr+tag;
					//g_printf("     - %d - count=%d, n3=%d, n4=%d, offset=%d\n", i, count, n3, n4, offset);
					break;
				default:
					//tag = 0;
					n0++;
			}
		}
		//tag = 0;
		//if (tag != 0) {
		//	
	}
	if (vor==1) { // correct backwards
		count=count-corr;
	}
	//g_printf("    end get_len - count=%d, n3=%d, n4=%d, n0=%d\n", count, n3, n4, n0);
	return(count);
}

/* **********************************************************
* --- Set window title according to the current filename ---
********************************************************** */
void set_window_title (GtkWidget *main_window)
{
  gchar *title, *filename_utf8 = NULL;

  /* need to convert the filename to UTF-8. */
  if (current_filename)
    filename_utf8 = g_filename_to_utf8 (current_filename, -1, NULL, NULL, NULL);

  title = g_strdup_printf ("ZMzettelkasten: %s",
			   filename_utf8 ? g_basename (filename_utf8)
			   : _("kein Zettelkasten"));
  gtk_window_set_title (GTK_WINDOW (main_window), title);
  //g_printf("+++ in set_window_title: %s\n", title);
  g_free (title);
  g_free (filename_utf8);
}

void set_window_e_title (int index)
{
  gchar *title, *filename_utf8 = NULL;

  /* need to convert the filename to UTF-8. */
  if (current_filename)
    filename_utf8 = g_filename_to_utf8 (current_filename, -1, NULL, NULL, NULL);

  title = g_strdup_printf (_("Edit %s - Zettel %d"),
			   filename_utf8 ? g_basename (filename_utf8)
			   : _("kein Zettelkasten"), index);
//  g_printf("+++ in set_window_e_title: %s\n", title);
  gtk_window_set_title (GTK_WINDOW (edit_window), title);
  g_free (title);
  g_free (filename_utf8);
}


/* ********************************************************
 * Routinen zum Lesen und Schreiben von einzelnen Feldern 
 * ****************************************************** */
/** fill g_links array with Links of index ***/
int get_links(long index)
 {
 	int i, len;

	// --- clear g_links
	for (i = 0; i < 1000; i++)
	{
		g_links[i] = -1;
 	}
	
 	g_ptr_data = zettel[index] [3];
	len = get_g_data();
	// --- teile g_zeile bei Blanks in Zahlen und füllt Array g_links
 	char data_token[]=" ";
	char *sub_zeile;
	sub_zeile = strtok(g_zeile, data_token);
	i = 0;
	while (sub_zeile != NULL) {
//		g_printf("i=%d: datum = %s\n", i, sub_zeile);
		g_links[i] = atoi(sub_zeile);
		sub_zeile = strtok(NULL, data_token);
	i++;
	}
//	g_printf("Anz. Elemente : %d\n", i);
	return i;
 }
 
 /*** saves links[] to RAM-file ***/
 void save_links(long index, int links[], int count)
 {
// 	g_printf("+++ in save_links: index=%d, links=%d\n", index, count);
 	int i;
 	char line[1000];
 	char zahl[10];
	long b_end, b_length;
	char *tmp;

	strcpy(line, "");
	for (i = 0; i < count; i++)
	{
		sprintf(zahl, "%d ", links[i]);
		strcat(line, zahl);
	}
//	g_print(line);

	/* allocate Memory - save (disattach) Data behind change */ 
	g_ptr_data = zettel[index] [4]; 	// aktueller Eintrag next data
	b_end = zettel[satz+1] [0];     	// Ende des Speichers
	b_length = b_end - g_ptr_data +1;	// ermittle Länge der Verschiebung
	tmp = malloc(b_length);
	if (b_length != 0 ) {
		/* --- hinteren Pufferbereich verschieben ---*/
		g_printf ("*** Verschiebe ab Satz %d [4] - Begin=%d Lenght=%d\n", index, g_ptr_data, b_length);
		memcpy (tmp, &g_data[g_ptr_data], b_length);
	}

	g_ptr_data = zettel[index] [3]; // aktueller Eintrag zum Überschreiben
	move2g_data(line);
	/* restore (attach) the rest of data-file and free up memory */
	/* letzte Speicheroperation setzt g_ptr_data */
	if (b_length != 0) {   
		g_printf ("Verschiebe zurück %d - Begin=%d Lenght=%d\n", index, g_ptr_data, b_length);
		memcpy (&g_data[g_ptr_data], tmp, b_length);
	}
	free(tmp);
	reindex(index);	

	file_changed = TRUE;
	GtkWidget *button;
	button = lookup_widget (GTK_WIDGET (main_window), "main_btn_save"); //save-button
	gtk_widget_set_sensitive(button, TRUE);
}

/*** checks if a "back link" exist in zettel index ***/ 
int exist_in_links(index)
{
	int i, ret=0;
	int anzahl = get_links(index+1);
//	g_print("+++ in exist_in_links Zettel %d - links: %d\n", index+1, anzahl);
	
	for (i=0; i<=anzahl; i++) {
	//g_print("    backlink %d in %d = %d\n", i, index, g_links[i]);
		if (g_links[i] == zindex-1) {
			//g_print("    found backlink in %d:%d\n", index, zindex);
			ret=1;
		}
	}
	return ret;
}

/*** add link "newlink" to Zettel Index ***/
void add_link(long index, int newlink)
{
	int i, j, anzahl;
	int x = newlink-1;
	anzahl = get_links(index);
	anzahl++;
	g_printf("+++ Link %d(%d) hinzufügen bei %d\n", newlink, x, index);
	int new_links[anzahl+1];
	i = j = 0;
	while (g_links[i] < x && g_links[i] != -1) {
//		g_printf("i<x\n");
		new_links[j] = g_links[i];
		j++;
		i++;
	}
	new_links[j] = x;
	j++;
	if (g_links[i] == x) { // falls x schon vorhanden ist überspringen
		i++;
	}
	while (i <= anzahl && g_links[i] != -1) {
//		g_printf("i<anzahl\n");
		new_links[j] = g_links[i];
		j++;
		i++;		
	}
	save_links(index, new_links, j);
}

/*** delete link "link" from Zettel Index ***/
void delete_link(long index, int link)
{
	g_printf("+++ in delete_link: delete=%d - from=%d\n", link, index);
	int i, j, anzahl;
	link--;  // is one to large
	anzahl = get_links(index);

	//g_printf("    zu löschen ist Element %d von %d\n", link, anzahl);
	int new_links[anzahl-1];
	j = 0;
	for (i = 0; i <= anzahl; i++) {
		if (g_links[i] != link) {
			//g_printf("    Element %d wird übernommen\n", g_links[i]);
			new_links[j] = g_links[i];
			j++;
		}
	}
	j--;
	save_links(index, new_links, j);
}

 void save_stichwort(long index, char *stichwort)
 {
 	g_printf("+++ in save_stichwort: index=%d, stw=%s\n", index, stichwort);
	long b_end, b_length;
	char *tmp;

	/* allocate Memory - save (disattach) Data behind change */ 
	g_ptr_data = zettel[index] [2]; 	// aktueller Eintrag next data
	b_end = zettel[satz+1] [0];     	// Ende des Speichers
	b_length = b_end - g_ptr_data +1;	// ermittle Länge der Verschiebung
	tmp = malloc(b_length);
	if (b_length != 0 ) {
		/* --- hinteren Pufferbereich verschieben ---*/
		g_printf ("*** Verschiebe ab Satz %d [4] - Begin=%d Lenght=%d\n", index, g_ptr_data, b_length);
		memcpy (tmp, &g_data[g_ptr_data], b_length);
	}

	g_ptr_data = zettel[index] [1]; // aktueller Eintrag zum Überschreiben
	move2g_data(stichwort);
	/* restore (attach) the rest of data-file and free up memory */
	/* letzte Speicheroperation setzt g_ptr_data */
	if (b_length != 0) {   
		g_printf ("Verschiebe zurück %d - Begin=%d Lenght=%d\n", index, g_ptr_data, b_length);
		memcpy (&g_data[g_ptr_data], tmp, b_length);
	}
	free(tmp);
	reindex(index);	

	file_changed = TRUE;
	GtkWidget *button;
	button = lookup_widget (GTK_WIDGET (main_window), "main_btn_save"); //save-button
	gtk_widget_set_sensitive(button, TRUE);
}


void save_text(long index, char *text)
 {
 	g_printf("+++ in save_text\n");
	long b_end, b_length;
	char *tmp;

	/* allocate Memory - save (disattach) Data behind change */ 
	g_ptr_data = zettel[index] [1]; 	// aktueller Eintrag next data
	b_end = zettel[satz+1] [0];     	// Ende des Speichers
	b_length = b_end - g_ptr_data +1;	// ermittle Länge der Verschiebung
	tmp = malloc(b_length);
	if (b_length != 0 ) {
		/* --- hinteren Pufferbereich verschieben ---*/
		g_printf ("*** Verschiebe ab Satz %d [1] - Begin=%d Lenght=%d\n", index, g_ptr_data, b_length);
		memcpy (tmp, &g_data[g_ptr_data], b_length);
	}

	g_ptr_data = zettel[index] [0]; // aktueller Eintrag zum Überschreiben
	move2g_data(text);
	/* restore (attach) the rest of data-file and free up memory */
	/* letzte Speicheroperation setzt g_ptr_data */
	if (b_length != 0) {   
		g_printf ("Verschiebe zurück %d - Begin=%d Lenght=%d\n", index, g_ptr_data, b_length);
		memcpy (&g_data[g_ptr_data], tmp, b_length);
	}
	free(tmp);
	reindex(index);	

	file_changed = TRUE;
	GtkWidget *button;
	button = lookup_widget (GTK_WIDGET (main_window), "main_btn_save"); //save-button
	gtk_widget_set_sensitive(button, TRUE);
}


/**********************************************************************
 * --- Achtung Dialog.
 ******************************************************************* */
void on_Achtung_activate               (//GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("msg_box: %s\n", user_data);
	static GtkWidget *achtung = NULL;
	GtkWidget *label;

	if (achtung == NULL) 
	{
      achtung = GTK_WIDGET(create_pu_msg_box ());
      /* set the widget pointer to NULL when the widget is destroyed */
		g_signal_connect (G_OBJECT (achtung), "destroy",
			G_CALLBACK (gtk_widget_destroyed), &achtung);

		label = lookup_widget (achtung, "pu_msg_label");
		gtk_label_set_text (GTK_LABEL(label), user_data);

    }

  /* Make sure the dialog is visible. */
  gtk_window_present (GTK_WINDOW (achtung));
}


/**********************************************************************
 * --- Save Zettel (from Edit Window) = Write Data to g_data = RAM file
 ******************************************************************* */
void save_data (GtkWidget *edit_window)
{
	g_printf("+++ in save_data\n");
	GtkWidget *text, *button, *entry;	
	GtkTextBuffer *text_buffer;
	GtkTextIter start, end, where;
	char *data; // nur pointer, kein puffer
	char *tmp;
	char data1[200]; // puffer für Datum und ???
	long b_end, b_length;
	int i, feld;
	char c;

	zindex = eindex; // restore zindex, maybe moved during edit
	// --- if new Zettel appended ---
	if (neuer_zettel == TRUE) {
		satz++;
		zindex = satz;
		g_print("    save_data: New slip: Satz=%d - %s\n", satz, last_action);
		//mystr_revcut_at(g_datum_c, ' ');
		get_datum(g_datum_c); // get date yyyy-mm-dd hh:mm
		g_datum_c[strlen(g_datum_c)-6]=0;	// terminate before hour
		sprintf(g_datum_e, _("Erstellt: %s"), g_datum_c);
		button = lookup_widget (GTK_WIDGET (main_window), "main_btn_edit");
		gtk_widget_set_sensitive(button, TRUE);
		//g_print("    g_edit_backlink=%d - g_edit_template=%d\n", g_edit_backlink, g_edit_template);
		if (g_edit_backlink==TRUE || g_edit_template==TRUE) {
		//if (!strcmp(last_action, "newz_x") || !strcmp(last_action, "zneu_f")) {
			//if (mindex <= 0) mindex=zindex;
			g_printf("    save_data: in create X-Link: satz=%d mindex=%d\n", satz, mindex);
			satz--;	// satz is already 1 too big for memory moving in add_link
			add_link(mindex, zindex);
			satz++;
			//strcpy(last_action,"");
			g_edit_backlink = FALSE;
			g_edit_template = FALSE;
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
		g_printf ("*** disattach RAM from set %d - Begin=%d Lenght=%d\n", zindex+1, g_ptr_data, b_length);
		memcpy (tmp, &g_data[g_ptr_data], b_length);
	}

	g_ptr_data = zettel[zindex] [0]; // aktueller Eintrag zum Überschreiben
	g_printf ("    store set %d ab %d = %x\n", zindex, g_ptr_data, g_data[g_ptr_data]);

	/*** zetteltext ***/
	//text = lookup_widget (GTK_WIDGET (edit_window), "txt_e_zettel");
	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (sView));
	gtk_text_buffer_get_bounds (text_buffer, &start, &end);
	data = gtk_text_buffer_get_text (text_buffer, &start, &end, TRUE);
  
	/* crypting - encoding? */
  	strcpy (g_zeile, data);
	if (data[0]=='@') {
  		g_print("    Slip will be encoded\n");
  		if (strlen(g_password)==0) {	// password not set
  			g_print("    *** ask for password ***\n");
			dialog_password(g_password);
		}
  		mystr_cut_vorn(g_zeile, 1); // take off the @
  		int ret=strencode(g_zeile);
  		if(ret) { // encoding failed
  			g_print("    *** encoding failed - save as normal ***\n");
  			mystr_cut_vorn(g_zeile, 1); // save without encoding
  			//ret=strencode(g_zeile);
		}
	}
	move2g_data(g_zeile);
//  g_printf("data_text= %s - ptr_to %d\n", data, g_ptr_data);
	/* --- get cursor position --- */
	gtk_text_buffer_get_iter_at_mark(text_buffer, &where, gtk_text_buffer_get_insert (text_buffer));
	cursor_pos = gtk_text_iter_get_offset(&where);
	g_printf("    edit-->cursor_pos=%d\n", cursor_pos);

	/*** stichwort ***/
	text = lookup_widget (GTK_WIDGET (edit_window), "edit_txt_stichwort");
	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_text_buffer_get_bounds (text_buffer, &start, &end);
	data = gtk_text_buffer_get_text (text_buffer, &start, &end, TRUE);
	mystr_replace(data, '\n', ',');
	move2g_data(data);
//	g_printf("data_stich= %s - ptr_to %d\n", data, g_ptr_data);

	/*** autor ***/
	text = lookup_widget (GTK_WIDGET (edit_window), "edit_txt_autor");
	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_text_buffer_get_bounds (text_buffer, &start, &end);
	data = gtk_text_buffer_get_text (text_buffer, &start, &end, TRUE);
	move2g_data(data);
//	g_printf("data_autor= %s - ptr_to %d\n", data, g_ptr_data);

	/*** links ***/
	text = lookup_widget (GTK_WIDGET (edit_window), "edit_txt_link");
	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_text_buffer_get_bounds (text_buffer, &start, &end);
	data = gtk_text_buffer_get_text (text_buffer, &start, &end, TRUE);
	mystr_replace(data, '\n', ' ');
	move2g_data(data);
//	g_printf("data_link= %s - ptr_to %d\n", data, g_ptr_data);

	/*** anmerkung ***/
	text = lookup_widget (GTK_WIDGET (edit_window), "edit_txt_vermerk");
	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_text_buffer_get_bounds (text_buffer, &start, &end);
	data = gtk_text_buffer_get_text (text_buffer, &start, &end, TRUE);
	move2g_data(data);
//	g_printf("data_anmerk= %s - ptr_to %d\n", data, g_ptr_data);

	/*** Datum ***/
	//strcpy(data1, "Erstellt: Okt.2006"); // zur Korrektur von falschen Daten
	strcpy(data1, g_datum_e);
	strcat(data1, _(";Geändert: "));
	get_datum(g_datum_c); // get date yyyy-mm-dd hh:mm
	strcat(data1, g_datum_c);
	move2g_data(data1);
//	g_printf("data_datum= %s - ptr_to %d\n", data1, g_ptr_data);

	/*** externe Links ***/
	strcpy(data1, verweis2);
	text = lookup_widget (GTK_WIDGET (edit_window), "edit_txt_extern");
	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_text_buffer_get_bounds (text_buffer, &start, &end);
	data = gtk_text_buffer_get_text (text_buffer, &start, &end, TRUE);
	mystr_replace(data, '\n', ';');
	move2g_data(data);
//	g_printf("data_links= %s - ptr_to %d\n", data1, g_ptr_data);

	/*** Zettel-Titel ***/
	entry = lookup_widget (GTK_WIDGET (edit_window), "edit_entry_titel");
	// *char data = *gchar gtk... ergibt Warning
	data = gtk_entry_get_text(GTK_ENTRY(entry));
	move2g_data(data);
//	g_printf("data_head= %s - ptr_next %d\n", data, g_ptr_data);

	/* restore (attach) the rest of data-file and free up memory */
	/* letzte Speicheroperation setzt g_ptr_data */
	if (b_length != 0) {   
		g_printf ("    attach back RAM at %d - Begin=%d Lenght=%d\n", zindex+1, g_ptr_data, b_length);
		memcpy (&g_data[g_ptr_data], tmp, b_length);
	}
	free(tmp);
	reindex(zindex);
	file_changed = TRUE;
	text_changed = FALSE;
	neuer_zettel = FALSE;

	/* enable Save-Button */
	button = lookup_widget (GTK_WIDGET (main_window), "main_btn_save"); //save-button
	gtk_widget_set_sensitive(button, TRUE);
//  g_printf("file_changed %d - text_changed %d\n", file_changed, text_changed);
	feld = get_i_config("auto_save");
	if (feld && current_filename) {
		g_print("*** do autosave ***\n");
    	real_save_file (current_filename);
	}
	return;
}

/**********************************************************************
 * --- Load Zettel into Edit Window
 ******************************************************************* */
void
show_edit_zettel(GtkWidget *edit_window)
{
	GtkWidget *text, *statusbar, *entry;
	GtkTextBuffer *text_buffer;
	GtkTextBuffer *buffer;
	GtkTextIter start, end;
	GtkTextIter where;
	GtkTextMark *cursor;
	int c_pos;
	int i; // zettelnr;
	char help[100];
	int over_delete=0;

	g_printf("+++ in show_e_zettel\n");
	if (neuer_zettel == FALSE) { // alte Zettel und neu von Template
		if(text_changed) {
			g_printf("*** text changed\n");
			//on_Achtung_activate (menuitem, "Zettel nicht gespeichert\nÄnderungen verwerfen?");
			on_Achtung_activate (_("Zettel nicht gespeichert\nÄnderungen verwerfen?"));
			return;
		}
		eindex = zindex; // set edit index position
		//zettelnr=zindex;
		g_printf("+++ in show_e_zettel => new==FALSE\n");
	
		/* read cursor position from main Zettel */
		text = lookup_widget (GTK_WIDGET(main_window), "main_txt_text");
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		gtk_text_buffer_get_iter_at_mark (buffer, &where, gtk_text_buffer_get_insert (buffer));
		c_pos = gtk_text_iter_get_offset(&where);
		g_printf("c_pos=%d\n", c_pos);
		
		g_ptr_data = zettel[zindex] [0];
	//	g_printf("ed_zpoint=%d - zindex=%d\n  ", g_ptr_data, zindex);
		// --- Text ---
		get_g_data();
		if (g_zeile[0]=='@') {
			g_print("    Slip is encoded\n");
			int ret=strdecode(g_zeile);
			if(ret) {
				g_print("    *** editing not possible - enter password first ***\n");
				gtk_widget_destroy (GTK_WIDGET(edit_window));
				edit_window=NULL;
				error_msg(_("Editieren nicht möglich!\nBitte zuerst Password eingeben"));
				return;
			}
			c_pos++;
		}
		
		// --- korrigiere c_pos um <x>-tags ---
		c_pos = get_len(c_pos, 0);
		g_printf("c_pos=%d\n", c_pos);
		// --- schreibe in Zettel/Text Feld ---
		gtk_source_buffer_begin_not_undoable_action(sBuf);
			gtk_text_buffer_set_text (GTK_TEXT_BUFFER(sBuf), g_zeile, -1);
		gtk_source_buffer_end_not_undoable_action(sBuf);
		// --- setze cursor in edit window ---
		gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(sBuf), &where, 0);
		gtk_text_iter_forward_chars (&where, c_pos);
		gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(sBuf), &where);
		/* --- moves Cursor line in View --- */
		cursor=gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(sBuf), NULL, &where, FALSE);
		//gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (sView), cursor);
		//gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (sView), &where, 0.0, TRUE, 0.5, 0.5);
		gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (sView), cursor, 0.0, TRUE, 0.0, 0.5);

		// --- Stichwort ---;
		get_g_data();
		g_print("    g_zeile='%s'\n", g_zeile);
		if (!strcmp(g_zeile, "##")) { // "neu" über gelöschten zettel
			sprintf(g_datum_e, _("Erstellt: %s"), g_datum_c);
			over_delete=1;
		}
		//g_printf("stich=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
		mystr_replace(g_zeile, ',', '\n');
//		g_printf("    last_action: %s\n", last_action);
		if (g_edit_template==TRUE) {
		//if (!strcmp(last_action, "zneu_f")) {
			g_printf("    neu von Template\n");
			g_zeile[0]='%';
			//zettelnr=satz+1;
			eindex=satz+1;
			//mindex=eindex;
			neuer_zettel=TRUE;
			//g_edit_template=FALSE;
		}
		/* schreibe in stichwort Feld */
		text = lookup_widget (edit_window, "edit_txt_stichwort");
		text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		gtk_text_buffer_set_text (text_buffer, g_zeile, -1);

		// --- Autor ---
		get_g_data();
	//	g_printf("autor=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
		/* schreibe in autor Feld */
		text = lookup_widget (edit_window, "edit_txt_autor");
		text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		gtk_text_buffer_set_text (text_buffer, g_zeile, -1);

		// --- Links auf Zettel ---
		get_g_data();
		text = lookup_widget (edit_window, "edit_txt_link");
		text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		if (neuer_zettel) { // add "backlink" to template
			sprintf(help, " %d", zindex-1);
			strcat(g_zeile, help);
		}
		gtk_text_buffer_set_text (text_buffer, g_zeile, -1);
	//	g_printf("links=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);

		// --- Anmerkung - Vermerk ---
		get_g_data();
	//	g_printf("anmerk=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
		/* schreibe in vermerk Feld */
		text = lookup_widget (edit_window, "edit_txt_vermerk");
		text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		gtk_text_buffer_set_text (text_buffer, g_zeile, -1);

		// --- Datum ---
		get_g_data();
		//strcpy (verweis1, g_zeile);
		char dat_tok[]=";";
		char *sub_zeile;
		sub_zeile = strtok(g_zeile, dat_tok);
		i = 0;
		if (over_delete==0) {
			while (sub_zeile != NULL) {
		//		g_printf("datum = %s\n", sub_zeile);
				if(i==0) strcpy(g_datum_e, sub_zeile);  // Kopiert bis EX-Semikolon
				if(i==1) strcpy (verweis1, sub_zeile);
				sub_zeile = strtok(NULL, dat_tok);
				i++;
			}
		}
		//g_printf("xdatum: c=%s - e=%s\n", g_datum_e, verweis1);

		// --- externe Links ---
		get_g_data();
		//strcpy (verweis2, g_zeile);
	//	g_printf("verweis2=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
		/* schreibe in verweis Feld */
		mystr_replace(g_zeile, ';', '\n');
		text = lookup_widget (edit_window, "edit_txt_extern");
		text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
		gtk_text_buffer_set_text (text_buffer, g_zeile, -1);

		// --- Headline ---
		get_g_data();
	//	g_printf("head=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
		/* schreibe in Headline Feld */
		entry = lookup_widget (edit_window, "edit_entry_titel");
		gtk_entry_set_text (GTK_ENTRY(entry), g_zeile);
		
	} else { // neue Zettel
		g_printf("    neuer Zettel:last_action=%s\n", last_action);
		if (g_edit_backlink==TRUE) { // new with X-link
			//if (!strcmp(last_action, "newz_x")) { // neu mit X-link
			sprintf(help, "%d", zindex-1);
			g_printf("    in new slip with X-Link to %s\n", help);
			text = lookup_widget (edit_window, "edit_txt_link");
			text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
			gtk_text_buffer_set_text (text_buffer, help, -1);
			//g_edit_backlink=FALSE;
		}
		if (get_i_config("e_autor")==1) {
			text = lookup_widget (edit_window, "edit_txt_autor");
			text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
			sprintf(help, "%s, %s", g_autor, g_month);
			gtk_text_buffer_set_text (text_buffer, help, -1);
		}
		eindex=satz+1;
		g_printf("    neuer Zettel: index=%d\n", eindex);
	}
	//g_print ("---MARK---\n");
	// Ende !neuer Zettel
	GtkWidget *button, *menu;
	button = lookup_widget (edit_window, "edit_btn_undo");
	gtk_widget_set_sensitive(button, FALSE);
	button = lookup_widget (edit_window, "edit_btn_redo");
	gtk_widget_set_sensitive(button, FALSE);
	menu = lookup_widget (edit_window, "edit_mnu_undo");
	gtk_widget_set_sensitive(menu, FALSE);
	menu = lookup_widget (edit_window, "edit_mnu_redo");
	gtk_widget_set_sensitive(menu, FALSE);
	button = lookup_widget (edit_window, "edit_btn_save");
	gtk_widget_set_sensitive(button, FALSE);

	/* if current_file != NULL => check for *.zkn file */
	if (current_filename && current_filename[strlen(current_filename)-1]=='n') {
		button = lookup_widget (GTK_WIDGET (edit_window), "edit_mnu_ins_check");
		gtk_widget_set_sensitive(button, FALSE);
		button = lookup_widget (GTK_WIDGET (edit_window), "edit_mnu_ins_arrow");
		gtk_widget_set_sensitive(button, FALSE);
	}
	text_changed = FALSE;
	gtk_widget_grab_focus(sView);
		//gtk_source_buffer_end_not_undoable_action(text_buffer);

	set_window_e_title(eindex);
	sprintf(help, _("Bearbeite Zettel %d"), eindex);
	statusbar = lookup_widget (edit_window, "edit_appbar");
 	gnome_appbar_set_status (GNOME_APPBAR (statusbar), help);
//	eindex = zindex; // save edit position
	g_printf("    show_edit_zettel => END\n");
}


/**********************************************************************
 * --- fill the tree-view (zettel link-list) in main window 
 ******************************************************************* */
/*** --- populate link-list with lines --- ***/ 
void fill_tree_view_m()
{
	GtkTreeIter iter;
	int i, size;
	char suche[30];
	char line1buf[250];
	char line2buf[250];
	char pix[50];
	char *line1, *save_line1;
	char token[]=" ";
	char *line2, *save_line2;
	char token1[]=" ";
	GdkPixbuf *image;

	strcpy(pix, "");
	gtk_list_store_clear (store_m); 
//	g_printf("+++ in fill_tree_view_m - g_zeile= %s\n", g_zeile);
	size = strlen(g_zeile);
	if (size != 0)
	{
		strcpy(line1buf, g_zeile);
    	line1 = strtok_r(line1buf, token, &save_line1);
    	while (line1 != NULL) { // letzter Teil ist gesuchter String
    		i = atoi(line1); 
    		i++; // zettelkasten index zählt ab 0
    		line1 = strtok_r(NULL, token, &save_line1);
    		/* check for Back-Links */
    		g_ptr_data = zettel[i] [3];	// addressiert Links
    		get_g_data();
    		strcpy(line2buf, g_zeile);
    		line2 = strtok_r(line2buf, token, &save_line2);
			strcpy(pix, "");
    		while (line2 !=NULL) {
    			if (atoi(line2) == zindex-1) {
    				strcpy(pix, "ZMzettelkasten/stock_interaction.png");
    				break;
				}
    			line2 = strtok_r(NULL, token1, &save_line2);
    		} 
    		g_ptr_data = zettel[i] [1]; // addressiert Stichwort
    		get_g_data();
    		switch (g_zeile[0]) { // check for 1.Char in Stichwort 
				case '$': // template
					//if (!g_ov_active) add_i=!gtk_toggle_button_get_active (togglebutton);
					image = create_pixbuf("ZMzettelkasten/template.png");
					break;
				case '%': // copy of template
					image = create_pixbuf("ZMzettelkasten/list.png");
					break;
				case '#': // inactive
					//if (!g_ov_active) add_i=!gtk_toggle_button_get_active (togglebutton);
					image = create_pixbuf("ZMzettelkasten/done.png");
					break;
				case '!': // wichtig - todo
					image = create_pixbuf("ZMzettelkasten/todo.png");
					break;
				case '*': // idee
					image = create_pixbuf("ZMzettelkasten/idea.png");
					break;
				case '?': // info
					image = create_pixbuf("ZMzettelkasten/info.png");
					break;
				case '[': // book
					image = create_pixbuf("ZMzettelkasten/book.png");
					break;
				case ']': // text
					image = create_pixbuf("ZMzettelkasten/text.png");
					break;
				case '&': // person
					image = create_pixbuf("ZMzettelkasten/person.png");
					break;
    			default:
    				/* create_pixbuf is defined in support.c */
            		image = create_pixbuf(pix);
            }
   				
    		g_ptr_data = zettel[i] [0]; // addressiert Text
    		get_g_data();
    		if(g_zeile[0]=='@') {
				image = create_pixbuf("ZMzettelkasten/lock.png");
			}
			// --- Titel ---
			g_ptr_data = zettel[i] [7]; // Adressiert Zetteltitel
			get_g_data(); // holt Zetteltitel nach g_zeile
			/* --- Append a row and fill in the data --- */
//			g_printf("    append %d %s\n", i, g_zeile);
			gtk_list_store_append (store_m, &iter);
			gtk_list_store_set (store_m, &iter, COL_PIX, image, COL_ZETTEL, i, COL_TITEL, g_zeile, -1);
   		}
	}
	return;
}


/* --- create Tree_view (Link-Liste) incl. headline --- */
GtkWidget * create_view_and_model_m (GtkWidget *view)
{
  GtkTreeViewColumn   *col;
  GtkCellRenderer     *renderer;

	/* --- Column #1  - icon --- */
	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1,
										NULL, renderer,		// keine Überschrift
										"pixbuf", COL_PIX, NULL);

	/* --- Column #2 - Zettelnr. --- */
	renderer = gtk_cell_renderer_text_new ();
	col = gtk_tree_view_column_new_with_attributes (_("Zettel"), renderer,
                                               "text", COL_ZETTEL, NULL);
	gtk_tree_view_column_set_sort_column_id (col, COL_ZETTEL);
	//gtk_tree_view_column_set_alignment (col, 0.0); // sollte gehen
	//gtk_tree_view_column_set_fixed_width (col, 100); // tut aber nicht
	gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);

	/* --- Column #3 - Zettel-Titel --- */
	renderer = gtk_cell_renderer_text_new ();
	col = gtk_tree_view_column_new_with_attributes (_("Titel"),  renderer,
                                               "text", COL_TITEL, NULL);
	gtk_tree_view_column_set_sort_column_id (col, COL_TITEL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);

	/* create store */
	store_m = gtk_list_store_new (NUM_COLS,  GDK_TYPE_PIXBUF,  G_TYPE_UINT, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(store_m));
	//gtk_widget_set_style (view, "vertical-separator");
	g_object_unref (store_m); /* destroy store automatically with view */

	return view;
}

/**********************************************************************
 * --- External Link Liste
 ******************************************************************** */
/* --- create Tree_view (Popup External-Link-Liste) incl. headline --- */
GtkWidget * create_view_and_model_ext (GtkWidget *view)
{
	GtkTreeViewColumn   *col;
	GtkCellRenderer     *renderer;

	if (store_ext_val == FALSE) { /* needed to avoid double columns
		* since popup destroy may cause core dumps ...
		* - and popup hide does not clean columns*/ 
		/* --- Column #1 --- */
		renderer = gtk_cell_renderer_text_new ();
		gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), -1,
												   _("External Links"), renderer,
												   "text", COL_TITEL,
												   NULL);
		store_ext_val = TRUE;
	}

	store_ext = gtk_list_store_new (NUM_COLS, GDK_TYPE_PIXBUF,  G_TYPE_UINT, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(store_ext));
	g_object_unref (store_ext); /* destroy store automatically with view */

	return view;
}


void fill_tree_view_ext()
{
	GtkTreeIter iter;
	int i, size;
	char suche[30];
	char line1[250];
	char name[200];
	//char fname[200];
	char *line;
	char token[]=";";

	gtk_list_store_clear (store_ext); 
	g_ptr_data = zettel[zindex] [6];
	get_g_data();
	g_printf("+++ in fill_tree_view_ext, ext=%s\n", g_zeile);
	
	size = strlen(g_zeile);
	if (size != 0)
	{
		//strcpy(line1, g_zeile);
    	//line = strtok(line1, token);
    	line = strtok(g_zeile, token);
    	while (line != NULL) { // Zerlege bei ; in Zeilen
    		g_printf("    z_index= '%s'\n", line);
			if (strncmp(line, "-z", 2)==0) {
				g_printf ("Zettelkasten %s\n", line);
				strcpy(name, line);
				get_head_line(name);
				g_print("    returned %s\n", name);
				sprintf(line1, "%s => %s", line, name);
			} else {
				strcpy(line1, line);
			}
			gtk_list_store_append (store_ext, &iter);
			gtk_list_store_set (store_ext, &iter, COL_TITEL, line1, -1);
    		line = strtok(NULL, token);
    	}
	}
	return;
}


void show_ext_links(GtkWidget *link_window)
{
	g_print("+++ in show_ext_links\n");
	GtkWidget *liste, *view, *label;

	label = lookup_widget (link_window, "ext_link_lbl");
	//gtk_label_set_text (GTK_LABEL(label), "");
	gtk_widget_hide(label);

	liste = lookup_widget (link_window, "ext_link_liste");
	view = create_view_and_model_ext(liste);
	fill_tree_view_ext();
	strcpy(last_action, "external");
	// --- set Window Titel Zeile ---
	//gtk_window_set_title (GTK_WINDOW (link_window), "External Links");
}


/**********************************************************************
 * --- Template Auswahl Liste
 ******************************************************************** */
// --- create Tree_view (Popup Template Auswahl) incl. headline ---
GtkWidget * create_view_and_model_temp (GtkWidget *view)
{
	GtkTreeViewColumn   *col;
	GtkCellRenderer     *renderer;
	GtkTreeIter iter;

	if (store_temp_val == FALSE) { /* needed to avoid double columns
		* since popup destroy may cause core dumps ...
		* - and popup hide does not clean columns*/ 
		/* --- Column #1 - Zettelnr. --- */
		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new_with_attributes ("Nr.", renderer,
												   "text", COL_ZETTEL, NULL);
		gtk_tree_view_column_set_sort_column_id (col, COL_ZETTEL);
		gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);

		/* --- Column #2 - Zettel-Titel --- */
		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new_with_attributes ("Template",  renderer,
												   "text", COL_TITEL, NULL);
		gtk_tree_view_column_set_sort_column_id (col, COL_TITEL);
		gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);
		store_temp_val = TRUE;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW (view), NULL);
	store_temp = gtk_list_store_new (NUM_COLS, GDK_TYPE_PIXBUF, G_TYPE_UINT, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(store_temp));
	g_object_unref (store_temp); /* destroy store automatically with view */
	
	//GtkWidget *label=lookup_widget(ext_link_window, "ext_link_lbl");
	//gtk_widget_hide(label);
	return view;
}

void fill_tree_view_temp()
{
	g_printf("+++ in fill_tree_view_temp\n");
	GtkTreeIter iter;
//	GdkPixbuf *image;
	int i, size;

	gtk_list_store_clear (store_temp);
	// von 1 bis satz
//    image = create_pixbuf("ZMzettelkasten/stock_new-template.png");
	for (i=1; i<=satz; i++) {
		g_ptr_data = zettel[i] [1];
		get_g_data();
		if (g_zeile[0] == '$') {
			g_printf("     found template at Zettel %d\n", i);
    		g_ptr_data = zettel[i] [7]; // Zettel Titel
    		get_g_data();
 			g_printf("     - %s\n", g_zeile);
 			gtk_list_store_append (store_temp, &iter);
			//gtk_list_store_set (store_ext, &iter, COL_TITEL, g_zeile, -1);
			gtk_list_store_set (store_temp, &iter, COL_ZETTEL, i, COL_TITEL, g_zeile, -1);
		}
	}
	//g_printf("ext=%s\n", g_zeile);
	return;
}

/* create template selection popup */
void show_zneu_temp(GtkWidget *link_window)
{
	GtkWidget *liste, *view;

	liste = lookup_widget (link_window, "ext_link_liste");
	view = create_view_and_model_temp(liste);
	fill_tree_view_temp();
	strcpy(last_action,"template");
}


int find_in_header (char *search, int dir)
{
	int i, beg, ret;
	
//	g_printf("+++ in find_in_header:%s -dir=%d\n", search, dir);
	if (dir < 1) { // search backwards
		for (i=zindex-1; i>0; i--) {
			g_ptr_data = zettel[i] [7]; // Adressiert Zetteltitel
			get_g_data(); // holt Zetteltitel nach g_zeile
			ret=mystr_such(g_zeile, search);
			if (ret) {
//				g_printf("    found- %s in Zettel %d\n", search, i);
				return(i);
			}
		}
	}
	/* if dir=2 first loop runs from current zindex
	 * second loop starts at first zettel */
	beg=zindex+1;
	while (dir > 0) {
		for (i=beg; i<=satz; i++) {
			g_ptr_data = zettel[i] [7]; // Adressiert Zetteltitel
			get_g_data(); // holt Zetteltitel nach g_zeile
			ret=mystr_such(g_zeile, search);
			if (ret) {
//				g_printf("    found+ %s in Zettel %d\n", search, i);
				return(i);
			}
		}
		dir--;
		beg=1;
	}
	return(-1);
}


/**********************************************************************
 * --- Input Dialog.
 ******************************************************************** */
void
on_input_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	g_printf("+++ in on_input_activate: dialog: %s\n", user_data);
	static GtkWidget *input_box = NULL;
	GtkWidget *label;

	if (input_box == NULL) 
	{
      input_box = GTK_WIDGET(create_pu_input_box ());
      /* set the widget pointer to NULL when the widget is destroyed */
		g_signal_connect (G_OBJECT (input_box), "destroy",
			G_CALLBACK (gtk_widget_destroyed), &input_box);

		label = lookup_widget (input_box, "pu_input_box_label");
		gtk_label_set_text (GTK_LABEL(label), user_data);

    }

  /* Make sure the dialog is visible. */
  gtk_window_present (GTK_WINDOW (input_box));
}


/**********************************************************************
 *  ---- edit Window stuff
 ******************************************************************* */
void edit_text_changed                 (GtkTextBuffer   *buffer,
                                        gpointer         user_data)
/* Enter here if txt fields changed */
{
//	g_printf("+++ in edit_text_changed\n");
	GtkWidget *button;
	text_changed = TRUE;
	button = lookup_widget (GTK_WIDGET (edit_window), "edit_btn_save");
	gtk_widget_set_sensitive(button, TRUE);
}

void edit_sv_text_changed              (GtkTextBuffer   *buffer,
                                        gpointer         user_data)
/* Enter here if sView/sBuf changed */
{
//	g_printf("+++ in edit_sv_text_changed\n");
	int undo;
	float filled, size;
	char help[100];
	GtkWidget *button, *menu;
	GnomeAppBar *statusbar;

	undo = gtk_source_buffer_can_undo (sBuf);
//	g_printf("    can undo %d\n", undo);
	button = lookup_widget (GTK_WIDGET (edit_window), "edit_btn_undo");
	gtk_widget_set_sensitive(button, undo);
	menu = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_undo");
	gtk_widget_set_sensitive(menu, undo);
	button = lookup_widget (GTK_WIDGET (edit_window), "edit_btn_save");
	gtk_widget_set_sensitive(button, TRUE);
	button = lookup_widget (GTK_WIDGET (edit_window), "edit_btn_redo");
	gtk_widget_set_sensitive(button, FALSE);
	menu = lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_redo");
	gtk_widget_set_sensitive(menu, FALSE);
	size = gtk_text_buffer_get_char_count(buffer);
	filled=size/g_maxzeile;
//	g_printf("    size=%.0f - filled=%f\n", size, filled);
	sprintf(help, _("Bearbeite Zettel %d - %.0f Zeichen"), eindex, size); 
	statusbar=GNOME_APPBAR(lookup_widget(GTK_WIDGET(edit_window), "edit_appbar"));
	gnome_appbar_set_status(statusbar, help);
 	gnome_appbar_set_progress_percentage(statusbar, filled);
	text_changed = TRUE;
}

void create_myedit_window ()
{
	g_printf("+++ in create_myedit_window\n");
	static GtkWidget *pScrollWin; // *sView;
	static PangoFontDescription *font_desc;
	static GtkSourceLanguagesManager *lm;
	GtkSourceLanguage *language = NULL;
	GtkWidget *txt_autor, *txt_stichwort, *txt_link, *txt_vermerk, *txt_extern;
	GtkTextBuffer *buf_autor, *buf_stichwort, *buf_link, *buf_vermerk, *buf_extern;
	GtkCheckMenuItem *item;
	char font[30];
	int ret;

//g_printf("Testmarke vorn\n");
	if (edit_window == NULL) {
		edit_window = GTK_WINDOW(create_edit_window ());
	}
//g_printf("Testmarke hinten\n");
	/* set the widget pointer to NULL when the widget is destroyed */
	g_signal_connect (G_OBJECT (edit_window), "destroy",
					  G_CALLBACK (gtk_widget_destroyed), &edit_window);

	/* Make sure the window doesn't disappear behind the main window. */
	//gtk_window_set_transient_for (GTK_WINDOW (edit_window), GTK_WINDOW (main_window));
	/* Allow user to resize the dialog */
	gtk_window_set_resizable (GTK_WINDOW (edit_window), TRUE);

	/* place GtkSourceView in edit_scrollwin1 */
	pScrollWin = lookup_widget (GTK_WIDGET(edit_window), "edit_scrollwin1");

	/* Now create a GtkSourceLanguagesManager */
	lm = gtk_source_languages_manager_new();

	/* and a GtkSourceBuffer to hold text (similar to GtkTextBuffer) */
	sBuf = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));
	g_object_ref (lm);
	g_object_set_data_full ( G_OBJECT (sBuf), "languages-manager",
							 lm, (GDestroyNotify) g_object_unref);

	/* Create the GtkSourceView and associate it with the buffer */
	sView = gtk_source_view_new_with_buffer(sBuf);
	g_signal_connect (sBuf, "changed", G_CALLBACK (edit_sv_text_changed), NULL);

	/* set markup language & highlight - zettel.lang shoul be installed */
	language = gtk_source_languages_manager_get_language_from_mime_type (lm, "text/zettel");
	g_print("Language: [%s]\n", gtk_source_language_get_name(language));
	gtk_source_buffer_set_language  (sBuf, language);
//will be set later on    g_object_set (G_OBJECT (sBuf), "highlight", TRUE, NULL);

	/* Set default Font name,size */
	ret=get_config(font, "e_font");
	if (ret) { strcpy(font, "courier 10"); }
	g_printf("    font=%s\n", font);

	font_desc = pango_font_description_from_string (font);
	gtk_widget_modify_font (sView, font_desc);
	pango_font_description_free (font_desc);

	/* Attach Spell Check */
	int lang=get_i_config("e_spell");
	g_print("    spell=%d\n", lang);
	switch (lang) {
	case (1):
		item = GTK_CHECK_MENU_ITEM(lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_sp_default"));
		gtk_check_menu_item_set_active (item, TRUE);
		break;
	case (2):
		item = GTK_CHECK_MENU_ITEM(lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_sp_german"));
		gtk_check_menu_item_set_active (item, TRUE);
		g_print("    set spell to german\n");
		break;
	case (3):
		item = GTK_CHECK_MENU_ITEM(lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_sp_english"));
		gtk_check_menu_item_set_active (item, TRUE);
		break;
	}	
	/* Attach the GtkSourceView to the scrolled Window */
	gtk_container_add (GTK_CONTAINER (pScrollWin), GTK_WIDGET (sView));
	gtk_widget_show_all (pScrollWin);
	
	// --- set some GtkSourceView defaults ---
	int val = get_i_config("e_margin");
	if (val != 0) {
		gtk_source_view_set_margin (GTK_SOURCE_VIEW(sView), val);
		gtk_source_view_set_show_margin (GTK_SOURCE_VIEW(sView), TRUE);
	}
	gtk_source_view_set_highlight_current_line (GTK_SOURCE_VIEW(sView), TRUE);
	gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW(sView), TRUE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(sView), GTK_WRAP_WORD);
	
	/* attach buffer to normal txt views */		
	txt_autor = lookup_widget (GTK_WIDGET(edit_window), "edit_txt_autor");
	buf_autor = gtk_text_view_get_buffer (GTK_TEXT_VIEW (txt_autor));
	g_signal_connect (buf_autor, "changed", G_CALLBACK (edit_text_changed), NULL);
	txt_stichwort = lookup_widget (GTK_WIDGET(edit_window), "edit_txt_stichwort");
	buf_stichwort = gtk_text_view_get_buffer (GTK_TEXT_VIEW (txt_stichwort));
	g_signal_connect (buf_stichwort, "changed", G_CALLBACK (edit_text_changed), NULL);
	txt_link = lookup_widget (GTK_WIDGET(edit_window), "edit_txt_link");
	buf_link = gtk_text_view_get_buffer (GTK_TEXT_VIEW (txt_link));
	g_signal_connect (buf_link, "changed", G_CALLBACK (edit_text_changed), NULL);
	txt_vermerk = lookup_widget (GTK_WIDGET(edit_window), "edit_txt_vermerk");
	buf_vermerk = gtk_text_view_get_buffer (GTK_TEXT_VIEW (txt_vermerk));
	g_signal_connect (buf_vermerk, "changed", G_CALLBACK (edit_text_changed), NULL);
//		text = lookup_widget (edit_window, "edit_txt_extern");
	txt_vermerk = lookup_widget (GTK_WIDGET(edit_window), "edit_txt_extern");
	buf_extern = gtk_text_view_get_buffer (GTK_TEXT_VIEW (txt_vermerk));
	g_signal_connect (buf_extern, "changed", G_CALLBACK (edit_text_changed), NULL);

	//gtk_window_present (GTK_WINDOW (edit_window));
	/* edit window position and sizes */
	GtkWidget *paned;
	int left, top, width, hight;
	int vpan;
	
	left = get_i_config("e_left");
	top  = get_i_config("e_top");
	width = get_i_config("e_width");
	hight = get_i_config("e_hight");
	if (left >= 0 && top >= 0) {
		g_printf("left=%d - top=%d\n", left, top);
		gtk_window_move(GTK_WINDOW(edit_window), left, top);
	}
	if (width <= 10) width = 600;
	if (hight <= 10) hight = 400;
	//g_printf("width=%d - hight=%d\n", width, hight);
	gtk_window_resize(GTK_WINDOW(edit_window), width, hight);

	vpan = get_i_config("e_vpan1");
	if (vpan != 0) {
		paned = lookup_widget (GTK_WIDGET (edit_window), "edit_vpaned1");
		gtk_paned_set_position(GTK_PANED (paned), vpan);
		//g_printf("Location vpan1=%d\n", vpan);
	}
	vpan = get_i_config("e_hpan1");
	if (vpan != 0) {
		paned = lookup_widget (GTK_WIDGET (edit_window), "edit_hpaned1");
		gtk_paned_set_position(GTK_PANED (paned), vpan);
		//g_printf("Location hpan1=%d\n", vpan);
	}
	vpan = get_i_config("toolbar");
	if (vpan==1) {
		paned = lookup_widget (GTK_WIDGET (edit_window), "edit_toolbar");
		gtk_toolbar_set_style (GTK_TOOLBAR (paned), GTK_TOOLBAR_ICONS);
	}
	if (vpan==2) {
		paned = lookup_widget (GTK_WIDGET (edit_window), "edit_toolbar");
		gtk_toolbar_set_style (GTK_TOOLBAR (paned), GTK_TOOLBAR_TEXT);
	}
	/* set line numbering */
	vpan = get_i_config("e_lineno");
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(sView), vpan);
	item = GTK_CHECK_MENU_ITEM(lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_lineno"));
	gtk_check_menu_item_set_active (item, vpan);
	/* set syntax checking */	
	vpan = get_i_config("e_syntax");
    g_object_set (G_OBJECT (sBuf), "highlight", vpan, NULL);
	item = GTK_CHECK_MENU_ITEM(lookup_widget(GTK_WIDGET(edit_window), "edit_mnu_syntax"));
	gtk_check_menu_item_set_active (item, vpan);	
	/* Spell Checker */
	//gtkspell_new_attach(GTK_TEXT_VIEW(sView), NULL, NULL);

}

/**********************************************************************
 *  ---- Index Window stuff
 ******************************************************************* */
/* --- populate tree view with lines --- */ 
void fill_tree_view_index()
{
  GtkTreeIter    iter;
  int i, size, add_s, add_t, add_a, add_i, add_n, add_h;
  int j;
  int qty=0;
  char suche[100];
  char help[50];
	GdkPixbuf *image;
	GtkToggleButton *togglebutton;
	GtkWidget *statusbar;

	togglebutton = GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(main_window), "main_cbtn_inactive"));
  for (i=1; i<=satz; i++) {
	add_a = 1; // autor
	add_s = 1; // stichwort
	add_t = 1; // text
	add_n = 1; // note/anmerkung
	add_h = 1; // titel/head
	add_i = 0; // inactive

    // --- check for for icon and inactive first ---
    j = indexz[i];
    if (j==0) break;
   	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
   	size = get_g_data();  // holt Stichworte nach g_zeile
	switch (g_zeile[0]) { // check for 1.Char in Stichwort
		case '$': // template
			if (!g_ov_active) add_i=!gtk_toggle_button_get_active (togglebutton);
			image = create_pixbuf("ZMzettelkasten/template.png");
			break;
		case '%': // liste - copy of template 
			//if (!g_ov_active) add_i=!gtk_toggle_button_get_active (togglebutton);
			image = create_pixbuf("ZMzettelkasten/list.png");
			break;
		case '#': // inactive
			if (!g_ov_active) add_i=!gtk_toggle_button_get_active (togglebutton);
			image = create_pixbuf("ZMzettelkasten/done.png");
			break;
		case '!': // wichtig - todo
			image = create_pixbuf("ZMzettelkasten/todo.png");
			break;
		case '?': // info
			image = create_pixbuf("ZMzettelkasten/info.png");
			break;
		case '*': // idee
			image = create_pixbuf("ZMzettelkasten/idea.png");
			break;
		case '[': // book
			image = create_pixbuf("ZMzettelkasten/book.png");
			break;
		case ']': // book
			image = create_pixbuf("ZMzettelkasten/text.png");
			break;
		case '&': // person
			image = create_pixbuf("ZMzettelkasten/person.png");
			break;
		default:
			/* create_pixbuf is defined in support.c */
			image = create_pixbuf("");
	}
	g_ptr_data = zettel[j] [0]; // addressiert Text
	get_g_data();
	/* --- check for keyed --- */
	if(g_zeile[0]=='@') {
		image = create_pixbuf("ZMzettelkasten/lock.png");
	}
    // --- Stichwort Filter ---
    if (strlen(g_s_stich)!=0) {
    	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
    	size = get_g_data(); // holt Stichworte nach g_zeile
    	add_s = mystr_such(g_zeile, g_s_stich);
    	add_i=0;	// overwrite inactive
    	//g_printf("Stich=%s - %d\n", g_zeile, add_s);
    }
    // --- Wort Filter ---
    if (strlen(g_s_text)!=0 && show_text==TRUE) {
    	size = g_ptr_data = zettel[j] [0]; // Adressiert Zetteltext
    	size = get_g_data(); // holt Text nach g_zeile
    	add_t = mystr_such(g_zeile, g_s_text);
    	add_i=0;	// overwrite inactive
    	//g_printf("Text=%s - %d\n", g_zeile, add_t);
    }
    // --- Autoren Filter ---
    if (strlen(g_s_autor)!=0 && show_autor==TRUE) {
    	size = g_ptr_data = zettel[j] [2]; // Adressiert Autor
    	size = get_g_data(); // holt Autor nach g_zeile
    	add_t = mystr_such(g_zeile, g_s_autor);
    	add_i=0;	// overwrite inactive
    	g_printf("    Autor=%s - %d\n", g_zeile, add_t);
    }
    // --- Note/Anmerkung Filter ---
    if (strlen(g_s_note)!=0 && show_note==TRUE) {
    	size = g_ptr_data = zettel[j] [4]; // Adressiert Anmerkung
    	size = get_g_data(); // holt Anmerkung nach g_zeile
    	add_n = mystr_such(g_zeile, g_s_note);
    	add_i=0;	// overwrite inactive
    	//g_printf("    Note=%s - %d\n", g_zeile, add_n);
    }
    // --- Titel Filter ---
    if (strlen(g_s_titel)!=0 && show_titel==TRUE) {
    	size = g_ptr_data = zettel[j] [7]; // Adressiert Titel
    	size = get_g_data(); // holt Titel nach g_zeile
    	add_h = mystr_such(g_zeile, g_s_titel);
    	add_i=0;	// overwrite inactive
    	//g_printf("    Titel=%s - %d\n", g_zeile, add_h);
    }
    // --- alle hidden Zettel 
    if (g_list_hidden) {
    	add_s=0;
    	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
    	size = get_g_data(); // holt Stichworte nach g_zeile
    	if (g_zeile[0]=='#') add_s=1;
    	if (g_zeile[0]=='$') add_s=1;
    	add_i=0;
    }
    // --- alle todo Zettel 
    if (g_list_todo) {
    	add_s=0;
    	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
    	size = get_g_data(); // holt Stichworte nach g_zeile
    	if (g_zeile[0]=='!') add_s=1;
    	add_i=0;
    }
    // --- alle info Zettel 
    if (g_list_info) {
    	add_s=0;
    	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
    	size = get_g_data(); // holt Stichworte nach g_zeile
    	if (g_zeile[0]=='?') add_s=1;
    	add_i=0;
    }
    // --- alle idea Zettel 
    if (g_list_idea) {
    	add_s=0;
    	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
    	size = get_g_data(); // holt Stichworte nach g_zeile
    	if (g_zeile[0]=='*') add_s=1;
    	add_i=0;
    }
    // --- alle list Zettel 
    if (g_list_list) {
    	add_s=0;
    	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
    	size = get_g_data(); // holt Stichworte nach g_zeile
    	if (g_zeile[0]=='%') add_s=1;
    	add_i=0;
    }
	// --- alle people Zettel 
    if (g_list_people) {
    	add_s=0;
    	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
    	size = get_g_data(); // holt Stichworte nach g_zeile
    	if (g_zeile[0]=='&') add_s=1;
    	add_i=0;
    }
    // --- alle book Zettel 
    if (g_list_book) {
    	add_s=0;
    	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
    	size = get_g_data(); // holt Stichworte nach g_zeile
    	if (g_zeile[0]=='[') add_s=1;
    	add_i=0;
    }
    // --- alle text Zettel 
    if (g_list_text) {
    	add_s=0;
    	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
    	size = get_g_data(); // holt Stichworte nach g_zeile
    	if (g_zeile[0]==']') add_s=1;
    	add_i=0;
    }
    // --- alle template Zettel 
    if (g_list_template) {
    	add_s=0;
    	size = g_ptr_data = zettel[j] [1]; // Adressiert Stichwort
    	size = get_g_data(); // holt Stichworte nach g_zeile
    	if (g_zeile[0]=='$') add_s=1;
    	add_i=0;
    }

    // --- alle verschlüsselten Zettel 
    if (g_list_keyed) {
    	add_s=0;
    	size = g_ptr_data = zettel[j] [0]; // Adressiert Text
    	size = get_g_data();
    	if (g_zeile[0]=='@') add_s=1;
    	add_i=0;
    }
    // --- alle Titel --- 
    size = g_ptr_data = zettel[j] [7]; // Adressiert Zetteltitel
    size = get_g_data(); // holt Zetteltitel nach g_zeile
    /* --- Append a row and fill in the data --- */
    //g_print("    %4d: add_a=%d, add_s=%d, add_t=%d add_n=%d, add_h=%d, add_i=%d\n", j, add_a, add_s, add_t, add_n, add_h, add_i);
    if(add_a!=0 && add_s!=0 && add_t!=0 && add_n!=0 && add_h!=0 && add_i==0) {
    	qty++;
    	gtk_list_store_append (store_index, &iter);
    	gtk_list_store_set (store_index, &iter, COL_PIX, image, COL_ZETTEL, j, COL_TITEL, g_zeile, -1);
    }
  }
	g_ov_active=FALSE;
	g_list_hidden=FALSE;
	g_list_todo=FALSE;
	g_list_info=FALSE;
	g_list_idea=FALSE;
	g_list_list=FALSE;
	g_list_people=FALSE;
	g_list_keyed=FALSE;
	g_list_book=FALSE;
	g_list_text=FALSE;
	g_list_template=FALSE;
	sprintf(help, _("%d Zettel gefunden"), qty);
//  	g_printf("%s\n", help);
	statusbar = lookup_widget (GTK_WIDGET(index_window), "index_appbar");
	gnome_appbar_set_status (GNOME_APPBAR (statusbar), help);
  return;
}

// --- fill lines into Tree_view (Such-Liste) ---
static GtkTreeModel *
create_and_fill_model_index (void)
{
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *menu;

	store_index = gtk_list_store_new (NUM_COLS, GDK_TYPE_PIXBUF, G_TYPE_UINT, G_TYPE_STRING);

	// --- clean fields for first view ---
	strcpy(g_s_stich, "");
	strcpy(g_s_text, "");
	if(show_text) {
		menu = lookup_widget (GTK_WIDGET (index_window), "index_mnu_text");
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(menu), 1);
		entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_text");
		label = lookup_widget (GTK_WIDGET (index_window), "index_lbl_text");
		gtk_widget_show (GTK_WIDGET (entry));
		gtk_widget_show (GTK_WIDGET (label));
	}
	if(show_autor) {
		menu = lookup_widget (GTK_WIDGET (index_window), "index_mnu_autor");
		gtk_check_menu_item_set_active  (GTK_CHECK_MENU_ITEM(menu), 1);
		entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_autor");
		label = lookup_widget (GTK_WIDGET (index_window), "index_lbl_autor");
		gtk_widget_show (GTK_WIDGET (entry));
		gtk_widget_show (GTK_WIDGET (label));
	}
	if(show_note) {
		menu = lookup_widget (GTK_WIDGET (index_window), "index_mnu_note");
		gtk_check_menu_item_set_active  (GTK_CHECK_MENU_ITEM(menu), 1);
		entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_note");
		label = lookup_widget (GTK_WIDGET (index_window), "index_lbl_note");
		gtk_widget_show (GTK_WIDGET (entry));
		gtk_widget_show (GTK_WIDGET (label));
	}
	if(show_titel) {
		menu = lookup_widget (GTK_WIDGET (index_window), "index_mnu_titel");
		gtk_check_menu_item_set_active  (GTK_CHECK_MENU_ITEM(menu), 1);
		entry = lookup_widget (GTK_WIDGET (index_window), "index_entry_titel");
		label = lookup_widget (GTK_WIDGET (index_window), "index_lbl_titel");
		gtk_widget_show (GTK_WIDGET (entry));
		gtk_widget_show (GTK_WIDGET (label));
	}
//	fill_tree_view_index();
	return GTK_TREE_MODEL (store_index);
}


// --- create Tree_view (Index - Such Liste) incl. headline ---
static GtkWidget *
create_view_and_model_index (GtkWidget *view)
{
  GtkTreeViewColumn   *col;
  GtkCellRenderer     *renderer;
  GtkTreeModel        *model;

	if (store_index_val == FALSE) { /* needed to avoid double columns
		* since popup destroy may cause core dumps ...
		* - and popup hide does not clean columns*/ 
		/* --- Column #1  - icon --- */
		renderer = gtk_cell_renderer_pixbuf_new();
		gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1,
											NULL, renderer,		// keine Überschrift
											"pixbuf", COL_PIX, NULL);

		/* --- Column #2 - Zettelnr. --- */
		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new_with_attributes (_("Zettel"), renderer,
												   "text", COL_ZETTEL, NULL);
		gtk_tree_view_column_set_sort_column_id (col, COL_ZETTEL);
		//gtk_tree_view_column_set_alignment (col, 0.0); // sollte gehen
		//gtk_tree_view_column_set_fixed_width (col, 100); // tut aber nicht
		gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);

		/* --- Column #3 - Zettel-Titel --- */
		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new_with_attributes (_("Titel"),  renderer,
												   "text", COL_TITEL, NULL);
		gtk_tree_view_column_set_sort_column_id (col, COL_TITEL);
		gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);
		
		store_index_val = TRUE;
	}
	model = create_and_fill_model_index ();
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
	//GtkTreeSelection *selection = gtk_tree_view_get_selection (view);
  	//gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
	g_object_unref (model); /* destroy model automatically with view */

  return view;
}

/**********************************************************************
 * list window - autoren, stichworte
 ******************************************************************* */
void show_index_titel(char *header)
{
	g_printf("+++ in show_index_titel\n");
	g_printf("    header: %s\n", header);
	GtkWidget *liste, *label, *view, *toolbar;
	int i, icon;

	if (index_window == NULL) { // create window if not exist
		index_window = GTK_WINDOW(create_index_window ());
		/* set the widget pointer to NULL when the widget is destroyed */
		g_signal_connect (G_OBJECT (index_window), "destroy",
						  G_CALLBACK (gtk_widget_destroyed), &index_window);

		/* Make sure the window doesn't disappear behind the main window. */
		gtk_window_set_transient_for (GTK_WINDOW (index_window), GTK_WINDOW (main_window));
		/* Allow user to resize the window */
		gtk_window_set_resizable (GTK_WINDOW (index_window), TRUE);

		/* --- restore Index Window geometrie --- */
		int left, top, width, hight;
		left = get_i_config("i_left");
		top  = get_i_config("i_top");
		width = get_i_config("i_width");
		hight = get_i_config("i_hight");
		if(offset) {
			left=left-g_offset_x;
			top=top+g_offset_y;
		}
		// follow the main window
		int follow, mleft, mtop, mwidth, mhight;
		follow = get_i_config("follow");
		if (follow > 0) {
			g_print("    follow=%d\n", follow);
			gtk_window_get_position(GTK_WINDOW (main_window), &mleft, &mtop);	
			gtk_window_get_size(GTK_WINDOW (main_window), &mwidth, &mhight);
			top=mtop;
			left=mleft+mwidth+follow;
			hight=mhight;
		}
		if (left >= 0 && top >= 0) {
			gtk_window_move(GTK_WINDOW (index_window), left, top);
		}
		if (width <= 10) width = 300;
		if (hight <= 10) hight = 400;
		icon = get_i_config("toolbar");
		if (icon!=0) {
			toolbar = lookup_widget (GTK_WIDGET (index_window), "index_toolbar");
			gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
		}
		//g_printf("width=%d - hight=%d\n", width, hight);
		gtk_window_resize(GTK_WINDOW (index_window), width, hight);
	}
	
	/* Make sure the window is visible. */
	gtk_window_present (GTK_WINDOW (index_window));

	liste = lookup_widget (GTK_WIDGET(index_window), "index_list");
	view = create_view_and_model_index(liste);
	label=lookup_widget(GTK_WIDGET(index_window), "index_lbl_info");
	char titel[100];
	sprintf(titel, "<b>%s</b>", header);
	gtk_label_set_markup (GTK_LABEL(label), titel);
	if (!strcmp(header, "Zettelkasten - Index")) { // initial setup
		g_printf("    in Kompletter Kasten\n");
		clear_indexz();
		for (i=0; i<=satz; i++) {
			indexz[i] = i;
		}
	}
	
	fill_tree_view_index();
}


// --- create Tree_view (Autor/Stichwort Liste) incl. headline ---
GtkWidget * create_view_and_model_liste (GtkWidget *view, char *header)
{
	GtkTreeViewColumn   *col;
	GtkCellRenderer     *renderer;
	GtkTreeIter iter;

	if (store_liste_val == FALSE) { /* needed to avoid double columns
		* since popup destroy may cause core dumps ...
		* - and popup hide does not clean columns*/ 
		/* --- Column #1 - Anzahl. --- */
		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new_with_attributes (_("Anz."), renderer,
												   "text", COL_ZETTEL, NULL);
		gtk_tree_view_column_set_sort_column_id (col, COL_ZETTEL);
		gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);

		/* --- Column #2 - Zettel-Titel --- */
		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new_with_attributes (header,  renderer,
												   "text", COL_TITEL, NULL);
		gtk_tree_view_column_set_sort_column_id (col, COL_TITEL);
		gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);
		store_liste_val = TRUE;
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW (view), NULL);
	store_liste = gtk_list_store_new (NUM_COLS, GDK_TYPE_PIXBUF, G_TYPE_UINT, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(store_liste));
	g_object_unref (store_liste); /* destroy store automatically with view */

	return view;
}


void show_list_window(int anz)
{
	g_print("+++ in show_list_window - anz=%d\n", anz);
	GtkWidget *liste, *label, *view, *toolbar;
	GtkWidget *statusbar;
	GtkTreeIter iter;
	GtkTreeViewColumn   *col;
	int i;
	char header[]="--- not defined ---          ";
	char status[30];
	if(store_liste_val==TRUE) gtk_list_store_clear (store_liste);
	if (!strcmp(list_action, "autor")) strcpy(header,_("Autoren/Quellen"));
	if (!strcmp(list_action, "stich")) strcpy(header,_("Stichworte"));
	if (list_window == NULL) { // create window if not exist
		list_window = GTK_WINDOW(create_list_window ());
		/* set the widget pointer to NULL when the widget is destroyed */
		g_signal_connect (G_OBJECT (list_window), "destroy",
						  G_CALLBACK (gtk_widget_destroyed), &list_window);

		/* Make sure the window doesn't disappear behind the main window. */
		//gtk_window_set_transient_for (GTK_WINDOW (list_window), GTK_WINDOW (main_window));
		/* Allow user to resize the window */
		gtk_window_set_resizable (GTK_WINDOW (list_window), TRUE);

		/* --- restore Index Window geometrie --- */
		int left, top, width, hight, icon;
		left = get_i_config("l_left");
		top  = get_i_config("l_top");
		width = get_i_config("l_width");
		hight = get_i_config("l_hight");
		if(offset) {
			left=left-g_offset_x;
			top=top+g_offset_y;
		}
		// follow the main window
		int follow, mleft, mtop, mwidth, mhight;
		follow = get_i_config("follow");
		if (follow > 0) {
			g_print("    follow=%d\n", follow);
			gtk_window_get_position(GTK_WINDOW (main_window), &mleft, &mtop);	
			gtk_window_get_size(GTK_WINDOW (main_window), &mwidth, &mhight);
			top=mtop;
			left=mleft-width-follow;
			hight=mhight;
		}
		if (left >= 0 && top >= 0) {
			gtk_window_move(GTK_WINDOW (list_window), left, top);
		}
		if (width <= 10) width = 300;
		if (hight <= 10) hight = 400;
		icon = get_i_config("toolbar");
		if (icon!=0) {
			toolbar = lookup_widget (GTK_WIDGET (list_window), "list_toolbar");
			gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
		}
		//g_printf("width=%d - hight=%d\n", width, hight);
		gtk_window_resize(GTK_WINDOW (list_window), width, hight);
	}
	
	/* Make sure the window is visible. */
	gtk_window_present (GTK_WINDOW (list_window));
	liste = lookup_widget (GTK_WIDGET(list_window), "list_treeview");
//g_printf("--- mark ---\n");	
	view = create_view_and_model_liste(liste, header);
	label=lookup_widget(GTK_WIDGET(list_window), "list_lbl_info");
	gtk_label_set_text (GTK_LABEL(label), header);
	/* set column header */
	col=gtk_tree_view_get_column(liste, 1);
	gtk_tree_view_column_set_title(col, header);
g_print("+++ MARK +++\n");

	for(i = 0; i < anz; i++) {
		//g_printf("    %3d: %3d %s\n", i, worte[i].anzahl, worte[i].wort);
		gtk_list_store_append (store_liste, &iter);
		gtk_list_store_set (store_liste, &iter, COL_ZETTEL, worte[i].anzahl, COL_TITEL, worte[i].wort, -1);
	}
	sprintf(status, _("%d %s gefunden"), anz, header);
	statusbar = lookup_widget (GTK_WIDGET(list_window), "list_appbar");
	gnome_appbar_set_status (GNOME_APPBAR (statusbar), status);

}

/* fill indexz by stichwort/catchwords */
int fill_indexz_stich(line)
{
//	g_print("+++ in fill_indexz_stich\n");
	char *line1;
	char token[]=",";
	int i, j=1, len, ret;
	
 	clear_indexz();
 	//len=strlen(line);
	for (i=1; i<=satz; i++) {
		ret = g_ptr_data = zettel[i] [1]; // Adressiert stichwort
		ret = get_g_data(); // holt data nach g_zeile
		strip_stichwort(g_zeile);
		line1 = strtok(g_zeile, token);
		while (line1 != NULL) { // Zerlege bei , in Zeilen
			//g_printf("    Stichwort=%s\n", line1);
			mystr_cut_at(line1, '/');
			if (!strcmp(line1, line)) {
				//g_print("    save to indexz[%d], %d\n", j, i);
				indexz[j]=i;
				j++;
			}
			line1 = strtok(NULL, token);
		}
	}
	return --j;
}

