#ifndef __MAIN_H__
#define __MAIN_H__

#include <GLES2D/GLES2D.h>

#include "get_apps.h"

GLES2D_Font *normal, *small;

GLES2D_Texture *sd1_icon, *sd2_icon, *clock_icon, *cpu_icon, *background, *fav_background, *app_background, *highlight, *highlight_fav, *app_highlight, *category_icon[CATEGORY_COUNT], *arrow[2], *confirm_box, *no_icon, *no_preview, \
		*icon[CATEGORY_COUNT][256], *tmp_preview, *logo, *buffer1, *buffer2;

int icon_x[CATEGORY_COUNT][256], icon_y[CATEGORY_COUNT][256];
int alpha, alpha_preview;
int alpha_up;

int scroll_count, reset_scroll_count;

#endif
