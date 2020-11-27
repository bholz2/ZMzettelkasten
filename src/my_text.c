/**********************************************************************
 * --- Text Widget Funktionen nach hier ausgelagert ---
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
#include <glib.h>

//#include "functions.h"
#include "support.h"
#include "globals.h"
#include "my_zettel.h"

char stichwort[50];
char sw_feld[200];

/**********************************************************************
 * some subroutines
 ******************************************************************* */
/* --- SR - liest von g_data[g_ptr_data] in g_zeile --- */
/* g_data = Zettelfile in Memory */
int get_g_data ()
{
	int i=0;
	char c;
	
	do {
		g_ptr_data++;
		c = g_data[g_ptr_data];
		g_zeile[i] = c;
		i++;
	} while (c != 0 );
	return(i);
}


/**********************************************************************
 * --- Tag-Table zur Definition der Schriftarten --- 
 ******************************************************************* */
void load_tag_table(GtkTextBuffer *buffer)
{
	int left_mar, left_ind;
	int tab_size;
	
	left_mar=10;	// margin in pixels does not work if desktop DPI != 96
	left_ind = get_i_config("d_l_ind"); // read left_indent from config file
	if (left_ind < 1) {
		left_ind=32;
	}
	tab_size = get_i_config("tab_size"); // read left_indent from config file
	if (tab_size < 1) {
		tab_size=32;
	}
	//g_print("+++ in load tags: dpi=%d, PANGO_SCALE=%d, Indent=%d, TabSize=%d\n", int_dpi, PANGO_SCALE, left_ind, tab_size);
	
	/* --- gtk Format Definitionen --- 
	 werden von main.c aufgerufen - müssen nur einmal pro Buffer geladen werden*/
	PangoTabArray *tabs, *tabl;
	tabs = pango_tab_array_new_with_positions (6,
    										 TRUE,
                                             PANGO_TAB_LEFT, tab_size,
                                             PANGO_TAB_LEFT, tab_size*2,
                                             PANGO_TAB_LEFT, tab_size*3,
                                             PANGO_TAB_LEFT, tab_size*4,
                                             PANGO_TAB_LEFT, tab_size*5,
                                             PANGO_TAB_LEFT, tab_size*6);

	tabl = pango_tab_array_new_with_positions (4,
    										 TRUE,
                                             PANGO_TAB_LEFT, left_ind,
                                             PANGO_TAB_LEFT, left_ind*2,
                                             PANGO_TAB_LEFT, left_ind*3,
                                             PANGO_TAB_LEFT, left_ind*4);
	left_ind=left_ind*-1;
  
	gtk_text_buffer_create_tag (buffer, "custom_tabs",
                                "tabs", tabs,
                                 NULL);
	pango_tab_array_free (tabs);


	/*** einfache ***/
	gtk_text_buffer_create_tag (buffer, "bold",
			      "weight", PANGO_WEIGHT_BOLD, NULL);
	gtk_text_buffer_create_tag (buffer, "italic",
			      "style", PANGO_STYLE_ITALIC, NULL);
	gtk_text_buffer_create_tag (buffer, "underline",
			      "underline", PANGO_UNDERLINE_SINGLE, NULL);
	gtk_text_buffer_create_tag (buffer, "center",
			      "justification", GTK_JUSTIFY_CENTER, NULL);
	gtk_text_buffer_create_tag (buffer, "strikethrough",
			      "strikethrough", TRUE, NULL);
	gtk_text_buffer_create_tag (buffer, "heading",
			      "weight", PANGO_WEIGHT_BOLD,
			      "size", 15 * PANGO_SCALE, NULL);
	gtk_text_buffer_create_tag (buffer, "heading_c",
			      "weight", PANGO_WEIGHT_BOLD,
			      "size", 15 * PANGO_SCALE,
			      "justification", GTK_JUSTIFY_CENTER, NULL);
	/* SuperScript und SubScript */
	gtk_text_buffer_create_tag (buffer, "superscript",
			      "rise", 5 * PANGO_SCALE,	  //* 10 pixels
			      "size", 8 * PANGO_SCALE,	  // 8 points
			      NULL);
	gtk_text_buffer_create_tag (buffer, "subscript",
			      "rise", -5 * PANGO_SCALE,   // 10 pixels
			      "size", 8 * PANGO_SCALE,	   // 8 points
			      NULL);
	/* monospace */
	gtk_text_buffer_create_tag (buffer, "monospace",
				  "size", 9 * PANGO_SCALE,
				  "left_margin", 10,
			      "family", "monospace", NULL);
	/* Liste single */
	gtk_text_buffer_create_tag (buffer, "liste",
					"left_margin", left_mar,
					"tabs", tabl,
					"indent", left_ind, NULL);

	/*** --- doppelte --- ***/
	gtk_text_buffer_create_tag (buffer, "b_und_u",
			      "weight", PANGO_WEIGHT_BOLD,
			      "underline", PANGO_UNDERLINE_SINGLE, NULL);
	gtk_text_buffer_create_tag (buffer, "b_und_c",
			      "weight", PANGO_WEIGHT_BOLD,
			      "justification", GTK_JUSTIFY_CENTER, NULL);
	gtk_text_buffer_create_tag (buffer, "b_und_k",
			      "weight", PANGO_WEIGHT_BOLD,
			      "style", PANGO_STYLE_ITALIC, NULL);
	gtk_text_buffer_create_tag (buffer, "u_und_k",
			      "underline", PANGO_UNDERLINE_SINGLE,
			      "style", PANGO_STYLE_ITALIC, NULL);
	/* Liste ++ */
	gtk_text_buffer_create_tag (buffer, "l_und_d",
					"strikethrough", TRUE,
					"left_margin", left_mar,
					"tabs", tabl,
					"indent", left_ind, NULL);
	gtk_text_buffer_create_tag (buffer, "l_und_f",
					"weight", PANGO_WEIGHT_BOLD,
					"left_margin", left_mar,
					"tabs", tabl,
					"indent", left_ind, NULL);

	/*** --- dreier --- ***/
	gtk_text_buffer_create_tag (buffer, "b_und_u_und_k",
			      "weight", PANGO_WEIGHT_BOLD,
			      "underline", PANGO_UNDERLINE_SINGLE,
			      "style", PANGO_STYLE_ITALIC, NULL);

	/*** --- COLORS --- ***/
	/*** text marker style ***/
	gtk_text_buffer_create_tag (buffer, "y_background",
			      "background", "yellow", NULL);
	/*** foreground color for stichworte ***/
	int bold = get_i_config("sw_bold"); // read bold from config file
	char color[10];
	int ret = get_config(color, "sw_color"); 		// read color from config file
//	if (strlen(color==0)) strcpy(color, "red");
	if (ret) strcpy(color, "red");
	g_print("    color = %s\n", color);
	if (bold == 1) {
		gtk_text_buffer_create_tag (buffer, "red",
					  "weight", PANGO_WEIGHT_BOLD,
					  "foreground", color, NULL);
	} else {
		gtk_text_buffer_create_tag (buffer, "red",
					  "weight", PANGO_WEIGHT_NORMAL,
					  "foreground", color, NULL);
	}

	pango_tab_array_free (tabl);

}


/**********************************************************************
 * --- mark Text on Main Window 
 ******************************************************************* */
void
mark_text(GtkWidget *main_window, char *suchstr, char color)
{
	//g_printf("+++ in mark_text: %s\n", suchstr);
	GtkWidget *text;
	GtkTextBuffer *text_buffer;
	GtkTextIter start, end;
	char *data; // nur pointer, kein puffer
	char UML=0xc3;			// Umlaut in file
	char UML1=0xc2;
	//char UML2=0xffffff9c;	// Umlaut in search string
	char UML3=0xe2;			// Sonderzeichen e2 80 xx
	char BILD=0xef;			// embedded images ef bf bc
	int i, j, korr;
	int wstart, wend, wlen, dlen;
	
	text = lookup_widget (main_window, "main_txt_text");
  	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_text_buffer_get_bounds (text_buffer, &start, &end);
	/* getting Text Buffer incl. code for Images */
	data = gtk_text_buffer_get_slice (text_buffer, &start, &end, TRUE);

	gchar *m_string, *m_such;
	dlen = strlen(data);
	m_string=g_utf8_strup(data, dlen);
	dlen = strlen(m_string);
	wlen = strlen(suchstr);
	/* trunkate possible blank(s) from suchstr */
	while (suchstr[wlen-1]==' ' || suchstr[wlen-1]=='\t') {
		suchstr[wlen-1]='\0';
		wlen--;
	}
	m_such=g_utf8_strup(suchstr, wlen);
	wlen = strlen(m_such);

	j=0;
	korr=0;
	wstart=0;
	for (i = 0; i < dlen; i++) {
		//g_print("%c=%x ", data[i], data[i]);
		/* UTF-8 Umlaute sind zwei Character - Einleitung ausblenden
		 * müssen in data gesucht werden, da "ß" zu "ss" wird */ 
		if (data[i] == UML || data[i] == UML1) {
			//g_printf("-%x", data[i]);
			korr++;
		}
		// UTF-8 Sonderzeichen - 3 fach
		if (data[i] == UML3 || data[i] == BILD) {
			//g_printf("=%x", data[i]);
			korr=korr+2;			
		}
		//g_printf("%c", m_string[i]);
		if (m_string[i] == m_such[j] || j >= wlen) {
			//g_printf("*%c%x", m_string[i], m_string[i]);
			if (j==0) {
				wstart=i-korr;
			}
			j++;
			if (j >= wlen) {
				//korr--;
				wend=i+1-korr;
				//g_printf("mark for %d to %d - %d\n", wstart, wend, korr);
				gtk_text_buffer_get_iter_at_offset (text_buffer, &start, wstart);
				gtk_text_buffer_get_iter_at_offset (text_buffer, &end, wend);
				if (color == 'r') {
					//gtk_text_buffer_apply_tag_by_name(text_buffer, "bold_red", &start, &end );
					gtk_text_buffer_apply_tag_by_name(text_buffer, "red", &start, &end );
				}
				if (color == 'y') {
					gtk_text_buffer_apply_tag_by_name(text_buffer, "y_background", &start, &end );
				}
				j=0;
				wstart=0;
			}	
		} else {
			j=0;
			if (m_string[i]==m_such[j]) {
				j++;
				wstart++;
			}
		}	
	}
    //g_printf("Text %s marked\n", m_such);
    g_free (m_string);
    g_free (m_such);
}


/**********************************************************************
 * marking Text
 * this routine is not longer used -maybe deleted
 ******************************************************************* */	
void
marking_text(GtkWidget *main_window)
{
	g_printf("+++ in marking_text\n");
	GtkWidget *text;
	GtkTextBuffer *text_buffer;
	GtkTextIter start, end;
	char *data; // nur pointer, kein puffer
	//char zeile[50000];
	char UML=0xc3;			// Umlaut in file
	char UML1=0xc2;
	char UML2=0xffffffc3;	// Umlaut in search string
	char UML3=0xe2;			// Sonderzeichen e2 80 xx
	int i, j, korr;
	int wstart, wend, wlen, dlen;
	
	text = lookup_widget (main_window, "main_txt_text");
  	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_text_buffer_get_bounds (text_buffer, &start, &end);
	data = gtk_text_buffer_get_text (text_buffer, &start, &end, TRUE);
	dlen = strlen(data);
	//wlen = strlen(suchstr);
//	g_printf("dlen=%d, wlen=%d\n", dlen, wlen);
//	for (i = 0; i < wlen; i++) {
//		g_printf("%c %x ", suchstr[i], suchstr[i]);
//	}
//	j=0;
	korr=0;
	wstart=0;
	wend=0;
	for (i = 0; i < dlen; i++) {
		// UTF-8 Umlaute sind zwei Character - Einleitung ausblenden
		if (data[i] == UML || data[i] == UML1) {
			korr++;
		}
		// UTF-8 Sonderzeichen
		if (data[i] == UML3) {
			korr=korr+2;			
		}
		
		//if (tolower(data[i]) == suchstr[j] || j >= wlen) {
		// --- <d> beginnt ---
		if (data[i]=='<' && data[i+1]=='U' && data[i+2]=='>') {
			wstart=i-korr;
			i=i+2;
			g_printf("wstart=%d\n", wstart);
		}
		if (data[i]=='<' && data[i+1]=='/' && data[i+2]=='U' && data[i+3]=='>') {
			wend=i-korr;
			i=i+3;
			g_printf("wend=%d\n", wend);
		}
//			j++;
			if (wstart > 0 && wend > 0) {
				//wend=wend-wstart;
			g_printf("wend=%d\n", wend);
				gtk_text_buffer_get_iter_at_offset (text_buffer, &start, wstart);
				gtk_text_buffer_get_iter_at_offset (text_buffer, &end, wend);
				gtk_text_buffer_apply_tag_by_name(text_buffer, "underline", &start, &end );
				j=0;
				wstart=0;
				wend=0;
			}	
//		} else {
//			j=0;
//		}	
	}
//    g_printf("Text %s marked\n", suchstr);
}


/**********************************************************************
 *  fill formated text into text field
 ******************************************************************* */
void fill_text(GtkTextBuffer *buffer)
{
	g_printf("+++ in fill_text\n");
  GtkTextIter iter;
  GtkTextIter start, end;
//  GtkTextMark *mstart, *mend;
  
	char *pstart, *pend;
	char zeile[50000];
	int i, j, length;
	int out, fett, fett_a, kursiv, kursiv_a;
	int unter, unter_a, mono, mono_a;
	int center, center_a, durch, durch_a;
	int head, head_a;
	int super, super_a, sub, sub_a;
	int liste, liste_a;
	int image_a;
	int tab;
	int os_start, os_end, os_cnt=0;

	char bild[130];
	char help[100];
	GdkPixbuf *bild_buf;
	int bc;
	
	/* clear the buffer first */
	gtk_text_buffer_set_text (buffer, "", -1);
   /* get start of buffer; each insertion will revalidate the
   * iterator to point to just after the inserted text. */
	gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

	pstart = strchr(g_zeile, '<');
	
	if (pstart == NULL) {
		gtk_text_buffer_set_text (buffer, g_zeile, -1);
	} else {

		length = strlen(g_zeile);
		j = 0;
		out = 0;
		fett = fett_a = 0;
		kursiv = kursiv_a = 0;
		unter = unter_a = 0;
		mono = mono_a = 0;
		center = center_a = 0;
		durch = durch_a = 0;
		head = head_a = 0;
		super = super_a = 0;
		sub = sub_a = 0;
		liste = liste_a = 0;
		tab = 0,
		image_a = 0;
		strcpy(zeile, "");
		for(i=0; i<length; i++) {  // i points to bytes in Buffer
//			g_printf("%c _ ", g_zeile[i]);
			if (g_zeile[i]=='<' && g_zeile[i+1]=='f' && g_zeile[i+2]=='>') {
				fett = 1;
				out = 1;
				i = i+2;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='f' && g_zeile[i+3]=='>') {
				fett = 0;
				out = 1;
				i = i+3;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='k' && g_zeile[i+2]=='>') {
				kursiv = 1;
				out = 1;
				i = i+2;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='k' && g_zeile[i+3]=='>') {
				kursiv = 0;
				out = 1;
				i = i+3;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='u' && g_zeile[i+2]=='>') {
				unter = 1;
				out = 1;
				i = i+2;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='u' && g_zeile[i+3]=='>') {
				unter = 0;
				out = 1;
				i = i+3;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='m' && g_zeile[i+2]=='>') {
				mono = 1;
				out = 1;
				i = i+2;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='m' && g_zeile[i+3]=='>') {
				mono = 0;
				out = 1;
				i = i+3;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='c' && g_zeile[i+2]=='>') {
				center = 1;
				out = 1;
				i = i+2;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='c' && g_zeile[i+3]=='>') {
				center = 0;
				out = 1;
				i = i+3;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='d' && g_zeile[i+2]=='>') {
				durch = 1;
				out = 1;
				i = i+2;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='d' && g_zeile[i+3]=='>') {
				durch = 0;
				out = 1;
				i = i+3;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='h' && g_zeile[i+2]=='>') {
				head = 1;
				out = 1;
				i = i+2;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='h' && g_zeile[i+3]=='>') {
				head = 0;
				out = 1;
				i = i+3;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='S' && g_zeile[i+2]=='>') {
				super = 1;
				out = 1;
				i = i+2;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='S' && g_zeile[i+3]=='>') {
				super = 0;
				out = 1;
				i = i+3;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='s' && g_zeile[i+2]=='>') {
				sub = 1;
				out = 1;
				i = i+2;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='s' && g_zeile[i+3]=='>') {
				sub = 0;
				out = 1;
				i = i+3;
			// Liste zum Testen
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='l' && g_zeile[i+2]=='>') {
				liste = 1;
				out = 1;
				i = i+2;
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='l' && g_zeile[i+3]=='>') {
				liste = 0;
				out = 1;
				i = i+3;
			// TABs zum Testen
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='t' && g_zeile[i+2]=='>') {
				tab = 1;
				out = 1;
				i = i+2;
				g_print("    + open Tab-Tag - %c\n", g_zeile[i+1]);
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='t' && g_zeile[i+3]=='>') {
				tab = 2;
				out = 1;
				i = i+3;
			/*** --- image tag found --- ***/
			} else if (g_zeile[i]=='<' && g_zeile[i+1]=='i' && g_zeile[i+2]=='>') {
				i = i+3;
				image_a = 1;
				out = 1;
				bc=0;
				// read name of image file
				while (i<length) {
					if (g_zeile[i]=='<' && g_zeile[i+1]=='/' && g_zeile[i+2]=='i' && g_zeile[i+3]=='>') {
						bild[bc]=0;
						g_print ("    pic-name: %s\n", bild);
						i = i+3;
						break;
					}
					bild[bc]=g_zeile[i];
					i++;
					bc++;
				}
			} else {
				zeile[j] = g_zeile[i];
				j++;
				zeile[j] = '\0';
				char UML1=0xc3;
				char UML2=0xc2;
				char UML3=0xe2;
				char UML4=0xa4;
				/* correct pointer for multi-byte UTF-8 */
				if (g_zeile[i] != UML1 && g_zeile[i] != UML2 && g_zeile[i] != UML3) {
					os_cnt++;
				} else if (g_zeile[i] == UML3) {
					os_cnt--;
					//g_print("%x:%x:%x ", g_zeile[i], g_zeile[i+1], g_zeile[i+2]);
				} else {
					//os_cnt--;
					//g_print("%x:%x ", g_zeile[i], g_zeile[i+1]);
				}
			}
			
			if (out == 1) { // write zeile to buffer if something changed
//				g_printf("\nTeil: %s i=%d j=%d f_a=%d k_a=%d u_a=%d m_a=%d\n", zeile, i, j, fett_a, kursiv_a, unter_a, mono_a);
				/*** --- tripple taged --- ***/
				// must be processed before dual tagged
				if (fett_a && unter_a && kursiv_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "b_und_u_und_k", NULL);
				/*** --- dual tagged --- ***/
				// must be prcessed before single tagged
				} else if (head_a && center_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "heading_c", NULL);
				} else if (fett_a && center_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "b_und_c", NULL);
				} else if (fett_a && unter_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "b_und_u", NULL);
				} else if (fett_a && kursiv_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "b_und_k", NULL);
				} else if (unter_a && kursiv_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "u_und_k", NULL);
				} else if (liste_a && durch_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "l_und_d", NULL);
				} else if (liste_a && fett_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "l_und_f", NULL);
				/*** --- single tagged are below --- ***/
				} else if (head_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "heading", NULL);
				// liste zum Testen
				} else if (liste_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "liste", NULL);
				} else if (mono_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "monospace", NULL);
				} else if (sub_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "subscript", NULL);
				} else if (super_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "superscript", NULL);
				} else if (durch_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "strikethrough", NULL);
				} else if (center_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "center", NULL);
				} else if (fett_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "bold", NULL);
				} else if (kursiv_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "italic", NULL);
				} else if (unter_a) {
					gtk_text_buffer_insert_with_tags_by_name (buffer, &iter,
						zeile, -1, "underline", NULL);
				} else {
					gtk_text_buffer_insert(buffer, &iter, zeile, -1);
				}
				/*** --- place image in buffer --- ***/
				if (image_a==1) {
					image_a = 0;
					if(bild[0]!='/') { //relativ name is given
						strcpy(help, bild);
						sprintf(bild, "%s/%s", g_path, help);
					}
					if(test_file_exist(bild)) {
						bild_buf = gdk_pixbuf_new_from_file(bild, NULL);
					} else {
						g_print ("*** pic-file not found: %s\n", bild);
						bild_buf = gdk_pixbuf_new_from_file("/usr/local/share/pixmaps/ZMzettelkasten/picture_missing.png", NULL);
					}	
					//g_print ("    out image: %s\n", bild);
					gtk_text_buffer_insert_pixbuf(buffer, &iter, bild_buf);
					os_cnt++;
				}
				/*** --- tab processing ---***/
				if (tab==1) {
					os_start=os_cnt;
					//g_print("    tab-Start: start=%d, i=%d\n", os_start, i);
					tab=0;
				}
				if (tab==2) {
					os_end=os_cnt;
					tab=0;
					g_print("    iter=%d\n", iter);
					/* get start und end iter */
					gtk_text_buffer_get_iter_at_offset (buffer, &start, os_start);
					gtk_text_buffer_get_iter_at_offset (buffer, &end, os_end);
					/* use yellow background while testing */
					//gtk_text_buffer_apply_tag_by_name (buffer, "y_background", &start, &end );
					gtk_text_buffer_apply_tag_by_name (buffer, "custom_tabs", &start, &end);
				}
					
				strcpy(zeile, "");
				out = 0;
				j = 0;
				fett_a = fett;
				kursiv_a = kursiv;
				unter_a = unter;
				mono_a = mono;
				center_a = center;
				durch_a = durch;
				head_a = head;
				sub_a = sub;
				super_a = super;
				liste_a = liste;
			}
		}
		if (strlen(zeile) != 0) {
			gtk_text_buffer_insert (buffer, &iter, zeile, -1);
		}
	}
	//gtk_text_buffer_get_bounds (buffer, &start, &end);
	//g_print("    buffer bounds start=%d, end=%d\n", start, end);
	//gtk_text_buffer_apply_tag_by_name (buffer, "custom_tabs", &start, &end);
	//marking_text(main_window);
	g_print("    -- end fill_text\n");
}


/**********************************************************************
 * --- Update Main Window - zeige Zettel
 ******************************************************************* */
void show_zettel(GtkWidget *main_window)
{
	g_print("+++ in show_zettel - %d\n", zindex);
	GtkWidget *text, *statusbar, *label, *img;
	GtkTextBuffer *text_buffer;
	GtkTextIter start, end, where;
	GtkWidget *menu;
	GdkPixbuf *image;
	GtkWidget *icon;
	int size;
	int i;
	
	push_zindex();	// push zindex to stack
	if (satz == 0) satz=1;
	size = g_ptr_data = zettel[zindex] [0];
	strcpy(g_datum_e, "");  // lösche g_datum_e 

	/*** --- Text --- ***/
	size = get_g_data();
	cursor_pos = get_len(cursor_pos, 1);
//	g_printf("load_text=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
	/* schreibe in text Feld */
	text = lookup_widget (main_window, "main_txt_text");
  	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
	gtk_widget_set_sensitive(text, TRUE);
  	//gtk_text_buffer_set_text (text_buffer, g_zeile, -1);
//	g_print("    1st char stichwort = '%c'\n", g_data[g_ptr_data+1]);

	/* test for encoded text */
	if (g_zeile[0]=='@') {
		g_print("    Text is encoded\n");
		image = create_pixbuf("ZMzettelkasten/lock.png");
		int ret=strdecode(g_zeile);
		if(!ret) {
			mystr_cut_vorn(g_zeile, 1);	// strip @	
  			fill_text(text_buffer);
		} else {
  			gtk_text_buffer_set_text (text_buffer, _("*** Text ist verschlüsselt ***"), -1);
		}
	} else {
		image = create_pixbuf("");
  		fill_text(text_buffer);
	}
	icon = lookup_widget(GTK_WIDGET(main_window), "main_img_keyed");
	gtk_image_set_from_pixbuf(icon, image);	

	/* place cursor in main window */
	gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(text_buffer), &where, 0);
	gtk_text_iter_forward_chars (&where, cursor_pos);
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(text_buffer), &where);
	GtkTextMark *tmark=gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(text_buffer), NULL, &where, FALSE);
	gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (text), tmark, 0.0, TRUE, 0.0, 0.5);

	/*** --- Stichwort --- ***/
	get_g_data();
//	g_printf("stichwort=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
  	// --- bearbeite Daten ---
  	mystr_replace(g_zeile, ',', '\n');
	/* schreibe in stichwort Feld */
	text = lookup_widget (GTK_WIDGET(main_window), "main_txt_stichwort");
  	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
//	char pix[50];
	switch (g_zeile[0]) { // check for 1.Char in Stichwort 
		case '$': // template
			menu = lookup_widget(GTK_WIDGET(main_window), "main_mnu_temp");
			image = create_pixbuf("ZMzettelkasten/template.png");
			mystr_cut_vorn(g_zeile,1);
			break;
		case '%': // copy of template - list - code
			menu = lookup_widget(GTK_WIDGET(main_window), "main_mnu_sz_list");
			image = create_pixbuf("ZMzettelkasten/list.png");
			mystr_cut_vorn(g_zeile,1);
			break;
		case '#': // inactive
			menu = lookup_widget(GTK_WIDGET(main_window), "main_mnu_done");
			image = create_pixbuf("ZMzettelkasten/done.png");
			mystr_cut_vorn(g_zeile,1);
			break;
		case '!': // wichtig - todo
			menu = lookup_widget(GTK_WIDGET(main_window), "main_mnu_todo");
			image = create_pixbuf("ZMzettelkasten/todo.png");
			mystr_cut_vorn(g_zeile,1);
			break;
		case '*': // idea
			menu = lookup_widget(GTK_WIDGET(main_window), "main_mnu_sz_idea");
			image = create_pixbuf("ZMzettelkasten/idea.png");
			mystr_cut_vorn(g_zeile,1);
			break;
		case '?': // info
			menu = lookup_widget(GTK_WIDGET(main_window), "main_mnu_sz_info");
			image = create_pixbuf("ZMzettelkasten/info.png");
			mystr_cut_vorn(g_zeile,1);
			break;
		case '[': // book
			menu = lookup_widget(GTK_WIDGET(main_window), "main_mnu_sz_book");
			image = create_pixbuf("ZMzettelkasten/book.png");
			mystr_cut_vorn(g_zeile,1);
			break;
		case ']': // book
			menu = lookup_widget(GTK_WIDGET(main_window), "main_mnu_sz_text");
			image = create_pixbuf("ZMzettelkasten/text.png");
			mystr_cut_vorn(g_zeile,1);
			break;
		case '&': // person
			menu = lookup_widget(GTK_WIDGET(main_window), "main_mnu_sz_people");
			image = create_pixbuf("ZMzettelkasten/person.png");
			mystr_cut_vorn(g_zeile,1);
			break;
		default:
			menu = lookup_widget(GTK_WIDGET(main_window), "main_mnu_norm");
            image = create_pixbuf("");
	}
  	gtk_text_buffer_set_text (text_buffer, g_zeile, -1);
	icon = lookup_widget(GTK_WIDGET(main_window), "main_img_stich");
//	icon = gtk_image_new_from_pixbuf(image);
	gtk_image_set_from_pixbuf(icon, image);	
	//gtk_toggle_button_set_active(menu, TRUE);
	checkmenu=TRUE; // prevent from working the emited signals
	//gtk_check_menu_item_set_state (menu, "TRUE");
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(menu), TRUE); 


	/* check toggle button state */
	int active;
	GtkWidget *button;
	button=lookup_widget(main_window, "main_cbtn_stichview");
	active=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
  	if (active) {	// Stichworte werden in Text markiert
		char dat_tok[]="\n";
		char stw_tok[]="/";
		char *ptr, *stw_zeile;
		char stichwort[80];
		
		stw_zeile = strtok(g_zeile, dat_tok);
		while (stw_zeile != NULL) {
			strcpy(stichwort, stw_zeile);
			if (stichwort[0]=='#' ||  stichwort[0]=='!') {
				mystr_cut_vorn(stichwort, 1);
			}
			/* cut stichwort at / if needed */
			ptr = strrchr(stichwort, '/');
			if (ptr!=0) *ptr = '\0';
			//g_printf("stichwort = %s\n", stichwort);
			mark_text(main_window, stichwort, 'r');
			stw_zeile = strtok(NULL, dat_tok);
		}
	}

	/*** --- Autor --- ***/
	get_g_data();
//	g_printf("autor=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
	/* schreibe in autor Feld */
	text = lookup_widget (main_window, "main_txt_autor");
  	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
  	gtk_text_buffer_set_text (text_buffer, g_zeile, -1);

	/*** --- Links auf Zettel --- ***/
	get_g_data();
//	g_printf("Links=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
	// --- fill Verweis-Liste in main_window ---
    fill_tree_view_m();
//	g_print("Zurueck\n");

	// --- restore g_ptr_data = zerstört von fill_tree_view ---
	g_ptr_data = zettel[zindex] [4];

	/*** --- Anmerkung - Vermerk --- ***/
	get_g_data();
//	g_printf("vermerk=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
	/* schreibe in vermerk Feld */
	text = lookup_widget (main_window, "main_txt_vermerk");
  	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
  	gtk_text_buffer_set_text (text_buffer, g_zeile, -1);

	/*** --- Datum --- ***/
	get_g_data();
//	g_printf("datum_= %s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
	strcpy (verweis1, "");
	char dat_tok[]=";";
	char *sub_zeile;
	
	sub_zeile = strtok(g_zeile, dat_tok);
	i = 0;
	while (sub_zeile != NULL) {
//		g_printf("datum = %s\n", sub_zeile);
		if(i==0) strcpy(g_datum_e, sub_zeile);  // Kopiert bis EX-Semikolon
		if(i==1) strcpy (verweis1, sub_zeile);
		sub_zeile = strtok(NULL, dat_tok);
		i++;
	}
//	g_printf("ydatum: c=%s - e=%s\n", g_datum_e, verweis1);

	/*** --- Externe Links --- ***/
	get_g_data();
//	g_printf("verweis2=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
	label = lookup_widget (main_window, "main_lbl_extern");
	img = lookup_widget (main_window, "main_image_extern");
	menu = lookup_widget (main_window, "main_mnu_exlinks");

	// --- enable/disable ---
	if (strlen(g_zeile) == 0) {
		gtk_widget_set_sensitive(label, FALSE);
		gtk_widget_set_sensitive(img, FALSE);
		gtk_widget_set_sensitive(menu, FALSE);
	} else {
		gtk_widget_set_sensitive(label, TRUE);
		gtk_widget_set_sensitive(img, TRUE);
		gtk_widget_set_sensitive(menu, TRUE);
	}
	
	/*** --- Titel/Headline --- ***/
	get_g_data();
//	g_printf("headline=%s - g_ptr_data=%d\n", g_zeile, g_ptr_data);
	/* schreibe in Headline Label Feld */
 	label = lookup_widget (main_window, "main_lbl_head");
  	gtk_label_set_text (GTK_LABEL(label), g_zeile);
	/* Zettelnummer in Statusbar */
	sprintf(g_zeile, _("Zettel %d von %d - %d Zeichen - %s - %s"), zindex, satz, size, g_datum_e, verweis1);
	//g_print("    %s\n", g_zeile);
	statusbar = lookup_widget (main_window, "main_appbar");
	//gtk_statusbar_pop (GTK_STATUSBAR (statusbar), 1);
	//gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, g_zeile);
 	gnome_appbar_set_status (GNOME_APPBAR (statusbar), g_zeile);
	button = lookup_widget (main_window, "main_btn_edit");
	if(!readonly) gtk_widget_set_sensitive(button, TRUE);
	button = lookup_widget (main_window, "main_btn_new");
	if(!readonly) gtk_widget_set_sensitive(button, TRUE);
	checkmenu = FALSE; // enable working the emited radio signals
	zettel_toggle = FALSE;
}


/**********************************************************************
 * --- Search & Replace Text in Edit Window 
 ******************************************************************* */
void
search_e_text(char *suchstr, int direction)
{
	g_printf("+++ in search_e_text: %s\n", suchstr);
  GtkTextMark *mark;
  GtkTextIter start, begin, end;

  gtk_text_buffer_get_selection_bounds (sBuf, &start, &end);
  
	if (gtk_text_iter_compare (&start, &end) != 0) {
		gtk_text_iter_forward_char (&start); // advance start+1
	}
	gboolean success;
	char datei[20];
	if (direction==0) {
  		success = gtk_text_iter_forward_search (&start, suchstr, 3, &begin, &end, NULL);
  		strcpy (datei, "Dateiende");
  	} else {
  		success = gtk_text_iter_backward_search (&start, suchstr, 0, &begin, &end, NULL);
  		strcpy (datei, "Dateianfang");
	}  		
  
  /* Select the instance on the screen if the string is found. Otherwise, tell
   * the user it has failed. */
  if (success) {
    mark = gtk_text_buffer_create_mark (sBuf, NULL, &begin, FALSE);
    //gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (editor->textview), mark);
	gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (sView), mark, 0.0, TRUE, 0.0, 0.5);
    gtk_text_buffer_delete_mark (sBuf, mark);
    gtk_text_buffer_select_range (sBuf, &begin, &end);
  } else {
  	gchar *string;
  	sprintf(string, "%s erreicht\n»%s«\nnicht (mehr) gefunden!", datei, suchstr);
    GtkWidget *dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, 
                                     GTK_MESSAGE_INFO, GTK_BUTTONS_OK, 
                                     string);
    
    //g_free (string);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
  }
}

void
replace_e_text(char *suchstr, int multiple)
{
	g_printf("+++ in replace_e_text: %s\n", suchstr);
	GtkTextMark *mark;
	GtkTextIter start, begin, end;

	gtk_text_buffer_get_selection_bounds (sBuf, &start, &end);
  
	gboolean success;
	char replace[80];
	GtkWidget *field = lookup_widget(GTK_WIDGET(search_dialoge), "replace_entry_new");
	strcpy(replace, gtk_entry_get_text(GTK_ENTRY(field)));
	
  	success = gtk_text_iter_forward_search (&start, suchstr, 3, &begin, &end, NULL);
	if(!multiple) {
		if (success) {
			gtk_text_buffer_select_range (GTK_TEXT_BUFFER(sBuf), &begin, &end);
			gtk_text_buffer_delete (GTK_TEXT_BUFFER(sBuf), &begin, &end);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(sBuf), replace, -1);
			search_e_text(suchstr, 0);
		}
	} else {
		//g_print("    in multiple\n");
		while (success) {
			gtk_text_buffer_select_range (GTK_TEXT_BUFFER(sBuf), &begin, &end);
			gtk_text_buffer_delete (GTK_TEXT_BUFFER(sBuf), &begin, &end);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(sBuf), replace, -1);
			search_e_text(suchstr, 0);
			gtk_text_buffer_get_selection_bounds (sBuf, &start, &end);
  			success = gtk_text_iter_forward_search (&start, suchstr, 3, &begin, &end, NULL);
		}
	}
}

void my_search_dialog (gchar *string)
{
	g_print("+++ in create search dialog - '%s'\n", string);
	if (search_dialoge == NULL) {
		search_dialoge = GTK_WIDGET(create_search_dialog());
		g_signal_connect (G_OBJECT (search_dialoge), "destroy",
			G_CALLBACK (gtk_widget_destroyed), &search_dialoge);		
	}
	GtkWidget *field = lookup_widget(search_dialoge, "search_entry_text");
	gtk_entry_set_text(field, string);
	field = lookup_widget(search_dialoge, "search_cbtn_case");
	gtk_widget_set_sensitive(field, FALSE);
	gtk_window_present (GTK_WINDOW (search_dialoge));
}

void my_replace_dialog (gchar *string)
{
	g_print("+++ in create search dialog - '%s'\n", string);
	if (search_dialoge == NULL) {
		search_dialoge = GTK_WIDGET(create_replace_dialog());
		g_signal_connect (G_OBJECT (search_dialoge), "destroy",
			G_CALLBACK (gtk_widget_destroyed), &search_dialoge);
	}
	GtkWidget *field = lookup_widget(search_dialoge, "replace_entry_old");
	gtk_entry_set_text(field, string);
	field = lookup_widget(search_dialoge, "replace_cbtn_case");
	gtk_widget_set_sensitive(field, FALSE);
	gtk_window_present (GTK_WINDOW (search_dialoge));
}


/**********************************************************************
 *  Routine for password question
 ******************************************************************* */
/* Dialog for entering password */
void dialog_password(char *word)
{
	GtkWidget *dialog, *label, *hbox, *input;

	dialog =gtk_dialog_new_with_buttons ("Password", main_window,
										GTK_DIALOG_MODAL,
										GTK_STOCK_OK, GTK_RESPONSE_OK,
										NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
	label = gtk_label_new ( _("Password"));
	input = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(input), TRUE); // passes the RETURN-key to default Button
	gtk_entry_set_max_length(GTK_ENTRY(input), 20);
	gtk_entry_set_visibility(GTK_ENTRY(input), FALSE);
	gtk_entry_set_invisible_char(GTK_ENTRY(input), '*');

	hbox = gtk_hbox_new (FALSE, 10);
	gtk_box_pack_start_defaults(GTK_BOX(hbox), label);				 
	gtk_box_pack_start_defaults(GTK_BOX(hbox), input);				 
	gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox);

	gtk_widget_show_all (dialog);

	gint result = gtk_dialog_run (GTK_DIALOG(dialog));
	if (result == GTK_RESPONSE_OK) {
//		g_printf("*** Password: %s\n", gtk_entry_get_text (GTK_ENTRY(input)));
		strcpy(word, gtk_entry_get_text (GTK_ENTRY(input)));
		if(word[0]==word[strlen(word)-1]) { // password muss ungleiche buchstaben haben
		    word[strlen(word)-1]+=1;
		}
	} else {
		strcpy(word, ""); //unset password
	}
	gtk_widget_destroy (dialog);		 
	                       
}

/**********************************************************************
 *  Routinen für stichwort popup menu
 ******************************************************************* */
/* Dialog for entering Stichwort text line */
void dialog_stichwort(char *text, int size, int action)
{
	g_print("+++ in dialog_stichwort\n");
	GtkWidget *dialog, *label, *image, *hbox, *vbox, *input;
	
	dialog = gtk_dialog_new_with_buttons (_("Stichwort"), main_window,
										 GTK_DIALOG_MODAL,
										 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
										 GTK_STOCK_OK, GTK_RESPONSE_OK,
										 NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
	
	/* Create Dialoge */
	if (action) {
		label = gtk_label_new ( _("Stichwort löschen"));
	} else {
		label = gtk_label_new ( _("Stichwort eingeben/editieren"));
	}
	image = gtk_image_new_from_file ("/usr/local/share/pixmaps/ZMzettelkasten/bookmark.png");
	input = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(input), TRUE); // passes the RETURN-key to default Button
	gtk_entry_set_max_length(GTK_ENTRY(input), size-15);
	gtk_entry_set_text(GTK_ENTRY(input), text);
	hbox = gtk_hbox_new (FALSE, 10);
	vbox = gtk_vbox_new (FALSE, 10);
	gtk_box_pack_start_defaults(GTK_BOX(vbox), label);				 
	gtk_box_pack_start_defaults(GTK_BOX(vbox), input);				 
	gtk_box_pack_start_defaults(GTK_BOX(hbox), image);				 
	gtk_box_pack_start_defaults(GTK_BOX(hbox), vbox);
	
	gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox);
	gtk_widget_show_all (dialog);
	
	strcpy(text, "");
	gint result = gtk_dialog_run (GTK_DIALOG(dialog));
	if (result == GTK_RESPONSE_OK) {
		sprintf(text, "%s", gtk_entry_get_text (GTK_ENTRY(input)));
	}
	gtk_widget_destroy (dialog);		 
}

void stichwort_new()
{
	g_printf("+++ in stichwort_new\n");
	dialog_stichwort(stichwort, sizeof(stichwort), 0);
	g_ptr_data = zettel[zindex] [1];
	int len=get_g_data();
	g_print("    g_zeile='%s' - sw='%s'\n", g_zeile, stichwort);
	if (g_zeile[len-2] == ',') g_zeile[len-2]=0; //???
	if(strlen(g_zeile) < 2) {
		sprintf(sw_feld, "%s%s", g_zeile, stichwort);
	} else {
		sprintf(sw_feld, "%s,%s", g_zeile, stichwort);
	}
	save_stichwort(zindex, sw_feld);
	show_zettel(main_window);
}

void stichwort_read(char *word)
{
	GtkWidget *widget = lookup_widget(main_window, "main_txt_stichwort");
	GtkTextBuffer *buffer;
	GtkTextIter where;
	GtkTextIter start, end;

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
    gtk_text_buffer_get_iter_at_mark (buffer, &where, gtk_text_buffer_get_insert (buffer));
	int line_no=gtk_text_iter_get_line(&where);

	gtk_text_buffer_get_iter_at_line(buffer, &start, line_no);
	gtk_text_buffer_get_iter_at_line(buffer, &end, line_no);
	gtk_text_iter_forward_to_line_end(&end);
	strcpy(word, gtk_text_buffer_get_text (buffer, &start, &end, TRUE));
//	g_print("    word=%s\n", word);
}

void stichwort_edit()
{
	char old_word[50];
	
	g_printf("+++ in stichwort_edit\n");
	stichwort_read(stichwort);
	strcpy(old_word, stichwort);
	dialog_stichwort(stichwort, sizeof(stichwort), 0);
	if (strcmp(stichwort, "") && strcmp(stichwort, old_word)) {
//		g_print("    change %s\n", stichwort);
		g_ptr_data = zettel[zindex] [1];
		int len=get_g_data();
		strcpy(sw_feld, g_zeile);
		mystr_replace_ss(sw_feld, old_word, stichwort);
		save_stichwort(zindex, sw_feld);
		show_zettel(main_window);
	}
}

void stichwort_delete()
{
	g_printf("+++ in stichwort_delete\n");
	stichwort_read(stichwort);
	dialog_stichwort(stichwort, sizeof(stichwort), 1);
	if (strcmp(stichwort, "")) {
//		g_print("    delete %s\n", stichwort);
		g_ptr_data = zettel[zindex] [1];
		int len=get_g_data();
		strcpy(sw_feld, g_zeile);
		mystr_replace_ss(sw_feld, stichwort, "");
		mystr_replace_ss(sw_feld, ",,", ",");			 // remove empty line
		if (sw_feld[0]==',') mystr_cut_vorn(sw_feld, 1); // remove leading ,
		len=strlen(sw_feld);
		if (sw_feld[len-1]==',') sw_feld[len-1]=0;	     // remove trailing linefeed
		save_stichwort(zindex, sw_feld);
		show_zettel(main_window);
	}
}
