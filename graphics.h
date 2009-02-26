#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_framerate.h>
#include <SDL_rotozoom.h>

#include "fav_config.h"

#define NORMAL 0
#define BLEND 1
#define SMALL 0
#define BIG 1

TTF_Font *font, *font_big;
SDL_Surface *message, *background, *fav_background, *app_background, *myscreen, *preview[3][256], *highlight, *category_icon[4], *confirm_box, *arrow[2];
SDL_Surface *fav_preview[FAV_MAX], *no_icon, *app_highlight;
int fav_preview_x[FAV_MAX], fav_preview_y[FAV_MAX];

SDL_Surface *load_image(char *filename );
SDL_Surface *load_image_alpha(char *filename );
SDL_Surface *load_image_rgba(char *filename );
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
void apply_surface_center( int x, int y, SDL_Surface* source, SDL_Surface* destination );
void draw_text(char *msg, int size, int type, SDL_Color COLOR, int x, int y);
void draw_text_center(char *msg, int size, int type, SDL_Color COLOR, int x, int y);
void draw_text_scroll(char *msg, int size, int type, SDL_Color COLOR, int x, int y, int w);

#endif
