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
#include "config_skin.h"
#include "config_pmenu.h"
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

    dp = opendir ( directory );
    if ( dp != NULL )
    {
        while ( ( ep = readdir (dp) ) )
        {
            if ( ep->d_name[0] == '.' ) continue;

            strcpy( applications[MEDIA]->name[applications_count[MEDIA]], ep->d_name );
            sprintf( applications[MEDIA]->fullpath[applications_count[MEDIA]], "%s/%s", directory, ep->d_name );

            stat( applications[MEDIA]->fullpath[applications_count[MEDIA]], &info );

            if ( S_ISDIR( info.st_mode ) )
            {
                applications[MEDIA]->type[applications_count[MEDIA]] = IS_DIR;
                debug_infof( "DIR => %s", applications[MEDIA]->fullpath[applications_count[MEDIA]] );
            }
            else if ( S_ISREG( info.st_mode ) )
            {
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

    debug_end();
    return 1;
}

void menu_media_draw()
{
        int y = gui->applications_box_y + 5;
		int i = list_start[MEDIA];

		char tmpStr[256];

		while ( i < ( list_start[MEDIA] + gui->media_max_files_per_page ) )
		{
			if ( i < list_num[MEDIA] )
			{
				if ( i == list_curpos[MEDIA] )
				{
					memset(tmpStr, 0, 256);
					strncpy(tmpStr, applications[MEDIA]->name[i], 29);

                    GLES2D_SetFontColor( fnt[MEDIA_FNT], HEXTOR(gui->media_text_color_highlight), HEXTOG(gui->media_text_color_highlight), HEXTOB(gui->media_text_color_highlight), HEXTOA(gui->media_text_color_highlight) );
					GLES2D_DrawFont( fnt[MEDIA_FNT], gui->applications_box_x + 5, y, tmpStr );
					GLES2D_SetFontColor( fnt[MEDIA_FNT], HEXTOR(gui->media_text_color), HEXTOG(gui->media_text_color), HEXTOB(gui->media_text_color), HEXTOA(gui->media_text_color) );
				}
				else
				{
				    GLES2D_SetFontColor( fnt[MEDIA_FNT], HEXTOR(gui->media_text_color), HEXTOG(gui->media_text_color), HEXTOB(gui->media_text_color), HEXTOA(gui->media_text_color) );

					memset(tmpStr, 0, 256);
					strncpy(tmpStr, applications[MEDIA]->name[i], 29);
					GLES2D_DrawFont( fnt[MEDIA_FNT], gui->applications_box_x + 5, y, tmpStr );
				}
			}
			i++;
			y += fnt[MEDIA_FNT]->lineskip;
		}
}
