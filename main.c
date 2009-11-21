#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "main.h"
#include "get_apps.h"
#include "fav_config.h"
#include "gui_config.h"
#include "pmenu_config.h"
#include "utils.h"
#include "cpuLoad.h"
#include "pnd_apps.h"
#include "pnd_notify.h"
#include "common.h"

#include "menu_settings.h"

pnd_notify_handle nh;
int nh_countdown = 60;

static int gui_done = 0, do_quit = 0, preview_timer = 30;
int preview_scale = 0;
int page[CATEGORY_COUNT] = {0, 0, 0, 0};

void gui_clean_fav();
void gui_load_fav();
void gui_clean_icon();
void gui_load_icon();

void app_scale()
{
    int i;

    for( i = 0; i < list_num[category]; i++ )
    {
        if ( i == list_curpos[category] )
        {
            if ( applications[category]->scale[i] < 42 )
                applications[category]->scale[i]+=1;
        }
        else
        {
            if ( applications[category]->scale[i] > 32 )
                applications[category]->scale[i]-=1;
        }
    }

}

void reset_preview_timer()
{
	if( tmp_preview != NULL )
	{
		GLES2D_FreeTexture( tmp_preview );
		tmp_preview = NULL;
	}
	preview_timer = 25;
}

void rediscover()
{
    debug_start( );

    int i;

    gui_clean_icon();

    for( i = 0; i < CATEGORY_COUNT-2; i++ )
	{
		free( applications[i] );
	}

	pnd_app_get_list();

    gui_load_icon();

	list_curpos[category] = 0;
	reset_preview_timer();

	gui_clean_fav();
	cfg_fav_read();
	gui_load_fav();

	debug_end();
}

void check_rediscover()
{
    nh_countdown--;
    if ( ! nh_countdown )
    {
        if ( pnd_notify_rediscover_p ( nh ) )
        {
            printf ( "Must do a rediscover!\n" );

            rediscover();

            nh_countdown = 120;
        }
        else
        {
            nh_countdown = 60;
        }
    }
}

int gui_init()
{
#ifdef I386
    GLES2D_InitVideo( 800, 480, 1, 1, 1 );
//    GLES2D_JoystickList();
//    GLES2D_JoystickInit(0);
#else
	putenv ("SDL_MOUSEDRV=TSLIB");
    GLES2D_InitVideo( 800, 480, 1, 1, 1 );
#endif
	return 0;
}

void gui_load_icon()
{
    debug_start();

    int i, j;

    for(i = 0; i < CATEGORY_COUNT - 2; i++)
	{
		for(j = 0; j < list_num[i]; j++)
		{
		    if (  icon[i][j] != NULL )
		    {
                GLES2D_FreeTexture( icon[i][j] );
                icon[i][j] = NULL;
		    }

            if ( ( icon[i][j] = GLES2D_CreateTexture( applications[i]->icon[j], 0  ) ) == NULL )
            {
                debug_errorf( "Could not load icon (%s);", applications[i]->icon[j] );
            }
            applications[i]->scale[j] = 32;
		}
	}
	debug_end();
}

void gui_clean_icon()
{
    debug_start();

    int i, j;

	for( i = 0; i < CATEGORY_COUNT-2; i++ )
	{
		for(j = 0; j < list_num[i]; j++)
		{
			if ( icon[i][j] != NULL )
            {
                    GLES2D_FreeTexture( icon[i][j] );
                    icon[i][j] = NULL;
            }
		}
		list_num[i] = 0;
	}
	debug_end();
}

void gui_load_preview( int cat, int n )
{
    debug_start();

	alpha_preview = 0;
	preview_scale = 0;

	if( tmp_preview != NULL )
	{
	    GLES2D_FreeTexture( tmp_preview );
	    tmp_preview = NULL;
	}

	if ( access ( applications[cat]->preview_pic1[n], R_OK ) == 0 )
	{
		if ( ( tmp_preview = GLES2D_CreateTexture( applications[cat]->preview_pic1[n], 0 ) ) == NULL )
		{
            debug_errorf( "GLES2D_CreateTexture(%s);", applications[cat]->preview_pic1[n] );
		}
	}
	else
	{
	    if ( access ( applications[cat]->fullpath[n], R_OK ) == 0 )
	    {
            if ( applications[cat]->type[n] == 2 )
            {
                char src[512];
                memset ( src, 0, 512 );
                sprintf( src, "/mnt/pnd/%s/%s", applications[cat]->id[n], applications[cat]->preview_pic1_name[n] );

                pnd_pnd_mount ( pndrun, applications[cat]->fullpath[n], applications[cat]->id[n] );

                if ( access ( src, R_OK ) == 0 )
                {
                    debug_infof("copy %s -> %s", src, applications[cat]->preview_pic1[n] );
                    copy( src, applications[cat]->preview_pic1[n] );

                    if ( access ( applications[cat]->preview_pic1[n], R_OK ) == 0 )
                    {
                        if ( ( tmp_preview = GLES2D_CreateTexture( applications[cat]->preview_pic1[n], 0 ) ) == NULL )
                        {
                            debug_errorf( "GLES2D_CreateTexture(%s);", applications[cat]->preview_pic1[n] );
                        }
                    }
                    else
                    {
                            debug_errorf( "Could not copy %s to %s", src, applications[cat]->preview_pic1[n] );
                    }
                }
                else
                {
                    debug_errorf( "Could not access cache preview from %s", src );
                }

                pnd_pnd_unmount ( pndrun, applications[cat]->fullpath[n], applications[cat]->id[n] );
            }
	    }
	    else
	    {
            debug_errorf("Preview pic do not exist (%s)", applications[cat]->preview_pic1[n]);
            debug_info("Will use fail safe preview pic");
	    }
	}
	debug_end();
}

void gui_load_fav()
{
    int i;

    for(i = 0; i < list_num[FAVORITES]; i++)
    {
        icon[FAVORITES][i] = NULL;
        icon[FAVORITES][i] = GLES2D_CreateTexture( applications[FAVORITES]->icon[i], 0  );
    }
}

void gui_clean_fav()
{
	int i;

    for(i = 0; i < list_num[FAVORITES]; i++)
    {
        if ( icon[FAVORITES][i] != NULL )
        {
                GLES2D_FreeTexture( icon[FAVORITES][i] );
                icon[FAVORITES][i] = NULL;
        }
	}
}

void gui_clean_skin()
{
    debug_start();

    int i;

    if ( background != NULL )
    {
        GLES2D_FreeTexture( background );
        background = NULL;
    }

    if ( highlight != NULL )
    {
        GLES2D_FreeTexture( highlight );
        highlight = NULL;
    }

    if ( highlight_fav != NULL )
    {
        GLES2D_FreeTexture( highlight_fav );
        highlight_fav = NULL;
    }

	if ( app_highlight != NULL )
	{
        GLES2D_FreeTexture( app_highlight );
        app_highlight = NULL;
	}

	if ( confirm_box != NULL )
	{
        GLES2D_FreeTexture( confirm_box );
        confirm_box = NULL;
	}

	if ( no_icon != NULL )
	{
        GLES2D_FreeTexture( no_icon );
        no_icon = NULL;
    }

	if ( no_preview != NULL )
	{
        GLES2D_FreeTexture( no_preview );
        no_preview = NULL;
	}

	if ( cpu_icon != NULL )
	{
        GLES2D_FreeTexture( cpu_icon );
        cpu_icon = NULL;

    }

    if ( clock_icon != NULL )
    {
        GLES2D_FreeTexture( clock_icon );
        clock_icon = NULL;
    }


    if ( sd1_icon != NULL )
    {
        GLES2D_FreeTexture( sd1_icon );
        sd1_icon = NULL;
    }

    if ( sd2_icon != NULL )
    {
        GLES2D_FreeTexture( sd2_icon );
        sd2_icon = NULL;
    }


	for ( i = 0; i < CATEGORY_COUNT; i++ )
	{
	    if ( category_icon[i] != NULL )
	    {
            GLES2D_FreeTexture( category_icon[i] );
            category_icon[i] = NULL;
	    }
	}

    if ( big != NULL )
    {
        GLES2D_FreeFont( big );
        big = NULL;
    }

    if ( small != NULL )
    {
        GLES2D_FreeFont( small );
        small = NULL;
    }

    debug_end();
}

int gui_load_skin()
{
    debug_start();

    char tmp[512];

    cfg_gui_read();

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/backg.bmp", pmenu->skin_dir );
	if ( ( background = GLES2D_CreateTexture( tmp, 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/highlight.bmp", pmenu->skin_dir );
	if ( ( highlight = GLES2D_CreateTexture( tmp, 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/highlight_fav.bmp", pmenu->skin_dir );
    if ( ( highlight_fav = GLES2D_CreateTexture( tmp, 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
    GLES2D_SetTextureAlpha ( highlight_fav, 100 );

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/app_highlight.bmp", pmenu->skin_dir );
	if ( ( app_highlight = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/favorites_icon.bmp", pmenu->skin_dir );
	if ( ( category_icon[FAVORITES] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/emulators_icon.bmp", pmenu->skin_dir );
	if ( ( category_icon[EMULATORS] = GLES2D_CreateTexture( tmp , 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/games_icon.bmp", pmenu->skin_dir );
	if ( ( category_icon[GAMES] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/applications_icon.bmp", pmenu->skin_dir );
	if ( ( category_icon[APPLICATIONS] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/divers_icon.bmp", pmenu->skin_dir );
	if ( ( category_icon[DIVERS] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/settings_icon.png", pmenu->skin_dir );
    if ( ( category_icon[SETTINGS] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/confirm_box.png", pmenu->skin_dir );
	if ( ( confirm_box = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/no_icon.bmp", pmenu->skin_dir );
	if ( ( no_icon = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/no_preview.bmp", pmenu->skin_dir );
	if ( ( no_preview = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/cpu.png", pmenu->skin_dir );
    if ( ( cpu_icon = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/clock.png", pmenu->skin_dir );
    if ( ( clock_icon = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/sd1.png", pmenu->skin_dir );
    if ( ( sd1_icon = GLES2D_CreateTexture( tmp , 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/sd2.png", pmenu->skin_dir );
    if ( ( sd2_icon = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/big.ttf", pmenu->skin_dir );
    if ( ( big = GLES2D_CreateFont( tmp, TTF_STYLE_NORMAL, gui->font_big_size ) ) == NULL )
    {
        debug_errorf( "Could not load font (%s)", tmp );
        debug_end();
        return 0;
    }

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/small.ttf", pmenu->skin_dir );
	if ( ( small = GLES2D_CreateFont( tmp, TTF_STYLE_NORMAL, gui->font_small_size ) ) == NULL )
	{
        debug_errorf( "Could not load font (%s)", tmp );
        debug_end();
        return 0;
	}

	debug_end();
	return 1;
}

void gui_load()
{
    debug_start();

    gui_load_skin();
    gui_load_icon();
    gui_load_fav();

    if ( ! ( nh = pnd_notify_init() ) )
    {
        debug_error ( "PND INOTIFY init problem spotted\n" );
    }

    initStatusCalls();
    cpuUsage();
    getCPULoad();

    set_cpu( pmenu->cpu_mhz );

    debug_end();
}

void gui_clean()
{
    debug_start();

    gui_clean_skin();
    gui_clean_icon();
    gui_clean_fav();

    if( tmp_preview != NULL )
	{
	    GLES2D_FreeTexture(tmp_preview);
	    tmp_preview = NULL;
	}

    pnd_notify_shutdown ( nh );
    doneStatusCalls();

    GLES2D_Quit();

    debug_end();
}

void gui_app_exec(int n)
{
	gui_clean();

	pnd_apps_exec ( pndrun, applications[category]->fullpath[n], \
		applications[category]->id[n], applications[category]->exec_name[n], \
			applications[category]->fullpath[n], 600, PND_EXEC_OPTION_BLOCK);

	gui_init();
	gui_load();

	gui_load_preview( category, n );
}

int gui_confirm_box( char *msg )
{
	int done = 0;

    GLES2D_DrawTextureSimple( confirm_box, 300, 125 );
    GLES2D_SetFontColor( small, HEXTOR(gui->font_small_color), HEXTOG(gui->font_small_color), HEXTOB(gui->font_small_color), HEXTOA(gui->font_small_color) );
    GLES2D_DrawFontBox ( small, 300, 125, confirm_box->dst->w, confirm_box->dst->h, msg );

    //GLES2D_DrawTextBoxFill( small, msg, 400 - 100, 280 - 50, 200, 100, 100, 100, 100, 200, 255, 255, 255, 255 );

    GLES2D_SwapBuffers();

	while(!done)
	{
        GLES2D_HandleEvents( 90 );

        if ( GLES2D_PadPressed ( A ) || GLES2D_KeyboardPressed ( XK_o ) ) // OK
        {
                return 1;
        }

        if ( GLES2D_PadPressed ( B ) || GLES2D_KeyboardPressed ( XK_n ) ) // Cancel
        {
                return 0;
        }

		SDL_Delay(1);
	}
	return 0;

}

void gui_draw()
{
    GLES2D_DrawTextureSimple ( background, 0, 0 );

	if( category != SETTINGS )
	{
	    app_scale();

		int i = list_start[category];

		char tmpStr[256];

		while (i < (list_start[category]+gui->max_app_per_page))
		{
			if (i < list_num[category])
			{
				if (i == list_curpos[category])
				{
					memset(tmpStr, 0, 256);
					strncpy(tmpStr, applications[category]->name[i], 29);

					GLES2D_DrawTextureSimple ( app_highlight, gui->applications_box_x - 54, \
						(((i-list_start[category])+gui->applications_box_y)*50) - 30 );

                    GLES2D_SetFontColor( big, HEXTOR(gui->font_big_color_highlight), HEXTOG(gui->font_big_color_highlight), HEXTOB(gui->font_big_color_highlight), HEXTOA(gui->font_big_color_highlight) );
					GLES2D_DrawFont( big, gui->applications_box_x, (((i-list_start[category])+gui->applications_box_y)*50) - 20 , tmpStr );
					GLES2D_SetFontColor( big, HEXTOR(gui->font_big_color), HEXTOG(gui->font_big_color), HEXTOB(gui->font_big_color), HEXTOA(gui->font_big_color) );

					if ( scroll_count < gui->applications_box_x - GLES2D_GetTextWidth ( small, applications[category]->description[i] ) )
                        scroll_count = gui->applications_box_x + 270;

                    if ( reset_scroll_count )
                    {
                        scroll_count = gui->applications_box_x;
                        reset_scroll_count = 0;
                    }
                   // GLES2D_SetFontColor( small, HEXTOR(gui->font_small_color_highlight), HEXTOG(gui->font_small_color_highlight), HEXTOB(gui->font_small_color_highlight), HEXTOA(gui->font_small_color_highlight) );
                    GLES2D_DrawFontScroll( small, scroll_count, ( ((i-list_start[category])+gui->applications_box_y)*50 + 20 ) - 20, gui->applications_box_x, gui->applications_box_x + 270,  applications[category]->description[i] );
                   // GLES2D_SetFontColor( small, HEXTOR(gui->font_small_color), HEXTOG(gui->font_small_color), HEXTOB(gui->font_small_color), HEXTOA(gui->font_small_color) );
                    scroll_count--;
				}
				else
				{
				    GLES2D_SetFontColor( big, HEXTOR(gui->font_big_color), HEXTOG(gui->font_big_color), HEXTOB(gui->font_big_color), HEXTOA(gui->font_big_color) );
				    GLES2D_SetFontColor( small, HEXTOR(gui->font_small_color), HEXTOG(gui->font_small_color), HEXTOB(gui->font_small_color), HEXTOA(gui->font_small_color) );

					memset(tmpStr, 0, 256);
					strncpy(tmpStr, applications[category]->name[i], 29);
					GLES2D_DrawFont( big, gui->applications_box_x, (((i-list_start[category])+gui->applications_box_y)*50) - 20, tmpStr );
                    memset(tmpStr, 0, 256);
					strncpy(tmpStr, applications[category]->description[i], 40 );
					GLES2D_DrawFont( small, gui->applications_box_x, (((i-list_start[category])+gui->applications_box_y)*50 + 20) - 20, tmpStr );
				}

				if ( icon[category][i] != NULL )
					GLES2D_DrawTextureScaledCentered( icon[category][i], gui->applications_box_x - 46 + 16, \
						(((i-list_start[category])+gui->applications_box_y)*50)-23 + 18, applications[category]->scale[i], applications[category]->scale[i] );
				else
					GLES2D_DrawTextureScaledCentered( no_icon, gui->applications_box_x - 46 + 16, \
						(((i-list_start[category])+gui->applications_box_y)*50)-23 + 18, applications[category]->scale[i], applications[category]->scale[i] );

				icon_x[category][i] = gui->applications_box_x - 46;
				icon_y[category][i] = (((i-list_start[category])+gui->applications_box_y)*50)-23;

			}
			i++;
		}

		if( list_num[category] )
		{
			if( preview_timer == 1 ) gui_load_preview( category, list_curpos[category] );

			if( preview_timer > 0 )
			{
				preview_timer--;
			}
			else
			{
				if(alpha_preview < 245) alpha_preview += 5;
				if(preview_scale < 330) preview_scale += 10;

				if( tmp_preview != NULL )
				{
					GLES2D_SetTextureAlpha ( tmp_preview, alpha_preview );
					GLES2D_DrawTextureScaledCentered( tmp_preview, gui->preview_pic_x, gui->preview_pic_y, preview_scale, \
						preview_scale / 1.666 );
				}
				else
				{
					GLES2D_SetTextureAlpha ( no_preview, alpha_preview );
					GLES2D_DrawTextureScaledCentered( no_preview, gui->preview_pic_x, gui->preview_pic_y, preview_scale, \
						preview_scale / 1.666 );
				}
			}
		}

	}
	else if ( category == SETTINGS )
	{
        settings_draw();
	}

	if( alpha_up )
		alpha += 5;
	else
		alpha -= 5;

	if( alpha > 250 )
		alpha_up = 0;
	else if(alpha < 50 )
		alpha_up = 1;

    GLES2D_SetTextureAlpha ( highlight, alpha );
	GLES2D_DrawTextureCentered( highlight, category_icon_x[category], category_icon_y[category] );


	int i;
	for ( i = 0; i < CATEGORY_COUNT; i++ )
	{
        GLES2D_DrawTextureCentered( category_icon[i], category_icon_x[i], category_icon_y[i] );

        if ( gui->show_category_title )
        {
            GLES2D_SetFontColor( small, HEXTOR(gui->font_small_color), HEXTOG(gui->font_small_color), HEXTOB(gui->font_small_color), HEXTOA(gui->font_small_color) );
            GLES2D_DrawFontCentered( small, category_icon[i]->dst->x + category_icon[i]->dst->w / 2, category_icon[i]->dst->y + category_icon[i]->dst->h + 10, gui->category_title[i] );
        }
	}


    char cpu_char[16];
    memset( cpu_char, 0, 16 );
//    sprintf( cpu_char, "%.0f%%", cpuUsage()*100 );
//    GLES2D_SetFontColor( small, HEXTOR(gui->font_small_color), HEXTOG(gui->font_small_color), HEXTOB(gui->font_small_color), HEXTOA(gui->font_small_color) );
    sprintf( cpu_char, "%i", pmenu->cpu_mhz );
    GLES2D_DrawFont( small, 152, 450, cpu_char );
	GLES2D_DrawTextureSimple( cpu_icon, 110, 440 );

    GLES2D_DrawFont(small, 542, 450, get_time_string(TIME_24) );
    GLES2D_DrawTextureSimple( clock_icon, 500, 440 );

    GLES2D_DrawTextureSimple( sd1_icon, 200, 440 );
    GLES2D_DrawFont( small, 242, 450, disk_space( cfg_fav_path[0] ) );

    GLES2D_DrawTextureSimple( sd2_icon, 350, 440 );
    GLES2D_DrawFont( small, 392, 450, disk_space( cfg_fav_path[1] ) );

}

void gui_draw_application_box(int item)
{
    printf(" gui_draw_application_box\n");
    int done = 0;
    char tmpString[256];

    while( !done )
	{
        GLES2D_HandleEvents( 90 );

        if ( GLES2D_PadPressed ( A ) || GLES2D_KeyboardPressed( XK_o ) )
            done = 1;

        gui_draw();


//        SDL_SetRenderDrawBlendMode(SDL_BLENDMODE_MASK);
//        SDL_SetRenderDrawColor( 153, 204, 50, 240 );
        GLES2D_SetDrawingColor( 153, 204, 50, 240 );
        GLES2D_DrawRectangle( 40, 40, 760, 440 );
//        SDL_SetRenderDrawBlendMode(SDL_BLENDMODE_BLEND);

//        GLES2D_SetDrawingColor( 255, 255, 255, 255 );
        sprintf( tmpString, "Title: %s", applications[category]->name[item] );
        GLES2D_DrawFont( big, 50, 50, tmpString );

        sprintf( tmpString, "Description: %s", applications[category]->description[item] );
        GLES2D_DrawFontBox( big, 50, 70, 200, 100, tmpString );
        //GLES2D_DrawText( normal, tmpString, 50, 50 );

        GLES2D_SwapBuffers();
	}
    SDL_Delay( 60 );
}

void gui_change_category( int requested_cat )
{
    debug_start();

	category = requested_cat;
	if( category != SETTINGS )
	{
		alpha = 0.0f;
		alpha_up = 1;
		reset_scroll_count = 1;

		if( list_num[category] )
		{
			debug_infof("Number of item in category[%i] : %i\n", category, list_num[category]);
			reset_preview_timer();
		}
		else
		{
			if( tmp_preview != NULL )
			{
				GLES2D_FreeTexture( tmp_preview );
				tmp_preview = NULL;
			}
		}
	}
	else
	{
        get_skins_list ();
        load_skin_preview();
	}
	debug_end();
}

void handle_dpad()
{
    GLES2D_HandleEvents( 90 );

    if ( GLES2D_PadPressed ( MENU ) || GLES2D_KeyboardPressed( XK_Escape ) )
    {
        do_quit = 1;
    }

        if( GLES2D_PadHold ( PAD_LEFT ) )
        {
            if ( category == SETTINGS )
            {
                if ( setting_current == MENU_CPU )
                {
                    if ( pmenu->cpu_mhz > 30 )
                        pmenu->cpu_mhz-=5;
                }
            }
        }

        if( GLES2D_PadPressed ( PAD_LEFT ) || GLES2D_KeyboardPressed( XK_Left ) )
        {
            if ( category == SETTINGS )
            {
                if ( setting_current == MENU_SKIN )
                {
                    if ( skin_current > 0 )
                    {
                        skin_previous = skin_current;
                        skin_current--;
                    }

                    load_skin_preview();
                }
            }

            else
            {
                if ( list_curpos[category] > gui->max_app_per_page - 1 )
                {
                    page[category] -= 1;
                    list_curpos[category] = page[category] * gui->max_app_per_page;
                    list_start[category] = page[category] * gui->max_app_per_page;
                    reset_scroll_count = 1;
                    reset_preview_timer();
                }
                else
                {
                    list_curpos[category] = 0;
                    list_start[category] = 0;
                    reset_scroll_count = 1;
                    reset_preview_timer();
                }
            }
        }

        if( GLES2D_PadHold ( PAD_RIGHT ) )
        {
            if ( category == SETTINGS )
            {
                if ( setting_current == MENU_CPU )
                {
                    if ( pmenu->cpu_mhz < 800 )
                        pmenu->cpu_mhz+=5;
                }
            }
        }

        if( GLES2D_PadPressed ( PAD_RIGHT ) || GLES2D_KeyboardPressed( XK_Right ) )
        {
            if ( category == SETTINGS )
            {
                if ( setting_current == MENU_SKIN )
                {
                    if ( skin_current < skin_count - 1 )
                    {
                        skin_previous = skin_current;
                        skin_current++;
                    }

                    load_skin_preview();
                }
            }
            else
            {
                if ( ( gui->max_app_per_page * ( page[category] + 1 ) ) < list_num[category] )
                {
                    page[category] += 1;
                    list_curpos[category] = page[category] * gui->max_app_per_page;
                    list_start[category] = page[category] * gui->max_app_per_page;
                    reset_scroll_count = 1;
                    reset_preview_timer();
                }
            }
        }

        if( GLES2D_PadHold ( PAD_UP ) || GLES2D_KeyboardHold( XK_Up ) )
        {
            if ( category == SETTINGS )
            {
                if ( setting_current > 0 )
                    setting_current--;
            }
            else
            {
                if ( list_curpos[category] > 0 )
                {
                    if ( list_curpos[category] == ( page[category] * gui->max_app_per_page ) )
                    {
                        page[category] -= 1;
                        list_curpos[category] -= 1;
                        list_start[category] = page[category] * gui->max_app_per_page;
                    }
                    else
                    {
                        list_curpos[category] -= 1;
                    }

                    reset_scroll_count = 1;
                    reset_preview_timer();
                }
            }
        }

        if( GLES2D_PadHold ( PAD_DOWN ) || GLES2D_KeyboardHold( XK_Down ) )
        {
            if ( category == SETTINGS )
            {
                if ( setting_current < SETTINGS_COUNT - 1 )
                    setting_current++;
            }
            else
            {
                if (list_curpos[category] < list_num[category] - 1 )
                {
                    if (list_curpos[category] == ( gui->max_app_per_page + (page[category] * gui->max_app_per_page) ) - 1)
                    {
                        page[category] += 1;
                        list_curpos[category] = page[category] * gui->max_app_per_page;
                        list_start[category] = page[category] * gui->max_app_per_page;
                    }
                    else
                    {
                        list_curpos[category] += 1;
                    }
                    reset_scroll_count = 1;
                    reset_preview_timer();
                }
            }
        }

    if ( GLES2D_PadPressed ( L ) || GLES2D_KeyboardHold( XK_F1 ) )
    {
        if( category > 0) gui_change_category( category - 1 );
            else gui_change_category( CATEGORY_COUNT - 1 );
    }

    if ( GLES2D_PadPressed ( R ) || GLES2D_KeyboardHold( XK_F2 ) )
    {
        if( category < CATEGORY_COUNT - 1) gui_change_category( category + 1 );
            else gui_change_category( 0 );
    }

    if ( GLES2D_PadPressed ( A ) )
    {
        GLES2D_Pad[ A ] = 0;

        if( category != SETTINGS )
        {
            if ( list_num[category] )
                gui_app_exec( list_curpos[ category ] );
        }
    }
/*
    if ( GLES2D_PadPressed ( B ) || GLES2D_KeyboardPressed( XK_b ) )
    {
        if ( category == FAVORITES )
        {
        }
        else
        {
            if ( list_num[category] )
                gui_draw_application_box( list_curpos[ category ] );
        }
    }
*/
    if ( GLES2D_PadPressed ( X ) || GLES2D_KeyboardPressed( XK_space ) )
    {
        if ( category == FAVORITES )
        {
                if ( list_num[category] )
                {
                    char tmpStr[512];
                    sprintf(tmpStr, "Do you want to remove %s from your favorites applications ?", applications[category]->name[list_curpos[ category ]] );

                    if( gui_confirm_box( tmpStr ) )
                        cfg_fav_del( list_curpos[ category ] );
                }
        }
        else if ( category == SETTINGS  )
        {
            if ( setting_current == MENU_SKIN )
            {
                cfg_pmenu_update_skin_path( skin[skin_current]->path );
                cfg_pmenu_read();
                gui_clean_skin();

                if ( ! gui_load_skin() )
                {
                    cfg_pmenu_update_skin_path( skin[skin_previous]->path );
                    cfg_pmenu_read();
                    gui_clean_skin();
                    gui_load_skin();

                }

                load_skin_preview();
            }
            else if ( setting_current == MENU_CPU )
            {
                cfg_pmenu_update_cpu_mhz( pmenu->cpu_mhz );
                set_cpu( pmenu->cpu_mhz );
            }
        }
        else
        {
            if ( list_num[category] )
            {
                if ( applications_count[FAVORITES] < FAV_MAX )
                {
                    char tmpString[256];
                    sprintf(tmpString, "Do you want to add %s to your favorites applications ?", applications[category]->name[list_curpos[ category ]] );

                    if (  gui_confirm_box( tmpString ) )
                    {
                        cfg_fav_add( applications[category]->name[list_curpos[ category ]], \
                            applications[category]->id[list_curpos[ category ]], \
                            applications[category]->category[list_curpos[ category ]], \
                            applications[category]->cache_path[list_curpos[ category ]], \
                            applications[category]->fullpath[list_curpos[ category ]], \
                            applications[category]->exec_name[list_curpos[ category ]], \
                            applications[category]->icon[list_curpos[ category ]], \
                            applications[category]->description[list_curpos[ category ]], \
                            applications[category]->preview_pic1[list_curpos[ category ]], \
                            applications[category]->preview_pic2[list_curpos[ category ]] );
                    }
                }
                else
                {
                    gui_confirm_box("Favorites are full, please remove a favorite before adding one by going under the favorite screen.");
                }
            }
        }
    }
}

int main( )
{
    gui_init();

	gui_done = 0;

	category = EMULATORS;
	alpha_up = 1;
	alpha = 150;
	reset_scroll_count = 1;

    if ( cfg_pmenu_read() < 1 )
    {
        debug_error( "cfg_pmenu_read failed, exiting..." );
        GLES2D_Quit();
        exit(0);
    }

	if ( cfg_gui_read() < 1 )
    {
        GLES2D_Quit();
        debug_error( "cfg_gui_read failed, exiting..." );
        exit(0);
    }

	pnd_app_get_list();
    cfg_fav_read();
	gui_load();

	GLES2D_FpsCounterInit();

    nh_countdown = 60;
    debug_func = 0;


//    debug_errorf( "R:%i G:%i B:%i", HEXTOR(color), HEXTOG(color), HEXTOB(color) );

	while( ! gui_done )
	{

        check_rediscover();

		gui_draw();
        GLES2D_DrawFont( small, 750, 0, GLES2D_GetFpsChar() );

		handle_dpad();

        GLES2D_SwapBuffers();

        GLES2D_FpsCounterUpdate();

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

