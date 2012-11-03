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


#include "xdxf.h"

static gchar **resv = NULL;
static gint len = 0;

static void add_res (gchar *s) {
	len++;
	resv = g_realloc (resv, sizeof (gchar*) * (len+1));
	resv[len-1] = s;
	resv[len] = NULL;
}

static gchar* build_res () {
	gchar* s = g_strjoinv ("\n\n", resv);
	g_strfreev (resv);
	resv = NULL;
	len = 0;
	return s;	
}


static void search_xml_node (xmlNode *a_node, xmlDoc *doc) {
	xmlNode *cur_node = a_node;
 	xmlChar *res;
 	
 	while (cur_node != NULL) { 
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (xmlStrcmp (cur_node->name, (const xmlChar*) "k") == 0) {
				xmlChar *k = xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
				if (xmlStrcmp (k,(const xmlChar*)  word) == 0) {
					res = xmlNodeListGetString(doc, cur_node->parent->xmlChildrenNode, 1);
					add_res (res);
				}
				xmlFree (k);
			}
			if (cur_node->children != NULL)
				search_xml_node(cur_node->children, doc);
		}
			cur_node = cur_node->next;
	}
}

void open_xdxf_file () {
	xmlDoc  *doc = NULL;
	xmlNode *root_element = NULL;
 
   doc = xmlReadFile (path_to_dict, NULL, 0);

	if (doc != NULL) {
		root_element = xmlDocGetRootElement (doc);
		if (root_element != NULL) {
			search_xml_node (root_element, doc);
			if (resv) 
				results_new_article (build_res(), TRUE);
		}
	}
	if (!results_get_count ()) 
		word_not_found = TRUE;
	xmlFreeDoc(doc);
	xmlCleanupParser();
	
}

void xdxf_init () {
	word = NULL;
	word_not_found = FALSE;

	path_to_dict = NULL;
	results_init ();
}


