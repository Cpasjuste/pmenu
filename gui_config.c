#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui_config.h"
#include "get_apps.h"
#include "utils.h"
#include "pmenu_config.h"

int cfg_gui_read()
{
	config_init(&cfg);

	char cfg_file[512];
    sprintf( cfg_file, "%s/gui.cfg", pmenu->skin_dir );

	if (!config_read_file( &cfg, cfg_file ) )
	{
		printf ("config_read_file() failed (line %d of %s)\n", (__LINE__ -1), __FILE__);;
		config_destroy(&cfg);
		return -1;
	}
	else
	{
		gui = (GUI_CONFIG *) malloc( sizeof(GUI_CONFIG));

		config_setting_t *search = NULL;

		search = config_lookup(&cfg, "gui_main");

		if (!search)
		{
			error("config_lookup() failed\n");
			config_destroy(&cfg);
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
			tmp = config_setting_get_member(search, "confirm_box_x");
			if(tmp)
			{
				gui->confirm_box_x = config_setting_get_int(tmp);
			}
			tmp = config_setting_get_member(search, "confirm_box_y");
			if(tmp)
			{
				gui->confirm_box_y = config_setting_get_int(tmp);
			}
			tmp = config_setting_get_member(search, "preview_pic_x");
			if(tmp)
			{
				gui->preview_pic_x = config_setting_get_int(tmp);
			}
			tmp = config_setting_get_member(search, "preview_pic_y");
			if(tmp)
			{
				gui->preview_pic_y = config_setting_get_int(tmp);
			}

			tmp = config_setting_get_member(search, "category_emulators_icon_x");
			if(tmp)
			{
				gui->category_emulators_icon_x = config_setting_get_int(tmp);
				category_icon_x[EMULATORS] = gui->category_emulators_icon_x;

			}
			tmp = config_setting_get_member(search, "category_emulators_icon_y");
			if(tmp)
			{
				gui->category_emulators_icon_y = config_setting_get_int(tmp);
				category_icon_y[EMULATORS] = gui->category_emulators_icon_y;
			}
            tmp = config_setting_get_member(search, "category_emulators_title");
            if(tmp)
            {
                    strcpy( gui->category_title[EMULATORS], config_setting_get_string(tmp));
            }

			tmp = config_setting_get_member(search, "category_games_icon_x");
			if(tmp)
			{
				gui->category_games_icon_x = config_setting_get_int(tmp);
				category_icon_x[GAMES] = gui->category_games_icon_x;
			}
			tmp = config_setting_get_member(search, "category_games_icon_y");
			if(tmp)
			{
				gui->category_games_icon_y = config_setting_get_int(tmp);
				category_icon_y[GAMES] = gui->category_games_icon_y;
			}
			tmp = config_setting_get_member(search, "category_games_title");
            if(tmp)
            {
                    strcpy( gui->category_title[GAMES], config_setting_get_string(tmp));
            }

			tmp = config_setting_get_member(search, "category_applications_icon_x");
			if(tmp)
			{
				gui->category_applications_icon_x = config_setting_get_int(tmp);
				category_icon_x[APPLICATIONS] = gui->category_applications_icon_x;
			}
			tmp = config_setting_get_member(search, "category_applications_icon_y");
			if(tmp)
			{
				gui->category_applications_icon_y = config_setting_get_int(tmp);
				category_icon_y[APPLICATIONS] = gui->category_applications_icon_y;
			}
			tmp = config_setting_get_member(search, "category_applications_title");
            if(tmp)
            {
                    strcpy( gui->category_title[APPLICATIONS], config_setting_get_string(tmp));
            }

			tmp = config_setting_get_member(search, "category_divers_icon_x");
			if(tmp)
			{
				gui->category_divers_icon_x = config_setting_get_int(tmp);
				category_icon_x[DIVERS] = gui->category_divers_icon_x;
			}
			tmp = config_setting_get_member(search, "category_divers_icon_y");
			if(tmp)
			{
				gui->category_divers_icon_y = config_setting_get_int(tmp);
				category_icon_y[DIVERS] = gui->category_divers_icon_y;
			}
			tmp = config_setting_get_member(search, "category_divers_title");
            if(tmp)
            {
                    strcpy( gui->category_title[DIVERS], config_setting_get_string(tmp));
            }

			tmp = config_setting_get_member(search, "category_favorites_icon_x");
			if(tmp)
			{
				gui->category_favorites_icon_x = config_setting_get_int(tmp);
				category_icon_x[FAVORITES] = gui->category_favorites_icon_x;
			}
			tmp = config_setting_get_member(search, "category_favorites_icon_y");
			if(tmp)
			{
				gui->category_favorites_icon_y = config_setting_get_int(tmp);
				category_icon_y[FAVORITES] = gui->category_favorites_icon_y;
			}
            tmp = config_setting_get_member(search, "category_favorites_title");
            if(tmp)
            {
                    strcpy( gui->category_title[FAVORITES], config_setting_get_string(tmp));
            }

            tmp = config_setting_get_member(search, "category_settings_icon_x");
			if(tmp)
			{
				gui->category_settings_icon_x = config_setting_get_int(tmp);
				category_icon_x[SETTINGS] = gui->category_settings_icon_x;
			}
			tmp = config_setting_get_member(search, "category_settings_icon_y");
			if(tmp)
			{
				gui->category_settings_icon_y = config_setting_get_int(tmp);
				category_icon_y[SETTINGS] = gui->category_settings_icon_y;
			}
            tmp = config_setting_get_member(search, "category_settings_title");
            if(tmp)
            {
                    strcpy( gui->category_title[SETTINGS], config_setting_get_string(tmp));
            }

			tmp = config_setting_get_member(search, "show_category_title");
			if(tmp)
			{
				gui->show_category_title = config_setting_get_int(tmp);
			}

			tmp = config_setting_get_member(search, "max_app_per_page");
			if(tmp)
			{
				gui->max_app_per_page = config_setting_get_int(tmp);
			}
		}
	}

	config_destroy(&cfg);

	return 1;
}

