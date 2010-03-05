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

int cfg_fav_exist( char *id )
{
    debug_start();

    // Set favorites file path
    char favorite_file[512];
    memset( favorite_file, 0, 512 );
    sprintf( favorite_file, "%s/%s", PMENU_PATH, "favorites.cfg" );

    // if favorite file do not exist, dont bother ...
    if ( access ( favorite_file, R_OK ) != 0 )
    {
        debug_end();
        return 0;
    }

    // Init configuration
    config_init( &cfg );

    // Try to read the file
    if ( ! config_read_file( &cfg, favorite_file ) )
    {
        debug_errorf ( "config_read_file(%s) failed", favorite_file );
        debug_end();
        config_destroy( &cfg );
        return 0;
    }

    config_setting_t *search = config_lookup( &cfg, id );

    if ( ! search )
    {
        // not found !
        config_destroy( &cfg );
        debug_end();
        return 0;
    }

    // found !
    config_destroy( &cfg );
    debug_end();

    return 1;
}

int cfg_fav_del( char *id )
{
    debug_start();

    // Set favorites file path
    char favorite_file[512];
    memset( favorite_file, 0, 512 );
    sprintf( favorite_file, "%s/%s", PMENU_PATH, "favorites.cfg" );

    config_init( &cfg );

    if ( ! config_read_file( &cfg, favorite_file ) )
    {
        debug_errorf ( "config_read_file(%s) failed", favorite_file );
        debug_end();
        config_destroy( &cfg );
        return 0;
    }

    config_setting_t *root = config_root_setting( &cfg );
    config_setting_remove( root, id );
    config_write_file( &cfg, favorite_file );
    config_destroy( &cfg );



/*
    char fav_to_del[64];
    char favorite_file[512];

    memset( fav_to_del, 0, 64 );
    sprintf( fav_to_del, "fav%i", applications[FAVORITES]->fav_num[fav] );

    memset( fav_path, 0, 512 );
    sprintf ( fav_path, "%s/favorites.cfg", applications[FAVORITES]->favorite_path[fav] );

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

    if ( applications[FAVORITES]->icon[fav] != NULL )
    {
        debug_infof( "Removing icon%i from memory\n", fav );
        GLES2D_FreeTexture ( applications[FAVORITES]->icon[fav] );
        applications[FAVORITES]->icon[fav] = NULL;
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

*/
    debug_end();

	return 1;
}

int cfg_fav_add( char *id )
{
    debug_start();

     // Set favorites file path
    char favorite_file[512];
    memset( favorite_file, 0, 512 );
    sprintf( favorite_file, "%s/%s", PMENU_PATH, "favorites.cfg" );

    // if we didnt't reached the maximum allowed favorites
    if ( applications_count[FAVORITES] < FAV_MAX )
    {
        // Init configuration
        config_init( &cfg );

        // Create favorite file if needed
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

        // Try to read the file
        if ( ! config_read_file( &cfg, favorite_file ) )
        {
            debug_errorf ( "config_read_file(%s) failed", favorite_file );
            debug_end();
            config_destroy( &cfg );
            return 0;
        }

        config_setting_t *search = config_lookup( &cfg, id );

        if ( ! search )
        {
            debug_infof("Adding Favorite application (uid:%s) to %s", id, favorite_file );

            config_setting_t *root = config_root_setting( &cfg );
            config_setting_t *new = NULL;
            new = config_setting_add( root, id, CONFIG_TYPE_GROUP );

            config_write_file( &cfg, favorite_file );
		}
		else
		{
            debug_error ( "application already added to favorites" );
		}

        config_destroy( &cfg );
    }
    else
    {
        debug_info( "cfg_fav_add( ) -> Maximum favorites applications reached\n\n" );
    }

    debug_end();

	return 1;
}


