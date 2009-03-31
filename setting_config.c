#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setting_config.h"

int cfg_setting_read()
{
/*
	config_init(&cfg);

	if (!config_read_file(&cfg, "setting.cfg"))
	{
		printf("config_read_file(setting.cfg) failed\n");
		return -1;
	}
	else
	{
		setting = (SETTING_CONFIG *) malloc( sizeof(SETTING_CONFIG));

		config_setting_t *search = NULL;
	
		search = config_lookup(&cfg, "settings");	

		if (!search)
		{
			printf("config_lookup() failed\n");
			return -1;
		}
		else
		{
			config_setting_t *tmp = config_setting_get_member(search, "x11_mode");
			if(tmp)
			{ 
				setting->x11_mode = config_setting_get_int(tmp);
			}
		}
	}

	config_destroy(&cfg);
*/
	return 1;

}

