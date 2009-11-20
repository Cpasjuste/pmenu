#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

#include "main.h"
#include "get_apps.h"
#include "menu_settings.h"
#include "gui_config.h"
#include "pmenu_config.h"
#include "common.h"

GLES2D_Texture *skin_preview_pic;

int setting_y[SETTINGS_COUNT] = { 130, 160, 190, 220 };

int load_skin_preview()
{
    debug_start();

    char preview_path[512];
    sprintf( preview_path, "%s/preview.bmp", skin[skin_current]->path );

    if ( skin_preview_pic != NULL )
        GLES2D_FreeTexture( skin_preview_pic );

    if ( ( skin_preview_pic = GLES2D_CreateTexture( preview_path, 0 ) ) == NULL )
    {
        debug_errorf( "Could not load skin preview pic (%s);", preview_path );
    }

    debug_end();

    return 1;
}

int get_skins_list ( )
{
    debug_start();

    DIR *dp;
    struct dirent *ep;

    skin_count = 0;
    skin_current = 0;
    setting_current = 0;

    int i;

    for ( i = 0; i < 256; i++ )
    {
        if ( skin[i] != NULL ) free ( skin[i] );
        skin[i] = (SKIN_CONFIG *) malloc(sizeof(SKIN_CONFIG));
    }

    dp = opendir ("skins");
    if ( dp != NULL )
    {
        while ( ( ep = readdir (dp) ) )
        {
            if ( ep->d_name[0] == '.' ) continue;

            char _path[512];
            sprintf( _path, "skins/%s/gui.cfg", ep->d_name );

            if ( access ( _path, R_OK ) != 0 ) continue;

            strcpy ( skin[skin_count]->cfg_path, _path );
            strcpy ( skin[skin_count]->name, ep->d_name );
            sprintf ( skin[skin_count]->path, "skins/%s", ep->d_name );

            debug_infof( "Skin found ( Num: %i, Name: %s, Path: %s, Config Path: %s )", skin_count, skin[skin_count]->name, skin[skin_count]->path, skin[skin_count]->cfg_path );

            skin_count++;
        }
        closedir (dp);
    }
    else
    {
        debug_error ( "ERROR: Couldn't open the skin directory" );
        debug_end();
        return 0;
    }

    debug_end();
    return 1;
}


char *setting_string[SETTINGS_COUNT] =
{
    "Skin:",
    "Cpu Speed:",
    "Brightness:",
    "Exit"
};

void settings_draw()
{
    int i;

    for ( i = 0; i < SETTINGS_COUNT; i++ )
    {
        GLES2D_DrawFont( small, 50, setting_y[i], setting_string[i] );

        if ( i == setting_current )
            GLES2D_SetFontColor( small, 0, 255, 200, 255 );

        if ( i == MENU_SKIN )
        {
            GLES2D_DrawFont( small, GLES2D_GetTextWidth( small, setting_string[i] ) + 55, setting_y[i], skin[skin_current]->name );

            if ( skin_preview_pic != NULL )
                GLES2D_DrawTextureScaledCentered( skin_preview_pic, gui->preview_pic_x, gui->preview_pic_y, 330, \
                    200 );
        }
        if ( i == MENU_CPU )
        {
            char cpu_str[3];
            sprintf( cpu_str, "%i", pmenu->cpu_mhz );

            GLES2D_DrawFont( small, GLES2D_GetTextWidth( small, setting_string[i] ) + 55, setting_y[i], cpu_str );
        }

        GLES2D_SetFontColor( small, 255, 255, 255, 255 );
    }
}

