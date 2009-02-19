#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>

#include "fav_config.h"

void gui_load_fav();
void gui_clean_fav();

const char *tmpFav[FAV_MAX] = {
	"fav0",
	"fav1",
	"fav2",
	"fav3",
	"fav4",
	"fav5",
	"fav6",
	"fav7",
	"fav8",
	"fav9",
	"fav10",
	"fav11",
	"fav12",
	"fav13",
	"fav14",
	"fav15",
	"fav16",
	"fav17",
	"fav18",
	"fav19",
	"fav20"
}; 

int cfg_fav_read()
{
	int i;

	config_init(&cfg);

	if (!config_read_file(&cfg, "data/favorites.cfg"))
	{
		printf("config_read_file() failed\n");
		return -1;
	}
	else
	{
		fav = (CONFIG *) malloc( sizeof(CONFIG));

		config_setting_t *search = NULL;
	
		for( i = 0; i < FAV_MAX ; i++ )
		{
			search = config_lookup(&cfg, tmpFav[i]);	

			if (!search)
			{
				printf("config_lookup() failed\n");
				return -1;
			}
			else
			{
				config_setting_t *tmp = config_setting_get_member(search, "enabled");
				if(tmp)
				{ 
					fav->enabled[i] = config_setting_get_int(tmp);
					//printf("[%i] fav->enabled = %i\n", i, fav->enabled[i]);
					if(fav->enabled[i])
					{
						tmp = config_setting_get_member(search, "name");
						if(tmp)
						{ 
							strcpy(fav->name[i], config_setting_get_string(tmp));
							//printf("[%i] fav->name = %s\n", i, fav->name[i]);
						}
						tmp = config_setting_get_member(search, "path");
						if(tmp)
						{ 
							strcpy(fav->path[i], config_setting_get_string(tmp));
							//printf("[%i] fav->path = %s\n", i, fav->path[i]);
						}
						tmp = config_setting_get_member(search, "exec_path");
						if(tmp)
						{ 
							strcpy(fav->exec_path[i], config_setting_get_string(tmp));
							//printf("[%i] fav->exec_path = %s\n", i, fav->exec_path[i]);
						}
						tmp = config_setting_get_member(search, "exec_name");
						if(tmp)
						{ 
							strcpy(fav->exec_name[i], config_setting_get_string(tmp));
							//printf("[%i] fav->exec_name = %s\n", i, fav->exec_name[i]);
						}
						tmp = config_setting_get_member(search, "icon");
						if(tmp)
						{ 
							strcpy(fav->icon[i], config_setting_get_string(tmp));
							//printf("[%i] fav->icon = %s\n", i, fav->icon[i]);
						}
						tmp = config_setting_get_member(search, "description");
						if(tmp)
						{ 
							strcpy(fav->description[i], config_setting_get_string(tmp));
							//printf("[%i] fav->description = %s\n", i, fav->description[i]);
						}
					}

				}
				else
				{
					printf("config_lookup() failed\n");
					return -1;
				}	
			}
		}
	}
	config_destroy(&cfg);

	gui_load_fav();

	return 1;
}

int cfg_fav_del( int fav_number )
{
	gui_clean_fav();
	//free(fav);

	fav->enabled[fav_number] = 0;

	config_init(&cfg);

	if (!config_read_file(&cfg, "data/favorites.cfg"))
	{
		printf("config_read_file() failed\n");
		return -1;
	}
	else
	{
		config_setting_t *item = NULL;

		int i, j = 0;

		for( i = 0; i < FAV_MAX ; i++ )
		{
			if( fav->enabled[i] )
			{
				item = config_lookup(&cfg, tmpFav[j]);

				if (!item) 
				{
					printf("config_lookup failed\n");
				}
				else 
				{
					config_setting_t *tmp = config_setting_get_member (item, "enabled");
					config_setting_set_int(tmp, 1);

					tmp = config_setting_get_member(item, "name");
					if(tmp) {
						config_setting_set_string(tmp, fav->name[i]);
					}

					tmp = config_setting_get_member(item, "path");
					if(tmp) {
						config_setting_set_string(tmp, fav->path[i]);
					}

					tmp = config_setting_get_member(item, "exec_path");
					if(tmp) {
						config_setting_set_string(tmp, fav->exec_path[i]);
					}

					tmp = config_setting_get_member(item, "exec_name");
					if(tmp) {
						config_setting_set_string(tmp, fav->exec_name[i]);
					}

					tmp = config_setting_get_member(item, "icon");
					if(tmp) {
						config_setting_set_string(tmp, fav->icon[i]);
					}

					tmp = config_setting_get_member(item, "description");
					if(tmp) {
						config_setting_set_string(tmp, fav->description[i]);
					}
				}
				j++;
			}
		}
		for( i = j; i < FAV_MAX; i++ )
		{
			item = config_lookup(&cfg, tmpFav[i]);

			if (!item) 
			{
				printf("config_lookup failed\n");
			}
			else 
			{
				config_setting_t *tmp = config_setting_get_member (item, "enabled");
				config_setting_set_int(tmp, 0);
			}
		}
	}

	config_write_file(&cfg, "data/favorites.cfg");
	config_destroy(&cfg);

	free(fav);
	cfg_fav_read();

	return 1;
}

int cfg_fav_add(int fav_number, char *name, char *path, char *exec_path, char *exec_name, char *icon, char *description)
{
	gui_clean_fav();
	free(fav);

	config_init(&cfg);

	if (!config_read_file(&cfg, "data/favorites.cfg"))
	{
		printf("config_read_file() failed\n");
		return -1;
	}
	else
	{
		config_setting_t *item = NULL;

		printf("Writing item : %s\n", tmpFav[fav_number]);

		item = config_lookup(&cfg, tmpFav[fav_number]);

		if (!item) 
		{
			printf("config_lookup failed\n");
		}
		else 
		{
			config_setting_t *tmp = config_setting_get_member (item, "enabled");
			config_setting_set_int(tmp, 1);

			tmp = config_setting_get_member(item, "name");
			if(tmp) {
				config_setting_set_string(tmp, name);
			}

			tmp = config_setting_get_member(item, "path");
			if(tmp) {
				config_setting_set_string(tmp, path);
			}

			tmp = config_setting_get_member(item, "exec_path");
			if(tmp) {
				config_setting_set_string(tmp, exec_path);
			}

			tmp = config_setting_get_member(item, "exec_name");
			if(tmp) {
				config_setting_set_string(tmp, exec_name);
			}

			tmp = config_setting_get_member(item, "icon");
			if(tmp) {
				config_setting_set_string(tmp, icon);
			}

			tmp = config_setting_get_member(item, "description");
			if(tmp) {
				config_setting_set_string(tmp, description);
			}
		}
	}
	config_write_file(&cfg, "data/favorites.cfg");
	config_destroy(&cfg);

	cfg_fav_read();

	return 0;
}


