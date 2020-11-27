/* *****************************************
* --- File-Funktionen nach hier ausgelagert ---
*     
***************************************** */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
//#include <math.h>

#include <gtk/gtk.h>

//#include "functions.h"
#include "support.h"
#include "globals.h"
#include "my_text.h"
//#include "myfunctions.h"
//#include "callbacks.h" // functions.c wird nach callbacks eingebunden
//#include "interface.h"

#include <libintl.h>
#define _(Text) gettext(Text)

/* We create the dialog boxes once and keep pointers to them in static
   variables so we can reuse them. */
static GtkWidget *open_filesel = NULL;
//static GtkWidget *save_filesel = NULL;


/* ********************************************************************
 * -- Liest Zeile aus (home).zmzettel.cfg Datei -- 
 * ***************************************************************** */
int get_config (char *found, char *such)
{
	char line[100];
	char *home;
	char *line1;
	int length;
	FILE *fp;
	char token[]=":= \n";
	int i, ret= -1;
	char *retc;
	
	home = getenv("HOME");
	strcpy(line, home);
	strcat(line, "/.zmzettel.cfg");
	fp = fopen (line, "r");
	if (fp == NULL) {	// --- if CFG file does not exist
		sprintf(line, "touch %s/.zmzettel.cfg", home); 
		system(line);	// create empty CFG file
		return (1);
	}
	
	while (fgets(line, 100, fp) != NULL) {
		length = strlen(such);
		retc = strchr(line, '\"');
    	if(!strncmp(line, such, length)) {	// suche Zeile
//			g_printf("+++ in get_config: retc=%s\n", retc);
			if (retc) {
//				g_printf("    modify token\n");
				strcpy (token, "\"");
			}
    		line1 = strtok(line, token);
//    		g_printf("    token=%s\n", token);
    		i=0;
    		while (line1 != NULL) { // letzter Teil ist gesuchter String
				ret = 0;
				i++;
    			strcpy(found, line1);
    			//g_printf("teil%d %s\n", i, found);
    			if (!strncmp(token, "\"", 1) && i==2) {
    				//g_printf("    found %s\n", found);
    				break;}
    			line1 = strtok(NULL, token);
    		}
		}
	}
	fclose(fp);
	return (ret);
}

/* reads and returns integer from config file */
int get_i_config(char *such)
{
	int ret;
	char line[20];
	
	ret = get_config(line, such);
	//g_printf("get_i_config: %s %s - ret %d\n", such, line, ret);
	if (ret == 0) ret = atoi(line);
	return(ret);
}


/* -- writes/replaces Line in (home).zmzettel.cfg File -- 
 *    string = Data zum Schreiben
 *    such   = Schlüsselwort */
char write_config (char *string, char *such)
{
	char line[100];
	char line1[100];
	char *home;
	char comand[100];
	int length, w = 0;
	FILE *fpin, *fpout;
	
	home = getenv("HOME");
	//g_printf("home: %s\n", home);
	strcpy(line, home);
	strcat(line, "/.zmzettel.cfg");
	strcpy(line1, home);
	strcat(line1, "/.zmzettel.tmp");
	sprintf(comand, "mv %s/.zmzettel.cfg %s/.zmzettel.tmp", home, home);
	//g_printf("comand: %s\n", comand);
	system(comand);
	fpin = fopen (line1, "r");
	if (fpin == NULL) {
		return (1);
	}
	fpout = fopen (line, "w");
	
	// -- copy CFG-file line by line --
	while (fgets(line, 100, fpin) != NULL) {
		length = strlen(such);
    	if(!strncmp(line, such, length)) {	// -- match *such ?
    		// -- replace line
    		sprintf(line, "%s=%s\n", such, string);
    		//g_printf("found: %s=%s\n", such, string);
			fputs(line, fpout);
    		w = 1;
		} else {	// -- just copy
			fputs(line, fpout);
		}
	}
	if (w == 0) { // line was not found = append line
    	sprintf(line, "%s=%s\n", such, string);
    	//g_printf("add: %s=%s\n", such, string);
		fputs(line, fpout);
    }
		
	fclose(fpin);
	fclose(fpout);
	sprintf(comand, "rm %s/.zmzettel.tmp", home);
	system("rm ~/.zmzettel.tmp");
	return (0);
}

/* writes an integer value into config file */
void write_i_config(int value, char *such)
{
	char line[20];
	sprintf(line, "%d", value);
	//g_printf("write_i_config: %s %s\n", line, such);
 	write_config(line, such);
	return;
}


/* ***********************************************************
 * --- some file test routines
 *********************************************************** */
/* check if file exists - return 1 if file exists */
int test_file_exist(char *filename)
{
	int ret=0;
	struct stat attribut;
	if (stat(filename, &attribut) == -1) {
		//g_printf("--- cannot stat %s ...\n", filename);
	} else {
		ret=1;
	}
	return(ret);
}

/* test for Lock-File if supported */
int test4lock(char *filename)
{
	g_printf("+++ in test4lock\n");
	int n, ret;
	char help[200];
	char line[300];

	ret = get_i_config("lock_file");
	g_print ("    ret = %d\n", ret);
	if (ret == 1 && readonly_c == FALSE) {	// Lock file support
		strcpy(help, filename);
		n = strlen(help);
		help[n-3] = '\0';
		strcat(help, "lck~");
		if (test_file_exist(help)) { // Lockfile found
			g_printf("    Lockfile %s found\n", help);
			GtkWidget *dialog = gtk_dialog_new_with_buttons(_("Lock File found"), main_window,
									GTK_DIALOG_MODAL,
									GTK_STOCK_OK, GTK_RESPONSE_OK,
//									GTK_STOCK_ADD, GTK_RESPONSE_ACCEPT,
									GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
									NULL);
			gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
//			sprintf(line, _("Datei bereits in Bearbeitung\nLock-File %s gefunden!\n\nDatei im ReadOnly-Mode öffnen?"), help);
			sprintf(line, _("Datei bereits in Bearbeitung\nLock-File %s gefunden!\n\nDatei trotzdem öffnen?"), help);
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
			g_print("    Antwort = %d\n", ret);
			if (ret==GTK_RESPONSE_CANCEL) {
				ret = 1;
			} else if (ret==GTK_RESPONSE_OK) {
				//readonly = TRUE;
				ret = 0;
			}
		} else { // Lockfile not found
			g_printf("    no Lockfile found %s ...\n", help);
			readonly = readonly_c;
			sprintf(line, "touch %s", help);
			//g_printf("    system: %s\n", line);
			system(line);
			ret = 0;
		}
	} else { // no Lock-File support
		ret = 0;
	}
	return ret;
}

/* remove Lock file (if exist) */
void rm_lock_file(char *filename)
{
	g_print("+++ in rm_lock_file\n");
	char help[200];
	char comand[200];
	//strcpy(help, current_filename);
	strcpy(help, filename);
	int len = strlen(help);
	help[len-3] = '\0';
	strcat(help, "lck~");
	g_print("    lock-filename = %s\n", help);
	if (test_file_exist(help)) { // Lockfile found
		g_printf("    remove file %s\n", help);
		sprintf(comand, "rm %s", help);
		//g_printf("    system: %s\n", comand);
		system(comand);
	}
}
/* ***********************************************************
 * --- Open and read File
 *********************************************************** */
//void real_open_file (const gchar *filename)
void real_open_file (const char *filename)
{
	GtkWidget *text, *statusbar, *button;
	GtkTextBuffer *text_buffer;
	GtkTextIter start, end;
	FILE *fp;
	gint bytes_read;
	float lsize;
	char line[60000];
	char help[200];
	char comand[300];
	char c;
	char *ret;
	int p=0, n=0, pmax=0;
	int len;

	g_printf("+++ in real_open_file %s\n", filename);
	
	memset(g_data, '\000', sizeof(g_data)); // clears g_data
  
	/* check for backup-date - output will go later in statusbar */
	strcpy(help, filename);
	int val = get_i_config("a_backup_std");
	//g_print("    return val = %d\n", val);
	if (val) {
		strcat(help, "~");
	} else {
		len = strlen(help);
		help[len-1]='b';
	}
	/* check if back file is older */
	struct stat attribut; // struct is needed for age
	if (stat(filename, &attribut) == -1) {
		g_printf("--- cannot stat dat-file %s ...\n", filename);
	}
	int ftime=attribut.st_mtime;
	if (stat(help, &attribut) == -1) {
		g_printf("--- cannot stat bak-file %s ...\n", help);
	}
	int btime=attribut.st_mtime;
//	g_printf("    file=%d, bak=%d\n", ftime, btime);

	text = lookup_widget (GTK_WIDGET(main_window), "main_txt_text");
	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	statusbar = lookup_widget (GTK_WIDGET(main_window), "main_appbar");

  gtk_text_buffer_get_bounds (text_buffer, &start, &end);
  gtk_text_buffer_delete (text_buffer, &start, &end);
  //g_free (current_filename); // macht Probleme???
//	g_printf("In real open file =2\n");
  current_filename = NULL;
  file_changed = FALSE;

  // --- Auto convert Windows Version ? --- 
  // mit umkopieren über *.tmp = kompliziert, kenne aber keinen besseren Weg
  if (!strncmp("1", g_conv, 1)) {
  	ret = strstr(filename, ".zkn");
	//g_printf("*** in read g_conv c=%s\n", ret);
  	if (ret != 0) {
  		//g_printf("    ZKN-file gefunden\n");
  		strcpy(help, filename);
  		n = strlen(help);
  		help[n-3] = 't';
  		help[n-2] = 'm';
  		help[n-1] = 'p';
		sprintf(comand, "cp %s %s", filename, help);
		//g_printf("comand: %s\n", comand);
		system(comand);
		sprintf(comand, "iconv -f windows-1252 -t utf-8 %s -o %s", help, filename);
		//g_printf("comand: %s\n", comand);
		system(comand);
  	}
  }
  
  fp = fopen (filename, "rb"); // (r+b) geht nicht für Help-File lesen
  if (fp == NULL) {
  	g_print("*** cannot open file: %s ***\n", filename);
	strcpy(line, "Couldn't open file ");
	strcat(line, filename);
	error_msg(line);
  	//gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, line);
 	gnome_appbar_set_status (GNOME_APPBAR (statusbar), line);
  	//return;
  	exit(1) ;
  }
	button = lookup_widget (GTK_WIDGET(main_window), "main_mnu_zneu_f");
	gtk_widget_set_sensitive(button, FALSE);

  /* -- get file size.-- */
  fseek (fp , 0 , SEEK_END);
  lsize= ftell(fp);
  rewind (fp);
  g_printf ("    File size = %.0f\n", lsize);

  /* -- read file header -- */
  n=0;
  do {
    if(n == 0 && p > 20) {
		g_printf("Fehler beim Oeffnen\n");
		sprintf(line, _("*** Datei ist keine Zettelkasten Datei ***"));
		error_msg(line);
 		gnome_appbar_set_status (GNOME_APPBAR (statusbar), line);
		return;
    }
    c = fgetc(fp);
    if (c == 0) {
      line[p] = c;
      n++;
      if (n == 1) {
        strcpy(version, line);
        if(strncmp(version, "Version2.6", 10) != 0) {
          sprintf(line, _("Falsche Zettelkasten Version ... kann falsch dargestellt werden"));
          //gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, line);
 		gnome_appbar_set_status (GNOME_APPBAR (statusbar), line);
          return;
        }
      }
      if (n == 2) strcpy(description, line);
      if (n == 3) satz = atoi(line);
      //g_printf("[00-%2d]Line: %s\n", n, line);
      p=0;
      strcpy(line, "");
/*    }else if (c == 0x0a) {
      strcat(line, "\n");
      p=p+8;
*/    } else {
      line[p] = c;
      p++;
      line[p] = 0x00;
    }
  } while (n < 3);
  g_printf("*** Header fertig - weiter mit Data ***\n");
  g_printf("    description:\n%s\n", description);

  int index=1, feld=0, stich=0;
  long g_ptr_data=0;
  //eindex=zindex;	//save zindex
  zindex=0;

  /* -- read file character by character -- */
  zettel[zindex] [feld] = g_ptr_data;
  zindex++;
  g_data[g_ptr_data] = c; // schreibt NULL an den Anfang des g_data	
  g_ptr_data++;

	n=0;
	while ( (c=fgetc(fp)) != EOF) {
		g_data[g_ptr_data] = c;
		if (stich==1) { //first character in stichwort
			stich=0;
			if (c == '$') { // template found
				button = lookup_widget (GTK_WIDGET(main_window), "main_mnu_zneu_f");
				gtk_widget_set_sensitive(button, TRUE);
			}
		}
		if (c == 0) { // begin new feld
			n++;
			feld++;
			zettel[zindex] [feld] = g_ptr_data; // save to zettel array
			if (feld == 1) { // stichworte
				stich=1;
			}
			if (feld == 8) { // 8 felder je Zettel
				zindex++;
				feld = 0;
			}
    	}
    	g_ptr_data++;
	}
	g_ptr_data--;	//points behind EOF
//  g_ptr_data--;	//should be EOF
	/* output control if file is read correctly */
	float mean=n*1.0/(zindex-1);
	g_print(" ++ N = %d / %d = %.3f >> last character=%x\n", n, zindex-1, mean, g_data[g_ptr_data]);
	zettel[zindex] [0] = g_ptr_data; // Ende von g_data/file
	g_data[g_ptr_data] = '\0'; // just in case Zero is missing
	g_printf("    EOB @ %d - index %d - c=%x\n", g_ptr_data, zindex, c);

  /* -- reload preset & show Zettel -- */
  zindex=eindex;	//restore zindex
  g_printf("*** open file: zindex=%d\n", zindex);
  if (zindex < 1 || zindex > satz){
  	zindex=1;
  }
  show_zettel(GTK_WIDGET(main_window));
  clear_zindex();

  /* If there is an error while loading, we reset everything to a good state. */
  if (ferror (fp)) {
    fclose (fp);
    gtk_text_buffer_get_bounds (text_buffer, &start, &end);
    gtk_text_buffer_delete (text_buffer, &start, &end);
    set_window_title (main_window);
 	gnome_appbar_set_status (GNOME_APPBAR (statusbar), "*** Error loading file! ***");
    return;
  }

  fclose (fp);

    // --- just copy Windows File back if needed ---
  	if (ret != 0) {
		sprintf(comand, "cp %s %s", help, filename);
		//g_printf("comand: %s\n", comand);
		system(comand);
		sprintf(comand, "rm %s", help);
		//g_printf("comand: %s\n", comand);
		system(comand);
		n=0;
  	}
  	
  current_filename = g_strdup (filename);
  set_window_title (main_window);
	// --- set status bar ---
	if (zindex == 1) {
//		g_printf("    file=%d, bak=%d\n", ftime, btime);
		if (ftime < btime) {
			g_printf("    Backup file ist younger then file\n");
			sprintf(line, _("*** WARNUNG: Backup-Datei ist jünger als Zettelkasten-Datei ***"));
		} else {	
	  		sprintf(line, _("Datei gelesen - %d Zettel - größter Zettel = %d Zeichen"), satz, pmax);
		}
 	gnome_appbar_set_status (GNOME_APPBAR (statusbar), line);
 	float filled = lsize/g_maxdata;
 	g_printf("    size=%.0f Bytes filled=%f %\n", lsize, (filled*100));
 	gnome_appbar_set_progress_percentage(GNOME_APPBAR(statusbar), filled);
 	
  }
  
	button = lookup_widget (GTK_WIDGET (main_window), "main_btn_save"); //save-button
	gtk_widget_set_sensitive(button, FALSE);
	/* --- read_only mode = disable some widgets --- */
	if(readonly) {
		button = lookup_widget (GTK_WIDGET (main_window), "main_mnu_edit1");
		gtk_widget_set_sensitive(button, FALSE);
		button = lookup_widget (GTK_WIDGET (main_window), "main_mnu_save");
		gtk_widget_set_sensitive(button, FALSE);
		button = lookup_widget (GTK_WIDGET (main_window), "main_mnu_mark1");
		gtk_widget_set_sensitive(button, FALSE);
//		GtkWidget *menu = button->parent; // points to main_menu
//		gtk_widget_hide(menu);
//		button = lookup_widget (GTK_WIDGET (main_window), "main_btn_open"); //open-button
//		gtk_widget_set_sensitive(button, FALSE);
		button = lookup_widget (GTK_WIDGET (main_window), "main_btn_new");
		gtk_widget_set_sensitive(button, FALSE);
		button = lookup_widget (GTK_WIDGET (main_window), "main_btn_edit");
		gtk_widget_set_sensitive(button, FALSE);
//		button = lookup_widget (GTK_WIDGET (main_window), "main_btn_stich");
//		gtk_widget_set_sensitive(button, FALSE);
	} else {
		button = lookup_widget (GTK_WIDGET (main_window), "main_mnu_edit1");
		gtk_widget_set_sensitive(button, TRUE);
		button = lookup_widget (GTK_WIDGET (main_window), "main_mnu_save");
		gtk_widget_set_sensitive(button, TRUE);
		button = lookup_widget (GTK_WIDGET (main_window), "main_mnu_mark1");
		gtk_widget_set_sensitive(button, TRUE);
		button = lookup_widget (GTK_WIDGET (main_window), "main_btn_new");
		gtk_widget_set_sensitive(button, TRUE);
		button = lookup_widget (GTK_WIDGET (main_window), "main_btn_edit");
		gtk_widget_set_sensitive(button, TRUE);
	}
	/* ggf Index Window updaten */
	if (index_window != NULL) {
		gtk_list_store_clear(store_index);
//		gtk_widget_hide(index_window);
//		fill_tree_view_index();
		show_index_titel("Zettelkasten - Index");
	}
	if (list_window != NULL) {
		gtk_list_store_clear(store_liste);
		gtk_widget_destroy(GTK_WIDGET(list_window));
		store_liste_val=FALSE;
//		fill_tree_view_index();
//		show_index_titel("Zettelkasten - Index");
	}
}


/* **************************************************
 * This saves the file, which is in on-disk encoding 
 **************************************************** */
void
real_save_file (const gchar *filename)
{
  GtkWidget *text, *statusbar;
  GtkTextBuffer *text_buffer;
  GtkTextIter start, end;
  gchar *data;
  FILE *fp;
  char *ret;
  gint bytes_written, len;
  char header[200];
  char comand[300];
  char help[132];
  int i, j, n;
  char *dot;

  if (current_filename == NULL || strcmp (current_filename, filename))
    {
      g_free (current_filename);
      current_filename = g_strdup (filename);
      set_window_title (main_window);
    }

  text = lookup_widget (GTK_WIDGET (main_window), "main_txt_text");
  text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
  statusbar = lookup_widget (GTK_WIDGET(main_window), "main_appbar");

  gtk_text_buffer_get_bounds (text_buffer, &start, &end);
  data = gtk_text_buffer_get_text (text_buffer, &start, &end, TRUE);

  /* -- note last file name into cfg-file -- */
  write_config(filename, "last");
  
  /* -- make backup file if g_back contains 1 -- */
  if (!strncmp("1", g_back, 1)) {
	int val = get_i_config("a_backup_std");
	strcpy(header, filename);
	if (val) {
		/* standard ~ Linux bak file */
		strcat(header, "~");
	} else {
		/* Windows comptible *.zkb file */
		for(i=0; header[i] != '\0'; i++) {
			if (header[i] == '.') header[i] = '\0';
		}
  		strcat(header, ".zkb");
	}
//  	sprintf(comand, "mv %s %s", filename, header);
  	sprintf(comand, "cp %s %s", filename, header);
  	system(comand);
  	g_print("    bkfname=%s\n", header);
  }

  fp = fopen (filename, "wb");
  if (fp == NULL)
    {
      g_free (data);
      return;
    }

	strcpy(version, "Version2.6");
	g_printf ("Filename= %s\n", filename);
	g_printf ("Version = %s\n", version);
	g_printf ("Descrip.= %s\n", description);
	g_printf ("Sätze   = %d\n", satz);
	
	j = 0;
	len = strlen(version);
	for (i=0; i<len; i++) {
		header[j] = version[i];
		j++;
	}
	header[j] = 0;
	j++;
	len = strlen(description);
	for (i=0; i<len; i++) {
		header[j] = description[i];
		j++;
	}
	header[j] = 0;
	j++;
	sprintf(version, "%d", satz);
	len = strlen(version);
	for (i=0; i<len; i++) {
		header[j] = version[i];
		j++;
	}
	header[j] = 0;
	
	//len = sizeof(header);
	g_printf("header-len %d\n", j);
  	fwrite(header, sizeof(gchar), j, fp);
	
	len = zettel[satz+1] [0] - zettel[0] [0] +1;
  	g_printf ("Bytes   = %d\n", len);

	bytes_written = fwrite(g_data, sizeof(gchar), len, fp); 
  fclose (fp);

  g_free (data);

  //gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
  if (len != bytes_written)
    {
      //gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, "Error saving file.");
 		gnome_appbar_set_status (GNOME_APPBAR (statusbar), "*** Error saving file ***");
      return;
    }

  /* --- Auto Convert to Windows if selected --- */
  if (!strncmp("1", g_conv, 1)) {
  	ret = strstr(filename, ".zkn");
	//g_printf("*** in write g_conv ret=%s\n", ret);
  	if (ret != 0) {
  		strcpy(help, filename);
  		n = strlen(help);
  		help[n-3] = 't';
  		help[n-2] = 'm';
  		help[n-1] = 'p';
		sprintf(comand, "cp %s %s", filename, help);
		//g_printf("comand: %s\n", comand);
		system(comand);
		sprintf(comand, "iconv -f utf-8 -t windows-1252 %s -o %s", help, filename);
		//g_printf("comand: %s\n", comand);
		system(comand);
		sprintf(comand, "rm %s", help);
		//g_printf("comand: %s\n", comand);
		system(comand);
  	}
  }
  
  file_changed = FALSE;
  GtkWidget *button;
  button = lookup_widget (GTK_WIDGET (main_window), "main_btn_save"); //save-button
  gtk_widget_set_sensitive(button, FALSE);
  //gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, "File saved.");
 	gnome_appbar_set_status (GNOME_APPBAR (statusbar), "File saved");
 	
 	float filled = len;
 	filled=filled/g_maxdata;
 	g_printf("    size=%d Bytes filled=%f %\n", len, (filled*100));
 	gnome_appbar_set_progress_percentage(GNOME_APPBAR(statusbar), filled);
	
} // --- end: real_save_file ---



/* This creates a new document, by clearing the text widget and setting the
   current filename to NULL. */
void new_file ()
/* --- muss noch geändert werden --- */
{
  GtkWidget *text, *statusbar;
  GtkTextBuffer *buffer;
  GtkTextIter start, end;
	int i;

	// --- clear header data ---
	satz = 1;
	zindex = 1;
	strcpy(description, "");
	// --- Clean Zettel Array
	for (i = 0; i < 8; i++)
	{
	    zettel[0] [i] = '\0';
	    zettel[1] [i] = '\0';
	}
	// --- Clean beginning of Data Buffer
	for (i = 0; i < 100; i++)
	{
	    g_data[i] = '\0';
	}
	// --- show empty Zettel
	//g_printf("Show clean Zettel\n");
	show_zettel(GTK_WIDGET(main_window));
	
  g_free (current_filename);
  current_filename = NULL;
  file_changed = FALSE;
  set_window_title (GTK_WIDGET(main_window));

	statusbar = lookup_widget (GTK_WIDGET(main_window), "main_appbar");
  //gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
  //gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, "New file.");
 	gnome_appbar_set_status (GNOME_APPBAR (statusbar), "New File");

	/* goto "save as" since a filename needs to be set */
  	strcpy(last_action, "save_as");
	save_as("Save New File as");
}


/* This shows the file selection dialog to open a file. */
//static void
void open_file ()
{
	/* --- use the same file selection widget each time, do not create it twice --- */
//	if (open_filesel == NULL) {
		GtkFileFilter *filter, *n_filter;
//		open_filesel = create_open_file_selection ();
		open_filesel = GTK_WIDGET(create_open_file_chooser ());
		/* Make sure the dialog doesn't disappear behind the main window. */
		gtk_window_set_transient_for (GTK_WINDOW (open_filesel), GTK_WINDOW (main_window));
	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (open_filesel), g_path );

	if (!strcmp(last_action, "open_file")) {
		/* --- n_filter = "*" = alle Files --- */
		n_filter = gtk_file_filter_new();
		gtk_file_filter_add_pattern(GTK_FILE_FILTER (n_filter), "*");
		gtk_file_filter_set_name(GTK_FILE_FILTER (n_filter), "all Files");
		/* --- filter = "muster" = realer Filter --- */
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(GTK_FILE_FILTER (filter), "*.zkx, *.zkn");
		gtk_file_filter_add_pattern(GTK_FILE_FILTER (filter), "*.zkx");
		gtk_file_filter_add_pattern(GTK_FILE_FILTER (filter), "*.zkn");

		/* --- wird realer Filter zuerst addiert, wird defaultmäßig gefiltert --- */	
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (open_filesel), filter);
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (open_filesel), n_filter);
	}
	if (!strcmp(last_action, "import_xml")) {
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(GTK_FILE_FILTER (filter), "*.xml");
		gtk_file_filter_add_pattern(GTK_FILE_FILTER (filter), "*.xml");
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (open_filesel), filter);
    }

  /* We save a pointer to the main window inside the file selection's
     data list, so we can get it easily in the callbacks. */
//  gtk_object_set_data (GTK_OBJECT (open_filesel), MainWindowKey, main_window);
  
  /* default file path */
  //gtk_file_selection_set_filename(GTK_FILE_SELECTION (open_filesel), g_path );
//  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (open_filesel), g_path );

  /* We make sure the dialog is visible. */
  gtk_window_present (GTK_WINDOW (open_filesel));
  /* preset Zettel index to 1 */
  eindex=1;
}


/* This shows the file selection dialog to save a file. */
void save_as (char *header)
{
	g_printf ("+++ in save_as - File: %s\n", current_filename);
	char help[80];

	GtkFileFilter *filter, *filter1, *n_filter;
	//save_filesel = create_save_file_selection ();
	GtkWidget *save_filesel = GTK_WIDGET(create_save_file_chooser ());
	/* Make sure the dialog doesn't disappear behind the main window. */
	gtk_window_set_transient_for (GTK_WINDOW (save_filesel), GTK_WINDOW (main_window));

	/* --- n_filter = "*" = alle Files --- */
	n_filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(GTK_FILE_FILTER (n_filter), "*");
	gtk_file_filter_set_name(GTK_FILE_FILTER (n_filter), "all Files");
	/* --- filter = "muster" = realer Filter --- */
	filter = gtk_file_filter_new();
	gtk_file_chooser_set_current_folder(save_filesel, g_path);
	if (!strcmp(last_action, "save_as")) {
		gtk_file_filter_set_name(GTK_FILE_FILTER (filter), "*.zkx, *.zkn");
		gtk_file_filter_add_pattern(GTK_FILE_FILTER (filter), "*.zkx");
		gtk_file_filter_add_pattern(GTK_FILE_FILTER (filter), "*.zkn");
		/* --- wird realer Filter zuerst addiert, wird defaultmäßig gefiltert --- */	
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (save_filesel), filter);
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (save_filesel), n_filter);
		if (current_filename) {
			gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (save_filesel), g_basename(current_filename));
		} else {
			gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (save_filesel), "new.zkx");
		}
  		gtk_window_set_title (GTK_WINDOW (save_filesel), header);
	}
	if (!strcmp(last_action, "export_as")) {
		gtk_file_filter_set_name(GTK_FILE_FILTER (filter), "LaTeX: *.tex");
		gtk_file_filter_add_pattern(GTK_FILE_FILTER (filter), "*.tex");
		filter1 = gtk_file_filter_new();
		gtk_file_filter_set_name(GTK_FILE_FILTER (filter1), "Export: *.xml");
		gtk_file_filter_add_pattern(GTK_FILE_FILTER (filter1), "*.xml");
		/* --- wird realer Filter zuerst addiert, wird defaultmäßig gefiltert --- */	
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (save_filesel), filter1);
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (save_filesel), filter);
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER (save_filesel), n_filter);
		sprintf(help, "%s_z%d.xml", g_basename(current_filename), zindex);
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (save_filesel), help);
	}

  /* We make sure the dialog is visible. */
  gtk_window_present (GTK_WINDOW (save_filesel));
}


/* ********************************************************************
 * --- open file info box and fill it --- 
 * ***************************************************************** */
void my_file_info()
{
	static GtkWidget *file_info = NULL;
	GtkWidget *label;
	GtkTextBuffer *text_buffer;
	GtkStyle *style;
	char help[500];
	int i, keyed=0, hidden=0;

	if (file_info == NULL) {
		file_info = GTK_WIDGET(create_file_info_box ());
		/* set the widget pointer to NULL when the widget is destroyed */
		g_signal_connect (G_OBJECT (file_info), "destroy",
			G_CALLBACK (gtk_widget_destroyed), &file_info);
		/* Make sure the dialog doesn't disappear behind the main window. */
		gtk_window_set_transient_for (GTK_WINDOW (file_info), GTK_WINDOW (main_window));
		/* Do not allow user to resize the dialog */
		gtk_window_set_resizable (GTK_WINDOW (file_info), FALSE);
	}

	/* --- read file attributes --- */
	struct stat attribut;
	if (stat(current_filename, &attribut) == -1) {
		g_printf("--- cannot stat %s ...\n", current_filename);
	}
	float ilinks=count_zettel_links();
	float links_pz = ilinks/satz;
	float istich=count_zettel_stich();
	float stich_pz = istich/satz;
	/* count hidden and keyed */
	for (i=1; i<=satz; i++) {
		g_ptr_data = zettel[i] [0];
		get_g_data();
		if (g_zeile[0]=='@') keyed++; // verschlüsselt
		g_ptr_data = zettel[i] [1];
		get_g_data();
		if (g_zeile[0]=='#') hidden++; // done
		if (g_zeile[0]=='$') hidden++; // template
	}

//	g_print("    links_pz=%f ilinks=%d, satz=%d\n", links_pz, ilinks, satz);
	/* fill file label */
	sprintf(help, _("Datei\t\t\t%s\nDateigröße\t\t%'d Byte\nAnzahl Zettel\t\t%'d\t(%'d inaktiv, %'d verschlüsselt)\
		\nØ Zettelgröße\t\t%'d Byte\nØ Links/Zettel\t\t%.1f\nØ Stichworte/Zettel\t%.1f"), \
		 current_filename, attribut.st_size, satz, hidden, keyed, attribut.st_size/satz, links_pz, stich_pz);
	label = lookup_widget (file_info, "file_info_lbl_file");
	gtk_label_set_text (GTK_LABEL(label), help);
	//style = gtk_widget_get_style(label);
    //pango_font_description_set_weight(style->font_desc, PANGO_WEIGHT_NORMAL);
    //gtk_widget_modify_font(label, style->font_desc);
	/* fill access label */
    char c_time[30];
    strcpy(c_time, ctime(&attribut.st_ctime));
    char m_time[30];
    strcpy(m_time, ctime(&attribut.st_mtime));
    char a_time[30];
    strcpy(a_time, ctime(&attribut.st_atime));
	sprintf(help, _("I-Node Änderung\t%sLetzes Speichern\t%sLetzter Zugriff\t\t%sZugriffsrechte\t\t%o"), \
		 c_time, m_time, a_time, attribut.st_mode & 0777);
	label = lookup_widget (file_info, "file_info_lbl_access");
	gtk_label_set_text (GTK_LABEL(label), help);
	//style = gtk_widget_get_style(label);
    //pango_font_description_set_weight(style->font_desc, PANGO_WEIGHT_NORMAL);
    //gtk_widget_modify_font(label, style->font_desc);

	/* show file header */
	label = lookup_widget (file_info, "file_info_txt_note");
  	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (label));
  	gtk_text_buffer_set_text (text_buffer, description, -1);

	/* Make sure the dialog is visible. */
	gtk_window_present (GTK_WINDOW (file_info));
}

/* ********************************************************************
 * --- open options window 
 * ***************************************************************** */
void my_option_window()
{
	g_print("+++ in my_option_window\n");
	//static GtkWidget *option_window = NULL;

	if (option_window == NULL) {
		option_window = GTK_WIDGET(create_option_window ());
		/* set the widget pointer to NULL when the widget is destroyed */
		g_signal_connect (G_OBJECT (option_window), "destroy",
			G_CALLBACK (gtk_widget_destroyed), &option_window);
		/* Make sure the dialog doesn't disappear behind the main window. */
		gtk_window_set_transient_for (GTK_WINDOW (option_window), GTK_WINDOW (main_window));
		/* Do not allow user to resize the dialog */
		gtk_window_set_resizable (GTK_WINDOW (option_window), TRUE);
	}
	/* Make sure the dialog is visible */
	gtk_window_present (GTK_WINDOW (option_window));

	char string[50];
	/*** Allgemein ***/
	GtkWidget *field;
	field = lookup_widget(option_window, "option_entry_path");
	gtk_entry_set_text(field, g_path);
	int val = get_i_config("auto_load");
	field = lookup_widget(option_window, "option_cbtn_autoload");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(field), val);
	val = get_i_config("auto_backup");
	field = lookup_widget(option_window, "option_cbtn_autobackup");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(field), val);
	val = get_i_config("a_backup_std");
	if (val) {
		field = lookup_widget(option_window, "option_rbtn_backup_std");
	} else {
		field = lookup_widget(option_window, "option_rbtn_backup_zkb");
	}
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(field), TRUE);
	val = get_i_config("auto_convert");
	field = lookup_widget(option_window, "option_cbtn_autoconvert");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(field), !val);
	val = get_i_config("lock_file");
	field = lookup_widget(option_window, "option_cbtn_lock_file");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(field), val);
	/*** Ansicht ***/
	val = get_i_config("toolbar");
	field = lookup_widget(option_window, "option_combo_toolbar");
	gtk_combo_box_set_active(field, val);
	/*** Editor ***/
	val = get_i_config("e_lineno");
	field = lookup_widget(option_window, "option_cbtn_lineno");
	gtk_toggle_button_set_active(field, val);
	val = get_i_config("e_syntax");
	field = lookup_widget(option_window, "option_cbtn_syntax");
	gtk_toggle_button_set_active(field, val);
	field = lookup_widget(option_window, "option_entry_font");
	int ret = get_config(string, "e_font");
	if (ret!=0) { strcpy(string, "Monospace 10"); }
	gtk_entry_set_text(field, string);
	val = get_i_config("e_margin");
	g_print("    rand=%d\n", val);
	field = lookup_widget(GTK_WIDGET(option_window), "option_cbtn_margin");
	if (val == 0) {
		gtk_toggle_button_set_active(field, FALSE);
		field = lookup_widget(GTK_WIDGET(option_window), "option_spin_margin");
		gtk_widget_set_sensitive(field, FALSE);
	} else {
		gtk_toggle_button_set_active(field, TRUE);
		field = lookup_widget(GTK_WIDGET(option_window), "option_spin_margin");
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(field), val);
		gtk_widget_set_sensitive(field, TRUE);
	}
	val = get_i_config("e_spell");
	field = lookup_widget(option_window, "option_combo_spell");
	gtk_combo_box_set_active(field, val);
	val = get_i_config("auto_save");
	field = lookup_widget(option_window, "option_cbtn_auto_save");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(field), val);
	/*** Autor ***/
	field = lookup_widget(option_window, "option_entry_autor");
	gtk_entry_set_text(field, g_autor);
	val = get_i_config("e_autor");
	field = lookup_widget(option_window, "option_cbtn_autor");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(field), val);
	field = lookup_widget(option_window, "option_entry_token");
	ret = get_config(string, "token");
	if (ret!=0) { strcpy(string, ":,;"); }
	gtk_entry_set_text(field, string);
	
}

/* get a headline - format in link = "-z nn filename" */
void get_head_line(char *link)
{
	char help[100];
	char fname[200];
	int znr;
	FILE *fp;
	char c;
	int n, j=0;
	
	strcpy(help, link);
	mystr_cut_vorn(help, 2);
	znr=atoi(help);
	strcpy (help, link);
	mystr_revcut_at(help, ' ');
	
//	g_print("+++ in get_hed_line: help=%s=%d\n", help, znr);
	if (help[0] != '/') {
		sprintf(fname, "%s/%s", g_path, help);
	} else {
		strcpy(fname, help);
	}
	strcpy(link, "");
	if (test_file_exist(fname)) {
		fp = fopen (fname, "r");
		if (fp == NULL) {
			g_print("*** Couldn't open file %s\n", fname);
			return;
		}
		n=znr*8+2;	// calculate the matching NULL
		/* it maybe slow, but it works */
		do {	// loop thru file til matching NULL
			c = fgetc(fp);
			if (c == 0) {
				n--;
				if (n == 0) {	// match reached
					do {		// read until next NULL
						c = fgetc(fp);
						link[j]=c; // into link as buffer
						j++;
					} while (c != 0);
				}
			}
  		} while (c != EOF);
		fclose(fp);
//		g_print("    found = '%s'\n", link);
	}
	return;
}

/**********************************************************************
 *   export functions
 * ***************************************************************** */
void export_tex_slip(FILE *fp, int znr)
{
	g_print("+++ in export_tex_slip\n");
	char line[120];
	FILE *hdr;
	GtkWidget *statusbar = lookup_widget (GTK_WIDGET(main_window), "main_appbar");
	
	strcpy(line, "/usr/local/share/applications/ZMzettelkasten/export.hdr");
	hdr = fopen (line, "r");
	if (hdr == NULL) {
		sprintf(line, "*** cannot open LaTeX-header %s\n", line);
		g_print("%s\n", line);
		gnome_appbar_set_status (GNOME_APPBAR (statusbar), line);
		return;
	}
	/* copy header file line by line */
	while (fgets(line, 100, hdr) != NULL) {
		//g_print("%s", line);
		if(strstr(line, "$TITLE")) {
			g_ptr_data = zettel[znr] [7]; // titel
			get_g_data();
			mystr_replace_ss(line, "$TITLE", g_zeile);
		}
		if(strstr(line, "$PAGE")) {
			sprintf(g_zeile, "%d", znr);
			mystr_replace_ss(line, "$PAGE", g_zeile);
		}
		if(strstr(line, "$AUTHOR")) {
			g_ptr_data = zettel[znr] [2];
			get_g_data();
			mystr_replace(g_zeile, '\n', '\0'); // first line only
			mystr_replace_ss(line, "$AUTHOR", g_zeile);
		}
		fputs(line, fp);
	}
	g_ptr_data = zettel[znr] [0]; // text
	get_g_data();

	mystr_replace_ss(g_zeile, "⇨", "$\\Rightarrow$");
	mystr_replace_ss(g_zeile, "✔", "$\\checkmark$");
	mystr_replace_ss(g_zeile, "LaTeX", "\\/LaTeX ");

	int len = strlen(g_zeile);
	int i=0;
	int cor=1, dcor=0, last=0;
	char c;
	int bc;
	char bild[100];
	while (i < len) {
		/* check for opening TAGs */
		if (g_zeile[i]=='<' && g_zeile[i+2]=='>') {
			c = g_zeile[i+1];
			//g_print("   found open tag %c%c%c\n", g_zeile[i], c, g_zeile[i+2]);
			i += 3;
			switch (c) {
			case 'c':
				fprintf(fp, "\\begin{center}");
				break;
			case 'd':
				fprintf(fp, "\\sout{");
				break;
			case 'f':
				fprintf(fp, "\\textbf{");
				break;
			case 'h':
				fprintf(fp, "{\\LARGE ");
				break;
			case 'i':
				/* need to place image declaration */
				bc=0;
				while (i<len) {
					if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='i' && g_zeile[i+3]=='>') {
						bild[bc]=0;
						g_print ("    pic-name: %s\n", bild);
						i = i+4;
						break;
					}
					bild[bc]=g_zeile[i];
					i++;
					bc++;
				}
				if(bild[0]!='/') { //relativ name is given
					char help[40];
					strcpy(help, bild);
					sprintf(bild, "%s/%s", g_path, help);
				}
				if(!test_file_exist(bild)) {
					g_print ("*** pic-file not found: %s\n", bild);
					strcpy(bild, "/usr/local/share/pixmaps/ZMzettelkasten/picture_missing.png");
				}
				// needs \usepackage{graphicx} in header file
				fprintf(fp, "\\includegraphics[scale=0.5]{%s}", bild);
				break;
			case 'k':
				fprintf(fp, "\\textsl{");
				break;
			case 'l':
				fprintf(fp, "\\begin{itemize}\n");
				//fprintf(fp, "\\begin{Tabbing}\nMMMMM \\= text \\kill\n");
				break;
			case 'm':
				if ((strstr(&g_zeile[i], "</m>")) < (strstr(&g_zeile[i], "\n"))) { // </m> noch vor CR
					fprintf(fp, "\\texttt{");
				} else {
					cor = 0;	// do not process "spezial" character
					fprintf(fp, "\\begin{verbatim}\n");
				}	
				break;
			case 's':
				fprintf(fp, "$_{");
				break;
			case 'S':
				fprintf(fp, "$^{");
				break;
			case 'u':
				fprintf(fp, "\\underline{");
				break;
			default:
				fprintf(fp, "$<$%c$>$", c);
			}
		/* check for closing TAGs */	
		} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+3]=='>') {
			c = g_zeile[i+2];
			//g_print("   found close tag %c/%c%c\n", g_zeile[i], c, g_zeile[i+3]);
			i += 4;
			switch (c) {
			case 'c':
				fprintf(fp, "\\end{center}");
				break;
			case 'd':
			case 'f':
			case 'h':
			case 'k':
			case 'u':
				fprintf(fp, "}");
				break;
			case 's':
			case 'S':
				fprintf(fp, "}$");
				break;
			case 'l':
				fprintf(fp, "\n\\end{itemize}\n");
				//fprintf(fp, "\n\\end{tabbing}\n");
				break;
			case 'm':
				if (cor==0) {
					fprintf(fp, "\\end{verbatim}\n");
					cor = 1;
				} else {
					fprintf(fp, "}");
				}
				break;
			default:
				fprintf(fp, "$<$/%c$>$", c);
			}
		/* newlines need special processing */ 
		} else if (g_zeile[i]=='\n' && cor==1) {
			if(last==0) { // last was normal
				fprintf(fp, "\\\\\n");
				last++;;	// tag CR
			} else if (last==1) {
				fputc('\n', fp);
				last++;
			} else if (last==2) {
				fprintf(fp, "\\vspace{4mm}\n\n");
				last++;	// tag vspace
			}
			i++;
		/* $\ combis switching to LaTeX math mode */ 
		} else if (g_zeile[i]=='$' && g_zeile[i+1]=='\\') {
			fprintf(fp, "$\\");
			dcor = 1;
			i += 2;
		} else if (g_zeile[i]=='$' && dcor==1) {
			fprintf(fp, "$");
			dcor = 0;
			i++;
		} else if (g_zeile[i]=='\\' && cor==1) {
			if (g_zeile[i+1]=='/') {
				fputc('\\', fp);
				i++;	// jump over /
			} else {
				fprintf(fp, "\\textbackslash ");
			}
			i++;
		} else {
			c = g_zeile[i];
			if (cor==1) { // also normal character may need corrections
				if (c=='%' || c=='$' || c=='&' || c=='{' || c=='}' || c=='_' || c=='#') {
					fprintf(fp, "\\%c", c);
				} else if (c=='<' || c=='>') {
					fprintf(fp, "$%c$", c);
				} else if (c=='\"') {
					fprintf(fp, "“");
				} else {
					fputc(c, fp);	// write character to file
				}
			} else { // but never if in verbatim mode
				fputc(c, fp);
			}
			i++;			// point to next
			last = 0;		// normal character = no CR
		}
	}
	fprintf(fp, "\n\\end{document}\n");
	fclose(hdr);
}

void export_file(char *filename)
{
	g_print("+++ in export_file: %s\n", filename);
	char help[120];
	FILE *fp;
	GtkWidget *statusbar = lookup_widget (GTK_WIDGET(main_window), "main_appbar");
	
	fp = fopen (filename, "wb");
	if (fp == NULL) {
		sprintf(help, "*** cannot open file %s for write", filename);
      	g_print("%s\n", help);
		gnome_appbar_set_status (GNOME_APPBAR (statusbar), help);
		return;
    }
	
	strcpy(help, g_basename(filename));
	mystr_revcut_at(help, '.');
	if (!strcmp(help, "tex")) {
		g_print("    tex - export needs programming\n");
		export_tex_slip(fp, zindex);
 	} else {
	//if (!strcmp(help, "xml")) {
		g_print("    xml - export needs programming\n");
		fprintf(fp, "<filename>%s</filename>\n", g_basename(current_filename));
		fprintf(fp, "<zettel>\n    <no>%d</no>\n", zindex);
		g_ptr_data = zettel[zindex] [7]; // titel
		get_g_data();
		fprintf(fp, "    <title>%s</title>\n", g_zeile);
		g_ptr_data = zettel[zindex] [0]; // text
		get_g_data();
		fprintf(fp, "    <text>\n%s\n</text>\n", g_zeile);
		get_g_data();
		fprintf(fp, "    <words>%s</words>\n", g_zeile);
		get_g_data();
		fprintf(fp, "    <author>%s</author>\n", g_zeile);
		get_g_data();
		fprintf(fp, "    <links>%s</links>\n", g_zeile);
		get_g_data();
		fprintf(fp, "    <notes>%s</notes>\n", g_zeile);
		get_g_data();
		fprintf(fp, "    <date>%s</date>\n", g_zeile);
		get_g_data();
		fprintf(fp, "    <external>%s</external>\n", g_zeile);
		fprintf(fp, "</zettel>\n");
	}
	fclose(fp);
	sprintf(help, _("Zettel %d als %s gespeichert"), zindex, filename);
 	gnome_appbar_set_status (GNOME_APPBAR (statusbar), help);
}

void import_xml(char *filename)
{
	g_print("+++ in import_xml: %s\n", filename);
	FILE *fp;
	char line[1000];
	char zettelfile[100];
	int zettelnr;
	char zetteltitel[100] = "";
	char zettelwords[1000] = "";
	char zettelauthor[500] = "";
	char zettellinks[500] = "";
	char zettelnotes[500] = "";
	char zetteldate[100] = "";
	char zettelextern[1000] = "";
	//char zetteltext[60000];
	
	GtkWidget *statusbar = lookup_widget (GTK_WIDGET(main_window), "main_appbar");

	fp = fopen (filename, "r");
	if (fp == NULL) {
		sprintf(line, "*** cannot open File %s for read", filename);
		g_print("%s\n", line);
 		gnome_appbar_set_status (GNOME_APPBAR (statusbar), line);
  		return;
	}
	int i = 0, err = 0;
	int gettext=0, getauthor=0, getnotes=0;
	while (fgets(line, 1000, fp) != NULL) {
		if (gettext==1) {
			//g_print(".");
			if (!strncmp(line, "</text>", 7)) {
				//g_print("   \nend of text\n%s\n", g_zeile);
				gettext = 0;
			} else {
				strcat(g_zeile, line);
			}
			continue;
		}
		
		line[strlen(line)-1]='\0'; // remove CR
		//g_print("    %d - '%s'\n", i, line);
		if(i==0) { // Filename must be in first line
			if (!strstr(line, "<filename>")) {
				err++;
				break;
			} else {
				mystr_replace_ss(line, "</filename>", "");
				mystr_replace_ss(line, "<filename>", "");
				strcpy(zettelfile, line);
				//g_print("    Filename=%s\n", zettelfile);
			}
			i++;
			continue;
		}
		if(i==1) {
			if (strncmp(line, "<zettel>", 8)) {
				err++;
				break;
			}
			i++;
		}
		if (strstr(line, "</no>")) {
			rm_whitespace(line);
			mystr_replace_ss(line, "<no>", "");
			zettelnr=atoi(line);
			//g_printf("    Zettelnr= %d\n", zettelnr);
			i++;
		}
		if (strstr(line, "</title>")) {
			rm_whitespace(line);
			mystr_replace_ss(line, "</title>", "");
			mystr_replace_ss(line, "<title>", "");
			strcpy(zetteltitel, line);
			//g_printf("    Zetteltitel: %s\n", zetteltitel);
			i++;
		}
		if (strstr(line, "<text>")) {
			//g_print("   found text start\n");
			rm_whitespace(line);
			strcpy(g_zeile, "");
			gettext=1;
			i++;
		}
		if (strstr(line, "</words>")) {
			rm_whitespace(line);
			mystr_replace_ss(line, "</words>", "");
			mystr_replace_ss(line, "<words>", "");
			strcpy(zettelwords, line);
			//g_printf("    Stichworte: %s\n", zettelwords);
			i++;
		}
		/* author maybe multiline */
		if (getauthor==1) {
			strcat(zettelauthor, line);
			//g_printf("    Author: %s\n", zettelauthor);
			i++;
			if (strstr(zettelauthor, "</author>")) { // closing Tag is in line
				mystr_replace_ss(zettelauthor, "</author>", "");
				getauthor=0;
			} else {
				strcat(zettelauthor, "\n");		// append another NL
			}
		}
		if (strstr(line, "<author>")) {
			rm_whitespace(line);
			mystr_replace_ss(line, "<author>", "");
			strcpy(zettelauthor, line);
			if (strstr(zettelauthor, "</author>")) { // closing Tag is in same line
				mystr_replace_ss(zettelauthor, "</author>", "");
			} else {
				strcat(zettelauthor, "\n");		// append NL
				getauthor=1;
			}
		}
		if (strstr(line, "</links>")) {
			rm_whitespace(line);
			mystr_replace_ss(line, "</links>", "");
			mystr_replace_ss(line, "<links>", "");
			strcpy(zettellinks, line);
			//g_printf("    Zettellinks: %s\n", zettellinks);
			i++;
		}
		/* notes maybe multiline */
		if (getnotes==1) {
			strcat(zettelnotes, line);
			g_printf("    Notes: %s\n", zettelnotes);
			i++;
			if (strstr(zettelnotes, "</notes>")) { // closing Tag is in line
				mystr_replace_ss(zettelnotes, "</notes>", "");
				getnotes=0;
			} else {
				strcat(zettelnotes, "\n");		// append another NL
			}
		}
		if (strstr(line, "<notes>")) {
			rm_whitespace(line);
			mystr_replace_ss(line, "<notes>", "");
			strcpy(zettelnotes, line);
			if (strstr(zettelnotes, "</notes>")) { // closing Tag is in same line
				mystr_replace_ss(zettelnotes, "</notes>", "");
			} else {
				strcat(zettelnotes, "\n");		// append NL
				getnotes=1;
			}
		}
		if (strstr(line, "</date>")) {
			rm_whitespace(line);
			mystr_replace_ss(line, "</date>", "");
			mystr_replace_ss(line, "<date>", "");
			strcpy(zetteldate, line);
			//g_printf("    Zetteldate: %s\n", zetteldate);
			i++;
		}
		if (strstr(line, "</external>")) {
			rm_whitespace(line);
			mystr_replace_ss(line, "</external>", "");
			mystr_replace_ss(line, "<external>", "");
			strcpy(zettelextern, line);
			//g_printf("    External: %s\n", zettelextern);
			i++;
		}
		
	}
	if (err!=0) {
		sprintf(line, _("*** %s ist keine gültig Import Datei"), filename);
		g_print("%s\n", line);
 		gnome_appbar_set_status (GNOME_APPBAR (statusbar), line);
  		return;
	}
	gint ret=GTK_RESPONSE_ACCEPT; // default is append
	if (!strcmp(zettelfile, basename(current_filename))) {
		g_print("*** Zettel ist aus aktueller Datei\n");
		if (zettelnr < satz) {
			GtkWidget *dialog = gtk_dialog_new_with_buttons(_("Zettel Import"), main_window,
									GTK_DIALOG_MODAL,
									GTK_STOCK_OK, GTK_RESPONSE_OK,
									GTK_STOCK_ADD, GTK_RESPONSE_ACCEPT,
									GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
									NULL);
			gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
			sprintf(line, _("Zettel Nummer existiert schon!\n\nOK = Zettel %d durch Import-Nr.%d überschreiben\noder + hinzufügen = als neuen Zettel anhängen?"), zindex, zettelnr);
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
			//g_print("    Antwort = %d\n", ret);
			if (ret==GTK_RESPONSE_CANCEL) {
				sprintf(line, _("*** Zettel Import abgebrochen ***"));
		 		gnome_appbar_set_status (GNOME_APPBAR (statusbar), line);
		 		return;
			}
			if (ret==GTK_RESPONSE_OK) {
				long b_end, b_length;
				g_print("    Zettel %d durch %d ersetzen!\n", zindex, zettelnr);
				/* allocate Memory - save (disattach) Data behind change */ 
				g_ptr_data = zettel[zindex+1] [0]; // aktueller Eintrag plus 1
				b_end = zettel[satz+1] [0];        // Ende des Speichers
				b_length = b_end - g_ptr_data +1;  // ermittle Länge der Verschiebung
				char *tmp = malloc(b_length);
				if (b_length != 0 ) {
					/* --- hinteren Pufferbereich verschieben ---*/
					/* g_ptr_data zeigt auf beginnende 00 des ersten zu verschiebenden Satzes */
					g_printf ("*** Verschiebe ab Satz %d - Begin=%d Lenght=%d\n", zindex+1, g_ptr_data, b_length);
					memcpy (tmp, &g_data[g_ptr_data], b_length);
				}

				g_ptr_data = zettel[zindex] [0]; // aktueller Eintrag zum Überschreiben
				g_printf ("Speicher Satz %d ab %d = %x\n", zindex, g_ptr_data, g_data[g_ptr_data]);
				/* start with saving zettel text */
				move2g_data(g_zeile);
				move2g_data(zettelwords);
				move2g_data(zettelauthor);
				move2g_data(zettellinks);
				move2g_data(zettelnotes);
				move2g_data(zetteldate);
				move2g_data(zettelextern);
				move2g_data(zetteltitel);
				/* restore (attach) the rest of data-file and free up memory */
				/* letzte Speicheroperation setzt g_ptr_data */
				if (b_length != 0) {   
					g_printf ("Verschiebe zurück %d - Begin=%d Lenght=%d\n", zindex+1, g_ptr_data, b_length);
					memcpy (&g_data[g_ptr_data], tmp, b_length);
				}
				free(tmp);
				reindex(zindex);

			}
		}
	}
	if (ret==GTK_RESPONSE_ACCEPT) {
		g_print("    Zettel %d als %d anhängen\n", zettelnr, satz+1);
		satz++;
		zindex = satz;
		g_printf("Neuer Zettel: Satz=%d - %s\n", satz, last_action);
		g_ptr_data = zettel[zindex] [0]; // aktueller Eintrag plus 1
		g_printf ("Speicher Satz %d ab %d = %x\n", zindex, g_ptr_data, g_data[g_ptr_data]);
		/* start with saving zettel text */
		move2g_data(g_zeile);
		move2g_data(zettelwords);
		move2g_data(zettelauthor);
		if (!strcmp(zettelfile, basename(current_filename))) {
			move2g_data(zettellinks);
			move2g_data(zettelnotes);
		} else {
			/* add links to zettelextern */
			move2g_data("");
			sprintf(zettelauthor, _("Importiert: Zettel %d aus %s\n%s"), zettelnr, zettelfile, zettelnotes);
			move2g_data(zettelauthor);
		}
		move2g_data(zetteldate);
		move2g_data(zettelextern);
		move2g_data(zetteltitel);
		reindex(zindex);
	}
	file_changed = TRUE;
	text_changed = FALSE;
	neuer_zettel = FALSE;
	/* enable Save-Button */
	GtkButton *button = lookup_widget (GTK_WIDGET (main_window), "main_btn_save"); //save-button
	gtk_widget_set_sensitive(button, TRUE);
	show_zettel(main_window);
}

int read_data_file_string (char* filename, char *found, char *such)
{
	//g_printf("+++ in read_data_file_string: such=%s\n", such);
	char line[100];
	int i, ret= -1;
	
	FILE *fp = fopen (filename, "r");
	if (fp == NULL) {	// --- if file does not exist
		return (ret);
	}
	
	int length = strlen(such);
	while (fgets(line, 100, fp) != NULL) {
    	if(!strncmp(line, such, length)) {	// suche Zeile
    		for (i=length; i<strlen(line); i++) {
    			if (line[i]!=' ' && line[i]!=':' && line[i]!='=' && line[i]!='\"') {
    				break;
				}
			}
			mystr_cut_vorn(line, i);
			while (line[strlen(line)-1]=='\n' || line[strlen(line)-1]=='\"') {
				line[strlen(line)-1] = 0;
			}
			strcpy(found, line);
			ret = 0;
		}
	}
	fclose(fp);
	return (ret);
}
