#include <gnome.h>

void
main_btn_open_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_btn_save_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_btn_new_clicked                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_btn_edit_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_btn_first_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_btn_back_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_btn_forward_clicked               (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_btn_last_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_btn_goto_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_btn_index_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_mnu_new_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_open_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_save_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_save_as_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_quit_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
main_lbl_extern_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
about_btn_close_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
open_file_btn_cancel_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
open_file_btn_ok_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
save_file_btn_cancel_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
save_file_btn_ok_clicked               (GtkButton       *button,
                                        gpointer         user_data);

gboolean
main_window_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
main_mnu_zgoto_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zneu_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zedit_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_about_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zfirst_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zback_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_mnu_znext_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zlast_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_forward_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zforward_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
open_file_chooser_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
save_file_chooser_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
main_cbtn_inactive_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
main_cbtn_stichview_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
main_link_list_row_activated           (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

void
main_link_list_cursor_changed          (GtkTreeView     *treeview,
                                        gpointer         user_data);

void
pu_mnu_liste_gehe_zu_zettel_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
pu_mnu_liste_link_anlegen_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
pu_mnu_liste_x_link_anlegen_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
pu_mnu_liste_link_loeschen_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
pu_msg_box_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
pu_msg_btn_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
pu_msg_btn_save_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
pu_input_box_btn_cancel_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
pu_input_box_btn_ok_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
main_mnu_hilfe_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_web_seite_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_save_view_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_index_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
ext_link_liste_row_activated           (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

void
ext_link_liste_cursor_changed          (GtkTreeView     *treeview,
                                        gpointer         user_data);

gboolean
ext_link_window_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_clear1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_properties1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
edit_window_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
edit_entry_titel_changed               (GtkEditable     *editable,
                                        gpointer         user_data);

void
edit_mnu_save_close_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_save_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_undu_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_redo_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_schliessen_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_btn_save_close_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
edit__btn_save_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
edit_btn_close_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
edit_btn_undo_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
edit_btn_redo_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
edit_btn_save_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
edit_mnu_vsave_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_lineno_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_close_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
main_link_list_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
pu_mnu_newz_x_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
pu_mnu_liste_newz_x_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zneu_x_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zfind_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zneu_f_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_quit_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_note_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_autor_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_text_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_titel_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_vsave_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_index_mnu_links_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_vsave_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_links_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_ffilter_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_fclear_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_btn_quit_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
index_button_filter_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
index_button_clear_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

gboolean
index_window_delete_event              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
index_list_cursor_changed              (GtkTreeView     *treeview,
                                        gpointer         user_data);

void
index_btn_xlink_clicked                (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_mnu_file_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
file_info_btn_cancel_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
file_info_btn_ok_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
main_mnu_undo_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_btn_undo_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_mnu_oaut_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_ostw_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_ohead_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_olink_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_norm_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_todo_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_done_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_temp_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
main_mnu_exlinks_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
main_label_extern_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
main_image_extern_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
main_mnu_autor_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_stichw_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_exlink_check_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_setup_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
list_mnu_quit_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
list_window_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_list_mnu_vsave_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
list_btn_close_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
list_mnu_vsave_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
list_treeview_row_activated            (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

void
main_btn_autor_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_btn_stich_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
edit_mnu_syntax_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_sp_aus_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_edit_mnu_sp_default_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_sp_english_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
option_btn_seldir_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
option_btn_close_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
option_cbtn_margin_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
option_btn_selfont_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
edit_mnu_sp_default_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_sp_german_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_search_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
search_dialog_delete_event             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
search_btn_close_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
search_btn_back_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
search_btn_vor_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
edit_btn_search_clicked                (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
edit_mnu_timestamp_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
option_btn_default_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_edit_mnu_autor_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
replace_dialog_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
replace_btn_close_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
replace_btn_search_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
replace_btn_replace_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
replace_btn_all_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_edit_mnu_replace_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_autor_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_ins_dual_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_ins_quote_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
option_cbtn_autobackup_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
edit_mnu_ins_check_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
edit_mnu_ins_arrow_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_z_export_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_z_import_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zbookmark_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
main_txt_stichwort_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
main_mnu_zv_pdf_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_btn_pdf_clicked                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_mnu_sz_idea_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_sz_info_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_sz_list_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_sz_people_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_sz_book_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_sz_text_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
pu_mnu_sw_new_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
pu_mnu_sw_edit_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
pu_mnu_sw_delete_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_password_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
main_img_keyed_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
main_img_keyed_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
main_img_keyed_focus_in_event          (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

void
on_index_mnu_fhidden_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_ftodo_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_fhidden_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_fkeyed_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_zv_copy_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_btn_copy_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_mnu_z_key_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_z_delete_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
main_mnu_v_back_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_fidea_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_finfo_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_flist_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_fpeople_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_fbook_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_ftext_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_mnu_ftemplate_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
index_button_todo_clicked              (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
index_button_info_clicked              (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
index_button_idea_clicked              (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
main_mnu_elast_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
