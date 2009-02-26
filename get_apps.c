#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include <pnd_conf.h>
#include <pnd_container.h>
#include <pnd_apps.h>
#include <pnd_pxml.h>
#include <pnd_discovery.h>
#include <pnd_locate.h>

#include "get_apps.h"

int pnd_app_get_list(void)
{
	char *configpath;
	char *appspath;
	char *overridespath;
	int i;

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

	/* find pnd runscript */

	if ( apph ) {
		run_searchpath = pnd_conf_get_as_char ( apph, PND_PNDRUN_SEARCHPATH_KEY );
		run_script = pnd_conf_get_as_char ( apph, PND_PNDRUN_KEY );
		pndrun = NULL;

		if ( ! run_searchpath ) {
			run_searchpath = PND_APPS_SEARCHPATH;
			run_script = PND_PNDRUN_FILENAME;
		}

	} else {
		run_searchpath = NULL;
		run_script = NULL;
		pndrun = PND_PNDRUN_DEFAULT;
	}

	if ( ! pndrun ) {
		pndrun = pnd_locate_filename ( run_searchpath, run_script );
	}

	if ( run_searchpath ) printf ( "Locating pnd run in %s\n", run_searchpath );
	if ( run_script ) printf ( "Locating pnd runscript as %s\n", run_script );
	if ( pndrun ) printf ( "Default pndrun is %s\n", pndrun );

	/* attempt to discover apps in the path */
	pnd_box_handle applist;

	applist = pnd_disco_search ( appspath, overridespath );

	for(i = EMULATORS; i< APPLICATIONS+1; i++)
	{
		applications[i] = (PND_APP *) malloc(sizeof(PND_APP));
		applications_count[i] = 0;
	}

	// list the found apps (if any)
	if ( applist )
	{
		pnd_disco_t *d = pnd_box_get_head ( applist );

		int tmpSection = 0;

		while ( d )
		{
 			if ( d -> main_category )
			{
				if((strcasecmp(d -> main_category, "emulators") == 0) | (strcasecmp(d -> main_category, "emulator") == 0))
				{
					tmpSection = EMULATORS;
				}
				else if((strcasecmp(d -> main_category, "games") == 0) | (strcasecmp(d -> main_category, "game") == 0))
				{
					tmpSection = GAMES;
				}
				else if((strcasecmp(d -> main_category, "applications") == 0) | (strcasecmp(d -> main_category, "application") == 0))
				{
					tmpSection = APPLICATIONS;
				}

				strcpy(applications[tmpSection]->category[applications_count[tmpSection]], d -> main_category);
				printf ( "  [%i] -> Category: %s\n", applications_count[tmpSection], \
					applications[tmpSection]->category[applications_count[tmpSection]] );

				if ( d -> title_en )
				{
					strcpy(applications[tmpSection]->name[applications_count[tmpSection]], d -> title_en);
					printf ( "  [%i] -> Name: %s\n", applications_count[tmpSection], \
						applications[tmpSection]->name[applications_count[tmpSection]]);
				}
				if ( d -> unique_id )
				{
					strcpy(applications[tmpSection]->id[applications_count[tmpSection]], d -> unique_id);
					printf ( "  [%i] -> Unique ID: %s\n", applications_count[tmpSection], \
						applications[tmpSection]->id[applications_count[tmpSection]] );
				}
				if ( d -> path_to_object )
				{
					strcpy(applications[tmpSection]->fullpath[applications_count[tmpSection]], d -> path_to_object);
					printf ( "  [%i] -> fullpath: %s\n", applications_count[tmpSection], \
						applications[tmpSection]->fullpath[applications_count[tmpSection]] );
				}
				if ( d -> icon )
				{
					sprintf(applications[tmpSection]->icon[applications_count[tmpSection]], "%s%s", d -> path_to_object, d -> icon);
					printf ( "  [%i] -> icon: %s\n", applications_count[tmpSection], \
						applications[tmpSection]->icon[applications_count[tmpSection]] );

				}
				if ( d -> exec )
				{
					strcpy(applications[tmpSection]->exec_name[applications_count[tmpSection]], d -> exec );
					printf ( "  [%i] -> Exec Name : %s\n", applications_count[tmpSection], \
						applications[tmpSection]->exec_name[applications_count[tmpSection]]);
				}

				pnd_pxml_handle pxmlh;
				pxmlh = pnd_pxml_fetch ( pnd_box_get_key ( d ) );
				if ( ! pxmlh )
				{
					return ( 0 ); 
				}
				if ( pnd_is_pxml_valid_app ( pxmlh ) )
				{
					sprintf( applications[tmpSection]->preview_pic1[applications_count[tmpSection]], "%s%s", d -> path_to_object, pnd_pxml_get_previewpic1 ( pxmlh ) );
					printf("  [%i] -> preview_pic1: %s\n", applications_count[tmpSection], \
						applications[tmpSection]->preview_pic1[applications_count[tmpSection]] );

					sprintf( applications[tmpSection]->preview_pic2[applications_count[tmpSection]], "%s%s", d -> path_to_object, pnd_pxml_get_previewpic2 ( pxmlh ) );
					printf("  [%i] -> preview_pic2: %s\n", applications_count[tmpSection], \
						applications[tmpSection]->preview_pic2[applications_count[tmpSection]] );

					strcpy(applications[tmpSection]->description[applications_count[tmpSection]], pnd_pxml_get_description_en ( pxmlh ) );
					printf ( "  [%i] -> Description: %s\n\n", applications_count[tmpSection], \
						applications[tmpSection]->description[applications_count[tmpSection]] );
				}
				pnd_pxml_delete ( pxmlh );

				applications_count[tmpSection]++;
 			}
			d = pnd_box_get_next ( d );
		}
		for(i= EMULATORS; i< APPLICATIONS+1; i++)
		{
			list_num[i] = applications_count[i];
			if (list_start[i] >= list_num[i]) { list_start[i] = list_num[i]-1; }
			if (list_start[i] < 0) { list_start[i]  = 0; }
			if (list_curpos[i] >= list_num[i]) { list_curpos[i] = list_num[i]-1; }
			if (list_curpos[i] < 0) { list_curpos[i] = 0; }
		}

	}
	else
	{
		printf ( "No applications found in search path\n" );
	}

	free ( configpath );
	if ( apph )
    		pnd_box_delete ( apph );

	return ( 0 );
}

