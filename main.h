#ifndef __MAIN_H__
#define __MAIN_H__

#define B_1 1
#define B_2 2
#define B_L 4
#define B_R 5
#define B_SELECT 8
#define B_START 9

#define LEFT 0
#define RIGHT 1

SDL_Color WHITE = { 255, 255, 255 }, BLUE = { 0, 0, 255 }, RED = { 255, 0, 0 }, \
	GREEN = { 0, 255, 0 }, GRAY = { 175, 175, 175 };

int scroll_count, reset_scroll_count, mouse_y_pos, page[3] = {0, 0, 0}, preview_x[3][256], preview_y[3][256], \
	mouse_hold_x = 0, mouse_hold_y = 0, reset_ts_pos = 1, exec_app = 0, app_number = 0, add_to_fav = 0, \
	hold_count = 0, fav_page = 0;

const int category_icon_x[4] = { 80, 170, 260, 350 }, category_icon_y = 42;

char fpsText[16];

FPSmanager sixteen;

SDL_Event event;

#endif
