#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "graphics.h"

void gui_clean();

extern SDL_Color WHITE, BLUE, RED, GREEN, GRAY;

SDL_Surface *load_image(char *filename )
{
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;

 	loadedImage = IMG_Load(filename);

	if( loadedImage != NULL )
	{
		optimizedImage = SDL_DisplayFormat( loadedImage );
		SDL_FreeSurface( loadedImage );
	}
	else
	{
		printf("Unable to load image (%s)\n", filename);
		gui_clean();
		exit(0);
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
		if ( loadedImage->format->palette )
			SDL_SetColorKey(loadedImage, SDL_SRCCOLORKEY, *(Uint8 *)loadedImage->pixels);

		optimizedImage = SDL_DisplayFormat( loadedImage );
		SDL_FreeSurface( loadedImage );
	}
	else
	{
		printf("Unable to load image (%s)\n", filename);
		gui_clean();
		exit(0);
	}

	return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
	SDL_Rect offset;

	offset.x = x;
	offset.y = y;

	SDL_BlitSurface( source, NULL, destination, &offset );
}

void apply_surface_center( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
	SDL_Rect offset;

	offset.x = (x - (source->w / 2));
	offset.y = (y - (source->h / 2));

	SDL_BlitSurface( source, NULL, destination, &offset );
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

	apply_surface( x, y, text_img, myscreen );

	SDL_FreeSurface( text_img );
}


