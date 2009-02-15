#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>

#include "graphics.h"
#include "main.h"
#include "get_apps.h"
#include "mouse.h"

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

	font = NULL; font_big = NULL; message = NULL; background = NULL; myscreen = NULL; 
	highlight = NULL; confirm_box = NULL;

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

	return 0;
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

	arrow[LEFT] =  load_image_alpha( "data/arrowleft.bmp" );
	arrow[RIGHT] = load_image_alpha( "data/arrowright.bmp" );

	confirm_box = load_image_alpha("data/confirm_box.bmp");

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

	for(i = 0; i < 4; i++) 
		if( category_icon[i] != NULL) SDL_FreeSurface( category_icon[i] );

	for(i = EMULATORS; i < APPLICATIONS+1; i++)
	{
		for(j = 0; j < list_num[i]; j++)
		{
			if( preview[i][j] != NULL) SDL_FreeSurface(preview[i][j]); 
		}
	}

	if( arrow[LEFT] != NULL) SDL_FreeSurface( arrow[LEFT] );
	if( arrow[RIGHT] != NULL) SDL_FreeSurface( arrow[RIGHT] );
	if( highlight != NULL) SDL_FreeSurface( highlight );
	if( confirm_box != NULL) SDL_FreeSurface( confirm_box );
	if( background != NULL) SDL_FreeSurface( background );
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

	draw_text(text, SMALL, NORMAL, WHITE, scroll_count, y);

	scroll_count -= 1;
}

void gui_draw()
{
	apply_surface( 0, 0, background, myscreen );

	if(category != FAVORITES)
	{
		int i = list_start[category];

		char tmpStr[256];

		while (i < (list_start[category]+MAXLIST)) 
		{
			if (i < list_num[category])
			{
				memset(tmpStr, 0, 256);
				strncpy(tmpStr, applications[category]->name[i], 40);

				if (i == list_curpos[category])
				{
					draw_text(tmpStr, BIG, BLEND, GREEN, 96, ((i-list_start[category])+2)*50);
				}
				else
				{
					draw_text(tmpStr, SMALL, BLEND, WHITE, 96, ((i-list_start[category])+2)*50);
				}

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
		apply_surface_center( 65, 425, arrow[LEFT], myscreen);
		apply_surface_center( 365, 425, arrow[RIGHT], myscreen);

		gui_scroll_text(455, applications[category]->description[list_curpos[category]]);
	}
}

int gui_confirm_box(char *msg)
{
	int done = 0;

	while(!done)
	{
		gui_draw();

		apply_surface_center(400, 240, confirm_box, myscreen);

		draw_text(msg, SMALL, BLEND, WHITE, 400, 240);

		get_mouse_loc();

		if(get_mouse_click(MOUSE_LEFT))
		{
			if(reset_ts_pos) { mouse_hold_x = MOUSE_X; mouse_hold_y = MOUSE_Y; reset_ts_pos = 0; }

			if(mouse_is_over(300, 378, 285, 322)) // OK
			{
				return 1;
			}
			else if(mouse_is_over(428, 508, 285, 322)) // Cancel
			{
				return 0;
			}
		}
		else reset_ts_pos = 1;

		SDL_Flip(myscreen);
		SDL_framerateDelay( &sixteen );
	}
	return 0;
}

void handle_mouse()
{
	int i;

	get_mouse_loc();

	if(get_mouse_click(MOUSE_LEFT))
	{	
		if(reset_ts_pos) { mouse_hold_x = MOUSE_X; mouse_hold_y = MOUSE_Y; reset_ts_pos = 0; }

		printf("X:%i Y:%i\n", MOUSE_X, MOUSE_Y);

		if(category != FAVORITES)
		{
			for(i = 0; i < MAXLIST; i++ )
			{
				printf("i=%i\ncategory=%i\npage=%i\n", i, category, page[category]);

				if(preview[category][i + (page[category] * MAXLIST)] != NULL)
				{
					if(mouse_is_over_surface(preview_x[category][i + (page[category] * MAXLIST)], \
						preview_y[category][i + (page[category] * MAXLIST)], \
							200, \
								preview[category][i + (page[category] * MAXLIST)]->h))
					{
						if(list_curpos[category] == i + (page[category] * MAXLIST))
						{
							app_number = (i + (page[category] * MAXLIST));

							hold_count++;

							if(hold_count == 8)
							{
								if(gui_confirm_box("Add to favorites ?"))
								{
									printf("Added to favorites : %s\n", applications[category]->exec_path[app_number]);
									add_to_fav = 0;
									hold_count = 0;
									exec_app = 0;
									SDL_Delay(120);
									break;
								}
								else
								{
									printf("Cancel\n");
									add_to_fav = 0;
									hold_count = 0;
									exec_app = 0;
									SDL_Delay(120);
									break;
								}
							}
							else exec_app = 1;
	
							SDL_Delay(120);
							scroll_count = 800;
							break;
						}
						else
						{
							list_curpos[category] = i + (page[category] * MAXLIST) ;
							SDL_Delay(120);
							scroll_count = 800;
							break; 
						}
					}
				}
			}

			if(mouse_is_over(130, 300, 400, 450))
			{
				if(MOUSE_X > (mouse_hold_x + 100))
				{
					if (list_curpos[category] < (list_num[category]-(MAXLIST-1)))
					{
						page[category] += 1;
						list_curpos[category] = page[category] * MAXLIST;
						list_start[category] = page[category] * MAXLIST;
						mouse_hold_x += 100;
					}		
				}
				else if(MOUSE_X < (mouse_hold_x - 100))
				{

					if (list_curpos[category] > MAXLIST-1)
					{
						page[category] -= 1;
						list_curpos[category] = page[category] * MAXLIST;
						list_start[category] = page[category] * MAXLIST;
						mouse_hold_x -= 100;
					}
					else
					{
						list_curpos[category] = 0;
						list_start[category] = 0;
						mouse_hold_x -= 100;
					}					
				}
				scroll_count = 800;
				SDL_Delay(120);
			}

			if(mouse_is_over_surface_center(65, 425, arrow[LEFT]->w, arrow[LEFT]->h))
			{

				if (list_curpos[category] > MAXLIST-1)
				{
					page[category] -= 1;
					list_curpos[category] = page[category] * MAXLIST;
					list_start[category] = page[category] * MAXLIST;
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
			if(mouse_is_over_surface_center(365, 425, arrow[RIGHT]->w, arrow[RIGHT]->h))
			{
				if (list_curpos[category] < (list_num[category]-(MAXLIST-1)))
				{
					page[category] += 1;
					list_curpos[category] = page[category] * MAXLIST;
					list_start[category] = page[category] * MAXLIST;
					scroll_count = 800;
					SDL_Delay(120);
				}
			}
		}

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
	}
	else
	{
		if(exec_app)
		{
			if(gui_confirm_box("Launch Application ?"))
			{
				gui_app_exec(app_number); exec_app = 0; hold_count = 0;
			}
			else { exec_app = 0; hold_count = 0; }
		}
		reset_ts_pos = 1;
	}
}

int main(int argc, char *argv[])
{

	int gui_done = 0;
	scroll_count = 800;
	category = FAVORITES;
	alpha_up = 1;
	alpha = 0;
	mouse_y_pos = 120;
	exec_app = 0;

	pnd_app_get_list();

	gui_init_sdl();
	gui_load();

	SDL_initFramerate( &sixteen );
	SDL_setFramerate( &sixteen, 65 );

	while(!gui_done)
	{
		handle_mouse();

		while( SDL_PollEvent( &event ) )
		{	
			switch(event.type)
			{
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
		SDL_Flip(myscreen);

		SDL_framerateDelay( &sixteen );
	}
	return 0;
}

