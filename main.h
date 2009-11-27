#ifndef __MAIN_H__
#define __MAIN_H__

#include <GLES2D/GLES2D.h>

#include "get_apps.h"

#define FONT_COUNT 7
#define BIG 0
#define SMALL 1
#define CLOCK 2
#define SD1 3
#define SD2 4
#define CPU 5
#define MEDIA_FNT 6

GLES2D_Font *fnt[7];

GLES2D_Texture *sd1_icon, *sd2_icon, *clock_icon, *cpu_icon, *background_settings, *background_media, *background_applications, *highlight, *highlight_fav, *app_highlight, *category_icon[CATEGORY_COUNT], *category_icon_highlight[CATEGORY_COUNT], *confirm_box, *no_icon, *no_preview, \
		*icon[CATEGORY_COUNT][256], *tmp_preview, *logo, *buffer1, *buffer2;

int icon_x[CATEGORY_COUNT][256], icon_y[CATEGORY_COUNT][256];
int alpha, alpha_preview;
int alpha_up;

int scroll_count, reset_scroll_count;

#endif
