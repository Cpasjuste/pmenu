#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config_skin.h"
#include "get_apps.h"
#include "utils.h"
#include "config_pmenu.h"
#include "common.h"

int cfg_gui_read()
{
    debug_start();

	config_init(&cfg);

	char cfg_file[512];
    sprintf( cfg_file, "%s/skin.cfg", pmenu->skin_dir );

	if (!config_read_file( &cfg, cfg_file ) )
	{
		printf ("config_read_file() failed (line %d of %s)\n", (__LINE__ -1), __FILE__);;
		config_destroy(&cfg);
	debug_end();
		return -1;
	}
	else
	{
	    if ( gui != NULL )
	    {
            free( gui );
	    }

		gui = (GUI_CONFIG *) malloc( sizeof(GUI_CONFIG));

		config_setting_t *search = NULL;

		search = config_lookup(&cfg, "gui_main");

		if (!search)
		{
			error("config_lookup() failed\n");
			config_destroy(&cfg);
            debug_end();
			return -1;
		}
		else
		{
            /* skin data files */
		    config_setting_t *tmp = config_setting_get_member(search, "background_applications");
            if(tmp)
            {
                    strcpy( gui->background_applications, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find background_applications in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "background_media");
            if(tmp)
            {
                    strcpy( gui->background_media, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find background_media in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "background_settings");
            if(tmp)
            {
                    strcpy( gui->background_settings, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find background_settings in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "highlight");
            if(tmp)
            {
                    strcpy( gui->highlight, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "highlight_enabled");
            if(tmp)
            {
                    gui->highlight_enabled = config_setting_get_int(tmp);
            }
            else
            {
                debug_errorf( "could not find highlight_enabled in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "application_highlight");
            if(tmp)
            {
                    strcpy( gui->application_highlight, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find application_highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "favorites_icon");
            if(tmp)
            {
                    strcpy( gui->favorites_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find favorites_icon in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "favorites_icon_highlight");
            if(tmp)
            {
                    strcpy( gui->favorites_icon_highlight, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find favorites_icon_highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "emulators_icon");
            if(tmp)
            {
                    strcpy( gui->emulators_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find emulators_icon in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "emulators_icon_highlight");
            if(tmp)
            {
                    strcpy( gui->emulators_icon_highlight, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find emulators_icon_highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }


            tmp = config_setting_get_member(search, "games_icon");
            if(tmp)
            {
                    strcpy( gui->games_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find games_icon in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "games_icon_highlight");
            if(tmp)
            {
                    strcpy( gui->games_icon_highlight, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find games_icon_highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "misc_icon");
            if(tmp)
            {
                    strcpy( gui->misc_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find misc_icon in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "misc_icon_highlight");
            if(tmp)
            {
                    strcpy( gui->misc_icon_highlight, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find misc_icon_highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "media_icon");
            if(tmp)
            {
                    strcpy( gui->media_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find media_icon in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "media_icon_highlight");
            if(tmp)
            {
                    strcpy( gui->media_icon_highlight, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find media_icon_highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "media_file_icon");
            if(tmp)
            {
                    strcpy( gui->media_file_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find media_file_icon in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "media_folder_icon");
            if(tmp)
            {
                    strcpy( gui->media_folder_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find media_folder_icon in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "settings_icon");
            if(tmp)
            {
                    strcpy( gui->settings_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find settings_icon in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "settings_icon_highlight");
            if(tmp)
            {
                    strcpy( gui->settings_icon_highlight, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find settings_icon_highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "confirm_box");
            if(tmp)
            {
                    strcpy( gui->confirm_box, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find confirm_box in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "no_icon");
            if(tmp)
            {
                    strcpy( gui->no_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find no_icon in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "no_preview");
            if(tmp)
            {
                    strcpy( gui->no_preview, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find no_preview in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "cpu_icon");
            if(tmp)
            {
                    strcpy( gui->cpu_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find cpu_icon in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "sd1_icon");
            if(tmp)
            {
                    strcpy( gui->sd1_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find sd1_icon in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "sd2_icon");
            if(tmp)
            {
                    strcpy( gui->sd2_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find sd2_icon in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "clock_icon");
            if(tmp)
            {
                    strcpy( gui->clock_icon, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find clock_icon in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "font_big");
            if(tmp)
            {
                    strcpy( gui->font_big, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find font_big in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "font_small");
            if(tmp)
            {
                    strcpy( gui->font_small, config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find font_small in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

		    tmp = config_setting_get_member( search, "font_small_size" );
		    if(tmp)
			{
				gui->font_small_size = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find font_small_size in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member( search, "font_small_color" );
			if(tmp)
			{
				gui->font_small_color = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find font_small_color in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member( search, "font_small_color_highlight" );
			if(tmp)
			{
				gui->font_small_color_highlight = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find font_small_color_highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "font_small_aliasing" );
			if(tmp)
			{
				gui->font_small_aliasing = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find font_small_aliasing in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "font_small_style" );
			if(tmp)
			{
				gui->font_small_style = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find font_small_style in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member( search, "font_big_size" );
			if(tmp)
			{
				gui->font_big_size = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find font_big_size in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member( search, "font_big_color" );
			if(tmp)
			{
				gui->font_big_color = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find font_big_color in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member( search, "font_big_color_highlight" );
			if(tmp)
			{
				gui->font_big_color_highlight = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find font_big_color_highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "font_big_aliasing" );
			if(tmp)
			{
				gui->font_big_aliasing = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find font_big_aliasing in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "font_big_style" );
			if(tmp)
			{
				gui->font_big_style = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find font_big_style in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "applications_box_x");
			if(tmp)
			{
				gui->applications_box_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find applications_box_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "applications_box_y");
			if(tmp)
			{
				gui->applications_box_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find applications_box_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "applications_box_w");
			if(tmp)
			{
				gui->applications_box_w = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find applications_box_w in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "confirm_box_x");
			if(tmp)
			{
				gui->confirm_box_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find confirm_box_x in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "confirm_box_y");
			if(tmp)
			{
				gui->confirm_box_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find confirm_box_y in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "preview_pic_x");
			if(tmp)
			{
				gui->preview_pic_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find preview_pic_x in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "preview_pic_y");
			if(tmp)
			{
				gui->preview_pic_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find preview_pic_y in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "preview_pic_w");
			if(tmp)
			{
				gui->preview_pic_w = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find preview_pic_w in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "emulators_icon_x");
			if(tmp)
			{
				gui->emulators_icon_x = config_setting_get_int(tmp);
				category_icon_x[EMULATORS] = gui->emulators_icon_x;
			}
			else
            {
                debug_errorf( "could not find emulators_icon_x in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "emulators_icon_y");
			if(tmp)
			{
				gui->emulators_icon_y = config_setting_get_int(tmp);
				category_icon_y[EMULATORS] = gui->emulators_icon_y;
			}
			else
            {
                debug_errorf( "could not find emulators_icon_y in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "emulators_title");
            if(tmp)
            {
                    strcpy( gui->title[EMULATORS], config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find emulators_title in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "games_icon_x");
			if(tmp)
			{
				gui->games_icon_x = config_setting_get_int(tmp);
				category_icon_x[GAMES] = gui->games_icon_x;
			}
			else
            {
                debug_errorf( "could not find games_icon_x in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "games_icon_y");
			if(tmp)
			{
				gui->games_icon_y = config_setting_get_int(tmp);
				category_icon_y[GAMES] = gui->games_icon_y;
			}
			else
            {
                debug_errorf( "could not find games_icon_y in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "games_title");
            if(tmp)
            {
                    strcpy( gui->title[GAMES], config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find games_title in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }


			tmp = config_setting_get_member(search, "misc_icon_x");
			if(tmp)
			{
				gui->misc_icon_x = config_setting_get_int(tmp);
				category_icon_x[MISC] = gui->misc_icon_x;
			}
			else
            {
                debug_errorf( "could not find misc_icon_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "misc_icon_y");
			if(tmp)
			{
				gui->misc_icon_y = config_setting_get_int(tmp);
				category_icon_y[MISC] = gui->misc_icon_y;
			}
			else
            {
                debug_errorf( "could not find misc_icon_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "misc_title");
            if(tmp)
            {
                    strcpy( gui->title[MISC], config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find misc_title in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "media_icon_x");
			if(tmp)
			{
				gui->media_icon_x = config_setting_get_int(tmp);
				category_icon_x[MEDIA] = gui->media_icon_x;
			}
			else
            {
                debug_errorf( "could not find media_icon_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "media_icon_y");
			if(tmp)
			{
				gui->media_icon_y = config_setting_get_int(tmp);
				category_icon_y[MEDIA] = gui->media_icon_y;
			}
			else
            {
                debug_errorf( "could not find media_icon_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "media_title");
            if(tmp)
            {
                    strcpy( gui->title[MEDIA], config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find media_title in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "media_text_font");
            if(tmp)
            {
                    strcpy( gui->media_text_font, config_setting_get_string(tmp) );
            }
            else
            {
                debug_errorf( "could not find media_text_font in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "media_text_size" );
            if(tmp)
            {
                gui->media_text_size = config_setting_get_int(tmp);
            }
            else
            {
                debug_errorf( "could not find media_text_size in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "media_text_color" );
            if(tmp)
            {
                gui->media_text_color = config_setting_get_int(tmp);
            }
            else
            {
                debug_errorf( "could not find media_text_color in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member(search, "media_text_color_highlight" );
            if(tmp)
            {
                gui->media_text_color_highlight = config_setting_get_int(tmp);
            }
            else
            {
                debug_errorf( "could not find media_text_color_highlight in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "media_text_aliasing" );
			if(tmp)
			{
				gui->media_text_aliasing = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find media_text_aliasing in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "media_text_style" );
			if(tmp)
			{
				gui->media_text_style = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find media_text_style in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "media_max_files_per_page");
            if(tmp)
            {
                gui->media_max_files_per_page = config_setting_get_int(tmp);
            }
            else
            {
                debug_errorf( "could not find media_max_files_per_page in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "favorites_icon_x");
			if(tmp)
			{
				gui->favorites_icon_x = config_setting_get_int(tmp);
				category_icon_x[FAVORITES] = gui->favorites_icon_x;
			}
			else
            {
                debug_errorf( "could not find favorites_icon_x in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "favorites_icon_y");
			if(tmp)
			{
				gui->favorites_icon_y = config_setting_get_int(tmp);
				category_icon_y[FAVORITES] = gui->favorites_icon_y;
			}
			else
            {
                debug_errorf( "could not find favorites_icon_y in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "favorites_title");
            if(tmp)
            {
                    strcpy( gui->title[FAVORITES], config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find favorites_title in %s", cfg_file );
                config_destroy(&cfg);
	debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "settings_icon_x");
			if(tmp)
			{
				gui->settings_icon_x = config_setting_get_int(tmp);
				category_icon_x[SETTINGS] = gui->settings_icon_x;
			}
			else
            {
                debug_errorf( "could not find settings_icon_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "settings_icon_y");
			if(tmp)
			{
				gui->settings_icon_y = config_setting_get_int(tmp);
				category_icon_y[SETTINGS] = gui->settings_icon_y;
			}
			else
            {
                debug_errorf( "could not find settings_icon_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			/* cpu infos */
            tmp = config_setting_get_member(search, "cpu_icon_x");
			if(tmp)
			{
				gui->cpu_icon_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find cpu_icon_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "cpu_icon_y");
			if(tmp)
			{
				gui->cpu_icon_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find cpu_icon_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "cpu_text_x");
			if(tmp)
			{
				gui->cpu_text_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find cpu_text_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "cpu_text_y");
			if(tmp)
			{
				gui->cpu_text_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find cpu_text_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member( search, "cpu_text_font" );
			if(tmp)
			{
				strcpy( gui->cpu_text_font, config_setting_get_string(tmp));
			}
			else
            {
                debug_errorf( "could not find cpu_text_font in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "cpu_text_size" );
			if(tmp)
			{
				gui->cpu_text_size = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find cpu_text_size in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member( search, "cpu_text_color" );
			if(tmp)
			{
				gui->cpu_text_color = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find cpu_text_color in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "cpu_text_aliasing" );
			if(tmp)
			{
				gui->cpu_text_aliasing = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find cpu_text_aliasing in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "cpu_text_style" );
			if(tmp)
			{
				gui->cpu_text_style = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find cpu_text_style in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }


			/*sd1 infos*/
            tmp = config_setting_get_member(search, "sd1_icon_x");
			if(tmp)
			{
				gui->sd1_icon_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd1_icon_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "sd1_icon_y");
			if(tmp)
			{
				gui->sd1_icon_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd1_icon_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "sd1_text_x");
			if(tmp)
			{
				gui->sd1_text_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd1_text_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "sd1_text_y");
			if(tmp)
			{
				gui->sd1_text_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd1_text_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member( search, "sd1_text_font" );
			if(tmp)
			{
				strcpy( gui->sd1_text_font, config_setting_get_string(tmp));
			}
			else
            {
                debug_errorf( "could not find sd1_text_font in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "sd1_text_size" );
			if(tmp)
			{
				gui->sd1_text_size = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd1_text_size in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
			tmp = config_setting_get_member( search, "sd1_text_color" );
			if(tmp)
			{
				gui->sd1_text_color = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd1_text_color in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "sd1_text_aliasing" );
			if(tmp)
			{
				gui->sd1_text_aliasing = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd1_text_aliasing in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "sd1_text_style" );
			if(tmp)
			{
				gui->sd1_text_style = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd1_text_style in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }


			/*sd2 infos*/
            tmp = config_setting_get_member(search, "sd2_icon_x");
			if(tmp)
			{
				gui->sd2_icon_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd2_icon_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "sd2_icon_y");
			if(tmp)
			{
				gui->sd2_icon_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd2_icon_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "sd2_text_x");
			if(tmp)
			{
				gui->sd2_text_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd2_text_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "sd2_text_y");
			if(tmp)
			{
				gui->sd2_text_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd2_text_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member( search, "sd2_text_font" );
			if(tmp)
			{
				strcpy( gui->sd2_text_font, config_setting_get_string(tmp));
			}
			else
            {
                debug_errorf( "could not find sd2_text_font in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "sd2_text_size" );
			if(tmp)
			{
				gui->sd2_text_size = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd2_text_size in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member( search, "sd2_text_color" );
			if(tmp)
			{
				gui->sd2_text_color = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd2_text_color in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "sd2_text_aliasing" );
			if(tmp)
			{
				gui->sd2_text_aliasing = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd2_text_aliasing in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "sd2_text_style" );
			if(tmp)
			{
				gui->sd2_text_style = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find sd2_text_style in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			/*clock infos*/
            tmp = config_setting_get_member(search, "clock_icon_x");
			if(tmp)
			{
				gui->clock_icon_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find clock_icon_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "clock_icon_y");
			if(tmp)
			{
				gui->clock_icon_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find clock_icon_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "clock_text_x");
			if(tmp)
			{
				gui->clock_text_x = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find clock_text_x in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "clock_text_y");
			if(tmp)
			{
				gui->clock_text_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find clock_text_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member( search, "clock_text_font" );
			if(tmp)
			{
				strcpy( gui->clock_text_font, config_setting_get_string(tmp));
			}
			else
            {
                debug_errorf( "could not find clock_text_font in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "clock_text_size" );
			if(tmp)
			{
				gui->clock_text_size = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find clock_text_size in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member( search, "clock_text_color" );
			if(tmp)
			{
				gui->clock_text_color = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find clock_text_color in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "clock_text_aliasing" );
			if(tmp)
			{
				gui->clock_text_aliasing = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find clock_text_aliasing in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
            tmp = config_setting_get_member( search, "clock_text_style" );
			if(tmp)
			{
				gui->clock_text_style = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find clock_text_style in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }


            tmp = config_setting_get_member(search, "settings_title");
            if(tmp)
            {
                    strcpy( gui->title[SETTINGS], config_setting_get_string(tmp));
            }
            else
            {
                debug_errorf( "could not find settings_title in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

			tmp = config_setting_get_member(search, "show_category_title");
			if(tmp)
			{
				gui->show_category_title = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find show_category_title in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }


			tmp = config_setting_get_member(search, "max_app_per_page");
			if(tmp)
			{
				gui->max_app_per_page = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find max_app_per_page in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "settings_text_spacing");
			if(tmp)
			{
				gui->settings_text_spacing = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find settings_text_spacing in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "icon_scale_max" );
			if(tmp)
			{
				gui->icon_scale_max = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find icon_scale_max in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "icon_scale_min" );
			if(tmp)
			{
				gui->icon_scale_min = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find icon_scale_min in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "applications_spacing" );
			if(tmp)
			{
				gui->applications_spacing = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find applications_spacing in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }

            tmp = config_setting_get_member(search, "applications_title_description_y" );
			if(tmp)
			{
				gui->applications_title_description_y = config_setting_get_int(tmp);
			}
			else
            {
                debug_errorf( "could not find applications_title_description_y in %s", cfg_file );
                config_destroy(&cfg);
                debug_end();
                return 0;
            }
		}
	}

	config_destroy(&cfg);
	debug_end();

	return 1;
}

