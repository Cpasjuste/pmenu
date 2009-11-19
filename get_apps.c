#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>

#include <pnd_conf.h>
#include <pnd_container.h>
#include <pnd_apps.h>
#include <pnd_pxml.h>
#include <pnd_discovery.h>
#include <pnd_locate.h>

#include "get_apps.h"
#include "gui_config.h"
#include "pmenu_config.h"
#include "fav_config.h"

int copy( char *src, char *dst );

int pnd_app_get_list( void )
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

	for(i = 0; i < CATEGORY_COUNT - 2; i++)
	{
		applications[i] = (PND_APP *) malloc(sizeof(PND_APP));
		applications_count[i] = 0;
	}

	// list the found apps (if any)
	if ( applist )
	{
		pnd_disco_t *d = pnd_box_get_head ( applist );

		int tmpSection = 0;
		cfg_fav_count = 0;

		while ( d )
		{
            tmpSection = 0;

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
				else
                {
                    tmpSection = DIVERS;
                }

                strcpy(applications[tmpSection]->category[applications_count[tmpSection]], d -> main_category);
				printf ( "  [%i] -> Category: %s\n", applications_count[tmpSection], \
					applications[tmpSection]->category[applications_count[tmpSection]] );
			}
            else
            {
                tmpSection = DIVERS;
                strcpy( applications[tmpSection]->category[applications_count[tmpSection]], "DIVERS"  );
                printf ( "  [%i] -> Category: %s\n", applications_count[tmpSection], \
                    applications[tmpSection]->category[applications_count[tmpSection]] );
            }


            if ( d -> title_en )
            {
                strncpy(applications[tmpSection]->name[applications_count[tmpSection]], d -> title_en, strlen(d -> title_en) );
                printf ( "  [%i] -> Name: %s\n", applications_count[tmpSection], \
                    applications[tmpSection]->name[applications_count[tmpSection]]);
            }
            if ( d -> unique_id )
            {
                strcpy(applications[tmpSection]->id[applications_count[tmpSection]], d -> unique_id);
                printf ( "  [%i] -> Unique ID: %s\n", applications_count[tmpSection], \
                    applications[tmpSection]->id[applications_count[tmpSection]] );
            }
            if ( d -> object_path )
            {
                int i;

                if( d -> object_type == 2 )
                {
                    applications[tmpSection]->type[applications_count[tmpSection]] = 2;
                    strcpy( applications[tmpSection]->fullpath[applications_count[tmpSection]], pnd_box_get_key ( d ) );

                }
                else
                {
                    strcpy( applications[tmpSection]->fullpath[applications_count[tmpSection]], d -> object_path );
                }

                printf ( "  [%i] -> fullpath: %s\n", applications_count[tmpSection], \
                    applications[tmpSection]->fullpath[applications_count[tmpSection]] );

                int cut_lenght = 0;

                for ( i = strlen( applications[tmpSection]->fullpath[applications_count[tmpSection]] ) - 2; i > 0; i-- )
                {
                    if ( applications[tmpSection]->fullpath[applications_count[tmpSection]][i] == '/' )
                        break;

                    cut_lenght++;
                }

                strncpy( applications[tmpSection]->cache_path[applications_count[tmpSection]], d -> object_path, strlen( applications[tmpSection]->fullpath[applications_count[tmpSection]] ) - cut_lenght - 6 );
                strcat ( applications[tmpSection]->cache_path[applications_count[tmpSection]], "pmenu" );

                printf("  [%i] -> cache: %s\n", applications_count[tmpSection], applications[tmpSection]->cache_path[applications_count[tmpSection]] );

                if ( access ( applications[tmpSection]->cache_path[applications_count[tmpSection]], R_OK ) != 0 )
                {
                        mkdir ( applications[tmpSection]->cache_path[applications_count[tmpSection]], 0755 );
                }

                /* Crappy routine to detect a device (SD card) */
                int new_device = 1;
                for( i = 0; i < cfg_fav_count; i++ )
                {
                    if ( cfg_fav_count >= 9 )
                    {
                        new_device = 0;
                        break;
                    }

                    if ( strstr ( cfg_fav_path[i], applications[tmpSection]->cache_path[applications_count[tmpSection]] ) != NULL )
                    {
                        new_device = 0;
                        break;
                    }
                }
                if ( new_device )
                {
                    strcpy( cfg_fav_path[cfg_fav_count], applications[tmpSection]->cache_path[applications_count[tmpSection]] );
                    printf("  [%i] -> New device detected : %s\n", applications_count[tmpSection], cfg_fav_path[cfg_fav_count] );
                    cfg_fav_count++;
                }
                /* End Crappy routine */
            }

            if ( d -> exec )
            {
                strcpy(applications[tmpSection]->exec_name[applications_count[tmpSection]], d -> exec );
                printf ( "  [%i] -> Exec Name : %s\n", applications_count[tmpSection], \
                    applications[tmpSection]->exec_name[applications_count[tmpSection]]);
            }

            if ( d -> desc_en )
            {
                strcpy(applications[tmpSection]->description[applications_count[tmpSection]], d -> desc_en );
                printf ( "  [%i] -> Description : %s\n", applications_count[tmpSection], \
                    applications[tmpSection]->description[applications_count[tmpSection]]);
            }

            if ( d -> preview_pic1 )
            {
                if( d -> object_type == 2 )
                {
                    sprintf( applications[tmpSection]->preview_pic1[applications_count[tmpSection]], "%s/%s.%s", applications[tmpSection]->cache_path[applications_count[tmpSection]], applications[tmpSection]->id[applications_count[tmpSection]], d -> preview_pic1 );
                    printf( "  [%i] -> Preview Pic : %s\n", applications_count[tmpSection], \
                        applications[tmpSection]->preview_pic1[applications_count[tmpSection]]);
                }
                else
                {
                    sprintf( applications[tmpSection]->preview_pic1[applications_count[tmpSection]], "%s%s", applications[tmpSection]->fullpath[applications_count[tmpSection]], d -> preview_pic1 );
                    printf( "  [%i] -> Preview Pic : %s\n", applications_count[tmpSection], \
                        applications[tmpSection]->preview_pic1[applications_count[tmpSection]]);
                }

                strcpy ( applications[tmpSection]->preview_pic1_name[applications_count[tmpSection]], d -> preview_pic1 );
            }
            else
            {
                sprintf( applications[tmpSection]->preview_pic1[applications_count[tmpSection]], "%s/no_preview.bmp", pmenu->skin_dir );
            }

            if ( d -> icon )
            {
                if( d -> object_type == 2 )
                {
                    char icon_cache [ 512 ];
                    sprintf( icon_cache, "%s/%s.png", applications[tmpSection]->cache_path[applications_count[tmpSection]], d -> unique_id );

                    if ( access ( icon_cache, R_OK ) != 0 )
                    {
                        if ( pnd_emit_icon ( applications[tmpSection]->cache_path[applications_count[tmpSection]], d ) )
                        {
                             if ( access ( icon_cache, R_OK ) == 0 )
                                strcpy(applications[tmpSection]->icon[applications_count[tmpSection]], icon_cache );
                            else
                                printf( "Could acces icon_cache (%s)\n", icon_cache );
                        }
                        else
                        {
                            strcpy(applications[tmpSection]->icon[applications_count[tmpSection]], "no icon" );
                            printf( "Could not emit icon to %s\n", icon_cache );
                        }
                    }
                    else
                    {
                        printf("  [%i] -> icon: Already extracted\n", applications_count[tmpSection] );
                        strcpy(applications[tmpSection]->icon[applications_count[tmpSection]], icon_cache );
                    }
                }
                else
                {
                    sprintf(applications[tmpSection]->icon[applications_count[tmpSection]], \
                        "%s%s", applications[tmpSection]->fullpath[applications_count[tmpSection]], d -> icon);
                }

                printf ( "  [%i] -> icon: %s\n", applications_count[tmpSection], \
                    applications[tmpSection]->icon[applications_count[tmpSection]] );
            }

            applications_count[tmpSection]++;

			d = pnd_box_get_next ( d );

			printf("\n\n");
		}

		for(i = 0; i < CATEGORY_COUNT - 2; i++)
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

