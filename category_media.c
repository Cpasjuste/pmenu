#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "main.h"
#include "get_apps.h"
#include "category_settings.h"
#include "category_media.h"
#include "config_skin.h"
#include "config_pmenu.h"
#include "utils.h"
#include "common.h"

int get_media_list ( char *directory )
{
    debug_start();

    applications_count[MEDIA] = 0;
    list_num[MEDIA] = 0;
    list_start[MEDIA] = 0;
    list_curpos[MEDIA] = 0;

    if ( applications[MEDIA] != NULL )
        free ( applications[MEDIA] );

    applications[MEDIA] = (PND_APP *) malloc( sizeof( PND_APP ) );

    DIR *dp;
    struct dirent *ep;
    struct stat info;
    char temp_path[MAX_PATH];
    memset( temp_path, 0, MAX_PATH );

    dp = opendir ( directory );
    if ( dp != NULL )
    {
        while ( ( ep = readdir (dp) ) )
        {
            if ( ep->d_name[0] == '.' ) continue;

            sprintf( temp_path, "%s/%s", directory, ep->d_name );
            stat( temp_path, &info );

            if ( S_ISDIR( info.st_mode ) )
            {
                strcpy( applications[MEDIA]->name[applications_count[MEDIA]], ep->d_name );
                strcpy( applications[MEDIA]->fullpath[applications_count[MEDIA]], temp_path );
                applications[MEDIA]->type[applications_count[MEDIA]] = IS_DIR;
                debug_infof( "DIR => %s", applications[MEDIA]->fullpath[applications_count[MEDIA]] );
            }
            else if ( S_ISREG( info.st_mode ) )
            {
                if ( ! is_avi( ep->d_name ) ) continue;

                strcpy( applications[MEDIA]->name[applications_count[MEDIA]], ep->d_name );
                strcpy( applications[MEDIA]->fullpath[applications_count[MEDIA]], temp_path );
                applications[MEDIA]->type[applications_count[MEDIA]] = IS_FILE;
                debug_infof( "FILE => %s", applications[MEDIA]->fullpath[applications_count[MEDIA]] );
            }

            applications_count[MEDIA]++;
        }
        closedir (dp);
    }
    else
    {
        debug_errorf ( "ERROR: Couldn't open the directory (%s)", directory );
        debug_end();
        return 0;
    }

    list_num[MEDIA] = applications_count[MEDIA];
    if ( list_start[MEDIA] >= list_num[MEDIA]) { list_start[MEDIA] = list_num[MEDIA] - 1; }
    if ( list_start[MEDIA] < 0) { list_start[MEDIA]  = 0; }
    if ( list_curpos[MEDIA] >= list_num[MEDIA]) { list_curpos[MEDIA] = list_num[MEDIA] - 1; }
    if ( list_curpos[MEDIA] < 0) { list_curpos[MEDIA] = 0; }

    debug_infof( "now_depth = %i\n", now_depth );
    debug_end();
    return 1;
}

void menu_media_input()
{

}

void menu_media_draw()
{
        int y = gui->applications_box_y + 10;
		int i = list_start[MEDIA];

		while ( i < ( list_start[MEDIA] + gui->media_max_files_per_page ) )
		{
			if ( i < list_num[MEDIA] )
			{
				if ( i == list_curpos[MEDIA] )
				{
                    GLES2D_SetFontColor( fnt[MEDIA_FNT], HEXTOR(gui->media_text_color_highlight), HEXTOG(gui->media_text_color_highlight), HEXTOB(gui->media_text_color_highlight), HEXTOA(gui->media_text_color_highlight) );
					GLES2D_DrawFontCut( fnt[MEDIA_FNT], gui->applications_box_x + 5 + media_folder_icon->dst->w, y, gui->applications_box_x + gui->applications_box_w, applications[MEDIA]->name[i] );
					GLES2D_SetFontColor( fnt[MEDIA_FNT], HEXTOR(gui->media_text_color), HEXTOG(gui->media_text_color), HEXTOB(gui->media_text_color), HEXTOA(gui->media_text_color) );
				}
				else
				{
				    GLES2D_SetFontColor( fnt[MEDIA_FNT], HEXTOR(gui->media_text_color), HEXTOG(gui->media_text_color), HEXTOB(gui->media_text_color), HEXTOA(gui->media_text_color) );
					GLES2D_DrawFontCut( fnt[MEDIA_FNT], gui->applications_box_x + 5 + media_folder_icon->dst->w, y, gui->applications_box_x + gui->applications_box_w, applications[MEDIA]->name[i] );
				}

				if ( applications[MEDIA]->type[i] == IS_DIR )
                    GLES2D_DrawTextureCentered( media_folder_icon, gui->applications_box_x + 15, y + fnt[MEDIA_FNT]->height / 2 );
                else
                    GLES2D_DrawTextureCentered( media_file_icon, gui->applications_box_x + 15, y + fnt[MEDIA_FNT]->height / 2 );

			}
			i++;
			y += media_folder_icon->dst->w;
		}
}
