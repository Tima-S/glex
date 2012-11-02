#ifndef _XDXF_H_
	#define _XDXF_H_
	
	#include <libxml/parser.h>
	#include <libxml/tree.h>
	
	#include <glib.h>
	#include <glib/gstdio.h>
	
	gchar *word;
	gboolean word_not_found;

	xmlChar *result;
	xmlChar *concat_res;

	gchar *path_to_dict;
	
	void open_xdxf_file ();
	void xdxf_init ();
	
#endif
