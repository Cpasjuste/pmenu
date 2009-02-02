#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include <pnd_conf.h>
#include <pnd_container.h>
#include <pnd_apps.h>
#include <pnd_pxml.h>
#include <pnd_discovery.h>

#include "get_apps.h"

int pnd_app_get_list(void)
{
	char *configpath;
	char *appspath;
	char *overridespath;

	/* attempt to sort out the config file madness */

	// attempt to fetch a sensible default searchpath for configs
	configpath = pnd_conf_query_searchpath();

	// attempt to fetch the apps config to pick up a searchpath
	pnd_conf_handle apph;

	apph = pnd_conf_fetch_by_id ( pnd_conf_apps, configpath );

	if ( apph )
	{
		appspath = pnd_conf_get_as_char ( apph, PND_APPS_KEY );

		if ( ! appspath )
		{
			appspath = PND_APPS_SEARCHPATH;
    		}

    		overridespath = pnd_conf_get_as_char ( apph, PND_PXML_OVERRIDE_KEY );

		if ( ! overridespath )
		{
			overridespath = PND_PXML_OVERRIDE_SEARCHPATH;
		}
	}
	else
	{
		// couldn't find a useful app search path so use the default
		appspath = PND_APPS_SEARCHPATH;
		overridespath = PND_PXML_OVERRIDE_SEARCHPATH;
	}

	printf ( "Apps searchpath is '%s'\n", appspath );
	printf ( "Apps overrides searchpath is '%s'\n", overridespath );

	/* attempt to discover apps in the path */
	pnd_box_handle applist;

	applist = pnd_disco_search ( appspath, overridespath );

	application = (PND_APP *) malloc(sizeof(PND_APP));
	pnd_app_count = 0;

	// list the found apps (if any)
	if ( applist )
	{
		pnd_disco_t *d = pnd_box_get_head ( applist );

		while ( d )
		{

			// display the app 'as is'

			printf ( "  [%i] : App: %s\n", pnd_app_count, pnd_box_get_key ( d ) );

			if ( d -> app_name )
			{
				strcpy(application->name[pnd_app_count], d -> app_name);
				printf ( "  [%i] : Name: %s\n", pnd_app_count, application->name[pnd_app_count]);
			}
			if ( d -> unique_id )
			{
				strcpy(application->id[pnd_app_count], d -> unique_id);
				printf ( "  [%i] : Unique ID: %s\n", pnd_app_count, application->id[pnd_app_count] );
			}
			if ( d -> icon )
			{
				strcpy(application->icon[pnd_app_count], d -> icon);
				printf ( "  [%i] : icon: %s\n", pnd_app_count, application->icon[pnd_app_count] );
			}
 			if ( d -> primary_category )
			{
				strcpy(application->category[pnd_app_count], d -> primary_category);
				printf ( "  [%i] : Category: %s\n", pnd_app_count, application->category[pnd_app_count] );
 			}
			if ( d -> exec_path )
			{
				strcpy(application->exec[pnd_app_count], d -> exec_path);
				printf ( "  [%i] : Executable: %s\n\n", pnd_app_count, application->exec[pnd_app_count] );
			}

			// next!
			pnd_app_count++;
			d = pnd_box_get_next ( d );

		} // while applist

		app_list_num = pnd_app_count;
		if (app_list_start >= app_list_num) { app_list_start = app_list_num-1; }
		if (app_list_start < 0) { app_list_start  = 0; }
		if (app_list_curpos >= app_list_num) { app_list_curpos = app_list_num-1; }
		if (app_list_curpos < 0) { app_list_curpos = 0; }

	}
	else
	{
		printf ( "No applications found in search path\n" );
	}

	// exeunt with alarums
	free ( configpath );
	if ( apph )
    		pnd_box_delete ( apph );

	return ( 0 );
}

