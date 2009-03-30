#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_framerate.h>
#include <SDL_rotozoom.h>

#include "fav_config.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 32

#define NORMAL 0
#define BLEND 1
#define SMALL 0
#define BIG 1

#define CENTERED 1

#include <GLES/gl.h>
#include <GLES/egl.h>

EGLDisplay sglDisplay;
EGLConfig  sglConfig;
EGLContext sglContext;
EGLSurface sglSurface;

typedef struct
{
	GLuint texture;
	GLfloat texMinX;
	GLfloat texMinY;
	GLfloat texMaxX;
	GLfloat texMaxY;
	int w;
	int h;

} TEXTURE;

TEXTURE *background, *fav_background, *app_background, *highlight, *app_highlight, *category_icon[4], *arrow[2], *confirm_box, *no_icon, \
		*preview[3][256], *tmp_preview, *fav_preview[FAV_MAX], *logo, *buffer1, *buffer2;

void copy_buffer_to_texture(TEXTURE *texture);
void draw_3d_box(TEXTURE *tex, int x, int y, GLfloat z);
void DrawTexture(TEXTURE *texture_struct, int x, int y, int centered, GLfloat factor, GLfloat alpha);
void DrawBufferTexture(TEXTURE *texture_struct, int x, int y, int centered, GLfloat factor, GLfloat alpha);
void load_image_to_texture( TEXTURE *texture_struct, char *filename);
void init_2D_view();
void gui_swap_buffer();
void init_raw_screen();
void destroy_raw_screen();

float alpha, alpha_preview;
int alpha_up;

TTF_Font *font, *font_big;
SDL_Surface *EGLscreen;
SDL_Surface *message, *myscreen;
SDL_Surface *text_img;
int fav_preview_x[FAV_MAX], fav_preview_y[FAV_MAX];

void draw_text(char *msg, int size, int type, SDL_Color COLOR, int x, int y);
void draw_text_center(char *msg, int size, int type, SDL_Color COLOR, int x, int y);
void draw_text_scroll(char *msg, int size, int type, SDL_Color COLOR, int x, int y, int w);

#endif
