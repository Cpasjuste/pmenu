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
#include <pnd_desktop.h>

#include "get_apps.h"
#include "config_skin.h"
#include "config_pmenu.h"
#include "config_favourite.h"
#include "common.h"

#include <SDL/SDL_image.h>

int copy( char *src, char *dst );

void pnd_app_clean_list()
{
    debug_start();

    int i, j;

	for( i = 0; i < CATEGORY_COUNT - 2; i++ )
	{
		for( j = 0; j < list_num[i]; j++ )
		{
			if ( applications[i]->icon[j] != NULL )
            {
                    GLES2D_FreeTexture( applications[i]->icon[j] );
                    applications[i]->icon[j] = NULL;
            }

            if ( applications[i]->name_cached[j] != NULL )
                GLES2D_FreeFontCache(  applications[i]->name_cached[j] );
            applications[i]->name_cached[j] = NULL;

            if ( applications[i]->description_cached[j] != NULL )
                GLES2D_FreeFontCache(  applications[i]->description_cached[j] );
            applications[i]->description_cached[j] = NULL;
		}

        if ( applications[i] != NULL )
            free( applications[i] );
	}

	debug_end();
}

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

    // Free applications
	for( i = 0; i < CATEGORY_COUNT - 2; i++ )
	{
	    //if ( applications[i] != NULL )
        //    free ( applications[i] );

		applications[i] = (PND_APP *) malloc(sizeof(PND_APP));
		applications_count[i] = 0;
		list_num[i] = 0;
	}

	// list the found apps (if any)
	if ( applist )
	{
		pnd_disco_t *d = pnd_box_get_head ( applist );

		int tmpSection = 0;
		int is_favorite = 0;

		while ( d )
		{
		    is_favorite = 0;
            tmpSection = 0;

 			if ( d -> main_category )
			{
			    if( ( strcasecmp( d -> main_category, "game" ) == 0 ) || ( strcasecmp( d -> main_category, "games" ) == 0 ) )
                {
                    if ( d -> main_category1 )
                    {
                        if ( ( strcasecmp( d -> main_category1, "emulator" ) == 0 ) || ( strcasecmp( d -> main_category1, "emulators" ) == 0 ) )
                        {
                            tmpSection = EMULATORS;
                            strcpy( applications[tmpSection]->category[applications_count[tmpSection]], "EMULATORS" );
                        }
                        else
                        {
                            tmpSection = GAMES;
                            strcpy( applications[tmpSection]->category[applications_count[tmpSection]], "GAMES" );
                        }
                    }
                    else
                    {
                        tmpSection = GAMES;
                        strcpy( applications[tmpSection]->category[applications_count[tmpSection]], "GAMES" );
                    }
                }
				else
                {
                    tmpSection = MISC;
                    strcpy( applications[tmpSection]->category[applications_count[tmpSection]], "MISC" );
                }

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

            if ( d -> unique_id )
            {
                strcpy( applications[tmpSection]->id[applications_count[tmpSection]], d -> unique_id );
                debug_infof ( "[%i] -> Unique ID: %s", applications_count[tmpSection], \
                    applications[tmpSection]->id[applications_count[tmpSection]] );

                if ( cfg_fav_exist( d -> unique_id ) )
                {
                    // This unique id is saved as a favorite !
                    is_favorite = 1;
                    strcpy( applications[FAVORITES]->category[applications_count[FAVORITES]], "FAVORITES" );
                    strcpy( applications[FAVORITES]->id[applications_count[FAVORITES]], d -> unique_id );
                    debug_info ( "!! This application is a favorite one !!" )
                }
            }

            if ( d -> title_en )
            {
                strncpy(applications[tmpSection]->name[applications_count[tmpSection]], d -> title_en, strlen(d -> title_en) );
                debug_infof ( "[%i] -> Name: %s", applications_count[tmpSection], \
                    applications[tmpSection]->name[applications_count[tmpSection]]);

                if ( is_favorite )
                    strncpy(applications[FAVORITES]->name[applications_count[FAVORITES]], d -> title_en, strlen(d -> title_en) );
            }

            if ( d -> object_path )
            {
                if( d -> object_type == pnd_object_type_pnd )
                {
                    applications[tmpSection]->type[applications_count[tmpSection]] = IS_PND;
                    strcpy( applications[tmpSection]->fullpath[applications_count[tmpSection]], pnd_box_get_key ( d ) );

                    if ( is_favorite )
                    {
                        applications[FAVORITES]->type[applications_count[FAVORITES]] = IS_PND;
                        strcpy( applications[FAVORITES]->fullpath[applications_count[FAVORITES]], pnd_box_get_key ( d ) );
                    }

                }
                else
                {
                    applications[tmpSection]->type[applications_count[tmpSection]] = 0;
                    strcpy( applications[tmpSection]->fullpath[applications_count[tmpSection]], d -> object_path );

                    if ( is_favorite )
                    {
                        applications[FAVORITES]->type[applications_count[FAVORITES]] = 0;
                        strcpy( applications[FAVORITES]->fullpath[applications_count[FAVORITES]], d -> object_path );
                    }
                }

                debug_infof( "[%i] -> fullpath: %s", applications_count[tmpSection], \
                    applications[tmpSection]->fullpath[applications_count[tmpSection]] );
            }

            if ( d -> exec )
            {
                strcpy(applications[tmpSection]->exec_name[applications_count[tmpSection]], d -> exec );
                debug_infof( "[%i] -> Exec Name : %s", applications_count[tmpSection], \
                    applications[tmpSection]->exec_name[applications_count[tmpSection]]);

                if ( is_favorite )
                    strcpy(applications[FAVORITES]->exec_name[applications_count[FAVORITES]], d -> exec );
            }

            if ( d -> desc_en )
            {
                strcpy(applications[tmpSection]->description[applications_count[tmpSection]], d -> desc_en );
                debug_infof( "[%i] -> Description : %s", applications_count[tmpSection], \
                    applications[tmpSection]->description[applications_count[tmpSection]]);

                if ( is_favorite )
                    strcpy(applications[FAVORITES]->description[applications_count[FAVORITES]], d -> desc_en );
            }
            else
            {
                strcpy(applications[tmpSection]->description[applications_count[tmpSection]], "No Description available" );
                debug_infof( "[%i] -> Description : %s", applications_count[tmpSection], \
                    applications[tmpSection]->description[applications_count[tmpSection]]);

                if ( is_favorite )
                    strcpy(applications[FAVORITES]->description[applications_count[FAVORITES]], "No Description available" );
            }

            if ( d -> clockspeed )
            {
                applications[tmpSection]->clock[applications_count[tmpSection]] = atoi( d -> clockspeed );
                debug_infof( "[%i] -> Clock Speed : %i", applications_count[tmpSection], \
                    applications[tmpSection]->clock[applications_count[tmpSection]]);

                if ( is_favorite )
                    applications[FAVORITES]->clock[applications_count[FAVORITES]] = atoi( d -> clockspeed );
            }
            else
            {
                applications[tmpSection]->clock[applications_count[tmpSection]] = 500;
                debug_infof( "[%i] -> Clock Speed : %i", applications_count[tmpSection], \
                    applications[tmpSection]->clock[applications_count[tmpSection]]);

                if ( is_favorite )
                    applications[FAVORITES]->clock[applications_count[FAVORITES]] = 500;
            }

            if ( d -> preview_pic1 )
            {
                if( d -> object_type == pnd_object_type_directory )
                {
                    sprintf( applications[tmpSection]->preview_pic1[applications_count[tmpSection]], "%s%s", applications[tmpSection]->fullpath[applications_count[tmpSection]], d -> preview_pic1 );
                    debug_infof( "[%i] -> Preview Pic : %s", applications_count[tmpSection], \
                        applications[tmpSection]->preview_pic1[applications_count[tmpSection]]);

                    if ( is_favorite )
                        sprintf( applications[FAVORITES]->preview_pic1[applications_count[FAVORITES]], "%s%s", applications[FAVORITES]->fullpath[applications_count[FAVORITES]], d -> preview_pic1 );
                }
                strcpy ( applications[tmpSection]->preview_pic1_name[applications_count[tmpSection]], d -> preview_pic1 );

                if ( is_favorite )
                    strcpy ( applications[FAVORITES]->preview_pic1_name[applications_count[FAVORITES]], d -> preview_pic1 );
            }
            else
            {
                sprintf( applications[tmpSection]->preview_pic1[applications_count[tmpSection]], "%s/no_preview.bmp", pmenu->skin_dir );

                if ( is_favorite )
                    sprintf( applications[FAVORITES]->preview_pic1[applications_count[FAVORITES]], "%s/no_preview.bmp", pmenu->skin_dir );
            }

            if ( d -> icon )
            {
                if( d -> object_type == pnd_object_type_pnd )
                {
                    SDL_RWops *tmpmem;
                    SDL_Surface *tmp;
                    unsigned char *buffer;
                    unsigned int *len = malloc(128);
                    buffer = pnd_emit_icon_to_buffer( d, len );
                    if ( buffer != NULL )
                    {
                        tmpmem = SDL_RWFromMem( buffer, (int)len );
                        tmp = IMG_LoadPNG_RW( tmpmem );

                        applications[tmpSection]->icon[applications_count[tmpSection]] = GLES2D_CreateTextureFromSurface( tmp, 0 );

                        if ( is_favorite )
                            applications[FAVORITES]->icon[applications_count[FAVORITES]] = GLES2D_CreateTextureFromSurface( tmp, 0 );

                        SDL_FreeSurface( tmp );
                        free( buffer );
                    }
                    free( len );
                }
                else
                {
                    char tmpPath[1024];

                    sprintf( tmpPath, "%s%s", applications[tmpSection]->fullpath[applications_count[tmpSection]], d -> icon );
                    applications[tmpSection]->icon[applications_count[tmpSection]] = GLES2D_CreateTexture( tmpPath, 0  );

                    if ( is_favorite )
                        applications[FAVORITES]->icon[applications_count[FAVORITES]] = GLES2D_CreateTexture( tmpPath, 0  );
                }
            }

            if ( d -> option_no_x11 )
            {
                applications[tmpSection]->noX[applications_count[tmpSection]] = atoi( d -> option_no_x11 );

                if ( is_favorite )
                    applications[FAVORITES]->noX[applications_count[FAVORITES]] = atoi( d -> option_no_x11 );
            }
            else
            {
                applications[tmpSection]->noX[applications_count[tmpSection]] = 0;

                if ( is_favorite )
                    applications[FAVORITES]->noX[applications_count[FAVORITES]] = 0;
            }
            debug_infof( "[%i] -> noX: %i", applications_count[tmpSection], applications[tmpSection]->noX[applications_count[tmpSection]] );

            applications_count[tmpSection]++;

             if ( is_favorite )
                applications_count[FAVORITES]++;

            printf( "\n" );

			d = pnd_box_get_next ( d );
		}

		for( i = 0; i < CATEGORY_COUNT - 2; i++ )
        {
            int j;

            list_num[i] = applications_count[i];
            if ( list_start[i] >= list_num[i] ) { list_start[i] = list_num[i] - 1; }
            if ( list_start[i] < 0 ) { list_start[i]  = 0; }
            if ( list_curpos[i] >= list_num[i] ) { list_curpos[i] = list_num[i] - 1; }
			if ( list_curpos[i] < 0 ) { list_curpos[i] = 0; }

            for( j = 0; j < list_num[i]; j++ )
            {
                char tmp[512];
                memset ( tmp, 0, 512 );
                sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->font_big );
                applications[i]->name_cached[j] = GLES2D_CreateFontCache( tmp, applications[i]->name[j], gui->font_big_style, gui->font_big_size, gui->applications_box_w - gui->icon_scale_max - 20 );

                memset ( tmp, 0, 512 );
                sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->font_small );
                applications[i]->description_cached[j] = GLES2D_CreateFontCache( tmp, applications[i]->description[j], gui->font_small_style, gui->font_small_size, gui->applications_box_w - gui->icon_scale_max - 20 );
            }
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

