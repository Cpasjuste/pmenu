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
#include <errno.h>

#include "libconfig.h"

#include "config_favourite.h"
#include "utils.h"
#include "get_apps.h"
#include "config_skin.h"
#include "config_pmenu.h"
#include "main.h"
#include "common.h"

void gui_load_fav();
void gui_clean_fav();
int _cfg_fav_read( char *fav_path );

int _cfg_fav_read( char *fav_path )
{
    debug_start( );

	if ( access ( fav_path, R_OK ) != 0 )
    {
            debug_infof( "Favorite configuration file do not exist, skipping ( %s )", fav_path );
            debug_end( );
            return 0;
    }

    config_init(&cfg);

	if ( !config_read_file( &cfg, fav_path ) )
	{
		debug_errorf( "config_read_file(%s) failed", fav_path );
		debug_end( );
        config_destroy(&cfg);
        return 0;

	}
	else
	{
        int i;
        char string_search[256];

        config_setting_t *search = NULL;

		for( i = 0; i < FAV_MAX ; i++ )
		{
            memset( string_search, 0, 256 );
		    sprintf( string_search, "fav%i", i );

			search = config_lookup( &cfg, string_search );

			if ( search )
			{
			    applications[FAVORITES]->fav_num[applications_count[FAVORITES]] = i;

                config_setting_t *tmp = config_setting_get_member(search, "name");
                if(tmp)
				{
                    strcpy( applications[FAVORITES]->name[applications_count[FAVORITES]], config_setting_get_string(tmp));
				}

                tmp = config_setting_get_member(search, "fullpath");
                if(tmp)
				{
                    strcpy( applications[FAVORITES]->fullpath[applications_count[FAVORITES]], config_setting_get_string(tmp));
				}

				tmp = config_setting_get_member(search, "exec_name");
                if(tmp)
				{
                    strcpy( applications[FAVORITES]->exec_name[applications_count[FAVORITES]], config_setting_get_string(tmp));
				}

                if ( access ( applications[FAVORITES]->fullpath[applications_count[FAVORITES]], R_OK ) != 0 )
                {
                    debug_infof( "%s do not exist anymore, removing from favorites", applications[FAVORITES]->fullpath[applications_count[FAVORITES]] );
                    debug_end();
                    config_destroy( &cfg );
                    cfg_fav_del( i );
                    return 0;
                }

				tmp = config_setting_get_member(search, "id");
                if(tmp)
				{
                    strcpy( applications[FAVORITES]->id[applications_count[FAVORITES]], config_setting_get_string(tmp));
				}

				tmp = config_setting_get_member(search, "category");
                if(tmp)
				{
                    strcpy( applications[FAVORITES]->category[applications_count[FAVORITES]], config_setting_get_string(tmp));
				}

                tmp = config_setting_get_member(search, "cache_path");
                if(tmp)
				{
                    strcpy( applications[FAVORITES]->cache_path[applications_count[FAVORITES]], config_setting_get_string(tmp));
				}

				tmp = config_setting_get_member(search, "icon");
                if(tmp)
				{
                    strcpy( applications[FAVORITES]->icon[applications_count[FAVORITES]], config_setting_get_string(tmp));
				}

				tmp = config_setting_get_member(search, "description");
                if(tmp)
				{
                    strcpy( applications[FAVORITES]->description[applications_count[FAVORITES]], config_setting_get_string(tmp));
				}

				tmp = config_setting_get_member(search, "preview_pic1");
                if(tmp)
				{
                    strcpy( applications[FAVORITES]->preview_pic1[applications_count[FAVORITES]], config_setting_get_string(tmp));
				}

				tmp = config_setting_get_member(search, "preview_pic2");
                if(tmp)
				{
                    strcpy( applications[FAVORITES]->preview_pic2[applications_count[FAVORITES]], config_setting_get_string(tmp));
				}

                applications[FAVORITES]->scale[applications_count[FAVORITES]] = 32;

				applications_count[FAVORITES]++;
			}
		}
	}

	config_destroy(&cfg);

	debug_end();

	return 1;
}

int cfg_fav_read( )
{
    char favorite_file[512];

    applications_count[FAVORITES] = 0;

    if ( applications[FAVORITES] != NULL ) free ( applications[FAVORITES] );

    applications[FAVORITES] = (PND_APP *) malloc(sizeof(PND_APP));

    int i;
    for( i = 0; i < cfg_fav_count; i++ )
    {
        memset( favorite_file, 0, 512 );
        sprintf( favorite_file, "%s/favorites.cfg", cfg_fav_path[i] );
        _cfg_fav_read( favorite_file );
    }

    list_num[FAVORITES] = applications_count[FAVORITES];
    if (list_start[FAVORITES] >= list_num[FAVORITES]) { list_start[FAVORITES] = list_num[FAVORITES]-1; }
    if (list_start[FAVORITES] < 0) { list_start[FAVORITES]  = 0; }
    if (list_curpos[FAVORITES] >= list_num[FAVORITES]) { list_curpos[FAVORITES] = list_num[FAVORITES]-1; }
    if (list_curpos[FAVORITES] < 0) { list_curpos[FAVORITES] = 0; }

    return 1;
}

int cfg_fav_del( int fav )
{
    debug_start();

    char fav_to_del[64];
    char fav_path[512];

    memset( fav_to_del, 0, 64 );
    sprintf( fav_to_del, "fav%i", applications[FAVORITES]->fav_num[fav] );

    memset( fav_path, 0, 512 );
    sprintf ( fav_path, "%s/favorites.cfg", applications[FAVORITES]->cache_path[fav] );

    config_init(&cfg);

    if ( ! config_read_file( &cfg, fav_path ) )
    {
        debug_errorf ( "config_read_file(%s) failed", fav_path );
        debug_errorf( "cfg_fav_del( %i ) failed", fav );
        debug_end();
        config_destroy( &cfg );
        return 0;
    }

    debug_infof( "Removing %s from %s", fav_to_del, fav_path );

    config_setting_t *root = config_root_setting( &cfg );

    config_setting_remove( root, fav_to_del );

    config_write_file( &cfg, fav_path );
    config_destroy( &cfg );

    if ( icon[FAVORITES][fav] != NULL )
    {
        debug_infof( "Removing icon%i from memory\n", fav );
        GLES2D_FreeTexture ( icon[FAVORITES][fav] );
        icon[FAVORITES][fav] = NULL;
    }

    if ( applications[FAVORITES]->name_cached[fav] != NULL )
    {
        GLES2D_FreeFontCache (  applications[FAVORITES]->name_cached[fav] );
         applications[FAVORITES]->name_cached[fav]= NULL;
    }

    if ( applications[FAVORITES]->description_cached[fav] != NULL )
    {
        GLES2D_FreeFontCache (  applications[FAVORITES]->description_cached[fav] );
         applications[FAVORITES]->description_cached[fav]= NULL;
    }

    gui_clean_fav();
    cfg_fav_read();
    gui_load_fav();

    debug_end();

	return 1;
}

int cfg_fav_add( char *name, char *id, char *category, char *cache_path, char *fullpath, char *exec_name, char *_icon, char *description, char *preview_pic1, char *preview_pic2 )
{
    debug_start();

    char string_search[256];
    char favorite_file[512];

    if ( applications_count[FAVORITES] < FAV_MAX )
    {
        config_init(&cfg);

        if ( access ( cache_path, R_OK ) != 0 )
        {
            debug_infof("Creating pmenu cache directory : %s", cache_path );
            if ( mkdir ( cache_path, 0755 ) < 0 )
            {
                debug_errorf("Could not create cache directory : %s", strerror(errno) );
                debug_end();
                return 0;
            }

        }

        memset( favorite_file, 0, 512 );
        strcpy( favorite_file, cache_path );
        strcat( favorite_file, "/favorites.cfg" );

        if ( access ( favorite_file, R_OK ) != 0 )
        {
            debug_infof("Creating favorites configuration file : %s", favorite_file );
            FILE *fav_file = fopen( favorite_file, "wt" );
            if ( !fav_file )
            {
                debug_errorf( "Could not create favorite configuration file : %s", favorite_file );
                fclose( fav_file );
                config_destroy( &cfg );
                debug_end();
                return 0;
            }

            fclose( fav_file );
        }

        if ( ! config_read_file( &cfg, favorite_file ) )
        {
            debug_errorf ( "config_read_file(%s) failed", favorite_file );
            debug_end();
            config_destroy( &cfg );
            return 0;
        }


        int i;
		for( i = 0; i < FAV_MAX ; i++ )
		{
            memset( string_search, 0, 256 );
		    sprintf( string_search, "fav%i", i );

			config_setting_t *search = config_lookup( &cfg, string_search );

			if ( ! search )
			{
				debug_infof("Adding Favorite application number %i (%s) to %s", i, name, favorite_file );

                config_setting_t *root = config_root_setting(&cfg);

                config_setting_t *new = config_setting_add( root, string_search, CONFIG_TYPE_GROUP);

                config_setting_t *item = config_setting_add( new, "name", CONFIG_TYPE_STRING );
                item = config_setting_add( new, "id", CONFIG_TYPE_STRING );
                item = config_setting_add( new, "category", CONFIG_TYPE_STRING );
                item = config_setting_add( new, "cache_path", CONFIG_TYPE_STRING );
                item = config_setting_add( new, "fullpath", CONFIG_TYPE_STRING );
                item = config_setting_add( new, "exec_name", CONFIG_TYPE_STRING );
                item = config_setting_add( new, "icon", CONFIG_TYPE_STRING );
                item = config_setting_add( new, "description", CONFIG_TYPE_STRING );
                item = config_setting_add( new, "preview_pic1", CONFIG_TYPE_STRING );
                item = config_setting_add( new, "preview_pic2", CONFIG_TYPE_STRING );

                item = config_lookup( &cfg, string_search );

                config_setting_t *value = config_setting_get_member(item, "name");
                config_setting_set_string(value, name);

                value = config_setting_get_member(item, "id");
                config_setting_set_string(value, id);

                value = config_setting_get_member(item, "cache_path");
                config_setting_set_string(value, cache_path);

                value = config_setting_get_member(item, "category");
                config_setting_set_string(value, category);

                value = config_setting_get_member(item, "fullpath");
                config_setting_set_string(value, fullpath);

                value = config_setting_get_member(item, "exec_name");
                config_setting_set_string(value, exec_name);

                value = config_setting_get_member(item, "icon");
                config_setting_set_string(value, _icon);

                value = config_setting_get_member(item, "description");
                config_setting_set_string(value, description);

                value = config_setting_get_member(item, "preview_pic1");
                config_setting_set_string(value, preview_pic1);

                value = config_setting_get_member(item, "preview_pic2");
                config_setting_set_string(value, preview_pic2);

                applications[FAVORITES]->fav_num[applications_count[FAVORITES]] = i;
                strcpy( applications[FAVORITES]->name[applications_count[FAVORITES]], name );
                strcpy( applications[FAVORITES]->id[applications_count[FAVORITES]], id );
                strcpy( applications[FAVORITES]->cache_path[applications_count[FAVORITES]], cache_path );
                strcpy( applications[FAVORITES]->category[applications_count[FAVORITES]], category );
				strcpy( applications[FAVORITES]->fullpath[applications_count[FAVORITES]], fullpath );
				strcpy( applications[FAVORITES]->exec_name[applications_count[FAVORITES]], exec_name );
				strcpy( applications[FAVORITES]->icon[applications_count[FAVORITES]], _icon );
				strcpy( applications[FAVORITES]->description[applications_count[FAVORITES]], description );
                strcpy( applications[FAVORITES]->preview_pic1[applications_count[FAVORITES]], preview_pic1 );
				strcpy( applications[FAVORITES]->preview_pic2[applications_count[FAVORITES]], preview_pic2 );

                icon[FAVORITES][applications_count[FAVORITES]] = NULL;
                icon[FAVORITES][applications_count[FAVORITES]] = GLES2D_CreateTexture( applications[FAVORITES]->icon[applications_count[FAVORITES]], 0  );

                applications[FAVORITES]->scale[applications_count[FAVORITES]] = gui->icon_scale_min;

                char tmp[512];
                memset ( tmp, 0, 512 );
                sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->font_big );
                applications[FAVORITES]->name_cached[applications_count[FAVORITES]] = GLES2D_CreateFontCache( tmp, applications[FAVORITES]->name[applications_count[FAVORITES]], gui->font_big_style, gui->font_big_size, gui->applications_box_w - gui->icon_scale_max - 20 );

                memset ( tmp, 0, 512 );
                sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->font_small );
                applications[FAVORITES]->description_cached[applications_count[FAVORITES]] = GLES2D_CreateFontCache( tmp, applications[FAVORITES]->description[applications_count[FAVORITES]], gui->font_small_style, gui->font_small_size, gui->applications_box_w - gui->icon_scale_max - 20 );

                applications_count[FAVORITES] += 1;

                list_num[FAVORITES] = applications_count[FAVORITES];
                if (list_start[FAVORITES] >= list_num[FAVORITES]) { list_start[FAVORITES] = list_num[FAVORITES]-1; }
                if (list_start[FAVORITES] < 0) { list_start[FAVORITES]  = 0; }
                if (list_curpos[FAVORITES] >= list_num[FAVORITES]) { list_curpos[FAVORITES] = list_num[FAVORITES]-1; }
                if (list_curpos[FAVORITES] < 0) { list_curpos[FAVORITES] = 0; }

                config_write_file( &cfg, favorite_file );
                config_destroy( &cfg );

                debug_end();

                return 1;
			}
		}
    }

    debug_info( "cfg_fav_add( ) -> Maximum favorites applications reached\n\n" );
    debug_end();

	return 0;
}


