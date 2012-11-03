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

#include <glib.h>
#include "results.h"

static GMutex *res;

/* The lenght of the array is res_count + 1,
   because the latest element must be NULL. */
static gint res_count;
static gchar **results;

void results_init () {
	res = g_mutex_new ();
	g_atomic_int_set (&res_count, 0);
	results = NULL;	
}

void results_free () {
	g_mutex_lock (res);
	
	g_strfreev (results);	
	results = NULL;
	g_atomic_int_set (&res_count, 0);
	
	g_mutex_unlock (res);
}

const gchar* results_get_article (gint i) {
	if (i < res_count) 
		return (const gchar*) results[i];
	else 
		return NULL;
}

void results_new_article (gchar *a, gboolean bind) {
/* If the bind parameter is true, the string is not copied.
   The function binds the pointer to store the string. */
   
	g_mutex_lock (res);
	
	results = g_realloc (results, sizeof (gchar*) * (res_count +2));

	if (!bind)
		results[res_count] = g_strdup (a);
	else 
		results[res_count] = a;
	
		results[res_count+1] = NULL;
	
	g_atomic_int_inc (&res_count);
		
	g_mutex_unlock (res);
}

const gint results_get_count () {
	gint r = g_atomic_int_get (&res_count);
	return r;
}





