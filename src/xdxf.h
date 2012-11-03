#ifndef _XDXF_H_
	#define _XDXF_H_
	
	#include <libxml/parser.h>
	#include <libxml/tree.h>
	
	#include <glib.h>
	#include <glib/gstdio.h>
	
	#include "results.h"
	
	gchar *word;
	gboolean word_not_found;

	gchar *path_to_dict;
	
	void open_xdxf_file ();
	void xdxf_init ();
	
#endif
