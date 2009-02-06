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

static int scroll_count;

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

	SDL_ShowCursor(SDL_ENABLE);
}

void gui_scroll_text(int y, char *text)
{
	int i = strlen(text) * 12;

	if(scroll_count < -i) scroll_count = 800;

	message = TTF_RenderUTF8_Solid( font, text, WHITE );
	SDL_Surface *new_message = SDL_DisplayFormat( message );
	SDL_FreeSurface( message );

	apply_surface( scroll_count, y, new_message, myscreen );
	SDL_FreeSurface( new_message );

	scroll_count -= 1;
}

void gui_draw()
{
	apply_surface( 0, 0, background, myscreen );

	int i = list_start[section];

	char tmpStr[256];

	while (i < (list_start[section]+7)) 
	{
		if (i < list_num[section])
		{
			memset(tmpStr, 0, 256);
			strncpy(tmpStr, applications[section]->name[i], 40);

			if (i == list_curpos[section])
			{
				message = TTF_RenderText_Solid( font, tmpStr, GREEN );
			}
			else
			{
				message = TTF_RenderText_Solid( font, tmpStr, WHITE );
			}
			apply_surface( 96, ((i-list_start[section])+2)*50, message, myscreen );
			SDL_FreeSurface( message );

			if (access (applications[section]->icon[i], W_OK) == 0)
			{
				preview = load_image( applications[section]->icon[i] );
				apply_surface( 50, ((i-list_start[section])+2)*50, preview, myscreen );
				SDL_FreeSurface( preview );
			}
		}
		i++;
	}

	gui_scroll_text(450, applications[section]->description[list_curpos[section]]);

	SDL_Flip(myscreen);
}

int main(char *argc, char *argv[])
{

	int gui_done = 0;
	scroll_count = 800;
	section = EMULATORS;

	gui_init_sdl();

	background = load_image( "data/backg.bmp" );
	apply_surface( 0, 0, background, myscreen );

	pnd_app_get_list();

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
					if (list_curpos[section] > 0) 
					{
						list_curpos[section]--;
						if (list_curpos[section] < list_start[section]) { list_start[section] = list_curpos[section]; }
						gui_draw();
					}
				}
				else if(event.key.keysym.sym == SDLK_DOWN)
				{
					if (list_curpos[section] < (list_num[section]-1)) 
					{
						list_curpos[section]++;
						if (list_curpos[section] >= (list_start[section]+7)) { list_start[section]++; }
						gui_draw();
					}
				}
				else if(event.key.keysym.sym == SDLK_LEFT)
				{
					if(section > EMULATORS) section--;
						else section = APPLICATIONS;
				}
				else if(event.key.keysym.sym == SDLK_RIGHT)
				{
					if(section < APPLICATIONS) section++;
						else section = EMULATORS;
				}
				scroll_count = 800;
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
							if (list_curpos[section] > 0) 
							{
								list_curpos[section]--;
								if (list_curpos[section] < list_start[section]) { list_start[section] = list_curpos[section]; }
								gui_draw();
							}
						}
						else if(event.jaxis.value > 0) // PAD DOWN
						{
							if (list_curpos[section] < (list_num[section]-1)) 
							{
								list_curpos[section]++;
								if (list_curpos[section] >= (list_start[section]+7)) { list_start[section]++; }
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
				
					int i;
					for(i=0; i<3; i++)
						free(applications[i]);
					exit(0);
				}
				break;
			}
		}
		gui_draw();
	}
	return 0;
}

