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
#include "setting_config.h"

#include "pnd_apps.h"

#include "sprig.h"

#include <GLES/gl.h>
#include <GLES/glues.h>

int do_quit = 0;

int gui_init_sdl()
{

	cfg_setting_read();

	putenv ("SDL_MOUSEDRV=TSLIB");
	//setenv ("SDL_MOUSEDRV", "TSLIB", 1);

	if( ! setting->x11_mode ) putenv ("SDL_VIDEODRIVER=fbcon");

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

	if( ! setting->x11_mode )
	{

		myscreen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
		if( myscreen == NULL )
		{
		    fprintf(stderr, "Couldn’t initialize VideoMode: %s\n", SDL_GetError());
		    exit(1);
		}

		init_raw_screen();
	}
	else
	{
		myscreen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGLES | SDL_FULLSCREEN);
		if( myscreen == NULL )
		{
		    fprintf(stderr, "Couldn’t initialize VideoMode: %s\n", SDL_GetError());
		    exit(1);
		}
	}

	init_2D_view();

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


void gui_clean_fav()
{
	int i;
	
	for( i = 0; i < FAV_MAX; i++)
	{

	}

}

void gui_load_preview(int cat, int n)
{
	alpha_preview = 0.0f;

	if( tmp_preview != NULL )
	{
		glDeleteTextures( 1, &tmp_preview->texture );
		free ( tmp_preview ); 
	}


	if ( access ( applications[cat]->preview_pic1[n], R_OK ) == 0 )
	{
		tmp_preview = (TEXTURE *) malloc( sizeof(TEXTURE));
		load_image_to_texture( tmp_preview, applications[cat]->preview_pic1[n] );
	}

}

void gui_load_fav()
{
	int i;
	
	for( i = 0; i < FAV_MAX; i++)
	{
		if(fav->enabled[i])
		{
			if (access (fav->fullpath[i], R_OK) == 0) 
			{
				if (access (fav->icon[i], R_OK) == 0) 
				{
					fav_preview[i] = (TEXTURE *) malloc( sizeof(TEXTURE));
					load_image_to_texture( fav_preview[i], fav->icon[i] );
			
				}
				else
				{
					fav_preview[i] = (TEXTURE *) malloc( sizeof(TEXTURE));
					load_image_to_texture( fav_preview[i], "data/no_icon.png" );
				}
			}
			else
			{
					cfg_fav_del( i );
					gui_load_fav();
			}
		}
	}

}

void gui_load()
{
	int i, j;

	buffer1 = (TEXTURE *) malloc( sizeof(TEXTURE));
	buffer2 = (TEXTURE *) malloc( sizeof(TEXTURE));

	logo = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( logo, "data/logo.bmp" );

	background = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( background, "data/backg.bmp" );

	fav_background = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( fav_background, "data/fav_backg.bmp" );

	app_background = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( app_background, "data/app_backg.bmp" );

	highlight = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( highlight, "data/highlight.bmp" );

	app_highlight = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( app_highlight, "data/app_highlight.bmp" );

	category_icon[FAVORITES] = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( category_icon[FAVORITES], "data/favorites_icon.bmp" );

	category_icon[EMULATORS] = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( category_icon[EMULATORS], "data/emulators_icon.bmp" );

	category_icon[GAMES] = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( category_icon[GAMES], "data/games_icon.bmp" );

	category_icon[APPLICATIONS] = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( category_icon[APPLICATIONS], "data/applications_icon.bmp" );

	arrow[LEFT] = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( arrow[LEFT], "data/arrowleft.bmp" );

	arrow[RIGHT] = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( arrow[RIGHT], "data/arrowright.bmp" );

	confirm_box = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( confirm_box, "data/confirm_box.bmp" );

	no_icon = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( no_icon, "data/no_icon.bmp" );


	for(i = EMULATORS; i < APPLICATIONS+1; i++)
	{
		for(j = 0; j < list_num[i]; j++)
		{
			preview[i][j] = (TEXTURE *) malloc( sizeof(TEXTURE));
			load_image_to_texture(preview[i][j], applications[i]->icon[j]);
		}
	}

	gui_load_fav();

}

void gui_clean()
{
	int i, j;

	glDeleteTextures( 1, &background->texture );
	free (background);

	glDeleteTextures( 1, &fav_background->texture );
	free (fav_background);

	glDeleteTextures( 1, &app_background->texture );
	free (app_background);

	glDeleteTextures( 1, &highlight->texture );
	free (highlight);

	glDeleteTextures( 1, &app_highlight->texture );
	free (app_highlight);

	glDeleteTextures( 1, &category_icon[FAVORITES]->texture );
	free (category_icon[FAVORITES]);

	glDeleteTextures( 1, &category_icon[EMULATORS]->texture );
	free (category_icon[EMULATORS]);

	glDeleteTextures( 1, &category_icon[GAMES]->texture );
	free (category_icon[GAMES]);

	glDeleteTextures( 1, &category_icon[APPLICATIONS]->texture );
	free (category_icon[APPLICATIONS]);

	glDeleteTextures( 1, &arrow[LEFT]->texture );
	free (arrow[LEFT]);

	glDeleteTextures( 1, &arrow[RIGHT]->texture );
	free (arrow[RIGHT]);

	glDeleteTextures( 1, &confirm_box->texture );
	free (confirm_box);

	glDeleteTextures( 1, &no_icon->texture );
	free (no_icon);

	for(i = EMULATORS; i < APPLICATIONS+1; i++)
	{
		for(j = 0; j < list_num[i]; j++)
		{
			glDeleteTextures( 1, &preview[i][j]->texture );
			free (preview[i][j]);
		}
	}

	TTF_CloseFont( font ); 
	TTF_CloseFont( font_big );
	TTF_Quit(); 
	SDL_Quit();

	if( ! setting->x11_mode ) destroy_raw_screen();
}

void gui_app_exec(int n)
{
	gui_clean();

	pnd_apps_exec ( pndrun, applications[category]->fullpath[n], \
		applications[category]->id[n], applications[category]->exec_name[n], \
			NULL, 500, PND_EXEC_OPTION_BLOCK);

	gui_init_sdl();
	gui_load();
	
	gui_load_preview( category, n );
}

void gui_app_exec_fav(int n)
{
	gui_clean();

	pnd_apps_exec ( pndrun, fav->fullpath[n], \
		NULL, fav->exec_name[n], \
			NULL, 500, PND_EXEC_OPTION_BLOCK );

	gui_init_sdl();
	gui_load();
}

int gui_confirm_box(char *msg)
{
	int done = 0;

	DrawTexture( confirm_box, 400, 240, CENTERED, 0, 1.0f );
//	draw_text_center(msg, SMALL, BLEND, WHITE, 400, 220);

	gui_swap_buffer();

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

#ifndef PANDORA
		SDL_framerateDelay( &sixteen );
#else
		SDL_Delay(1);
#endif
	}
	return 0;
}

void gui_draw()
{
	if(category != FAVORITES)
	{
		DrawTexture( background, 0, 0, NORMAL, 0, 1.0f);

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
					
					DrawTexture( app_highlight, gui->applications_box_x - 54, (((i-list_start[category])+gui->applications_box_y)*50) - 7, NORMAL, 0, 1.0f);
					draw_text(tmpStr, BIG, BLEND, GREEN, gui->applications_box_x, ((i-list_start[category])+gui->applications_box_y)*50);
				}
				else
				{
					draw_text(tmpStr, SMALL, BLEND, WHITE, gui->applications_box_x, ((i-list_start[category])+gui->applications_box_y)*50);
				}


				if (access (applications[category]->icon[i], W_OK) == 0)
				{
					DrawTexture( preview[category][i], gui->applications_box_x - 46, ((i-list_start[category])+gui->applications_box_y)*50, NORMAL, 0.6f, 1.0f );
					preview_x[category][i] = gui->applications_box_x - 46;
					preview_y[category][i] = ((i-list_start[category])+gui->applications_box_y)*50;
				}

			}
			i++;
		}

		if( tmp_preview != NULL )
		{	
			if(alpha_preview < 1.0f) alpha_preview += 0.05f;
	 
			DrawTexture( tmp_preview, 600, 200, CENTERED, 0.4f, alpha_preview);


		}

	}
	else if( category == FAVORITES )
	{
		DrawTexture( fav_background, 0, 0, NORMAL, 0, 1.0f);

		int i, x = 60, y = 120;

		for( i = 0; i < FAV_MAX; i++)
		{
			if( !fav->enabled[i] ) break;

			if (( i == 7 ) | ( i == 14 )) { y += 100; x = 60; }

			if( fav_preview[i] != NULL )
			{
				DrawTexture( fav_preview[i], x, y, NORMAL, 0, 0.9f);
			}

			fav_preview_x[i] = x;
			fav_preview_y[i] = y;

			x += 100;
		}
	}


	if(alpha_up)
		alpha += 0.05f;
	else
		alpha -= 0.05f;

	if(alpha > 1.0f)
		alpha_up = 0;
	else if(alpha < 0.3f)
		alpha_up = 1;
 
	DrawTexture( highlight, category_icon_x[category], category_icon_y, CENTERED, 0  , alpha);

	DrawTexture( category_icon[FAVORITES], category_icon_x[FAVORITES], category_icon_y, CENTERED, 0, 1.0f );
	DrawTexture( category_icon[EMULATORS], category_icon_x[EMULATORS], category_icon_y, CENTERED, 0, 1.0f );
	DrawTexture( category_icon[GAMES], category_icon_x[GAMES], category_icon_y, CENTERED, 0, 1.0f );
	DrawTexture( category_icon[APPLICATIONS], category_icon_x[APPLICATIONS], category_icon_y, CENTERED, 0, 1.0f );

	if(category != FAVORITES)
	{
		DrawTexture ( arrow[LEFT], gui->arrow_left_x, gui->arrow_left_y, CENTERED, 0, 1.0f );
		DrawTexture ( arrow[RIGHT], gui->arrow_right_x, gui->arrow_right_y, CENTERED, 0, 1.0f );

//		if( list_num[category] ) draw_text_scroll(applications[category]->description[list_curpos[category]], SMALL, NORMAL, WHITE, 440, 320, 315);
	}

	draw_3d_box(logo, 750, 45, 15.0f);

	draw_text("Exit", SMALL, BLEND, WHITE, 755, 450);
}

void gui_draw_application_box(int item)
{
	int done = 0;

	TEXTURE *tmp_pic1 = NULL;
	TEXTURE *tmp_icon = NULL;

	tmp_icon = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( tmp_icon, applications[category]->icon[item] );

	tmp_pic1 = (TEXTURE *) malloc( sizeof(TEXTURE));
	load_image_to_texture( tmp_pic1, applications[category]->preview_pic1[item] );

	copy_buffer_to_texture(buffer1);

	GLfloat alpha_old = 1.0f;
	GLfloat alpha_new = 0.0f;
	GLfloat reduce = 1.0f;
	GLfloat maximize = 0.0f;

	while(!done)
	{
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if( alpha_old > 0.0f)
		{

			DrawBufferTexture(buffer1, 400, 240, CENTERED, reduce, alpha_old);
			
			alpha_old -= 0.02f;
			reduce -= 0.02f;
		}
	
		DrawTexture(app_background, 0, 0, NORMAL, maximize, alpha_new);
		DrawTexture(tmp_icon, 50, 40, NORMAL, maximize, alpha_new);
		DrawTexture(tmp_pic1, 50, 140, NORMAL, maximize - 0.4f , alpha_new);

		if( alpha_new < 1.0f)
			alpha_new += 0.02f;
		if( maximize < 1.0f )
			maximize += 0.02f;
	
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
						cfg_fav_add(j, applications[category]->name[item], \
							applications[category]->fullpath[item], \
							applications[category]->exec_name[item], \
							applications[category]->icon[item], \
							applications[category]->description[item]);
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

		gui_swap_buffer();

	}

	copy_buffer_to_texture(buffer2);

	alpha_old = 1.0f;
	alpha_new = 0.0f;
	reduce = 1.0f;
	maximize = 0.0f;

	done = 0;
	while(!done)
	{
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if( alpha_old > 0.0f)
		{

			DrawBufferTexture(buffer2, 400, 240, CENTERED, reduce, alpha_old);
			
			alpha_old -= 0.02f;
			reduce -= 0.02f;
		}

		DrawBufferTexture(buffer1, 400, 240, CENTERED, maximize, alpha_new);

		if( maximize < 1.0f )
			maximize += 0.02f;

		gui_swap_buffer();

		if( alpha_new < 1.0f) alpha_new += 0.02f;
			else done = 1;
	}
}

int mouse_delay = 0;

void handle_mouse()
{
	int i;

	mouse_delay++;

	if(mouse_delay > 10)
	{
		get_mouse_loc();

		if(get_mouse_click(MOUSE_LEFT))
		{	
			mouse_delay = 0;

			reset_scroll_count = 1;

			printf( "X:%i Y:%i\n", MOUSE_X, MOUSE_Y );
		
			if(reset_ts_pos) { mouse_hold_x = MOUSE_X; mouse_hold_y = MOUSE_Y; reset_ts_pos = 0; }

			if(category == FAVORITES)
			{
				if(mouse_is_over(35, 755, 85, 435))
				{
					if(MOUSE_X > (mouse_hold_x + 100))
					{
							fav_page += 1;
							mouse_hold_x += 100;	
					}
				}

				for(i = 0; i < FAV_MAX; i++ )
				{
					if( !fav->enabled[i] ) break;

					if( mouse_is_over_surface( fav_preview_x[i], fav_preview_y[i], fav_preview[i]->w, fav_preview[i]->h ) )
					{
						hold_count++;

						if(hold_count == 3)
						{
							if(gui_confirm_box("Delete from favorites ?"))
							{
								printf("deleting from fav (%i)\n", i);
								cfg_fav_del( i );
								hold_count = 0;
								exec_app = 0;
								break;
							}
							else
							{
								hold_count = 0;
								exec_app = 0;
								break;
							}
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
									32))
						{
							if(list_curpos[category] == i + (page[category] * MAXLIST))
							{
								app_number = (i + (page[category] * MAXLIST));

								hold_count++;

								if(hold_count == 3)
								{
									gui_draw_application_box( app_number );
									exec_app = 0;
									hold_count = 0;
								}
								else exec_app = 1;
	
								reset_scroll_count = 1;
								break;
							}
							else
							{
								list_curpos[category] = i + (page[category] * MAXLIST) ;

								gui_load_preview( category, list_curpos[category] );
	
								//SDL_Delay(120);
								reset_scroll_count = 1;
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
							gui_load_preview( category, list_curpos[category] );
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
							gui_load_preview( category, list_curpos[category] );
						}
						else
						{
							list_curpos[category] = 0;
							list_start[category] = 0;
							mouse_hold_x -= 50;
							gui_load_preview( category, list_curpos[category] );
						}					
					}
					reset_scroll_count = 1;
					//SDL_Delay(120);
				}

				if(mouse_is_over_surface_center(gui->arrow_left_x, gui->arrow_left_y, arrow[LEFT]->w, arrow[LEFT]->h))
				{

					if (list_curpos[category] > MAXLIST-1)
					{
						page[category] -= 1;
						list_curpos[category] = page[category] * MAXLIST;
						list_start[category] = page[category] * MAXLIST;
						reset_scroll_count = 1;
						//SDL_Delay(120);
						gui_load_preview( category, list_curpos[category] );
					}
					else
					{
						list_curpos[category] = 0;
						list_start[category] = 0;
						reset_scroll_count = 1;
						//SDL_Delay(120);
						gui_load_preview( category, list_curpos[category] );
					}			
				}
				if(mouse_is_over_surface_center(gui->arrow_right_x, gui->arrow_right_y, arrow[RIGHT]->w, arrow[RIGHT]->h))
				{
					//if (list_curpos[category] < (list_num[category]-1))
					if (list_curpos[category] < (list_num[category]-(MAXLIST-1)))
					{
						page[category] += 1;
						list_curpos[category] = page[category] * MAXLIST;
						list_start[category] = page[category] * MAXLIST;
						reset_scroll_count = 1;
						//SDL_Delay(120);
						gui_load_preview( category, list_curpos[category] );
					}
				}
			}

			for(i = 0; i < 4; i++ )
			{
				if(mouse_is_over_surface_center(category_icon_x[i], 42, category_icon[i]->w, category_icon[i]->h))
				{
					category = i;
					alpha = 0.0f;
					alpha_up = 1;
					reset_scroll_count = 1;

					if( tmp_preview != NULL )
					{
						free( tmp_preview ); 
						tmp_preview = NULL;
					}

					if( list_num[category] > 0 )
						gui_load_preview( category, list_curpos[category] );
				}
			}

			if(mouse_is_over( 740, 800, 440, 480))
			{
				do_quit = 1;		
			}


		}
		else
		{
			if(exec_app)
			{
				if( category == FAVORITES ) { gui_app_exec_fav(app_number); exec_app = 0; hold_count = 0; }
				else { gui_app_exec(app_number); exec_app = 0; hold_count = 0; }
			}
			reset_ts_pos = 1;
		}
	}
}

int main(int argc, char *argv[])
{

	int gui_done = 0;
	category = FAVORITES;
	alpha_up = 1;
	alpha = 0.0;
	mouse_y_pos = 120;
	exec_app = 0;
	reset_scroll_count = 1;
	text_img = NULL;

	pnd_app_get_list();

	gui_init_sdl();

	cfg_fav_read();
	cfg_gui_read();

	gui_load();

	SDL_initFramerate( &sixteen );
	SDL_setFramerate( &sixteen, 60 );
			
	char fpscount[64];

	while(!gui_done)
	{

		Uint32 start_time, frame_time;
		float fps;
		start_time = SDL_GetTicks();

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gui_draw();

		handle_mouse();

		while( SDL_PollEvent( &event ) )
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_UP) { printf("UP\n"); break; }

					if (event.key.keysym.sym == SDLK_DOWN) { printf("DOWN\n"); break; }
	
					if (event.key.keysym.sym == SDLK_s) { printf("s\n"); break; }
				break;

				case SDL_QUIT:
				{
					do_quit = 1;
				}
				break;
			}
		}

		draw_text(fpscount, SMALL, NORMAL, WHITE, 10, 460);

		gui_swap_buffer();

		frame_time = SDL_GetTicks()-start_time;
		fps = (frame_time > 0) ? 1000.0f / frame_time : 0.0f;
		sprintf(fpscount, "%2.0f fps", fps );

		SDL_framerateDelay( &sixteen );

		if(do_quit) gui_done = 1;
	}

	gui_clean();

	int i;
	for(i = EMULATORS; i < APPLICATIONS+1; i++)
	{
		free(applications[i]);
	}

	return 0;
}

