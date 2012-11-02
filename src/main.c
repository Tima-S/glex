/* GLex
 *
 * Copyright (C) 2011 Timur R. Salikhov <tima-s@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>

#include <gdk/gdk.h>

#include <string.h>

#include "xdxf.h"

GtkTextBuffer *text_view_buffer;
GtkWidget *combo_entry;



gint max_count_elem_in_combo_entry = 5;
gint window_width = 400;
gint window_height = 500;

gboolean no_save_size = FALSE; /* When window is maximized or fullscreened */

#define CONF_FILE_NAME "glex.conf"
#define DICT_FILE_NAME "dict.xdxf"


gboolean combo_entry_has_item (gchar *item) {
	GtkTreeModel *model = GTK_TREE_MODEL(gtk_combo_box_get_model(GTK_COMBO_BOX (combo_entry)));
	GtkTreeIter iter;
	if (gtk_tree_model_get_iter_first (model, &iter)) 
		do {
			gchar *str;
			gtk_tree_model_get( model, &iter, 0, &str, -1 );
			if  (g_strcmp0 (str, item) == 0) {
				g_free (str);
				return TRUE;
			}
		} while (gtk_tree_model_iter_next (model, &iter));	
	
	return FALSE;
	
} 

gboolean idl_print_result () {
	GtkWidget * entry = gtk_bin_get_child ( GTK_BIN (combo_entry));
	if (result) {
		gtk_text_buffer_set_text (text_view_buffer, result, -1);
		
		if (!combo_entry_has_item (word)) {	
			gint count = gtk_tree_model_iter_n_children(GTK_TREE_MODEL(gtk_combo_box_get_model(GTK_COMBO_BOX (combo_entry))),NULL);
			if (count < max_count_elem_in_combo_entry)
 				if (GTK_MAJOR_VERSION > 2) 
					gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_entry), word);
				else
					gtk_combo_box_append_text (GTK_COMBO_BOX (combo_entry), word);
					
			else 
				if (GTK_MAJOR_VERSION > 2) {
					gtk_combo_box_text_remove (GTK_COMBO_BOX_TEXT (combo_entry), 0);
					gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_entry), word);
				}
				else {
					gtk_combo_box_remove_text (GTK_COMBO_BOX (combo_entry), 0);
					gtk_combo_box_append_text (GTK_COMBO_BOX (combo_entry), word);
				}
		}
		
		gtk_widget_set_sensitive (combo_entry, TRUE);
		gtk_editable_select_region(GTK_EDITABLE(GTK_ENTRY(entry)), 0, -1);
		gtk_widget_grab_focus (entry);
		
		return FALSE;
	}
	else
		if (word_not_found == FALSE)
			return TRUE;
		else {
			gtk_widget_set_sensitive (combo_entry, TRUE);
			gtk_editable_select_region(GTK_EDITABLE(GTK_ENTRY(entry)), 0, -1);
			gtk_widget_grab_focus (entry);
			return FALSE;
		}
		
}

void on_combo_activate (GtkEntry *entry) {
	
	if (word) {
		g_free (word);
		word = NULL;
	}
	
	word = pango_trim_string (gtk_entry_get_text (entry));
	if (g_strcmp0 (word, "") != 0) {
	
		gtk_widget_set_sensitive (combo_entry, FALSE);
	
		if (result) {
			g_free (result);
			result = NULL;
			concat_res = NULL;
		}
		
		word_not_found = FALSE;
		
		g_thread_create ( (GThreadFunc)open_xdxf_file, NULL, FALSE, NULL);
		gtk_text_buffer_set_text (text_view_buffer, "", -1);	
		g_idle_add ( (GSourceFunc) idl_print_result, NULL);
	}
}

gchar* get_app_home_dir () {
	const gchar *home = g_getenv("HOME");
	gchar *app_home_dir = g_strconcat (home,"/.glex/", NULL);
	//g_free (home);
	return app_home_dir;
}

void save_config_file (GtkWidget *window) {
	gchar *dir = get_app_home_dir ();
	gchar *config_file_path = g_strconcat (dir, CONF_FILE_NAME, NULL);
	
	if (!g_file_test(g_path_get_dirname(config_file_path), G_FILE_TEST_EXISTS))
		g_mkdir(g_path_get_dirname(config_file_path), 0700);
		
	FILE *f = g_fopen (config_file_path, "w");
	if (f) {
		GKeyFile *kf = g_key_file_new();
		if (no_save_size == FALSE) 
			gtk_window_get_size (GTK_WINDOW (window), &window_width, &window_height);

		g_key_file_set_integer (kf,"Geometry","width", window_width);
		g_key_file_set_integer (kf,"Geometry","height", window_height);
		
		g_key_file_set_integer (kf,"Interface","history", max_count_elem_in_combo_entry);
		g_key_file_set_string (kf,"Dict", "dict", path_to_dict);
		
		gchar *wkf = g_key_file_to_data (kf,NULL,NULL);
		g_fprintf (f,"%s",wkf);
		fclose(f);
		g_free(wkf);
		g_key_file_free (kf);
	}
	
	g_free (dir);
	g_free (config_file_path);
 	
}

void read_config_file () {
	gchar *dir = get_app_home_dir ();
	gchar *config_file_path = g_strconcat (dir, CONF_FILE_NAME, NULL);

	GKeyFile *kf = g_key_file_new();		
	if (g_key_file_load_from_file (kf, config_file_path, G_KEY_FILE_NONE,NULL)) {
		window_width = g_key_file_get_integer (kf,"Geometry", "width", NULL);
		window_height = g_key_file_get_integer (kf,"Geometry", "height", NULL);
		max_count_elem_in_combo_entry = g_key_file_get_integer (kf,"Interface", "history", NULL);
		path_to_dict = g_key_file_get_string (kf,"Dict", "dict", NULL);
		
	}
	
	if (window_width == 0)
		window_width = 400;
	if (window_height == 0)
		window_height = 500;
	if (max_count_elem_in_combo_entry == 0)
		max_count_elem_in_combo_entry = 5;
	if (path_to_dict == NULL) {
		path_to_dict = g_strconcat (dir, "dict/", DICT_FILE_NAME, NULL);
	}
	
	
	g_free (dir);
	g_free (config_file_path);
	g_key_file_free (kf);
}

void on_quit () {
	gtk_main_quit ();
}

void on_window_restate (GtkWidget *w, GdkEvent  *event) {
	if (event->window_state.new_window_state == GDK_WINDOW_STATE_MAXIMIZED)
		no_save_size = TRUE;
	else
		no_save_size = FALSE;
	
} 

gboolean on_window_delete (GtkWidget *w) {
	save_config_file (w);
	return FALSE;
}

int main (int argc, char *argv[]) {
	gtk_init (&argc, &argv);
	
	g_thread_init (NULL);
	xdxf_init ();
	
	GtkWidget *entry;

	#if GTK_MAJOR_VERSION == 3 
		combo_entry = gtk_combo_box_text_new_with_entry ();
	#else
		combo_entry = gtk_combo_box_entry_new_text ();
	#endif

	entry = gtk_bin_get_child (GTK_BIN (combo_entry));

	GtkWidget *text_view;
	text_view = gtk_text_view_new ();
	text_view_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

	if (argc > 1) {
		gtk_entry_set_text (GTK_ENTRY (entry), argv[1]);
		on_combo_activate (GTK_ENTRY (entry));
	}

	read_config_file ();

	GtkWidget *mainwin;	
	mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	GtkWidget *vbox, *align;
	
	#if GTK_MAJOR_VERSION == 3
		vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
		gtk_box_set_homogeneous (GTK_BOX (vbox), FALSE);
	#else
		vbox = gtk_vbox_new (FALSE, 1);
	#endif
	
	align = gtk_alignment_new (0,0,1,0);

	gtk_window_resize (GTK_WINDOW (mainwin),
				window_width,
				window_height);
							
	gtk_window_set_title(GTK_WINDOW (mainwin),"GLex");

	gtk_container_add(GTK_CONTAINER (mainwin), vbox);
	gtk_container_add(GTK_CONTAINER (vbox), align);
	gtk_container_add(GTK_CONTAINER (align), combo_entry);
	
	gtk_text_view_set_cursor_visible ( GTK_TEXT_VIEW (text_view), FALSE);
	gtk_text_view_set_editable ( GTK_TEXT_VIEW (text_view), FALSE);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD);

	GtkWidget *scrwin;
	scrwin = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrwin),
						GTK_POLICY_AUTOMATIC,
						GTK_POLICY_AUTOMATIC);
	
	gtk_container_add(GTK_CONTAINER (vbox), scrwin);
	gtk_container_add(GTK_CONTAINER (scrwin), text_view);	
	
	gtk_box_set_child_packing ( GTK_BOX (vbox), align, FALSE, FALSE, 0, GTK_PACK_START);
	gtk_box_set_child_packing ( GTK_BOX (vbox), scrwin, TRUE, TRUE, 0, GTK_PACK_END);
	
	g_signal_connect(mainwin, "destroy", G_CALLBACK (on_quit), NULL);
	g_signal_connect(mainwin, "window-state-event", G_CALLBACK (on_window_restate), NULL);
	g_signal_connect(mainwin, "delete-event", G_CALLBACK (on_window_delete), NULL);

	g_signal_connect(entry, "activate", G_CALLBACK (on_combo_activate), NULL);


	gtk_widget_show_all (mainwin);
	gtk_main ();

	return 0;
}


