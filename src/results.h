#ifndef _RESULTS_H_
	#define _RESULTS_H_
	
	void results_init ();
	void results_free ();
	const gchar* results_get_article (gint i);
	void results_new_article (gchar *a, gboolean bind);
	const gint results_get_count ();
	
#endif
