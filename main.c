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
const int SCREEN_BPP = 32;

const int pnd_button_1 = 1, pnd_button_2 = 2, pnd_button_L = 4, pnd_button_R = 5, \
	pnd_button_SELECT = 8, pnd_button_START = 9;

static int scroll_count, alpha, alpha_up;

const int category_icon_x[3] = { 110, 210, 310 };
const int category_icon_y = 42;

enum {MOUSE_LEFT = 1, MOUSE_RIGHT, MOUSE_MIDDLE};
int MOUSE_X, MOUSE_Y; 
int mouse_y_pos;

TTF_Font *font;
TTF_Font *font_big;

SDL_Event event;

SDL_Surface *message = NULL;
SDL_Surface *background = NULL;
SDL_Surface *myscreen = NULL;
SDL_Surface *preview = NULL;
SDL_Surface *highlight = NULL;
SDL_Surface *category_icon[3];

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

	font_big = TTF_OpenFont( "data/font.ttf", 20); 
	if( font == NULL )
	{
		printf("TTF_OpenFont failed : %s\n", SDL_GetError());
		return 1; 
	} 

	SDL_ShowCursor(SDL_ENABLE);
}

void gui_load()
{
	int i;
	background = load_image( "data/backg.bmp" );
	highlight = load_image_alpha( "data/highlight.bmp" );
	
	category_icon[EMULATORS] = load_image( "data/emulators_icon.png" );
	category_icon[GAMES] = load_image( "data/games_icon.png" );
	category_icon[APPLICATIONS] = load_image( "data/applications_icon.png" );
}

void gui_quit()
{
	SDL_FreeSurface( background );
	TTF_CloseFont( font ); 
	TTF_CloseFont( font_big );
	TTF_Quit(); 
	SDL_Quit();
				
	int i;
	for(i=0; i<3; i++)
	{
		SDL_FreeSurface( category_icon[i] );
		free(applications[i]);
	}
	exit(0);
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

	int i = list_start[category];

	char tmpStr[256];

	while (i < (list_start[category]+7)) 
	{
		if (i < list_num[category])
		{
			memset(tmpStr, 0, 256);
			strncpy(tmpStr, applications[category]->name[i], 40);

			if (i == list_curpos[category])
			{
				if( alpha < 170 && alpha_up == 1)
				{
					alpha += 2;
					if(alpha == 168) alpha_up = 0;
						else alpha_up = 1;
				}
				else if( alpha > 100 && alpha_up != 1)
				{
					alpha -= 2;
					if(alpha == 102) alpha_up = 1;
						else alpha_up = 0;
				}
				message = TTF_RenderUTF8_Blended( font_big, tmpStr, GREEN );
			}
			else
			{
				message = TTF_RenderUTF8_Blended( font, tmpStr, WHITE );
			}
			apply_surface( 96, ((i-list_start[category])+2)*50, message, myscreen );
			SDL_FreeSurface( message );

			if (access (applications[category]->icon[i], W_OK) == 0)
			{
				preview = load_image( applications[category]->icon[i] );
				apply_surface( 50, ((i-list_start[category])+2)*50, preview, myscreen );
				SDL_FreeSurface( preview );
			}
		}
		i++;
	}

	SDL_SetAlpha( highlight, SDL_SRCALPHA | SDL_RLEACCEL, alpha );
	apply_surface_center( category_icon_x[category], category_icon_y, highlight, myscreen );

	apply_surface_center( category_icon_x[EMULATORS], category_icon_y, category_icon[EMULATORS], myscreen);
	apply_surface_center( category_icon_x[GAMES], category_icon_y, category_icon[GAMES], myscreen);
	apply_surface_center( category_icon_x[APPLICATIONS], category_icon_y, category_icon[APPLICATIONS], myscreen);

	gui_scroll_text(455, applications[category]->description[list_curpos[category]]);

	SDL_Flip(myscreen);
}

int get_mouse_click(int button)
{
	SDL_PumpEvents();
	switch(button)
	{
		case MOUSE_LEFT:
			if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
			return 1;
		break;

		case MOUSE_MIDDLE:
			if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(2))
			return 1;
		break;

		case MOUSE_RIGHT:
			if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3))
			return 1;
		break;
	}
	return 0;
}

void get_mouse_loc()
{
	SDL_PumpEvents();
	SDL_GetMouseState(&MOUSE_X, &MOUSE_Y);
}

void set_mouse_loc(int x, int y)
{
	SDL_WarpMouse(x, y);
}

int mouse_is_over(int x1, int x2, int y1, int y2)
{
	if(MOUSE_X < x1) return 0;
	if(MOUSE_X > x2) return 0;
	if(MOUSE_Y < y1) return 0;
	if(MOUSE_Y > y2) return 0;
	return 1;
}

int mouse_is_over_center(int x, int y, int w, int h)
{
	if(MOUSE_X > (x + (w / 2))) return 0;
	if(MOUSE_X < (x - (w / 2))) return 0;
	if(MOUSE_Y > (y + (h / 2))) return 0;
	if(MOUSE_Y < (y - (h / 2))) return 0;
	return 1;
}

void handle_mouse()
{
	get_mouse_loc();

	if(get_mouse_click(MOUSE_LEFT))
	{		
		int i;
		for(i = 0; i < 3; i++ )
		{
			if(mouse_is_over_center(category_icon_x[i], 42, category_icon[i]->w, category_icon[i]->h))
			{
				category = i;
				alpha = 0;
				alpha_up = 1;
				scroll_count = 800;
			}
		}

		if(mouse_is_over(96, 392, 100, 440))
		{
			if(MOUSE_Y > (mouse_y_pos + 50))
			{
				if (list_curpos[category] > 0) 
				{
					list_curpos[category]--;
					if (list_curpos[category] < list_start[category]) { list_start[category] = list_curpos[category]; }

					mouse_y_pos += 50;
				}			
			}
			if(MOUSE_Y < (mouse_y_pos - 50))
			{
				if (list_curpos[category] < (list_num[category]-1)) 
				{
					list_curpos[category]++;
					if (list_curpos[category] >= (list_start[category]+7)) { list_start[category]++; }

					mouse_y_pos -= 50;
				}					
			}
			scroll_count = 800;
		}
	}
	else mouse_y_pos = MOUSE_Y;
}

int main(char *argc, char *argv[])
{

	int gui_done = 0;
	scroll_count = 800;
	category = EMULATORS;
	alpha_up = 1;
	alpha = 0;
	mouse_y_pos = 40;

	gui_init_sdl();
	gui_load();

	pnd_app_get_list();

	while(!gui_done)
	{

		handle_mouse();

		while( SDL_PollEvent( &event ) )
		{	
			switch(event.type)
			{
				case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_UP)
				{
					if (list_curpos[category] > 0) 
					{
						list_curpos[category]--;
						if (list_curpos[category] < list_start[category]) { list_start[category] = list_curpos[category]; }
					}
				}
				else if(event.key.keysym.sym == SDLK_DOWN)
				{
					if (list_curpos[category] < (list_num[category]-1)) 
					{
						list_curpos[category]++;
						if (list_curpos[category] >= (list_start[category]+7)) { list_start[category]++; }
					}
				}
				else if(event.key.keysym.sym == SDLK_LEFT)
				{
					if(category > EMULATORS) category--;
						else category = APPLICATIONS;
						alpha = 0; alpha_up = 1;
				}
				else if(event.key.keysym.sym == SDLK_RIGHT)
				{
					if(category < APPLICATIONS) category++;
						else category = EMULATORS;
						alpha = 0; alpha_up = 1;
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
							if (list_curpos[category] > 0) 
							{
								list_curpos[category]--;
								if (list_curpos[category] < list_start[category]) { list_start[category] = list_curpos[category]; }
								gui_draw();
							}
						}
						else if(event.jaxis.value > 0) // PAD DOWN
						{
							if (list_curpos[category] < (list_num[category]-1)) 
							{
								list_curpos[category]++;
								if (list_curpos[category] >= (list_start[category]+7)) { list_start[category]++; }
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
					gui_quit();
				}
				break;
			}
		}
		gui_draw();
	}
	return 0;
}

