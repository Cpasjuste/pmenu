#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui_config.h"

int cfg_gui_read()
{
	config_init(&cfg);

	if (!config_read_file(&cfg, "data/gui.cfg"))
	{
		printf("config_read_file() failed\n");
		return -1;
	}
	else
	{
		gui = (GUI_CONFIG *) malloc( sizeof(GUI_CONFIG));

		config_setting_t *search = NULL;
	
		search = config_lookup(&cfg, "gui");	

		if (!search)
		{
			printf("config_lookup() failed\n");
			return -1;
		}
		else
		{
			config_setting_t *tmp = config_setting_get_member(search, "applications_box_x");
			if(tmp)
			{ 
				gui->applications_box_x = config_setting_get_int(tmp);
			}
			tmp = config_setting_get_member(search, "applications_box_y");
			if(tmp)
			{ 
				gui->applications_box_y = config_setting_get_int(tmp);
			}
			tmp = config_setting_get_member(search, "arrow_left_x");
			if(tmp)
			{ 
				gui->arrow_left_x = config_setting_get_int(tmp);
			}
			tmp = config_setting_get_member(search, "arrow_left_y");
			if(tmp)
			{ 
				gui->arrow_left_y = config_setting_get_int(tmp);
			}
			tmp = config_setting_get_member(search, "arrow_right_x");
			if(tmp)
			{ 
				gui->arrow_right_x = config_setting_get_int(tmp);
			}
			tmp = config_setting_get_member(search, "arrow_right_y");
			if(tmp)
			{ 
				gui->arrow_right_y = config_setting_get_int(tmp);
			}
		}
	}

	config_destroy(&cfg);

	return 1;
}

