#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config_pmenu.h"
#include "get_apps.h"
#include "utils.h"
#include "common.h"

int cfg_pmenu_read()
{
	config_init(&cfg);

	char cfg_path[512];
	sprintf( cfg_path, "%s/%s", PMENU_PATH, "pmenu.cfg" );

	if ( !config_read_file( &cfg, cfg_path ) )
	{
		printf ("config_read_file() failed : %s (line %d of %s)\n", cfg_path, (__LINE__ -1), __FILE__);;
		config_destroy(&cfg);
		return -1;
	}
	else
	{
	    if ( pmenu != NULL )
            free ( pmenu );

		pmenu = (PMENU_CONFIG *) malloc( sizeof(PMENU_CONFIG));

		config_setting_t *search = NULL;

		search = config_lookup(&cfg, "pmenu_main");

		if (!search)
		{
			error("config_lookup() failed\n");
			return -1;
		}
		else
		{
		    config_setting_t *tmp = NULL;

            tmp = config_setting_get_member(search, "skin_dir");
            if(tmp)
            {
#ifdef I386
                strcpy( pmenu->skin_dir, config_setting_get_string(tmp) );
#else
                char skin_path[512];
                sprintf( skin_path, "%s", config_setting_get_string(tmp) );
                strcpy( pmenu->skin_dir, skin_path );
                printf( "pmenu->skin_dir : %s\n", skin_path );
#endif
            }

            tmp = config_setting_get_member( search, "cpu_mhz" );
            if( tmp )
            {
                pmenu->cpu_mhz = config_setting_get_int( tmp );
            }
            else
            {
                pmenu->cpu_mhz = 600;
            }

            tmp = config_setting_get_member( search, "brightness" );
            if( tmp )
            {
                pmenu->brightness = config_setting_get_int( tmp );
            }
            else
            {
                pmenu->brightness = 51;
            }
		}
	}

	config_destroy(&cfg);

	return 1;
}

int cfg_pmenu_update_skin_path( char *skin_path )
{

    strcpy( pmenu->skin_dir, skin_path );

	/* Initialize the configuration */
	config_init(&cfg);

    char cfg_path[512];
	sprintf( cfg_path, "%s/%s", PMENU_PATH, "pmenu.cfg" );

	if ( !config_read_file( &cfg, cfg_path ) )
	{
		printf ("config_read_file() failed (line %d of %s)\n", (__LINE__ -1), __FILE__);;
		config_destroy(&cfg);
		return -1;
	}
	else
	{
		config_setting_t *item = NULL;
		item = config_lookup( &cfg, "pmenu_main" );

		if (!item)
		{
			printf("cfg_pmenu_update_skin_path: config_lookup failed\n");
		}
			else
			{
				config_setting_t *tmp = config_setting_get_member(item, "skin_dir");
				if(tmp)
				{
					config_setting_set_string( tmp, skin_path );
				}
			}
    }
	/* Write configuration */
	config_write_file( &cfg, cfg_path );

	/* Free the configuration */
	config_destroy( &cfg );

	return 0;
}

int cfg_pmenu_update_cpu_mhz( int mhz )
{
    debug_start();

	/* Initialize the configuration */
	config_init(&cfg);

    char cfg_path[512];
	sprintf( cfg_path, "%s/%s", PMENU_PATH, "pmenu.cfg" );

	if ( !config_read_file( &cfg, cfg_path ) )
	{
		debug_error( "config_read_file(pmenu.cfg) failed" );;
		config_destroy(&cfg);
		return -1;
	}
	else
	{
		config_setting_t *item = NULL;
		item = config_lookup( &cfg, "pmenu_main" );

		if ( !item )
		{
			debug_error("cfg_pmenu_update_skin_path: config_lookup failed");
		}
			else
			{
				config_setting_t *tmp = config_setting_get_member( item, "cpu_mhz" );
				if( tmp )
				{
					config_setting_set_int( tmp, mhz );
				}
			}
    }
	/* Write configuration */
	config_write_file( &cfg, cfg_path );

	/* Free the configuration */
	config_destroy( &cfg );

	debug_end();

	return 0;
}

