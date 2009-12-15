#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

#include "main.h"
#include "get_apps.h"
#include "category_settings.h"
#include "config_skin.h"
#include "config_pmenu.h"
#include "common.h"

GLES2D_Texture *skin_preview_pic;

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

    dp = opendir ( PMENU_PATH"/skins" );

    if ( dp != NULL )
    {
        while ( ( ep = readdir (dp) ) )
        {
            if ( ep->d_name[0] == '.' ) continue;

            char _path[512];

            sprintf( _path, PMENU_PATH"/skins/%s/skin.cfg", ep->d_name );

            if ( access ( _path, R_OK ) != 0 ) continue;

            strcpy ( skin[skin_count]->cfg_path, _path );
            strcpy ( skin[skin_count]->name, ep->d_name );
            sprintf ( skin[skin_count]->path, PMENU_PATH"/skins/%s", ep->d_name );

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

void menu_settings_draw()
{
    int i;
    int setting_y = gui->applications_box_y + 5 + gui->settings_text_spacing * 2;
    int x = gui->applications_box_x + 5;

    for ( i = 0; i < SETTINGS_COUNT; i++ )
    {
        GLES2D_DrawFont( fnt[SMALL], x, gui->applications_box_y + 5, "Press X to apply setting" );

        if ( ( i == setting_current && i == MENU_EXIT ) )
            GLES2D_SetFontColor( fnt[SMALL], HEXTOR(gui->font_small_color_highlight), HEXTOG(gui->font_small_color_highlight), HEXTOB(gui->font_small_color_highlight), HEXTOA(gui->font_small_color_highlight) );

        GLES2D_DrawFont( fnt[SMALL], x, setting_y, setting_string[i] );

        if ( i == setting_current )
            GLES2D_SetFontColor( fnt[SMALL], HEXTOR(gui->font_small_color_highlight), HEXTOG(gui->font_small_color_highlight), HEXTOB(gui->font_small_color_highlight), HEXTOA(gui->font_small_color_highlight) );

        if ( i == MENU_SKIN )
        {
            GLES2D_DrawFont( fnt[SMALL], GLES2D_GetTextWidth( fnt[SMALL], setting_string[i] ) + x + 5, setting_y, skin[skin_current]->name );

            if ( skin_preview_pic != NULL )
                GLES2D_DrawTextureScaledCentered( skin_preview_pic, gui->preview_pic_x, gui->preview_pic_y, gui->preview_pic_w, gui->preview_pic_w / 1.666 );

        }
        else if ( i == MENU_CPU )
        {
            char cpu_str[3];
            sprintf( cpu_str, "%i", pmenu->cpu_mhz );

            GLES2D_DrawFont( fnt[SMALL], GLES2D_GetTextWidth( fnt[SMALL], setting_string[i] ) + x + 5, setting_y, cpu_str );
        }
        else if ( i == MENU_BRIGHTNESS )
        {
            char brightness_str[3];
            sprintf( brightness_str, "%i", pmenu->brightness );

            GLES2D_DrawFont( fnt[SMALL], GLES2D_GetTextWidth( fnt[SMALL], setting_string[i] ) + x + 5, setting_y, brightness_str );
        }

        GLES2D_SetFontColor( fnt[SMALL], HEXTOR(gui->font_small_color), HEXTOG(gui->font_small_color), HEXTOB(gui->font_small_color), HEXTOA(gui->font_small_color) );

        setting_y += gui->settings_text_spacing;
    }
}

