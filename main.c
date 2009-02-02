#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "main.h"
#include "get_apps.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 16;

const int pnd_button_1 = 1, pnd_button_2 = 2, pnd_button_L = 4, pnd_button_R = 5, \
	pnd_button_SELECT = 8, pnd_button_START = 9;

TTF_Font *font;
SDL_Event event;

SDL_Surface *message = NULL;
SDL_Surface *background = NULL;
SDL_Surface *myscreen = NULL;
SDL_Surface *preview = NULL;

SDL_Color WHITE = { 255, 255, 255 };
SDL_Color BLUE = { 0, 0, 255 };
SDL_Color RED = { 255, 0, 0 };
SDL_Color GREEN = { 0, 255, 0 };
SDL_Color GRAY = { 175, 175, 175 };


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

int gui_init_sdl()
{
	if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) == -1 )
	{
		fprintf(stderr, "Couldn’t initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	int joystick = 0, x = 0;
	joystick = SDL_NumJoysticks();
	printf("Nombres de joysticks attachés: %d\n", joystick);

	for (x = 0; x < joystick; x++)
	{
		SDL_Joystick * joy = SDL_JoystickOpen(x);
		printf("Joystick %i: %s\n", x, SDL_JoystickName(x));
		printf("Axes: %d\n", SDL_JoystickNumAxes(joy));
		printf("Boutons: %d\n", SDL_JoystickNumButtons(joy));
		printf("Trackballs: %d\n", SDL_JoystickNumBalls(joy));
		printf("Hats: %d\n\n", SDL_JoystickNumHats(joy));
	}

	myscreen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

	if( myscreen == NULL )
	{
	    fprintf(stderr, "Couldn’t initialize VideoMode: %s\n", SDL_GetError());
	    exit(1);
	}

	if( TTF_Init() == -1 )
	{ 
		printf("TTF_Init() failed : %s\n", SDL_GetError());
		return 1;
	} 

	font = TTF_OpenFont( "data/font.ttf", 16); 
	if( font == NULL )
	{
		printf("TTF_OpenFont failed : %s\n", SDL_GetError());
		return 1; 
	} 

	SDL_ShowCursor(SDL_DISABLE);
}

void gui_draw()
{
	apply_surface( 0, 0, background, myscreen );

	int i = app_list_start;

	while (i < (app_list_start+7)) 
	{
		if (i < app_list_num)
		{
			if (i == app_list_curpos)
			{
				message = TTF_RenderText_Solid( font, application->name[i], GREEN );

				/*if (access (application->icon[i], W_OK) == 0)
				{
					preview = load_image( application->icon[i] );
					apply_surface( 590, 260, preview, myscreen );
					SDL_FreeSurface( preview );
				}*/
			}
			else
			{
				message = TTF_RenderText_Solid( font, application->name[i], WHITE );
			}
			apply_surface( 96, ((i-app_list_start)+2)*50, message, myscreen );
			SDL_FreeSurface( message );

			if (access (application->icon[i], W_OK) == 0)
			{
				preview = load_image( application->icon[i] );
				apply_surface( 50, ((i-app_list_start)+2)*50, preview, myscreen );
				SDL_FreeSurface( preview );
			}
		}
		i++;
	}
	SDL_Flip(myscreen);
}

int main(char *argc, char *argv[])
{

	int gui_done = 0;

	gui_init_sdl();

	background = load_image( "data/backg.bmp" );
	apply_surface( 0, 0, background, myscreen );

	pnd_app_get_list();
	gui_draw();

	while(!gui_done)
	{
		SDL_Delay(1);

		while( SDL_PollEvent( &event ) )
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_UP)
				{
					if (app_list_curpos > 0) 
					{
						app_list_curpos--;
						if (app_list_curpos < app_list_start) { app_list_start = app_list_curpos; }
						gui_draw();
					}
				}
				else if(event.key.keysym.sym == SDLK_DOWN)
				{
					if (app_list_curpos < (app_list_num-1)) 
					{
						app_list_curpos++;
						if (app_list_curpos >= (app_list_start+7)) { app_list_start++; }
						gui_draw();
					}
				}
				break;

				case SDL_JOYAXISMOTION:
				if (( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ))
				{
					if( event.jaxis.axis == 0)
					{
						if(event.jaxis.value < 0) // PAD LEFT
						{

						}
						else if(event.jaxis.value > 0) // PAD RIGHT
						{
	
						}

					}

					else if( event.jaxis.axis == 1)
					{
						if(event.jaxis.value < 0) // PAD UP
						{
							if (app_list_curpos > 0) 
							{
								app_list_curpos--;
								if (app_list_curpos < app_list_start) { app_list_start = app_list_curpos; }
								gui_draw();
							}
						}
						else if(event.jaxis.value > 0) // PAD DOWN
						{
							if (app_list_curpos < (app_list_num-1)) 
							{
								app_list_curpos++;
								if (app_list_curpos >= (app_list_start+7)) { app_list_start++; }
								gui_draw();
							}
						}
					}
				}
				break;

				case SDL_JOYBUTTONDOWN:
				if ( event.jbutton.button == pnd_button_L )
				{

				}
				else if ( event.jbutton.button == pnd_button_R )
				{

				}
				else if ( event.jbutton.button == pnd_button_1 )
				{
	
				}
				else if(event.jbutton.button == pnd_button_2)
				{

				}
				break;

				case SDL_QUIT:
				{
					SDL_FreeSurface( background );
					TTF_CloseFont( font ); 
					TTF_Quit(); 
					SDL_Quit();
				
					free(application);
					gui_done = 1;
				}
				break;
			}
		}
	}
	return 0;
}

