#ifndef __GUI_CONFIG_H__
#define __GUI_CONFIG_H__

#include "libconfig.h"
#include "get_apps.h"

struct config_t cfg;


typedef struct
{
    int font_small_size;
    int font_small_color;
    int font_small_color_highlight;
    int font_big_size;
    int font_big_color;
    int font_big_color_highlight;
	int applications_box_x;
	int applications_box_y;
	int confirm_box_x;
	int confirm_box_y;
	int preview_pic_x;
	int preview_pic_y;
	int category_emulators_icon_x;
	int category_emulators_icon_y;
	int category_games_icon_x;
	int category_games_icon_y;
	int category_applications_icon_x;
	int category_applications_icon_y;
	int category_divers_icon_x;
	int category_divers_icon_y;
	int category_favorites_icon_x;
	int category_favorites_icon_y;
    int category_settings_icon_x;
	int category_settings_icon_y;
    char category_title[CATEGORY_COUNT][256];
    int show_category_title;
	int max_app_per_page;

} GUI_CONFIG;

GUI_CONFIG *gui;

int cfg_gui_read();

#endif


