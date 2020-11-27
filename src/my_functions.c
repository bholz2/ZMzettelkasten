/* --- myFunktions ---*/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>

#include "my_functions.h"
#include "globals.h"

#include <libintl.h>
#define _(Text) gettext(Text)

#define LEN_ZSTACK 10
gint zstack[LEN_ZSTACK];

/**********************************************************************
 *    S T R I N G - F U N C T I O N S
 ******************************************************************* */
/* -- Tauscht einzelne Zeichen in String aus -- */
void mystr_replace (char *string, char in, char out)
{
	int i, length;
	length = strlen(string);
	for(i=0; i<length; i++) {
		if (string[i] == in) {
			//g_printf("%c ", string[i]);
			string[i] = out;
		}
	}
	//g_printf("replace: %s %i %i %d\n", string, in, out, i);
}

/* tauscht substr in string aus */
//char *replace(const char *s, const char *old, const char *new)
void mystr_replace_ss(char *str, char *old, char *new)
{
	char *s;
	s = malloc(strlen(str)+100);
	strcpy (s, str);
	int i;
	size_t newlen = strlen(new);
	size_t oldlen = strlen(old);
	
	//g_print("+++ in replace_ss: %s, %s, %d, %s, %d\n", s, old, oldlen, new, newlen);

	i = 0;
	while (*s) {
		if (s[0] == old[0] && str[i-1] != '\\') {	// just compare single bytes for faster loop
		//g_print("    found %d: %c%c%c\n", i, s[0], s[1], s[2]);
			if (strstr(s, old) == s) {	// if first byte matches ... look for the complete string
				strcpy(&str[i], new);
				i += newlen;
				s += oldlen;
			} else {
				str[i++] = *s++;
			}
		} else {
			str[i++] = *s++;
		}
	}
	str[i] = '\0';
	//free(s); // will crash ... no idea why
	//g_print("*** new line=%s", str);
}

/* -- Sucht Substring in String & ignore case */
/*    liefert 1 wenn gefunden und 0 wenn falsch */
int mystr_such (char *string, gchar *such)
{
	int i, length;
//	int zeich;
//	gunichar zeich;
	gchar *m_string, *m_such;
	length = strlen(string);
	//g_printf("in strsuch:%s=%d - %s\n", string, length, such);
	m_string=g_utf8_strup(string, length);
	length = strlen(such);
	m_such=g_utf8_strup(such, length);
	//g_printf("in strsuch: %d:%d - %s\n", length, i, m_such);
	if (strstr(m_string, m_such)) {
		return(1);
	} else {
		return(0);
	}
}

/* Schneidet vorn am String "cut" Zeichen ab 
 * bzw. verschiebt String um cut Zeichen nach vorn */
void mystr_cut_vorn(char *string, int cut)
{
//	g_printf("in mystr_cut_vorn: %s, -%d\n", string, cut);
	int i, j=0, length;
	length = strlen(string);
	
	for (i=0; i<length; i++) {
		if (i < cut) {
			//g_printf("cut %d\n", i);
		} else {
			string[j]=string[i];
			j++;
		}
	}
	string[j]='\0'; // terminate string
	return;
}

/* removed leading & trailing whitespace and CR*/ 
void rm_whitespace(char *string)
{
	int i, j=0, length;
	length = strlen(string);
	
	/* take off trailing whitespace first */
	for (i=length-1; i>=0; i--) {
		if (string[i]==' ' || string[i]=='\t' || string[i]=='\n') {
			string[i] = '\0';
		} else {
			break;
		}
	}		
	length = strlen(string);
	for (i=0; i<length; i++) {
		if (string[i]==' ' || string[i]=='\t') {
			j++;
		} else {
			break;
		}
	}
	/* j contains nr. of whitespace */
	if (j!=0) {
		mystr_cut_vorn(string, j);
	}
}

/* cuts string a first occurents of "cut"
 * and stripps possible blank space - returns first part of string */
void mystr_cut_at(char *string, char cut)
{
	//g_printf("+++ in mystr_cut_at: %s, %c\n", string, cut);
	int i, j=0, length;

	length = strlen(string);
	for (i=0; i<length; i++) {
		if (string[i]==cut) {
			length=i;
		}
	}
	while (string[length-1]==' ' || string[length-1]=='\t'){
		length--;
	}
	string[length]='\0'; //place string terminator
//	g_printf("    outstring=%s\n", string);
}

/* cuts string a the last occurents of "cut"
 * returns last part of string */
void mystr_revcut_at(char *string, char cut)
{
	//g_printf("+++ in mystr_revcut_at: %s, %c\n", string, cut);
	int i, j=0, length;

	length = strlen(string);
	for (i=length; i>0; i--) {
		if (string[i]==cut) {
			//pos=i;
			break;
		}
	}
	i++;
	mystr_cut_vorn(string, i);
	//g_printf("    outstring='%s'\n", string);
}

/* cuts zettel marking characters for line */
void strip_stichwort(char *line)
{
	if (line[0]=='$' || line[0]=='%' || line[0]=='#' || line[0]=='!' || line[0]=='*' || line[0]=='?' || line[0]=='[' || line[0]==']' || line[0]=='&') {
		mystr_cut_vorn(line, 1);
	}
}

/**********************************************************************
 *    String ver- und entschlüsseln
 ******************************************************************* */
/*** verschluesseln ***/
int strencode(char *string) {
	g_print("+++ encode Zetteltext\n");
//	g_print("    password: '%s'\n", g_password);
	int i, j=0, n;
	int out, key;
	int count,last;
	char *sin;

	if (strlen(g_password)==0) return 1; // password not set
	sin = malloc(strlen(string)+10);
	strcpy (sin, string);
	string[j]='@';	// set keymark first

	int klen=strlen(g_password);
	key= g_password[klen-1];
//	g_print("    pw-len: %d, %c\n", klen, key);
	for (i=0; i<klen; i++) {
		out = g_password[i] ^ key;
		j++;
		string[j] = out;
		key = out;
	}
	n=0;
	count=0;
	for (i=0; i<strlen(sin); i++) { // sin should start with '@' not decoded
		count++;
		out = sin[i] ^ g_password[n] ^ key;
		out&=0xFF;
		j++;
		n++;
		if(n>=klen) { n=0; }
		if (out==0) {
			out=0xfe;
			string[j]=0x01;
			j++;
//			g_print("f0+");
		}
		if (out==0xff) {
			out=0xfc;
			string[j]=0x01;
			j++;
//			g_print("f0+");
		}
		//if (last==0xf0 && out==0xfe) g_print("+++ Byte: %d = %c - last=01-FE\n", count, sin[i]);
		//if (last==0xf0 && out==0xfc) g_print("+++ Byte: %d = %c - last=01-FC\n", count, sin[i]);
		last=out;
		string[j] = out;
		key = out;
//		g_print("%x ", out);
	}
	j++;
	string[j]=0; // terminate string
	g_print("    encoded Bytes: %d\n", count);
	free(sin);
//	g_print("\n");
	return 0;
}

/*** entschlüsseln ***/
int strdecode(char *string) {
	g_print("+++ decode Zetteltext\n");
//	g_print("    password: '%s'\n", g_password);
	char *sin;
	char keyword[20];
	int i, j=0, n;
	int out, key;
	int in, in1;
	int count=0;

	if (strlen(g_password)==0) return 1; // password not set
	sin = malloc(strlen(string)+10);
	strcpy (sin, string);
//	mystr_cut_vorn(sin, 1);
	string[j]='@';	// set keymark first
	
	int klen=strlen(g_password);
	key= g_password[klen-1];
	for (i=0; i<klen; i++) {
		j++;
		out = sin[j] ^ key & 0xFF;
		keyword[i] = out;
		key = sin[j];
	}
	keyword[i]=0;
//	g_print("    kword=%s - password=%s\n", keyword, g_password);
	if (strncmp(keyword, g_password, klen))	{
		g_print("*** password does not match ***\n");
		return 1;
	}
	n=0;
	i=1;
	j++;
	while(sin[j]!=0) {
		count++;
		in = sin[j]&0x0ff;
//		g_print("%x-", in);
		if (in == 0x01) {
			in1 = sin[j+1]&0x0ff;
			if (in1 == 0xfe) {
//				g_print("»FE« ");
				in = 0;
				j++; // points to 2nd FE
				out = in ^ g_password[n] ^ key;
				//g_print("+++ Byte: %d = %c-%x - last=01-FE \n", count, out, out);
				key=0xfe;
			} else if (in1 == 0xfc) {
//				g_print("»FE« ");
				in = 0xff;
				j++; // points to 2nd FE
				out = in ^ g_password[n] ^ key;
				//g_print("+++ Byte: %d = %c-%x - last=01-FC \n", count, out, out);
				key=0xfc;
			} else {
				out = in ^ g_password[n] ^ key;
				key=in;
			}
		} else {
			out = in ^ g_password[n] ^ key;
			key=in;
		}
		
//		g_print("%c=%x ", out, out);
		string[i]=out;
		n++;
		if (n>=klen) { n=0; }
		j++;
		i++;
	}
	string[i]=0;	// terminate string
	g_print("    decoded Bytes: %d\n", count);
//	g_print("    decodet str=%s\n", string);
	
	free(sin);
	return 0;
}

/**********************************************************************
 *    D A T E - F U N C T I O N S
 ******************************************************************* */
/* schreibt System Datum nach g_datum in der Form tt.mm.yyyy */
void get_datum(char *datum)
{
	struct tm *tmnow;
	
	time_t tnow;
	time(&tnow);
	tmnow = localtime(&tnow);
	/* Format %a=day %b=Month, %d=day, %Y=Year */
	//strftime(datum, 30, _("%a %d. %b %Y"), tmnow);
	strftime(datum, 30, "%a %Y-%m-%d %H:%M", tmnow);
	//strftime(g_timestamp, 80, "--- %H:%M (%Z) %A, %d %B %Y ---",tmnow);
	strftime(g_month, 40, "%B %Y",tmnow);
	//g_print("    %s\n", g_month);
	
	//g_print("+++ Today: %d.%d.%d\n", tmnow->tm_mday, tmnow->tm_mon + 1, tmnow->tm_year + 1900);
	/* prints date in local format */
	char string[80];
	//strftime(string, 80, "Es ist %H Uhr und %M Minuten (%Z) %A, %d %B %Y",tmnow);
	strftime(string, 80, "--- %c ---",tmnow);
	//g_printf("    %s\n",string);
}

void time_stamp(char *stamp)
{
	struct tm *tmnow;
	time_t tnow;
	time(&tnow);
	tmnow = localtime(&tnow);
	/* %H=Hour %M=Minute %Z=Timezone */
	strftime(stamp, 80, "--- %H:%M (%Z) %A, %d %B %Y ---",tmnow);
}


/**********************************************************************
 *    error PopUps
 ******************************************************************* */
void error_msg(char *string)
{
	GtkWidget *dialog;
	
	dialog = gtk_message_dialog_new(main_window, GTK_DIALOG_MODAL,
									GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
									string);
	gtk_window_set_title(GTK_WINDOW(dialog), "Information");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void error_not_exist()
{
	error_msg(_("ACHTUNG\nFunktion noch nicht implementiert"));
}

/**********************************************************************
 *    specific ZETTELKASTEN - F U N C T I O N S
 ******************************************************************* */
/* -- Reindex zettelkasten in Memory -- */
void reindex(int index)
{
	int i, feld;
	char c;
	
	g_ptr_data = zettel[index] [0]; // --- ab aktuellem Zettel
//	g_printf ("in ReIndex: Satz %d ab %d = %x\n", index, g_ptr_data, g_data[g_ptr_data]);
	g_ptr_data++;
	i = index;	
	feld = 0;
	do {
		c = g_data[g_ptr_data];
		if (c == 0) {
			feld++;
			zettel[i] [feld] = g_ptr_data;
			if (feld == 8) {
				i++;
				feld = 0;
//				g_printf ("Index: Satz %d ab %d = %x\n", i, g_ptr_data, g_data[g_ptr_data]);
			}
		}
		g_ptr_data++;
	} while (i <= satz);
	g_ptr_data--; // korrigiere End of Buffer
	zettel[i] [0] = g_ptr_data; // Ende von g_data
	g_printf("*** Satz=%d - auf %d\n", i, g_ptr_data);
}   /* --- End Reindex --- */


void push_zindex()
{
	int i;
	GtkWidget *button;
	
//	g_printf("+++ in push_zindex (%d)", zindex);
	if (zstack[LEN_ZSTACK]!=zindex){
		for (i=0; i<LEN_ZSTACK; i++) {
			zstack[i] = zstack[i+1];
//			g_printf(" %d", zstack[i]);
		}
		zstack[i] = zindex;
		button = lookup_widget (GTK_WIDGET (main_window), "main_btn_undo"); 
		gtk_widget_set_sensitive(button, TRUE);
		button = lookup_widget (GTK_WIDGET (main_window), "main_mnu_undo"); 
		gtk_widget_set_sensitive(button, TRUE);
	}
//	g_printf("\n");
}

void pop_zindex ()
{
	int i;
	GtkWidget *button;
	
//	g_printf("+++ in pop_zindex");
	for (i=LEN_ZSTACK; i>0; i--) {
		zstack[i] = zstack[i-1];
//		g_print(" %d", zstack[i]);
	}
	zstack[i] = 0;
	zindex = zstack[LEN_ZSTACK];
	if (zstack[LEN_ZSTACK-1] == 0) {
		button = lookup_widget (GTK_WIDGET (main_window), "main_btn_undo"); 
		gtk_widget_set_sensitive(button, FALSE);
		button = lookup_widget (GTK_WIDGET (main_window), "main_mnu_undo"); 
		gtk_widget_set_sensitive(button, FALSE);
	}
//	g_printf("\n");
}

void clear_zindex()
{
	int i;
	GtkWidget *button;
	for (i=0; i<LEN_ZSTACK; i++) {
		zstack[i] = 0;
	}
	button = lookup_widget (GTK_WIDGET (main_window), "main_btn_undo"); 
	gtk_widget_set_sensitive(button, FALSE);
	button = lookup_widget (GTK_WIDGET (main_window), "main_mnu_undo"); 
	gtk_widget_set_sensitive(button, FALSE);
}

/* clear indexz=array for fillup index window */
void clear_indexz()
{
	int i;
	for (i=0; i<=satz; i++) {
		indexz[i]=0;
	}
}

/* clearing word structure */
void clear_words()
{
	int i;
	for (i=0; i<1000; i++) {
		strcpy(worte[i].wort, "");
	}
}

void set_ztype(char *string)
{
	g_printf("+++ in set_ztype: %s\n", string);
	GtkWidget *text;
	GtkTextBuffer *text_buffer;
	char help[500];
	
	strcpy(help, string);
	g_ptr_data = zettel[zindex] [1]; // addressiert Stichwort
	get_g_data(); // holt Stw nach g_zeile
	mystr_replace(g_zeile, ',', '\n');
	/* strip first character if one of those */
//	if (g_zeile[0]=='!' || g_zeile[0]=='$' || g_zeile[0]=='#' || g_zeile[0]=='%') {
	if (g_zeile[0]=='$' || g_zeile[0]=='%' || g_zeile[0]=='#' || g_zeile[0]=='!' || g_zeile[0]=='*' || g_zeile[0]=='?' || g_zeile[0]=='[' || g_zeile[0]==']' || g_zeile[0]=='&') {
		mystr_cut_vorn(g_zeile, 1);	
	}
	strcat(help, g_zeile);
	text = lookup_widget (GTK_WIDGET(main_window), "main_txt_stichwort");
  	text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
  	gtk_text_buffer_set_text (text_buffer, help, -1);
  	mystr_replace(help, '\n', ',');
  	save_stichwort(zindex, help);
  	show_zettel(main_window);
}


/**********************************************************************
 *  check if external links are valid
 ******************************************************************* */
/* --- create Tree_view (Popup External-Link-Liste) incl. headline --- */
GtkWidget * create_view_and_model_cext (GtkWidget *view)
{
	GtkTreeViewColumn   *col;
	GtkCellRenderer     *renderer;

	if (store_ext_val == FALSE) { /* needed to avoid double columns
		* since popup destroy may cause core dumps ...
		* - and popup hide does not clean columns*/ 
		/* --- Column #1 --- */
		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new_with_attributes ("Nr.", renderer,
												   "text", COL_ZETTEL, NULL);
		gtk_tree_view_column_set_sort_column_id (col, COL_ZETTEL);
		gtk_tree_view_append_column (GTK_TREE_VIEW(view), col);
		renderer = gtk_cell_renderer_text_new ();
		gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), -1,
												   "defekte Links", renderer,
												   "text", COL_TITEL,
												   NULL);
		store_ext_val = TRUE;
	}

	store_ext = gtk_list_store_new (NUM_COLS, GDK_TYPE_PIXBUF,  G_TYPE_UINT, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), GTK_TREE_MODEL(store_ext));
	g_object_unref (store_ext); /* destroy store automatically with view */

	return view;
}


void check_exlinks()
{
	g_printf("+++ in check_exlinks +++\n");
	int c,i,j;
	int count=0, ccount=0, fcount=0, fmcount=0, mcount=0, wcount=0, zcount=0, zmcount=0;
	int icount=0;
	int len, len1;
	char *line;
	char token[]=";";
	char cmd[500];
	char bild[130];
	char fname[130];

	GtkWidget *liste, *view, *label;
	GtkStyle *style;
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
	liste = lookup_widget (link_window, "ext_link_liste");
//g_printf("+++ MARK +++\n");
	view = create_view_and_model_cext(liste);
	//fill_tree_view_ext();
	strcpy(last_action, "chk_ext");

	GtkTreeIter iter;
	
	for (i=1; i<=satz; i++) {
		// address text field
		g_ptr_data = zettel[i] [0];
		get_g_data();
		if (mystr_such(g_zeile, "<i>")==1) {
//			g_print("found <i> in Zettel %d\n", i);
			len=strlen(g_zeile);
			for (j=0; j<len; j++) {
				if (g_zeile[j]=='<' && g_zeile[j+1]=='i' && g_zeile[j+2]=='>') {
					icount++;
					count++;
					j+=3;
					c=0;
					while (g_zeile[j]!='<') {
						bild[c]=g_zeile[j];
						c++;
						bild[c]=0;
						j++;
					}
					if (bild[0]=='/') {
						strcpy(fname,bild);
					} else {
						sprintf(fname, "%s/%s", g_path, bild);
					}
//					g_print("Bild-File=%s\n", fname);
					if(!test_file_exist(fname)) {
						//g_print("    File %s ok\n", bild);
//					} else {
						g_print("    *** File %s not found ***\n", fname);
						gtk_list_store_append (store_ext, &iter);
						gtk_list_store_set (store_ext, &iter, COL_ZETTEL, i, COL_TITEL, bild, -1);
						fmcount++;
					}
				}
			}
					
		}
		// adress ext-Links field
		g_ptr_data = zettel[i] [6];
		get_g_data();
		/* check if Links noted */
		len=strlen(g_zeile);
//		g_printf("Zettel %d/%d - %s\n", i, len, g_zeile);
		if (len !=0) {
			/* links found - split line */
     		line = strtok(g_zeile, token);
 			while (line != NULL) { // Zerlege bei ; in Zeilen
//     			g_printf("    Link=%s\n", line);
				count++;
     			if (!strncmp(line, "http", 4)) { // http(s) links
//     				g_printf("    Web-Link not tested: %s\n", line);
     				wcount++;
     			} else if (!strncmp(line, "mail", 4)) { // mail links
//     				g_printf("    Mail-Link not tested: %s\n", line);
     				mcount++;
     			} else if (!strncmp(line, "-z", 2)) { // zettelkasten links
     				len1=strlen(line);
     				for (j=3; j<len1; j++) {
     					if (line[j] == ' ') {
							j++;
							mystr_cut_vorn(line, j);
							break;
						}
					}    						 
     				if (line[0]=='/') { 
     					strcpy(cmd, line);
     				} else {
     					sprintf(cmd, "%s/%s", g_path, line);
					}
					mystr_cut_at(cmd, ' ');
     				j = test_file_exist(cmd);
     				if (j==0) {
     					g_printf("*** Zettel %d: File %s does not exist ***\n", i, cmd);
						gtk_list_store_append (store_ext, &iter);
						gtk_list_store_set (store_ext, &iter, COL_ZETTEL, i, COL_TITEL, cmd, -1);
						fmcount++;
 					}
					zcount++;
     			} else if (!strncmp(line, "zms ", 4)) { // ZMmanuskript links
     				len1=strlen(line);
					mystr_cut_vorn(line, 4);
					mystr_cut_at(line, '=');
					/* lese Path aus HOME/.zms.cfg */
					char HOME[50];
					strcpy(HOME, g_get_home_dir());				 
					sprintf(cmd, "%s/.zms.cfg", HOME);
					read_data_file_string(cmd, HOME, "PATH");
     				sprintf(cmd, "%s%s", HOME, line);
     				//g_print("    => %s\n", cmd);
     				j = test_file_exist(cmd);
     				if (j==0) {
     					g_printf("*** ZMmanuskript File %s does not exist ***\n", cmd);
						gtk_list_store_append (store_ext, &iter);
						gtk_list_store_set (store_ext, &iter, COL_ZETTEL, i, COL_TITEL, cmd, -1);
						fmcount++;
 					}
					zmcount++;
				} else if (!strncmp(line, "!", 1)) { // executables
				     //mystr_cut_vorn(line, 1);
					g_printf("    Zettel %d: Executable found: %s\n", i, line);
					g_printf("    path=%s\n", getenv("PATH"));
					g_printf("    *** not tested ***\n");
					ccount++;
     			} else {
     				if(!strncmp(line, "file:/", 6)) { //file references
     					mystr_cut_vorn(line, 6);
     				} else if (line[0] == '/')  {
     					sprintf(cmd, "%s", line);
     				} else {
     					sprintf(cmd, "%s/%s", g_path, line); // relative links
     				}
     				j = test_file_exist(cmd);
     				if (j==0) {
     					g_printf("*** Zettel %d: File %s does not exist ***\n", i, cmd);
						gtk_list_store_append (store_ext, &iter);
						gtk_list_store_set (store_ext, &iter, COL_ZETTEL, i, COL_TITEL, cmd, -1);
						fmcount++;
     				} else {
//     					g_printf("    Datei %s existiert\n", cmd);
					}
					fcount++;
				}
    			line = strtok(NULL, token);
			}
		}
	}
//g_printf("+++ MARK +++\n");
	sprintf(cmd, _("<b>=== EXTERNAL LINK CHECK ===</b>\n<tt>%7d Links insgesamt gefunden, davon:\n"), count);
	if (icount!=0) sprintf(cmd, _("%s%7d eingebundene Bilder getestet\n"), cmd, icount);
	if (wcount!=0) sprintf(cmd, _("%s%7d Web-Links - nicht getestet\n"), cmd, wcount);
	if (mcount!=0) sprintf(cmd, _("%s%7d Mail-Links - nicht getestet\n"), cmd, mcount);
	if (ccount!=0) sprintf(cmd, _("%s%7d Executables - nicht getestet\n"), cmd, ccount);
	if (zcount!=0) sprintf(cmd, _("%s%7d ZMzettelkasten Links getestet\n"), cmd, zcount);
	if (zmcount!=0) sprintf(cmd, _("%s%7d ZMmanuskript Links getestet\n"), cmd, zmcount);
	if (fcount!=0) sprintf(cmd, _("%s%7d lokale Dateien getestet\n"), cmd, fcount);
	if (fmcount!=0) {
		sprintf(cmd, _("%s%7d Dateien nicht gefunden</tt>"), cmd, fmcount);
	} else {
		sprintf(cmd, _("%s</tt>=== alle Links sind ok ==="), cmd);
	}
	g_printf("%s\n", cmd);

	label = lookup_widget (link_window, "ext_link_lbl");
	gtk_label_set_markup (GTK_LABEL(label), cmd);
	//gtk_widget_show(label);
	//style = gtk_widget_get_style(label);
    //pango_font_description_set_weight(style->font_desc, PANGO_WEIGHT_NORMAL);
    //gtk_widget_modify_font(label, style->font_desc);
}

int count_zettel_links()
{
	g_printf("+++ in count_zettel_links +++\n");
	int count=0;
	int i, len;
	char *line;
	char token[]=" ";

	for (i=1; i<=satz; i++) {
		g_ptr_data = zettel[i] [3];
		get_g_data();
		/* check if Links noted */
		len=strlen(g_zeile);
//		g_printf("Zettel %d/%d - %s\n", i, len, g_zeile);
		if (len !=0) {
			/* links found - split line */
     		line = strtok(g_zeile, token);
 			while (line != NULL) { // Zerlege bei ' ' in Zeilen
				count++;
    			line = strtok(NULL, token);
			}
		}
	}
	g_print("    found %d Links total\n", count);
	return (count);
}

int count_zettel_stich()
{
	g_printf("+++ in count_zettel_stich +++\n");
	int count=0;
	int i, len;
	char *line;
	char token[]=",";

	for (i=1; i<=satz; i++) {
		g_ptr_data = zettel[i] [1];
		get_g_data();
		/* check if Links noted */
		len=strlen(g_zeile);
//		g_printf("Zettel %d/%d - %s\n", i, len, g_zeile);
		if (len !=0) {
			/* links found - split line */
     		line = strtok(g_zeile, token);
 			while (line != NULL) { // Zerlege bei ' ' in Zeilen
				count++;
    			line = strtok(NULL, token);
			}
		}
	}
	g_print("    found %d Stichworte total\n", count);
	return (count);
}

/**********************************************************************
 *  suche und liste stichworte
 ******************************************************************* */
void list_stichwort()
{
	g_printf("+++ in list_stichwort +++\n");
	int i,j, d;
	int n=0;
	int len;
	char *line;
	char token[]=",";

	clear_words(); //clear structure, defined in globals.h
	for (i=1; i<=satz; i++) {
		g_ptr_data = zettel[i] [1];
		get_g_data();
		strip_stichwort(g_zeile);
//		if (g_zeile[0]=='!' || g_zeile[0]=='#' || g_zeile[0]=='$' || g_zeile[0]=='%') {
//			mystr_cut_vorn(g_zeile, 1);
//		}
		/* check if Links noted */
		len=strlen(g_zeile);
//		g_printf("Zettel %d/%d - %s\n", i, len, g_zeile);
		/* fill array-structure */
		if (len !=0) {
     		line = strtok(g_zeile, token);
 			while (line != NULL) { // Zerlege bei , in Zeilen
     			//g_printf("    Stichwort='%s'\n", line);
     			/* check if in array */
     			mystr_cut_at(line, '/');
     			d=0;
     			for (j=0; j<=n; j++) {
     				if (!strcmp(worte[j].wort, line)) {
     					worte[j].anzahl++;
     					//g_printf("    %3d:add   %s=%d\n", i, line, worte[j].anzahl, j);
     					d=1;
     				}
     			}
     			if (d==0) { // addiere neues Wort
     				strcpy(worte[n].wort, line);
     				worte[n].anzahl=1;
     				n++;
     				//g_printf("    %3d:found '%s'=1 @ %d\n", i, line, n-1);
     			}
				//count++;
    			line = strtok(NULL, token);
			}
		}
	}
//	g_printf("....Starte sortieren!!\n");
	for(i = 0; i < n; i++) {
		for(j=i+1;j<n;j++) {
			if(strcmp(worte[i].wort, worte[j].wort) > 0) {
				*temp=worte[j];
				worte[j]=worte[i];
				worte[i]=*temp;
			}
		}
	}
//	g_printf("....Sortiert!!\n");
	g_printf("+++ fertig: %d Stichwörter gelistet\n", n);
	strcpy(list_action, "stich");
	show_list_window(n);
}


/**********************************************************************
 *  suche und liste autoren/quellen
 ******************************************************************* */
void list_autor()
{
	g_printf("+++ in list_autor +++\n");
	int i,j, d;
	int n=0, ret;
	int len;
	char *line;
	//char token[]=",:\n";
	char token[20];
	ret=get_config(token, "token");
	if (ret < 0) { strcpy(token, ":"); }
	
	g_print("    token='%s'\n", token);
	strcat(token, "\n"); // add a linefeed to token

	clear_words(); // clear structure, defined in globals.h
	for (i=1; i<=satz; i++) {
		g_ptr_data = zettel[i] [2];
		get_g_data();
		len=strlen(g_zeile);
//		g_printf("Zettel %d/%d - %s\n", i, len, g_zeile);
		/* fill array-structure */
		if (len !=0) {
     		line = strtok(g_zeile, token);
     		rm_whitespace(line);
			d=0;
			for (j=0; j<=n; j++) {
				if (!strcmp(worte[j].wort, line)) {
					worte[j].anzahl++;
					//g_printf("    %3d:add  '%s'=%d\n", i, line, worte[j].anzahl);
					d=1;
				}
			}
			if (d==0) { // addiere neues Wort
				strcpy(worte[n].wort, line);
				worte[n].anzahl=1;
				n++;
				//g_printf("    %3d:found %s=1 @ %d\n", i, line, n-1);
			}
		}
	}
//	g_printf("....Starte sortieren!!\n");
	for(i = 0; i < n; i++) {
		for(j=i+1;j<n;j++) {
			if(strcmp(worte[i].wort, worte[j].wort) > 0) {
				*temp=worte[j];
				worte[j]=worte[i];
				worte[i]=*temp;
			}
		}
	}
//	g_printf("....Sortiert!!\n");
	g_printf("+++ fertig: %d Autoren gelistet\n", n);
	strcpy(list_action, "autor");
	show_list_window(n);
}

/**********************************************************************
 *  find last edited slip
 ******************************************************************* */
int last_edited()
{
	g_printf("+++ in last_edited +++\n");
	int i,j, d;
	int n=0, ret=0;
	int len;
	char *line;
	char token[]=";";
	char token1[]=" ";
	char lastdate[60];
	strcpy(lastdate, "");
	
	clear_words(); // clear structure, defined in globals.h
	for (i=1; i<=satz; i++) {
		g_ptr_data = zettel[i] [5];
		get_g_data();
		//g_print("  + found '%s'\n", g_zeile);
		if(strlen(g_zeile)<=10) continue;
		line = strtok(g_zeile, token);
		line = strtok(NULL, token);
		if(line==NULL) continue;
		//g_print("    Date(%d): %s\n", i, line);
		len = strlen(line);
		mystr_cut_vorn(line, len-16);
		if (strcmp(lastdate, line) < 0 && line[0]=='2') {
			strcpy(lastdate, line);
			ret=i;
		}
	}
	g_print("--- result is: %d, %s\n", ret, lastdate);
	return ret;
}
