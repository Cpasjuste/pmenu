#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "graphics.h"
#include "sprig.h"

void gui_clean();

extern SDL_Color WHITE, BLUE, RED, GREEN, GRAY;


SDL_Surface *load_image(char *filename )
{
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;

	loadedImage = IMG_Load(filename);

	if( loadedImage != NULL )
	{
		optimizedImage = SPG_DisplayFormat( loadedImage );
	}
	else
	{
		printf("Unable to load image (%s)\n", filename);
		//gui_clean();
		//exit(0);
	}

	return optimizedImage;
}

SDL_Surface *load_image_alpha(char *filename )
{
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;

	loadedImage = IMG_Load(filename);

	if( loadedImage != NULL )
	{
		optimizedImage = SPG_DisplayFormat( loadedImage );
		SPG_SetColorkey(optimizedImage, 0x00ffff);
	}
	else
	{
		printf("Unable to load image (%s)\n", filename);
		//gui_clean();
		//exit(0);
	}

	return optimizedImage;
}

SDL_Surface *load_image_rgba(char *filename )
{
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;

	loadedImage = IMG_Load(filename);

	if( loadedImage != NULL )
	{
		optimizedImage = SPG_DisplayFormatAlpha( loadedImage );
	}
	else
	{
		printf("Unable to load image (%s)\n", filename);
		//gui_clean();
		//exit(0);
	}

	return optimizedImage;
}

void draw_text(char *msg, int size, int type, SDL_Color COLOR, int x, int y)
{
	SDL_Surface* text_img = NULL;

	if(size == SMALL)
	{
		if(type == NORMAL) text_img = TTF_RenderUTF8_Solid( font, msg, COLOR );
			else if(type == BLEND) text_img = TTF_RenderUTF8_Blended( font, msg, COLOR );
	}
	else if(size == BIG)
	{
		if(type == NORMAL) text_img = TTF_RenderUTF8_Solid( font_big, msg, COLOR );
			else if(type == BLEND) text_img = TTF_RenderUTF8_Blended( font_big, msg, COLOR );
	}

	SPG_Draw( text_img, myscreen, x, y );

	SDL_FreeSurface( text_img );
}

void draw_text_center(char *msg, int size, int type, SDL_Color COLOR, int x, int y)
{
	SDL_Surface* text_img = NULL;

	if(size == SMALL)
	{
		if(type == NORMAL) text_img = TTF_RenderUTF8_Solid( font, msg, COLOR );
			else if(type == BLEND) text_img = TTF_RenderUTF8_Blended( font, msg, COLOR );
	}
	else if(size == BIG)
	{
		if(type == NORMAL) text_img = TTF_RenderUTF8_Solid( font_big, msg, COLOR );
			else if(type == BLEND) text_img = TTF_RenderUTF8_Blended( font_big, msg, COLOR );
	}

	SPG_DrawCenter( text_img, myscreen, x, y );

	SDL_FreeSurface( text_img );
}

extern int scroll_count;

void draw_text_scroll(char *msg, int size, int type, SDL_Color COLOR, int x, int y, int w)
{
	SDL_Surface* text_img = NULL;

	if(size == SMALL)
	{
		if(type == NORMAL) text_img = TTF_RenderUTF8_Solid( font, msg, COLOR );
			else if(type == BLEND) text_img = TTF_RenderUTF8_Blended( font, msg, COLOR );
	}
	else if(size == BIG)
	{
		if(type == NORMAL) text_img = TTF_RenderUTF8_Solid( font_big, msg, COLOR );
			else if(type == BLEND) text_img = TTF_RenderUTF8_Blended( font_big, msg, COLOR );
	}

	SDL_Rect clipRect = { x, y, w, 40 };
	SPG_SetClip(myscreen, clipRect);

	SPG_Draw( text_img, myscreen, scroll_count, y );

	SPG_RestoreClip(myscreen);  // Reset it

	SDL_FreeSurface( text_img );

	scroll_count -= 1;

	int i = strlen(msg) * 10;

	if((scroll_count + x) == -i) scroll_count = x + w;
}


