/* functions.h */

void my_file_info();
void new_file ();
void open_file ();
void real_open_file	(const gchar *filename);
void real_save_file (const gchar *filename);
void save_as ();

int get_config (char *found, char *such);
void write_config (char *string, char *such);
