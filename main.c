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
#include <SDL_framerate.h>
#include <SDL_rotozoom.h>

#include "main.h"
#include "get_apps.h"

int gui_draw_favorites = 0;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 16;

const int pnd_button_1 = 1, pnd_button_2 = 2, pnd_button_L = 4, pnd_button_R = 5, \
	pnd_button_SELECT = 8, pnd_button_START = 9;

static int scroll_count, alpha, alpha_up;

const int category_icon_x[4] = { 80, 170, 260, 350 };
const int category_icon_y = 42;

enum {MOUSE_LEFT = 1, MOUSE_RIGHT, MOUSE_MIDDLE};
int MOUSE_X, MOUSE_Y; 
int mouse_y_pos;

char fpsText[16];

int page[3] = {0, 0, 0};

TTF_Font *font;
TTF_Font *font_big;

SDL_Event event;

FPSmanager sixteen;

SDL_Surface *message = NULL;
SDL_Surface *background = NULL;
SDL_Surface *myscreen = NULL;
SDL_Surface *preview[3][256];
SDL_Surface *highlight = NULL;
SDL_Surface *category_icon[4];

#define UP 0
#define DOWN 1
SDL_Surface *arrow[2];

static int preview_x[3][256];
static int preview_y[3][256];

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
	
#ifndef PANDORA
	SDL_ShowCursor(SDL_ENABLE);
#else
	SDL_ShowCursor(SDL_DISABLE);
#endif
}

void gui_load()
{
	int i, j;

	background = load_image( "data/backg.bmp" );
	highlight = load_image_alpha( "data/highlight.bmp" );
	
	category_icon[FAVORITES] = load_image_alpha( "data/favorites_icon.bmp" );
	category_icon[EMULATORS] = load_image( "data/emulators_icon.png" );
	category_icon[GAMES] = load_image( "data/games_icon.png" );
	category_icon[APPLICATIONS] = load_image( "data/applications_icon.png" );

	arrow[UP] =  load_image_alpha( "data/arrowup.bmp" );
	arrow[DOWN] = load_image_alpha( "data/arrowdown.bmp" );

	for(i = EMULATORS; i < APPLICATIONS+1; i++)
	{
		for(j = 0; j < list_num[i]; j++)
		{
#ifdef PANDORA
			SDL_Surface *to_resize = load_image( applications[i]->icon[j]);
			preview[i][j] = shrinkSurface(to_resize, 2, 2);
			SDL_FreeSurface ( to_resize );
#else
			preview[i][j] = load_image( applications[i]->icon[j]);
#endif
		}
	}
}

void gui_clean()
{
	int i, j;

	for(i = 0; i < 4; i++) SDL_FreeSurface( category_icon[i] );

	for(i = EMULATORS; i < APPLICATIONS+1; i++)
	{
		for(j = 0; j < list_num[i]; j++)
		{
			preview[i][j] = load_image( applications[i]->icon[j] );
		}
	}

	SDL_FreeSurface( arrow[UP] );
	SDL_FreeSurface( arrow[DOWN] );
	SDL_FreeSurface( highlight );
	SDL_FreeSurface( background );
	TTF_CloseFont( font ); 
	TTF_CloseFont( font_big );
	TTF_Quit(); 
	SDL_Quit();
}

void gui_app_exec(int n)
{
	pid_t childpid;
	int status;

	gui_clean();

	printf("exec = %s\n", applications[category]->exec_path[n]);

	if ((childpid = fork()) == -1)
	{
		printf("Error in the fork");
	}
	else if (childpid == 0)
	{
   		if (execl("./pnd_run.sh", applications[category]->exec_path[n], "-p", applications[category]->path[n], "-e", applications[category]->exec_name[n], NULL) < 0)
		{
			printf("Exec failed");
		}
	}
	else if (childpid != wait(&status))
	{ 
		printf("A signal occurred before the child exited");
	}

	gui_init_sdl();
	gui_load();
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

	if(gui_draw_favorites)
	{
		message = TTF_RenderUTF8_Blended( font_big, "Add Selected Application to Favorites ?", WHITE );
		apply_surface( 200, 200, message, myscreen );
		SDL_FreeSurface( message );
	}
	else
	{
		if(category != FAVORITES)
		{
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
						apply_surface( 50, ((i-list_start[category])+2)*50, preview[category][i], myscreen );
						preview_x[category][i] = 50;
						preview_y[category][i] = ((i-list_start[category])+2)*50;
					}
				}
				i++;
			}
		}

		if( alpha < 172 && alpha_up == 1)
		{
			alpha += 4;
			if(alpha == 168) alpha_up = 0;
				else alpha_up = 1;
		}
		else if( alpha > 100 && alpha_up != 1)
		{
			alpha -= 4;
			if(alpha == 100) alpha_up = 1;
				else alpha_up = 0;
		}
		SDL_SetAlpha( highlight, SDL_SRCALPHA | SDL_RLEACCEL, alpha );
		apply_surface_center( category_icon_x[category], category_icon_y, highlight, myscreen );

		apply_surface_center( category_icon_x[FAVORITES], category_icon_y, category_icon[FAVORITES], myscreen);
		apply_surface_center( category_icon_x[EMULATORS], category_icon_y, category_icon[EMULATORS], myscreen);
		apply_surface_center( category_icon_x[GAMES], category_icon_y, category_icon[GAMES], myscreen);
		apply_surface_center( category_icon_x[APPLICATIONS], category_icon_y, category_icon[APPLICATIONS], myscreen);

		if(category != FAVORITES)
		{
			apply_surface_center( 360, 200, arrow[UP], myscreen);
			apply_surface_center( 360, 260, arrow[DOWN], myscreen);

			gui_scroll_text(455, applications[category]->description[list_curpos[category]]);
		}
	}

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

int mouse_is_over_surface_center(int x, int y, int w, int h)
{
	if(MOUSE_X > (x + (w / 2))) return 0;
	if(MOUSE_X < (x - (w / 2))) return 0;
	if(MOUSE_Y > (y + (h / 2))) return 0;
	if(MOUSE_Y < (y - (h / 2))) return 0;
	return 1;
}

int mouse_is_over_surface(int x, int y, int w, int h)
{
    if(MOUSE_X < x) return 0;
    if(MOUSE_X > (x + w)) return 0;
    if(MOUSE_Y < y) return 0;
    if(MOUSE_Y > (y + h)) return 0;
    return 1;
}

int mouse_hold_x = 0;
int mouse_hold_y = 0;
int reset_ts_pos = 1;
int exec_app = 0;
int exec_app_number = 0;

void handle_mouse()
{

	get_mouse_loc();

	if(get_mouse_click(MOUSE_LEFT))
	{	
		if(reset_ts_pos) { mouse_hold_x = MOUSE_X; mouse_hold_y = MOUSE_Y; reset_ts_pos = 0; }

		int i;
		for(i = 0; i < 4; i++ )
		{
			if(mouse_is_over_surface_center(category_icon_x[i], 42, category_icon[i]->w, category_icon[i]->h))
			{
				category = i;
				alpha = 0;
				alpha_up = 1;
				scroll_count = 800;
			}
		}

		if(category != FAVORITES)
		{
			if(mouse_is_over(96, 392, 100, 440))
			{
				if(MOUSE_Y > (mouse_hold_y + 50))
				{
					if (list_curpos[category] > 0) 
					{
						list_curpos[category]--;
						if (list_curpos[category] < list_start[category]) { list_start[category] = list_curpos[category]; }

						mouse_hold_y += 50;
					}			
				}
				else if(MOUSE_Y < (mouse_hold_y - 50))
				{
					if (list_curpos[category] < (list_num[category]-1)) 
					{
						list_curpos[category]++;
						if (list_curpos[category] >= (list_start[category]+7)) { list_start[category]++; }

						mouse_hold_y -= 50;
					}					
				}
				scroll_count = 800;
				SDL_Delay(60);
			}

			for(i = 0; i < 7; i++ )
			{
				if(preview[category][i + (page[category] * 7)] != NULL)
				{
					if(mouse_is_over_surface(preview_x[category][i + (page[category] * 7)], \
						preview_y[category][i + (page[category] * 7)], \
							200, \
								preview[category][i + (page[category] * 7)]->h))
					{
						if(list_curpos[category] == i + (page[category] * 7))
						{
							if((mouse_hold_x + 60) <= MOUSE_X) { printf("selection screen called\n"); gui_draw_favorites = 1; }
								else { printf("exec app called\n"); exec_app = 1; exec_app_number = (i + (page[category] * 7)); }
							
							SDL_Delay(120);
							scroll_count = 800;
							break;
						}
						else
						{
							list_curpos[category] = i + (page[category] * 7) ;
							SDL_Delay(120);
							scroll_count = 800;
							break; 
						}
					}
				}
			}

			if(mouse_is_over_surface_center(360, 200, arrow[UP]->w, arrow[UP]->h))
			{

				if (list_curpos[category] > 6)
				{
					page[category] -= 1;
					list_curpos[category] = page[category] * 7;
					list_start[category] = page[category] * 7;
					scroll_count = 800;
					SDL_Delay(120);
				}
				else
				{
					list_curpos[category] = 0;
					list_start[category] = 0;
					scroll_count = 800;
					SDL_Delay(120);
				}			
			}
			if(mouse_is_over_surface_center(360, 260, arrow[DOWN]->w, arrow[DOWN]->h))
			{
				if (list_curpos[category] < (list_num[category]-6))
				{
					page[category] += 1;
					list_curpos[category] = page[category] * 7;
					list_start[category] = page[category] * 7;
					scroll_count = 800;
					SDL_Delay(120);
				}
				else
				{
					list_curpos[category] = list_num[category]-1;
					scroll_count = 800;
					SDL_Delay(120);				
				}
			}
		}
	}
	else
	{
		if(exec_app) { gui_app_exec(exec_app_number); exec_app = 0; }
		reset_ts_pos = 1;
	}
}

int main(char *argc, char *argv[])
{

	int gui_done = 0;
	scroll_count = 800;
	category = FAVORITES;
	alpha_up = 1;
	alpha = 0;
	mouse_y_pos = 120;

	pnd_app_get_list();

	gui_init_sdl();
	gui_load();

	SDL_initFramerate( &sixteen );
	SDL_setFramerate( &sixteen, 65 );

	while(!gui_done)
	{
		if(!gui_draw_favorites) handle_mouse();

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
					if(category > FAVORITES) category--;
						else category = APPLICATIONS;
						alpha = 0; alpha_up = 1;
				}
				else if(event.key.keysym.sym == SDLK_RIGHT)
				{
					if(category < APPLICATIONS) category++;
						else category = FAVORITES;
						alpha = 0; alpha_up = 1;
				}
				else if(event.key.keysym.sym == SDLK_RETURN)
				{
					gui_app_exec(list_curpos[category]);
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
					gui_clean();

					int i;
					for(i = EMULATORS; i < APPLICATIONS+1; i++)
					{
						free(applications[i]);
					}
					exit(0);
				}
				break;
			}
		}

		gui_draw();

		SDL_framerateDelay( &sixteen );
	}
	return 0;
}

