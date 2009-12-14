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
#include "config_skin.h"
#include "config_pmenu.h"
#include "config_favourite.h"
#include "common.h"

int copy( char *src, char *dst );

int pnd_app_get_list( void )
{
    debug_start();

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

	debug_infof ( "Apps searchpath is '%s'", appspath );
	debug_infof ( "Apps overrides searchpath is '%s'", overridespath );

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

	if ( run_searchpath ) debug_infof ( "Locating pnd run in %s", run_searchpath );
	if ( run_script ) debug_infof ( "Locating pnd runscript as %s", run_script );
	if ( pndrun ) debug_infof ( "Default pndrun is %s", pndrun );

	/* attempt to discover apps in the path */
	pnd_box_handle applist;

	applist = pnd_disco_search ( appspath, overridespath );

	for(i = 0; i < CATEGORY_COUNT - 3; i++)
	{
		applications[i] = (PND_APP *) malloc(sizeof(PND_APP));
		applications_count[i] = 0;
		list_num[i] = 0;
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
				if( ( strcasecmp(d -> main_category, "emulators") == 0 ) | ( strcasecmp( d -> main_category, "emulator" ) == 0 ) )
				{
					tmpSection = EMULATORS;
				}
				else if( strcasecmp( d -> main_category, "game" ) == 0 )
				{
					tmpSection = GAMES;
				}
				else
                {
                    tmpSection = MISC;
                }

                strcpy(applications[tmpSection]->category[applications_count[tmpSection]], d -> main_category);
				debug_infof ( "[%i] -> Category: %s", applications_count[tmpSection], \
					applications[tmpSection]->category[applications_count[tmpSection]] );
			}
            else
            {
                tmpSection = MISC;
                strcpy( applications[tmpSection]->category[applications_count[tmpSection]], "MISC"  );
                debug_infof ( "[%i] -> Category: %s", applications_count[tmpSection], \
                    applications[tmpSection]->category[applications_count[tmpSection]] );
            }


            if ( d -> title_en )
            {
                strncpy(applications[tmpSection]->name[applications_count[tmpSection]], d -> title_en, strlen(d -> title_en) );
                debug_infof ( "[%i] -> Name: %s", applications_count[tmpSection], \
                    applications[tmpSection]->name[applications_count[tmpSection]]);

            }
            if ( d -> unique_id )
            {
                strcpy(applications[tmpSection]->id[applications_count[tmpSection]], d -> unique_id);
                debug_infof ( "[%i] -> Unique ID: %s", applications_count[tmpSection], \
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

                debug_infof( "[%i] -> fullpath: %s", applications_count[tmpSection], \
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

                debug_infof( "[%i] -> cache: %s", applications_count[tmpSection], applications[tmpSection]->cache_path[applications_count[tmpSection]] );

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
                    debug_infof( "[%i] -> New device detected : %s", applications_count[tmpSection], cfg_fav_path[cfg_fav_count] );
                    cfg_fav_count++;
                }
                /* End Crappy routine */
            }

            if ( d -> exec )
            {
                strcpy(applications[tmpSection]->exec_name[applications_count[tmpSection]], d -> exec );
                debug_infof( "[%i] -> Exec Name : %s", applications_count[tmpSection], \
                    applications[tmpSection]->exec_name[applications_count[tmpSection]]);
            }

            if ( d -> desc_en )
            {
                strcpy(applications[tmpSection]->description[applications_count[tmpSection]], d -> desc_en );
                debug_infof( "[%i] -> Description : %s", applications_count[tmpSection], \
                    applications[tmpSection]->description[applications_count[tmpSection]]);
            }

            if ( d -> preview_pic1 )
            {
                if( d -> object_type == 2 )
                {
                    sprintf( applications[tmpSection]->preview_pic1[applications_count[tmpSection]], "%s/%s.%s", applications[tmpSection]->cache_path[applications_count[tmpSection]], applications[tmpSection]->id[applications_count[tmpSection]], d -> preview_pic1 );
                    debug_infof( "[%i] -> Preview Pic : %s", applications_count[tmpSection], \
                        applications[tmpSection]->preview_pic1[applications_count[tmpSection]]);
                }
                else
                {
                    sprintf( applications[tmpSection]->preview_pic1[applications_count[tmpSection]], "%s%s", applications[tmpSection]->fullpath[applications_count[tmpSection]], d -> preview_pic1 );
                    debug_infof( "[%i] -> Preview Pic : %s", applications_count[tmpSection], \
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
                                debug_infof( "Could not acces icon_cache (%s)", icon_cache );
                        }
                        else
                        {
                            strcpy(applications[tmpSection]->icon[applications_count[tmpSection]], "no icon" );
                            debug_infof( "Could not emit icon to %s", icon_cache );
                        }
                    }
                    else
                    {
                        debug_infof( "[%i] -> icon: Already extracted", applications_count[tmpSection] );
                        strcpy(applications[tmpSection]->icon[applications_count[tmpSection]], icon_cache );
                    }
                }
                else
                {
                    sprintf(applications[tmpSection]->icon[applications_count[tmpSection]], \
                        "%s%s", applications[tmpSection]->fullpath[applications_count[tmpSection]], d -> icon);
                }

                debug_infof( "[%i] -> icon: %s", applications_count[tmpSection], \
                    applications[tmpSection]->icon[applications_count[tmpSection]] );
            }

            applications_count[tmpSection]++;

			d = pnd_box_get_next ( d );
		}

		for( i = 0; i < CATEGORY_COUNT - 3; i++ )
        {
            list_num[tmpSection] = applications_count[tmpSection];
            if (list_start[tmpSection] >= list_num[tmpSection]) { list_start[tmpSection] = list_num[tmpSection]-1; }
            if (list_start[tmpSection] < 0) { list_start[tmpSection]  = 0; }
            if (list_curpos[tmpSection] >= list_num[tmpSection]) { list_curpos[tmpSection] = list_num[tmpSection]-1; }
			if (list_curpos[tmpSection] < 0) { list_curpos[tmpSection] = 0; }
        }

	}
	else
	{
		debug_info ( "No applications found in search path\n" );
	}

	free ( configpath );
	if ( apph )
    		pnd_box_delete ( apph );

    debug_end();

	return ( 0 );
}

