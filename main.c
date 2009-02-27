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
#include "fav_config.h"
#include "gui_config.h"

#include "pnd_apps.h"

#include "sprig.h"

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

	myscreen = NULL;
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

	font = NULL;
	font = TTF_OpenFont( "data/font.ttf", 16); 
	if( font == NULL )
	{
		printf("TTF_OpenFont failed : %s\n", SDL_GetError());
		return 1; 
	} 

	font_big = NULL;
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

void gui_load_fav()
{
	int i;
	
	for( i = 0; i < FAV_MAX; i++)
	{
		if(fav->enabled[i])
		{
			fav_preview[i] = NULL;
			fav_preview[i] = load_image_rgba( fav->icon[i] );
		}
	}

}

void gui_clean_fav()
{
	int i;
	
	for( i = 0; i < FAV_MAX; i++)
	{
		if(fav->enabled[i])
		{
			if(fav_preview[i] != NULL ) SDL_FreeSurface( fav_preview[i] );
		}
	}

}

void gui_load()
{
	int i, j;

	background = NULL;
	background = load_image( "data/backg.png" );

	fav_background = NULL;
	fav_background = load_image( "data/fav_backg.png" );

	app_background = NULL;
	app_background = load_image( "data/app_backg.png" );

	highlight = NULL;
	highlight = load_image_alpha( "data/highlight.bmp" );

	app_highlight = NULL;
	app_highlight = load_image_rgba( "data/app_highlight.bmp" );
	
	category_icon[FAVORITES] = NULL;
	category_icon[FAVORITES] = load_image_rgba( "data/favorites_icon.bmp" );

	category_icon[EMULATORS] = NULL;
	category_icon[EMULATORS] = load_image_rgba( "data/emulators_icon.png" );

	category_icon[GAMES] = NULL;
	category_icon[GAMES] = load_image_rgba( "data/games_icon.png" );

	category_icon[APPLICATIONS] = NULL;
	category_icon[APPLICATIONS] = load_image_rgba( "data/applications_icon.png" );

	arrow[LEFT] = NULL;
	arrow[LEFT] =  load_image_rgba( "data/arrowleft.bmp" );

	arrow[RIGHT] = NULL;
	arrow[RIGHT] = load_image_rgba( "data/arrowright.bmp" );


	confirm_box = NULL;
	confirm_box = load_image_rgba("data/confirm_box.bmp");

	no_icon = NULL;
	no_icon = load_image_rgba("data/no_icon.png");	

	for(i = EMULATORS; i < APPLICATIONS+1; i++)
	{
		for(j = 0; j < list_num[i]; j++)
		{
			SDL_Surface *to_resize = load_image_rgba( applications[i]->icon[j] );
			
			preview[i][j] = NULL;
			preview[i][j] = SPG_ScaleAA(to_resize, 0.5f, 0.5f);

			SDL_FreeSurface ( to_resize );
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

	if( arrow[LEFT] != NULL ) SDL_FreeSurface( arrow[LEFT] );
	if( arrow[RIGHT] != NULL ) SDL_FreeSurface( arrow[RIGHT] );
	if( highlight != NULL ) SDL_FreeSurface( highlight );

	if( app_highlight != NULL ) SDL_FreeSurface( app_highlight );

	if( confirm_box != NULL) SDL_FreeSurface( confirm_box );
	if( background != NULL) SDL_FreeSurface( background );
	if( fav_background != NULL) SDL_FreeSurface( fav_background );
	if( app_background != NULL) SDL_FreeSurface( app_background );
	if( no_icon != NULL ) SDL_FreeSurface( no_icon );
	if( tmp_preview != NULL ) SDL_FreeSurface ( tmp_preview );
	TTF_CloseFont( font ); 
	TTF_CloseFont( font_big );
	TTF_Quit(); 
	SDL_Quit();
}

void gui_app_exec(int n)
{
	gui_clean();
	
	pnd_apps_exec ( pndrun, applications[category]->fullpath[n], \
		applications[category]->id[n], applications[category]->exec_name[n], \
			NULL, 500 );

	gui_init_sdl();
	gui_load();
}

void gui_app_exec_fav(int n)
{
	gui_clean();

	pnd_apps_exec ( pndrun, fav->fullpath[n], \
		NULL, fav->exec_name[n], \
			NULL, 500 );

	gui_init_sdl();
	gui_load();
}

int gui_confirm_box(char *msg)
{
	int done = 0;

	if( confirm_box != NULL )
		SPG_DrawCenter( confirm_box, myscreen, 400, 240 );

	draw_text_center(msg, SMALL, BLEND, WHITE, 400, 220);
	SDL_Flip(myscreen);

	while(!done)
	{
		get_mouse_loc();

		if(get_mouse_click(MOUSE_LEFT))
		{
			if(reset_ts_pos) { mouse_hold_x = MOUSE_X; mouse_hold_y = MOUSE_Y; reset_ts_pos = 0; }

			if(mouse_is_over(280, 370, 270, 310)) // OK
			{
				return 1;
			}
			else if(mouse_is_over(428, 517, 270, 310)) // Cancel
			{
				return 0;
			}
		}
		else reset_ts_pos = 1;

		SDL_framerateDelay( &sixteen );
	}
	return 0;
}

void gui_draw()
{
	if(category == FAVORITES) {
		if( fav_background != NULL )
			SPG_Draw( fav_background, myscreen, 0, 0 );
	}

	if(category != FAVORITES)
	{
		if( background != NULL )
			SPG_Draw( background, myscreen, 0, 0 );

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
					
					SPG_Draw( app_highlight, myscreen, gui->applications_box_x - 54, (((i-list_start[category])+gui->applications_box_y)*50) - 7 );
					draw_text(tmpStr, BIG, BLEND, GREEN, gui->applications_box_x, ((i-list_start[category])+gui->applications_box_y)*50);
				}
				else
				{
					draw_text(tmpStr, SMALL, BLEND, WHITE, gui->applications_box_x, ((i-list_start[category])+gui->applications_box_y)*50);
				}

				if (access (applications[category]->icon[i], W_OK) == 0)
				{
					if( preview[category][i] != NULL )
						SPG_Draw( preview[category][i], myscreen, gui->applications_box_x - 46, ((i-list_start[category])+gui->applications_box_y)*50 );
					preview_x[category][i] = gui->applications_box_x - 46;
					preview_y[category][i] = ((i-list_start[category])+gui->applications_box_y)*50;
				}
			}
			i++;
		}

		if( tmp_preview != NULL ) SPG_DrawCenter( tmp_preview, myscreen, 600, 200 );
	}
	else if( category == FAVORITES )
	{
		int i, x = 60, y = 120;

		for( i = 0; i < FAV_MAX; i++)
		{
			if( !fav->enabled[i] ) break;

			if (( i == 7 ) | ( i == 14 )) { y += 100; x = 60; }

			if( fav_preview[i] != NULL )
			{
				SPG_Draw( fav_preview[i], myscreen , x, y);
			}
			else
			{
				if (access (fav->fullpath[i], W_OK) == 0) 
				{
					SPG_Draw( no_icon, myscreen , x, y);
				}
				else
				{
					gui_confirm_box("Application do not exist");
					cfg_fav_del( i );
				}
			}

			fav_preview_x[i] = x;
			fav_preview_y[i] = y;

			x += 100;
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
	
	//SPG_SetSurfaceAlpha(highlight, 0xFF);

	if ( SDL_SetAlpha( highlight, SDL_SRCALPHA | SDL_RLEACCEL, alpha ) < 0 )
		printf ("Erreur lors de la functon SDL_SetAlpha");

	if( highlight != NULL )
		SPG_DrawCenter( highlight, myscreen, category_icon_x[category], category_icon_y  );

	if( category_icon[FAVORITES] != NULL )
		SPG_DrawCenter( category_icon[FAVORITES], myscreen, category_icon_x[FAVORITES], category_icon_y );

	if( category_icon[EMULATORS] != NULL )
		SPG_DrawCenter( category_icon[EMULATORS], myscreen, category_icon_x[EMULATORS], category_icon_y );

	if( category_icon[GAMES] != NULL )
		SPG_DrawCenter( category_icon[GAMES], myscreen, category_icon_x[GAMES], category_icon_y );

	if( category_icon[APPLICATIONS] != NULL )
		SPG_DrawCenter( category_icon[APPLICATIONS], myscreen, category_icon_x[APPLICATIONS], category_icon_y );

	if(category != FAVORITES)
	{
		if( arrow[LEFT] != NULL )
			SPG_DrawCenter( arrow[LEFT], myscreen, gui->arrow_left_x, gui->arrow_left_y );

		if( arrow[RIGHT] != NULL )
			SPG_DrawCenter( arrow[RIGHT], myscreen, gui->arrow_right_x, gui->arrow_right_y );

		draw_text_scroll(applications[category]->description[list_curpos[category]], SMALL, NORMAL, WHITE, 440, 320, 315);
	}
}

void gui_draw_application_box(int item)
{
	int done = 0;
	SDL_Surface *tmp_pic1 = NULL;
	SDL_Surface *tmp_icon = NULL;

	SDL_Surface *myscreen_copy = SPG_CopySurface(myscreen);

	tmp_icon = load_image_rgba( applications[category]->icon[item]);

	SDL_Surface *to_resize = load_image_rgba( applications[category]->preview_pic1[item]);

	if(to_resize != NULL)
	{
		tmp_pic1 = SPG_ScaleAA(to_resize, 0.6f, 0.6f);
		SDL_FreeSurface ( to_resize );
	}

	float x = 1.00f;
	int i = 0;

	while(!done)
	{
		if( app_background != NULL )
			SPG_Draw(app_background, myscreen, 0, 0);

		if(tmp_icon != NULL)
			SPG_Draw(tmp_icon, myscreen, 50, 40);

		if(tmp_pic1 != NULL)
			SPG_Draw(tmp_pic1, myscreen, 50, 140);

		draw_text_scroll(applications[category]->description[list_curpos[category]], SMALL, NORMAL, WHITE, 160, 65, 380);
		
		if( x > 0.00f)
		{
			SDL_Surface *scaled_surf = SPG_Scale(myscreen_copy, x, 1.00f);
			SPG_Draw(scaled_surf, myscreen, 0, 0);
			SDL_FreeSurface ( scaled_surf );

			x -= 0.05f;
			i++;
		}
		else
		{
			get_mouse_loc();

			if(get_mouse_click(MOUSE_LEFT))
			{
				if(reset_ts_pos) { mouse_hold_x = MOUSE_X; mouse_hold_y = MOUSE_Y; reset_ts_pos = 0; }

				if(mouse_is_over(595, 755, 170, 240)) // launch
				{
					gui_app_exec(item);
					done = 1;
				}
				else if(mouse_is_over(595, 755, 270, 340)) // add to fav
				{
					int j;
					for( j = 0; j < FAV_MAX; j++ )
					{
						if(fav->enabled[FAV_MAX-1])
						{
							gui_confirm_box("Favorites are full");
							break;
						}
						else if(!fav->enabled[j])
						{
							cfg_fav_add(j, applications[category]->name[item], applications[category]->fullpath[item], applications[category]->exec_name[item], applications[category]->icon[item], applications[category]->description[item]);
							break;
						}
					}
					done = 1;
				}
				else if(mouse_is_over(595, 755, 370, 440)) // Cancel
				{
					done = 1;
				}
			}
			else reset_ts_pos = 1;							
		}

		SDL_Flip(myscreen);
		SDL_framerateDelay( &sixteen );

	}

	done = 0;
	x = 0.00f;
	while(!done)
	{
		if(app_background != NULL)
			SPG_Draw(app_background, myscreen, 0, 0);

		if(tmp_icon != NULL)
			SPG_Draw(tmp_icon, myscreen, 50, 40);

		if(tmp_pic1 != NULL)
			SPG_Draw(tmp_pic1, myscreen, 50, 140);

		draw_text_scroll(applications[category]->description[list_curpos[category]], SMALL, NORMAL, WHITE, 160, 65, 380);
		
		
		SDL_Surface *scaled_surf = SPG_Scale(myscreen_copy, x, 1.00f);
		SPG_Draw(scaled_surf, myscreen, 0, 0);
		SDL_FreeSurface ( scaled_surf );

		x += 0.05f;
		
		if( x > 1.00f) done = 1;

		SDL_Flip(myscreen);
		SDL_framerateDelay( &sixteen );	
	}

	SDL_FreeSurface ( myscreen_copy );
	if(tmp_icon != NULL) SDL_FreeSurface ( tmp_icon );
	if(tmp_pic1 != NULL) SDL_FreeSurface ( tmp_pic1 );
}

void handle_mouse()
{
	int i;

	get_mouse_loc();

	if(get_mouse_click(MOUSE_LEFT))
	{	
		//printf( "X:%i Y:%i\n", MOUSE_X, MOUSE_Y );
		
		if(reset_ts_pos) { mouse_hold_x = MOUSE_X; mouse_hold_y = MOUSE_Y; reset_ts_pos = 0; }

		if(category == FAVORITES)
		{
			if(mouse_is_over(35, 755, 85, 435))
			{
				printf( "X:%i Y:%i\n", MOUSE_X, MOUSE_Y );

				if(MOUSE_X > (mouse_hold_x + 100))
				{
					//if (list_curpos[category] < (list_num[category]-(MAXLIST-1)))
					//{
						fav_page += 1;
					//	list_curpos[category] = page[category] * MAXLIST;
					//	list_start[category] = page[category] * MAXLIST;
						mouse_hold_x += 100;
					//}		
				}
			}

			for(i = 0; i < FAV_MAX; i++ )
			{
				if( !fav->enabled[i] ) break;

				if( mouse_is_over_surface( fav_preview_x[i], fav_preview_y[i], fav_preview[i]->w, fav_preview[i]->h ) )
				{
					SDL_Delay(120);

					hold_count++;

					if(hold_count == 8)
					{
						if(gui_confirm_box("Delete from favorites ?"))
						{
							printf("deleting from fav (%i)\n", i);
							cfg_fav_del( i );
							hold_count = 0;
							exec_app = 0;
							SDL_Delay(120);
							break;
						}
						else { hold_count = 0; exec_app = 0; SDL_Delay(120); break; }
					}
					else { app_number = i; exec_app = 1; break; }
				}
			}
		}

		else if(category != FAVORITES)
		{
			for(i = 0; i < MAXLIST; i++ )
			{
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
								if( !fav->enabled[FAV_MAX-1] )
								{
									if(gui_confirm_box("Add to favorites ?"))
									{
										printf("Added to favorites : %s\n", applications[category]->fullpath[app_number]);
										int j;
										for( j = 0; j < FAV_MAX; j++ )
										{
											if(!fav->enabled[j])
											{
												cfg_fav_add(j, applications[category]->name[app_number], applications[category]->fullpath[app_number], applications[category]->exec_name[app_number], applications[category]->icon[app_number], applications[category]->description[app_number]);
												break;
											}
										}
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
								else
								{
									gui_confirm_box("Favorites are full"); 										add_to_fav = 0;
									hold_count = 0;
									exec_app = 0;
									//SDL_Delay(120);
								} 
							}
							else exec_app = 1;
	
							SDL_Delay(120);
							scroll_count = 750;
							break;
						}
						else
						{
							list_curpos[category] = i + (page[category] * MAXLIST) ;
						
							if( tmp_preview != NULL ) SDL_FreeSurface ( tmp_preview );
							SDL_Surface *to_resize = load_image_rgba( applications[category]->preview_pic1[list_curpos[category]]);
							if(to_resize != NULL)
							{
								tmp_preview = SPG_ScaleAA(to_resize, 0.4f, 0.4f);
								SDL_FreeSurface ( to_resize );
							}
						

							SDL_Delay(120);
							scroll_count = 750;
							break; 
						}
					}
				}
			}

			if(mouse_is_over(130, 300, 400, 450))
			{
				if(MOUSE_X > (mouse_hold_x + 50))
				{
					if (list_curpos[category] < (list_num[category]-(MAXLIST-1)))
					{
						page[category] += 1;
						list_curpos[category] = page[category] * MAXLIST;
						list_start[category] = page[category] * MAXLIST;
						mouse_hold_x += 50;
					}		
				}
				else if(MOUSE_X < (mouse_hold_x - 50))
				{

					if (list_curpos[category] > MAXLIST-1)
					{
						page[category] -= 1;
						list_curpos[category] = page[category] * MAXLIST;
						list_start[category] = page[category] * MAXLIST;
						mouse_hold_x -= 50;
					}
					else
					{
						list_curpos[category] = 0;
						list_start[category] = 0;
						mouse_hold_x -= 50;
					}					
				}
				scroll_count = 750;
				SDL_Delay(120);
			}

			if(mouse_is_over_surface_center(gui->arrow_left_x, gui->arrow_left_y, arrow[LEFT]->w, arrow[LEFT]->h))
			{

				if (list_curpos[category] > MAXLIST-1)
				{
					page[category] -= 1;
					list_curpos[category] = page[category] * MAXLIST;
					list_start[category] = page[category] * MAXLIST;
					scroll_count = 750;
					SDL_Delay(120);
				}
				else
				{
					list_curpos[category] = 0;
					list_start[category] = 0;
					scroll_count = 750;
					SDL_Delay(120);
				}			
			}
			if(mouse_is_over_surface_center(gui->arrow_right_x, gui->arrow_right_y, arrow[RIGHT]->w, arrow[RIGHT]->h))
			{
				if (list_curpos[category] < (list_num[category]-1))
				{
					page[category] += 1;
					list_curpos[category] = page[category] * MAXLIST;
					list_start[category] = page[category] * MAXLIST;
					scroll_count = 750;
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
				scroll_count = 750;
			}
		}
	}
	else
	{
		if(exec_app)
		{
			if( category == FAVORITES ) { gui_app_exec_fav(app_number); exec_app = 0; hold_count = 0; }
			else { gui_draw_application_box( app_number ); exec_app = 0; hold_count = 0; }
/*
			if(gui_confirm_box("Launch Application ?"))
			{
				if( category == FAVORITES ) { gui_app_exec_fav(app_number); exec_app = 0; hold_count = 0; }
					else { gui_app_exec(app_number); exec_app = 0; hold_count = 0; }
			}
			else { exec_app = 0; hold_count = 0; }
*/
		}
		reset_ts_pos = 1;
	}
}


int main(int argc, char *argv[])
{

	int gui_done = 0;
	scroll_count = 750;
	category = FAVORITES;
	alpha_up = 1;
	alpha = 0;
	mouse_y_pos = 120;
	exec_app = 0;

	pnd_app_get_list();

	gui_init_sdl();
	gui_load();

	cfg_fav_read();
	cfg_gui_read();

	SDL_initFramerate( &sixteen );
	SDL_setFramerate( &sixteen, 65 );

	while(!gui_done)
	{
		handle_mouse();

		while( SDL_PollEvent( &event ) )
		{	
			switch(event.type)
			{
				case SDL_KEYDOWN:
					case SDLK_s:
						SDL_SaveBMP(myscreen, "srcshot.bmp");
					break;
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

		SDL_Flip(myscreen);

		SDL_framerateDelay( &sixteen );
	}
	return 0;
}

