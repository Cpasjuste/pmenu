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

		char buffer [ 512 ];
		char *s;

		while ( d )
		{
 			if ( d -> main_category )
			{
				if(strcmp(d -> main_category, "emulators") == 0)
				{
					tmpSection = EMULATORS;

					strcpy ( buffer, pnd_box_get_key ( d ) );
					s = strstr ( buffer, PXML_FILENAME );
					strcpy ( s, strdup(""));
					strncpy(applications[tmpSection]->path[applications_count[tmpSection]], buffer, strlen(buffer) - 1);

					printf ( "  [%i] -> Emulator spotted in : %s\n", applications_count[tmpSection], \
						applications[tmpSection]->path[applications_count[tmpSection]]);
					
				}
				else if(strcmp(d -> main_category, "games") == 0)
				{
					tmpSection = GAMES;

					strcpy ( buffer, pnd_box_get_key ( d ) );
					s = strstr ( buffer, PXML_FILENAME );
					strcpy ( s, strdup(""));
					strncpy(applications[tmpSection]->path[applications_count[tmpSection]], buffer, strlen(buffer) - 1);

					printf ( "  [%i] -> Game spotted in : %s\n", applications_count[tmpSection], \
						applications[tmpSection]->path[applications_count[tmpSection]]);
				}
				else if(strcmp(d -> main_category, "applications") == 0)
				{
					tmpSection = APPLICATIONS;

					strcpy ( buffer, pnd_box_get_key ( d ) );
					s = strstr ( buffer, PXML_FILENAME );
					strcpy ( s, strdup(""));
					strncpy(applications[tmpSection]->path[applications_count[tmpSection]], buffer, strlen(buffer) - 1);

					printf ( "  [%i] -> Application spotted in : %s\n", applications_count[tmpSection], \
						applications[tmpSection]->path[applications_count[tmpSection]]);
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
				if ( d -> icon )
				{
					strcpy(applications[tmpSection]->icon[applications_count[tmpSection]], d -> icon);
					printf ( "  [%i] -> icon: %s\n", applications_count[tmpSection], \
						applications[tmpSection]->icon[applications_count[tmpSection]] );
				}
				if ( d -> exec )
				{
					strcpy(applications[tmpSection]->exec_path[applications_count[tmpSection]], d -> exec);
					printf ( "  [%i] -> Exec Path: %s\n", applications_count[tmpSection], \
						applications[tmpSection]->exec_path[applications_count[tmpSection]] );

					strcpy ( buffer, applications[tmpSection]->exec_path[applications_count[tmpSection]] );
					s = strrchr (buffer, '/');
					strcpy ( buffer, s);

					for(i = 1; i < strlen(buffer); i++)
					{
						sprintf(s, "%c", buffer[i]);
						strcat(applications[tmpSection]->exec_name[applications_count[tmpSection]], s);
					}
					printf ( "  [%i] -> Exec Name : %s\n", applications_count[tmpSection], \
						applications[tmpSection]->exec_name[applications_count[tmpSection]]);
				}
				if ( d -> description_en )
				{
					strcpy(applications[tmpSection]->description[applications_count[tmpSection]], d -> description_en);
					printf ( "  [%i] -> Description: %s\n\n", applications_count[tmpSection], \
						applications[tmpSection]->description[applications_count[tmpSection]] );
				}
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

	// exeunt with alarums
	free ( configpath );
	if ( apph )
    		pnd_box_delete ( apph );

	return ( 0 );
}

