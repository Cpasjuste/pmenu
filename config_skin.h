#ifndef __GUI_SKIN_H__
#define __GUI_SKIN_H__

#include <GLES2D/GLES2D.h>
#include "libconfig.h"
#include "get_apps.h"

struct config_t cfg;

typedef struct
{
    char background_applications[512];
	char background_media[512];
	char background_settings[512];
    char highlight[512];
    int highlight_enabled;
    char application_highlight[512];
    char favorites_icon[512];
    char favorites_icon_highlight[512];
    char emulators_icon[512];
    char emulators_icon_highlight[512];
    char games_icon[512];
    char games_icon_highlight[512];
    char misc_icon[512];
    char misc_icon_highlight[512];
    char media_icon[512];
    char media_icon_highlight[512];
    char settings_icon[512];
    char settings_icon_highlight[512];
    char confirm_box[512];
    char no_icon[512];
    char no_preview[512];
    char cpu_icon[512];
    char sd1_icon[512];
    char sd2_icon[512];
    char clock_icon[512];
    char font_big[512];
    char font_small[512];
    int font_small_size;
    int font_small_color;
    int font_small_color_highlight;
    int font_small_aliasing;
    int font_small_style;
    int font_big_size;
    int font_big_color;
    int font_big_color_highlight;
    int font_big_aliasing;
    int font_big_style;
	int applications_box_x;
	int applications_box_y;
	int applications_box_w;
	int confirm_box_x;
	int confirm_box_y;
	int preview_pic_x;
	int preview_pic_y;
	int preview_pic_w;
	int emulators_icon_x;
	int emulators_icon_y;
	int games_icon_x;
	int games_icon_y;
	int misc_icon_x;
	int misc_icon_y;
    int media_icon_x;
	int media_icon_y;
    char media_file_icon[512];
	char media_folder_icon[512];
	char media_text_font[512];
	int media_text_size;
	int media_text_color;
	int media_text_color_highlight;
	int media_text_aliasing;
	int media_text_style;
	int media_max_files_per_page;
	int favorites_icon_x;
	int favorites_icon_y;
    int settings_icon_x;
	int settings_icon_y;
	char title[CATEGORY_COUNT][256];
	GLES2D_FontCache *title_cache[CATEGORY_COUNT];
	int cpu_icon_x;
	int cpu_icon_y;
	int cpu_text_x;
	int cpu_text_y;
    char cpu_text_font[512];
	int cpu_text_size;
	int cpu_text_color;
	int cpu_text_aliasing;
	int cpu_text_style;
    int sd1_icon_x;
	int sd1_icon_y;
	int sd1_text_x;
	int sd1_text_y;
    char sd1_text_font[512];
	int sd1_text_size;
	int sd1_text_color;
	int sd1_text_aliasing;
	int sd1_text_style;
    int sd2_icon_x;
	int sd2_icon_y;
	int sd2_text_x;
	int sd2_text_y;
    char sd2_text_font[512];
	int sd2_text_size;
	int sd2_text_color;
	int sd2_text_aliasing;
	int sd2_text_style;
    int clock_icon_x;
	int clock_icon_y;
	int clock_text_x;
	int clock_text_y;
    char clock_text_font[512];
	int clock_text_size;
	int clock_text_color;
	int clock_text_aliasing;
	int clock_text_style;
    int show_category_title;
	int max_app_per_page;
	int settings_text_spacing;

	int icon_scale_max;
	int icon_scale_min;
	int applications_spacing;
	int applications_title_description_y;

} GUI_CONFIG;

GUI_CONFIG *gui;

int cfg_gui_read();

#endif


