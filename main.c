#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "main.h"
#include "get_apps.h"
#include "config_favourite.h"
#include "config_skin.h"
#include "config_pmenu.h"
#include "utils.h"
#include "utils_cpu.h"
#include "pnd_apps.h"
#include "pnd_notify.h"
#include "common.h"
#include "utils_mplayer.h"

#include "category_settings.h"
#include "category_media.h"

pnd_notify_handle nh;
int nh_countdown = 60;

static int gui_done = 0, do_quit = 0, preview_timer = 100;
int preview_scale = 0;
int page[CATEGORY_COUNT-1] = { 0, 0, 0, 0, 0 };
int scroll_wait = 0;

int pnd_mounted = 0;

void pnd_app_clean_list();
void gui_unload_preview();
void gui_draw();
void gui_clean_skin();
int gui_load_skin();

void app_scale()
{
    int i;

    for( i = 0; i < list_num[category]; i++ )
    {
        if ( i == list_curpos[category] )
        {
            if ( applications[category]->scale[i] < gui->icon_scale_max )
                applications[category]->scale[i] += 1;
            else if ( applications[category]->scale[i] > gui->icon_scale_max )
                applications[category]->scale[i] = gui->icon_scale_max;
        }
        else
        {
            if ( applications[category]->scale[i] > gui->icon_scale_min )
                applications[category]->scale[i] -= 1;
            else if ( applications[category]->scale[i] < gui->icon_scale_min )
                applications[category]->scale[i] = gui->icon_scale_min;
        }
    }

}

void rediscover()
{
    debug_start( );

    pnd_app_clean_list();
	pnd_app_get_list();
	list_curpos[category] = 0;
	gui_unload_preview();

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
    now_depth = 0;

#ifdef I386
    GLES2D_InitVideo( 800, 480, 1, 1, 1, VIDEO_X11 );
//    GLES2D_JoystickList();
//    GLES2D_JoystickInit(0);
#else
//	putenv ("SDL_MOUSEDRV=TSLIB");
	putenv ("DISPLAY=:0");
    GLES2D_InitVideo( 800, 480, 1, 1, 1, VIDEO_X11 );
#endif
	return 0;
}

void gui_change_skin()
{
    debug_start();

    int alpha;
    for ( alpha = 0; alpha < 245; alpha += 10 )
    {
        gui_draw();
        GLES2D_SetDrawingColor( 0, 0, 0, alpha );
        GLES2D_DrawRectangle ( 0, 0, 800, 480 );
        GLES2D_SwapBuffers();
    }

    char prev_skin[512];
    memset( prev_skin, 0, 512 );
    strcpy( prev_skin, pmenu->skin_dir_relative );

    debug_infof( "Updating skin path : %s\n", skin[skin_current]->path );
    cfg_pmenu_update_skin_path( skin[skin_current]->path );

    if ( ! cfg_gui_read() )
    {
        cfg_pmenu_update_skin_path( prev_skin );
        cfg_gui_read();
    }
    else
    {
        gui_clean_skin();

        if ( ! gui_load_skin() )
        {
            cfg_pmenu_update_skin_path( prev_skin );
            gui_clean_skin();
            gui_load_skin();
        }
    }

    for ( alpha = 255; alpha > 10; alpha-=10 )
    {
        gui_draw();
        GLES2D_SetDrawingColor( 0, 0, 0, alpha );
        GLES2D_DrawRectangle ( 0, 0, 800, 480 );
        GLES2D_SwapBuffers();
    }

    load_skin_preview();

    debug_end();
}

void gui_unload_preview()
{
    debug_start();

    if( tmp_preview != NULL )
    {
        GLES2D_FreeTexture( tmp_preview );
        tmp_preview = NULL;
    }

    video_quit( );

    if ( applications[category]->type[list_curpos[ category ]] ) // .pnd spotted
    {
        if( pnd_mounted )
        {
            pnd_pnd_unmount( pndrun, applications[category]->fullpath[list_curpos[ category ]], applications[category]->id[list_curpos[ category ]] );
            pnd_mounted = 0;
        }
    }

    preview_timer = 100;

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
	    if ( is_avi( applications[cat]->preview_pic1[n] ) )
	    {
            debug_info( "Preview is a video" );
            video_play_preview( applications[cat]->preview_pic1[n] );
	    }
	    else
	    {
	        debug_info( "Preview is a picture" );
            if ( ( tmp_preview = GLES2D_CreateTexture( applications[cat]->preview_pic1[n], 0 ) ) == NULL )
            {
                debug_errorf( "GLES2D_CreateTexture(%s);", applications[cat]->preview_pic1[n] );
            }
	    }
	}
	else
	{
	    if ( access ( applications[cat]->fullpath[n], R_OK ) == 0 )
	    {
            if ( applications[cat]->type[n] == IS_PND )
            {
                char src[512];
                memset ( src, 0, 512 );
                sprintf( src, "/mnt/pnd/%s/%s", applications[cat]->id[n], applications[cat]->preview_pic1_name[n] );

                if ( is_avi( applications[cat]->preview_pic1_name[n] ) )
                {
                    pnd_pnd_mount ( pndrun, applications[cat]->fullpath[n], applications[cat]->id[n] );
                    pnd_mounted = 1;

                    if ( access ( src, R_OK ) == 0 )
                    {
                        pnd_pnd_mount ( pndrun, applications[cat]->fullpath[n], applications[cat]->id[n] );
                        pnd_mounted = 1;

                        video_play_preview( src );
                    }
                    else
                    {
                        pnd_pnd_unmount( pndrun, applications[cat]->fullpath[n], applications[cat]->id[n] );
                        pnd_mounted = 0;
                        debug_errorf("Preview pic do not exist (%s)", applications[cat]->preview_pic1[n]);
                        debug_info("Will use fail safe preview pic");
                    }
                }
                else
                {
                    debug_infof( "pnd_pnd_mount( pndrun, %s, %s );\n", applications[cat]->fullpath[n], applications[cat]->id[n] );
                    pnd_pnd_mount ( pndrun, applications[cat]->fullpath[n], applications[cat]->id[n] );
                    debug_info( "Done : pnd_pnd_mount()\n" );

                    debug_infof( "Creating preview texture from %s\n", src );

                    if ( ( tmp_preview = GLES2D_CreateTexture( src, 0 ) ) == NULL )
                    {
                        debug_errorf( "GLES2D_CreateTexture(%s);", applications[cat]->preview_pic1[n] );
                    }

                    debug_info( "pnd_pnd_umount()\n" );
                    pnd_pnd_unmount ( pndrun, applications[cat]->fullpath[n], applications[cat]->id[n] );
                    debug_info( "Done : pnd_pnd_umount()\n" );
                }
            }
            else
            {
                debug_errorf("Preview pic do not exist (%s)", applications[cat]->preview_pic1[n]);
                debug_info("Will use fail safe preview pic");
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

void gui_clean_skin()
{
    debug_start();

    int i;

    if ( background_applications != NULL )
    {
        GLES2D_FreeTexture( background_applications );
        background_applications = NULL;
    }
    if ( background_media != NULL )
    {
        GLES2D_FreeTexture( background_media );
        background_media = NULL;
    }
    if ( background_settings != NULL )
    {
        GLES2D_FreeTexture( background_settings );
        background_settings = NULL;
    }

    if ( highlight != NULL )
    {
        GLES2D_FreeTexture( highlight );
        highlight = NULL;
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
	    if ( category_icon_highlight[i] != NULL )
	    {
            GLES2D_FreeTexture( category_icon_highlight[i] );
            category_icon_highlight[i] = NULL;
	    }
	}

	for ( i = 0; i < FONT_COUNT; i++ )
	{
        if ( fnt[i] != NULL )
        {
            GLES2D_FreeFont( fnt[i] );
            fnt[i] = NULL;
        }
	}

    debug_end();
}

int gui_load_skin()
{
    debug_start();

    char tmp[512];

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->background_applications );
	if ( ( background_applications = GLES2D_CreateTexture( tmp, 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->background_media );
	if ( ( background_media = GLES2D_CreateTexture( tmp, 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->background_settings );
	if ( ( background_settings = GLES2D_CreateTexture( tmp, 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->highlight );
	if ( ( highlight = GLES2D_CreateTexture( tmp, 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->application_highlight );
	if ( ( app_highlight = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->favorites_icon );
	if ( ( category_icon[FAVORITES] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->favorites_icon_highlight );
	if ( ( category_icon_highlight[FAVORITES] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->emulators_icon );
	if ( ( category_icon[EMULATORS] = GLES2D_CreateTexture( tmp , 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->emulators_icon_highlight );
	if ( ( category_icon_highlight[EMULATORS] = GLES2D_CreateTexture( tmp , 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->games_icon );
	if ( ( category_icon[GAMES] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->games_icon_highlight );
	if ( ( category_icon_highlight[GAMES] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->misc_icon );
	if ( ( category_icon[MISC] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->misc_icon_highlight );
	if ( ( category_icon_highlight[MISC] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->media_icon );
	if ( ( category_icon[MEDIA] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->media_icon_highlight );
	if ( ( category_icon_highlight[MEDIA] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->media_file_icon );
	if ( ( media_file_icon = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->media_folder_icon );
	if ( ( media_folder_icon = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}


	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->settings_icon );
    if ( ( category_icon[SETTINGS] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}
	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->settings_icon_highlight );
    if ( ( category_icon_highlight[SETTINGS] = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->confirm_box );
	if ( ( confirm_box = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->no_icon );
	if ( ( no_icon = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->no_preview );
	if ( ( no_preview = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->cpu_icon );
    if ( ( cpu_icon = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->clock_icon );
    if ( ( clock_icon = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->sd1_icon );
    if ( ( sd1_icon = GLES2D_CreateTexture( tmp , 0 ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->sd2_icon );
    if ( ( sd2_icon = GLES2D_CreateTexture( tmp, 0  ) ) == NULL )
	{
        debug_errorf( "Could not load image (%s)", tmp );
        debug_end();
        return 0;
	}

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->font_big );
    if ( ( fnt[BIG] = GLES2D_CreateFont( tmp, gui->font_big_style, gui->font_big_size ) ) == NULL )
    {
        debug_errorf( "Could not load font (%s)", tmp );
        debug_end();
        return 0;
    }
    GLES2D_SetFontFiltering( fnt[BIG], gui->font_big_aliasing );

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->font_small );
	if ( ( fnt[SMALL] = GLES2D_CreateFont( tmp, gui->font_small_style, gui->font_small_size ) ) == NULL )
	{
        debug_errorf( "Could not load font (%s)", tmp );
        debug_end();
        return 0;
	}
	GLES2D_SetFontFiltering( fnt[SMALL], gui->font_small_aliasing );

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->clock_text_font );
	if ( ( fnt[CLOCK] = GLES2D_CreateFont( tmp, gui->clock_text_style, gui->clock_text_size ) ) == NULL )
	{
        debug_errorf( "Could not load font (%s)", tmp );
        debug_end();
        return 0;
	}
	GLES2D_SetFontColor( fnt[CLOCK], HEXTOR(gui->clock_text_color), HEXTOG(gui->clock_text_color), HEXTOB(gui->clock_text_color), HEXTOA(gui->clock_text_color) );
    GLES2D_SetFontFiltering( fnt[CLOCK], gui->clock_text_aliasing );

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->sd1_text_font );
	if ( ( fnt[SD1] = GLES2D_CreateFont( tmp, gui->sd1_text_style, gui->sd1_text_size ) ) == NULL )
	{
        debug_errorf( "Could not load font (%s)", tmp );
        debug_end();
        return 0;
	}
	GLES2D_SetFontColor( fnt[SD1], HEXTOR(gui->sd1_text_color), HEXTOG(gui->sd1_text_color), HEXTOB(gui->sd1_text_color), HEXTOA(gui->sd1_text_color) );
    GLES2D_SetFontFiltering( fnt[SD1], gui->sd1_text_aliasing );

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->sd2_text_font );
	if ( ( fnt[SD2] = GLES2D_CreateFont( tmp, gui->sd2_text_style, gui->sd2_text_size ) ) == NULL )
	{
        debug_errorf( "Could not load font (%s)", tmp );
        debug_end();
        return 0;
	}
	GLES2D_SetFontColor( fnt[SD2], HEXTOR(gui->sd2_text_color), HEXTOG(gui->sd2_text_color), HEXTOB(gui->sd2_text_color), HEXTOA(gui->sd2_text_color) );
    GLES2D_SetFontFiltering( fnt[SD2], gui->sd2_text_aliasing );

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->cpu_text_font );
	if ( ( fnt[CPU] = GLES2D_CreateFont( tmp, gui->cpu_text_style, gui->cpu_text_size ) ) == NULL )
	{
        debug_errorf( "Could not load font (%s)", tmp );
        debug_end();
        return 0;
	}
	GLES2D_SetFontColor( fnt[CPU], HEXTOR(gui->cpu_text_color), HEXTOG(gui->cpu_text_color), HEXTOB(gui->cpu_text_color), HEXTOA(gui->cpu_text_color) );
    GLES2D_SetFontFiltering( fnt[CPU], gui->cpu_text_aliasing );

	memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->media_text_font );
	if ( ( fnt[MEDIA_FNT] = GLES2D_CreateFont( tmp, gui->media_text_style, gui->media_text_size ) ) == NULL )
	{
        debug_errorf( "Could not load font (%s)", tmp );
        debug_end();
        return 0;
	}
	GLES2D_SetFontColor( fnt[MEDIA_FNT], HEXTOR(gui->media_text_color), HEXTOG(gui->media_text_color), HEXTOB(gui->media_text_color), HEXTOA(gui->media_text_color) );
    GLES2D_SetFontFiltering( fnt[MEDIA_FNT], gui->media_text_aliasing );

    memset ( tmp, 0, 512 );
    sprintf( tmp, "%s/%s", pmenu->skin_dir, gui->font_small );

    int i;
    for ( i = 0; i < CATEGORY_COUNT; i++ )
	{
	    gui->title_cache[i] = GLES2D_CreateFontCache( tmp, gui->title[i], gui->font_small_style, gui->font_small_size, 800 );
	}

	debug_end();
	return 1;
}

void gui_load()
{
    debug_start();

    pnd_app_get_list();
    cfg_gui_read();
    gui_load_skin();

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

    video_quit();

    gui_clean_skin();
    gui_unload_preview();
    pnd_app_clean_list();

    pnd_notify_shutdown ( nh );
    doneStatusCalls();

    GLES2D_Quit();

    debug_end();
}

void gui_app_exec( int n )
{
    char fullpath[512];
    strcpy( fullpath, applications[category]->fullpath[n] );

    char id[512];
    strcpy( id, applications[category]->id[n] );

    char exec_name[512];
    strcpy( exec_name, applications[category]->exec_name[n] );

    int clock = applications[category]->clock[n];
    int nox = applications[category]->noX[n];

	gui_clean();

    if ( nox )
    {
        printf( "Application ask noX\n" );

        pnd_apps_exec ( pndrun, fullpath, id, exec_name, fullpath, NULL, clock, PND_EXEC_OPTION_NOX11 );

        exit( 0 );
    }
    else
    {
        printf( "Application do not ask X shutdown\n" );

        pnd_apps_exec ( pndrun, fullpath, id, exec_name, fullpath, NULL, clock, PND_EXEC_OPTION_BLOCK );

        gui_init();
        gui_load();
        //gui_load_preview( category, n );
    }
}

int gui_confirm_box( char *msg )
{
	int done = 0;

    GLES2D_DrawTextureCentered( confirm_box, gui->confirm_box_x, gui->confirm_box_y );
    GLES2D_SetFontColor( fnt[SMALL], HEXTOR(gui->font_small_color), HEXTOG(gui->font_small_color), HEXTOB(gui->font_small_color), HEXTOA(gui->font_small_color) );
    GLES2D_DrawFontBox ( fnt[SMALL], confirm_box->dst->x, confirm_box->dst->y, confirm_box->dst->w, confirm_box->dst->h, msg );

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
    if ( category == SETTINGS )
        GLES2D_DrawTextureSimple ( background_settings, 0, 0 );
    else if ( category == MEDIA )
        GLES2D_DrawTextureSimple ( background_media, 0, 0 );
    else
        GLES2D_DrawTextureSimple ( background_applications, 0, 0 );

    if ( category == MEDIA )
    {
        menu_media_draw();
    }
	else if ( category == SETTINGS )
	{
        menu_settings_draw();
	}
    else
	{
	    app_scale();

        int y = gui->applications_box_y;
		int i = list_start[category];

		while (i < ( list_start[category]+gui->max_app_per_page ) )
		{
			if ( i < list_num[category] )
			{
				if ( i == list_curpos[category] )
				{
					GLES2D_DrawTextureSimple ( app_highlight, gui->applications_box_x, y );

                    GLES2D_SetFontCacheColor( applications[category]->name_cached[i], HEXTOR(gui->font_big_color_highlight), HEXTOG(gui->font_big_color_highlight), HEXTOB(gui->font_big_color_highlight), HEXTOA(gui->font_big_color_highlight) );
					GLES2D_DrawFontCache( applications[category]->name_cached[i], gui->applications_box_x + gui->icon_scale_max + 10, y + gui->applications_title_description_y );

                    y += applications[category]->name_cached[i]->height;

					if ( scroll_count < gui->applications_box_x - GLES2D_GetTextWidth ( fnt[SMALL], applications[category]->description[i] ) )
                        scroll_count = gui->applications_box_x + gui->icon_scale_max + gui->applications_box_w + 10;

                    if ( reset_scroll_count )
                    {
                        scroll_count = gui->applications_box_x + gui->icon_scale_max + 10;
                        reset_scroll_count = 0;
                        scroll_wait = 0;
                    }

                    GLES2D_SetFontColor( fnt[SMALL], HEXTOR(gui->font_small_color_highlight), HEXTOG(gui->font_small_color_highlight), HEXTOB(gui->font_small_color_highlight), HEXTOA(gui->font_small_color_highlight) );
                    GLES2D_DrawFontScroll( fnt[SMALL], scroll_count, y + gui->applications_title_description_y, gui->applications_box_x + gui->icon_scale_max + 10, gui->applications_box_x + gui->applications_box_w - 10,  applications[category]->description[i] );

                    if ( scroll_wait > 60 ) scroll_count--;
                    scroll_wait++;
				}
				else
				{
				    GLES2D_SetFontCacheColor( applications[category]->name_cached[i], HEXTOR(gui->font_big_color), HEXTOG(gui->font_big_color), HEXTOB(gui->font_big_color), HEXTOA(gui->font_big_color) );
					GLES2D_DrawFontCache( applications[category]->name_cached[i], gui->applications_box_x + gui->icon_scale_max + 10, y + gui->applications_title_description_y );

					y += applications[category]->name_cached[i]->height;

                    GLES2D_SetFontCacheColor( applications[category]->description_cached[i], HEXTOR(gui->font_small_color), HEXTOG(gui->font_small_color), HEXTOB(gui->font_small_color), HEXTOA(gui->font_small_color) );
					GLES2D_DrawFontCache( applications[category]->description_cached[i], gui->applications_box_x + gui->icon_scale_max + 10, y + gui->applications_title_description_y );
				}

				if ( applications[category]->icon[i] != NULL )
					GLES2D_DrawTextureScaledCentered( applications[category]->icon[i], gui->applications_box_x + 5 + gui->icon_scale_max / 2, y + 4, applications[category]->scale[i], applications[category]->scale[i] );
				else
					GLES2D_DrawTextureScaledCentered( no_icon, gui->applications_box_x + 5 + gui->icon_scale_max / 2, y + 4, applications[category]->scale[i], applications[category]->scale[i] );
			}
			i++;
			y += gui->applications_spacing;
		}

		if( list_num[category] )
		{
			if( preview_timer == 1 )
                gui_load_preview( category, list_curpos[category] );

			if( preview_timer > 0 )
			{
				preview_timer--;
			}
			else
			{
			    if ( ! video_playing )
			    {
                    if( alpha_preview < 250 ) alpha_preview += 5;
                    if( preview_scale < gui->preview_pic_w ) preview_scale += 10;

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

	}

    if ( gui->highlight_enabled )
    {
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
    }

	int i;
	for ( i = 0; i < CATEGORY_COUNT; i++ )
	{
	    if ( category == i )
	    {
            GLES2D_DrawTextureCentered( category_icon_highlight[i], category_icon_x[i], category_icon_y[i] );
            if ( gui->show_category_title )
            {
                GLES2D_SetFontCacheColor( gui->title_cache[i], HEXTOR(gui->font_small_color), HEXTOG(gui->font_small_color), HEXTOB(gui->font_small_color), HEXTOA(gui->font_small_color) );
                GLES2D_DrawFontCacheCentered( gui->title_cache[i], category_icon_highlight[i]->dst->x + category_icon_highlight[i]->dst->w / 2, category_icon_highlight[i]->dst->y + category_icon_highlight[i]->dst->h + 15 );
            }
	    }
	    else
	    {
            GLES2D_DrawTextureCentered( category_icon[i], category_icon_x[i], category_icon_y[i] );
            if ( gui->show_category_title )
            {
                GLES2D_SetFontCacheColor( gui->title_cache[i], HEXTOR(gui->font_small_color), HEXTOG(gui->font_small_color), HEXTOB(gui->font_small_color), HEXTOA(gui->font_small_color) );
                GLES2D_DrawFontCacheCentered( gui->title_cache[i], category_icon[i]->dst->x + category_icon[i]->dst->w / 2, category_icon[i]->dst->y + category_icon[i]->dst->h + 15 );
            }
	    }
	}


    char cpu_char[16];
    memset( cpu_char, 0, 16 );
    sprintf( cpu_char, "%.0f%%", cpuUsage()*100 );
    //sprintf( cpu_char, "%i", pmenu->cpu_mhz );
    GLES2D_DrawFont( fnt[CPU], gui->cpu_text_x, gui->cpu_text_y, cpu_char );
	GLES2D_DrawTextureSimple( cpu_icon, gui->cpu_icon_x, gui->cpu_icon_y );

    GLES2D_DrawFont( fnt[CLOCK], gui->clock_text_x, gui->clock_text_y, get_time_string(TIME_24) );
    GLES2D_DrawTextureSimple( clock_icon, gui->clock_icon_x, gui->clock_icon_y );

    GLES2D_DrawTextureSimple( sd1_icon, gui->sd1_icon_x, gui->sd1_icon_y );
//    GLES2D_DrawFont( fnt[SD1], gui->sd1_text_x, gui->sd1_text_y, disk_space( cfg_fav_path[0] ) );

    GLES2D_DrawTextureSimple( sd2_icon, gui->sd2_icon_x, gui->sd2_icon_y );
//    GLES2D_DrawFont( fnt[SD2], gui->sd2_text_x, gui->sd2_text_y, disk_space( cfg_fav_path[1] ) );

}

void gui_change_category( int requested_cat )
{
    debug_start();

    if( category != SETTINGS )
        gui_unload_preview();

	category = requested_cat;

    if( category != SETTINGS )
    {
        alpha = 0.0f;
        alpha_up = 1;
        reset_scroll_count = 1;
    }

	if( category == SETTINGS )
	{
        get_skins_list ();
        load_skin_preview();
	}
	else if( category == MEDIA )
	{
        get_media_list( now_path );
	}

	debug_end();
}

void gui_pad_left()
{
    if ( category == SETTINGS )
    {
        if ( setting_current == MENU_SKIN )
        {
            if ( skin_current > 0 )
            {
                skin_current--;
            }

            load_skin_preview();
        }
    }
    else if ( category == MEDIA )
    {
        if ( list_curpos[MEDIA] > gui->media_max_files_per_page - 1 )
        {
            page[MEDIA] -= 1;
            list_curpos[MEDIA] = page[MEDIA] * gui->media_max_files_per_page;
            list_start[MEDIA] = page[MEDIA] * gui->media_max_files_per_page;
            reset_scroll_count = 1;
        }
        else
        {
            list_curpos[category] = 0;
            list_start[category] = 0;
            reset_scroll_count = 1;
        }
    }
    else
    {
        if (  list_curpos[category] != 0 )
        {
            gui_unload_preview();

            if ( list_curpos[category] > gui->max_app_per_page - 1 )
            {
                page[category] -= 1;
                list_curpos[category] = page[category] * gui->max_app_per_page;
                list_start[category] = page[category] * gui->max_app_per_page;
                reset_scroll_count = 1;
            }
            else
            {
                list_curpos[category] = 0;
                list_start[category] = 0;
                reset_scroll_count = 1;
            }
        }
    }
}

void gui_pad_right()
{
    if ( category == SETTINGS )
    {
        if ( setting_current == MENU_SKIN )
        {
            if ( skin_current < skin_count - 1 )
            {
                skin_current++;
            }

            load_skin_preview();
        }
    }
    else if ( category == MEDIA )
    {
        if ( ( gui->media_max_files_per_page * ( page[MEDIA] + 1 ) ) < list_num[MEDIA] )
        {
            page[MEDIA] += 1;
            list_curpos[MEDIA] = page[MEDIA] * gui->media_max_files_per_page;
            list_start[MEDIA] = page[MEDIA] * gui->media_max_files_per_page;
            reset_scroll_count = 1;
        }
    }
    else
    {
        if ( ( gui->max_app_per_page * ( page[category] + 1 ) ) < list_num[category] )
        {
            gui_unload_preview();
            page[category] += 1;
            list_curpos[category] = page[category] * gui->max_app_per_page;
            list_start[category] = page[category] * gui->max_app_per_page;
            reset_scroll_count = 1;
        }
    }
}

void gui_pad_up()
{
    if ( category == SETTINGS )
    {
        if ( setting_current > 0 )
            setting_current--;
    }
    else if ( category == MEDIA )
    {
        if ( list_curpos[MEDIA] > 0 )
        {
            if ( list_curpos[MEDIA] == ( page[MEDIA] * gui->media_max_files_per_page ) )
            {
                page[MEDIA] -= 1;
                list_curpos[MEDIA] -= 1;
                list_start[MEDIA] = page[MEDIA] * gui->media_max_files_per_page;
            }
            else
            {
                list_curpos[MEDIA] -= 1;
            }
        }
    }
    else
    {
        if ( list_curpos[category] > 0 )
        {
            gui_unload_preview();

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
        }
    }
}

void gui_pad_down()
{
    if ( category == SETTINGS )
    {
        if ( setting_current < SETTINGS_COUNT - 1 )
            setting_current++;
    }
    else if ( category == MEDIA )
    {
        if (list_curpos[MEDIA] < list_num[MEDIA] - 1 )
        {
            if (list_curpos[MEDIA] == ( gui->media_max_files_per_page + ( page[MEDIA] * gui->media_max_files_per_page ) ) - 1)
            {
                page[MEDIA] += 1;
                list_curpos[MEDIA] = page[MEDIA] * gui->media_max_files_per_page;
                list_start[MEDIA] = page[MEDIA] * gui->media_max_files_per_page;
            }
            else
            {
                list_curpos[MEDIA] += 1;
            }
        }
    }
    else
    {
        if (list_curpos[category] < list_num[category] - 1 )
        {
            gui_unload_preview();

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
        }
    }
}

void handle_dpad()
{
    GLES2D_HandleEvents( 90 );

    if ( GLES2D_PadPressed ( MENU ) || GLES2D_KeyboardPressed( XK_Escape ) )
    {
        do_quit = 1;
    }

    else if ( GLES2D_PadPressed ( L ) || GLES2D_KeyboardPressed( XK_F1 ) )
    {
        if( category > 0) gui_change_category( category - 1 );
            else gui_change_category( CATEGORY_COUNT - 1 );

        if ( !pmenu->effect )
        {
            gui_draw();
            GLES2D_SwapBuffers();
        }
    }

    else if ( GLES2D_PadPressed ( R ) || GLES2D_KeyboardPressed( XK_F2 ) )
    {
        if( category < CATEGORY_COUNT - 1) gui_change_category( category + 1 );
            else gui_change_category( 0 );

        if ( !pmenu->effect )
        {
            gui_draw();
            GLES2D_SwapBuffers();
        }
    }

    else if( GLES2D_PadHold ( PAD_LEFT ) )
    {
        if ( category == SETTINGS )
        {
            if ( setting_current == MENU_CPU )
            {
                if ( pmenu->cpu_mhz > 30 )
                {
                    pmenu->cpu_mhz-=5;
                    set_cpu( pmenu->cpu_mhz );
                }
            }
            else if ( setting_current == MENU_BRIGHTNESS )
            {
                if ( pmenu->brightness > 10 )
                {
                    pmenu->brightness -= 5;
                    set_brightness( pmenu->brightness );
                }
            }
            else if ( setting_current == MENU_EFFECT )
            {
                pmenu->effect = !pmenu->effect;
            }
        }

        if ( !pmenu->effect )
        {
            gui_draw();
            GLES2D_SwapBuffers();
        }
    }

    else if( GLES2D_PadPressed ( PAD_LEFT ) || GLES2D_KeyboardPressed( XK_Left ) )
    {
        gui_pad_left();

        if ( !pmenu->effect )
        {
            gui_draw();
            GLES2D_SwapBuffers();
        }
    }

    else if( GLES2D_PadHold ( PAD_RIGHT ) )
    {
        if ( category == SETTINGS )
        {
            if ( setting_current == MENU_CPU )
            {
                if ( pmenu->cpu_mhz < 800 )
                {
                    pmenu->cpu_mhz += 5;
                    set_cpu( pmenu->cpu_mhz );
                }
            }
            else if ( setting_current == MENU_BRIGHTNESS )
            {
                if ( pmenu->brightness < 56 )
                {
                    pmenu->brightness += 5;
                    set_brightness( pmenu->brightness );
                }
            }
            else if ( setting_current == MENU_EFFECT )
            {
                pmenu->effect = !pmenu->effect;
            }
        }

        if ( !pmenu->effect )
        {
            gui_draw();
            GLES2D_SwapBuffers();
        }
    }

    else if( GLES2D_PadPressed ( PAD_RIGHT ) || GLES2D_KeyboardPressed( XK_Right ) )
    {
        gui_pad_right();

        if ( !pmenu->effect )
        {
            gui_draw();
            GLES2D_SwapBuffers();
        }
    }

    else if( GLES2D_PadHold ( PAD_UP ) || GLES2D_KeyboardPressed( XK_Up ) )
    {
        gui_pad_up();

        if ( !pmenu->effect )
        {
            gui_draw();
            GLES2D_SwapBuffers();
        }
    }

    else if( GLES2D_PadHold ( PAD_DOWN ) || GLES2D_KeyboardPressed( XK_Down ) )
    {
        gui_pad_down();

        if ( !pmenu->effect )
        {
            gui_draw();
            GLES2D_SwapBuffers();
        }
    }

    else if ( GLES2D_PadPressed ( A ) || GLES2D_KeyboardPressed( XK_l ) )
    {
        GLES2D_Pad[ A ] = 0;

        if( ( category != SETTINGS ) && ( category != MEDIA ) )
        {
            if ( list_num[category] )
                gui_app_exec( list_curpos[ category ] );
        }

        if ( !pmenu->effect )
        {
            gui_draw();
            GLES2D_SwapBuffers();
        }
    }

    else if ( GLES2D_PadPressed ( B ) )
    {
        if ( now_depth > 0 )
        {
            int i;

            for( i = 0; i < MAX_PATH; i++ )
                if ( now_path[i] == 0 ) break;

            i--;

            while( i > 4 )
            {
                if ( now_path[i] == '/' )
                {
                    now_path[i] = 0;
                    break;
                }
                i--;
            }
            now_depth--;
            get_media_list( now_path );
        }
    }

    else if ( GLES2D_PadPressed ( X ) || GLES2D_KeyboardPressed( XK_space ) )
    {
        if ( category == FAVORITES )
        {
                if ( list_num[category] )
                {
                    char tmpStr[512];
                    sprintf(tmpStr, "Do you want to remove %s from your favourite applications ?", applications[category]->name[list_curpos[ category ]] );

                    if( gui_confirm_box( tmpStr ) )
                        cfg_fav_del( applications[category]->id[list_curpos[ category ]] );
                }
        }
        else if ( category == SETTINGS  )
        {
            if ( setting_current == MENU_SKIN )
            {
                gui_change_skin();
            }
            else if ( setting_current == MENU_CPU )
            {
                cfg_pmenu_update_cpu_mhz( pmenu->cpu_mhz );
                set_cpu( pmenu->cpu_mhz );
            }
            else if ( setting_current == MENU_EFFECT )
            {
                cfg_pmenu_update_effect( pmenu->effect );
            }
            else if ( setting_current == MENU_EXIT )
            {
                do_quit = 1;
            }
            else if ( setting_current == MENU_POWEROFF )
            {
		int ret;
		ret = system( "poweroff" );
		}
        }
        else if ( category == MEDIA )
        {
            if ( now_depth < MAX_DEPTH )
            {
                if ( applications[MEDIA]->type[list_curpos[MEDIA]] == IS_DIR )
                {
                    if ( list_num[MEDIA] )
                    {
                        strcat( now_path, "/" );
                        strcat( now_path, applications[MEDIA]->name[list_curpos[MEDIA]] );
                        now_depth++;
                        get_media_list( now_path );
                    }
                }
                else if ( is_avi( applications[MEDIA]->name[list_curpos[MEDIA]] ) )
                {
                    char video_path[1024];
                    strcpy( video_path, now_path );
                    strcat( video_path, "/" );
                    strcat( video_path, applications[MEDIA]->name[list_curpos[MEDIA]] );
                    video_play( video_path );
                }
            }
        }
        else
        {
            if ( list_num[category] )
            {
                if ( applications_count[FAVORITES] < FAV_MAX )
                {
                    char tmpString[256];
                    sprintf(tmpString, "Do you want to add %s to your favourite applications ?", applications[category]->name[list_curpos[ category ]] );

                    if (  gui_confirm_box( tmpString ) )
                    {
                        cfg_fav_add( applications[category]->id[list_curpos[ category ]] );
                    }
                }
                else
                {
                    gui_confirm_box("Favorites are full, please remove a favourite before adding one by going under the favourite screen.");
                }
            }
        }

        if ( !pmenu->effect )
        {
            gui_draw();
            GLES2D_SwapBuffers();
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

	gui_load();

	GLES2D_FpsCounterInit();

    nh_countdown = 60;
    debug_func = 0;

    strcpy( now_path, "/media" );

	while( ! gui_done )
	{
        check_rediscover();

		handle_dpad();

		if ( pmenu->effect )
		{
            gui_draw();
            GLES2D_DrawFont( fnt[SMALL], 750, 0, GLES2D_GetFpsChar() );
            GLES2D_SwapBuffers();
            GLES2D_FpsCounterUpdate();
		}

		if( do_quit )
            gui_done = 1;

		//usleep( 10000 );
	}

	gui_clean();

	system( "echo startxfce4 > /tmp/gui.load" );

	return 0;
}

