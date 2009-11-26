#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <sys/statvfs.h>

#include <GLES2D/GLES2D.h>
#include "common.h"
#include "config_pmenu.h"
#include "utils_mplayer.h"

extern void gui_draw();

void video_quit( )
{
    system( "echo quit > /tmp/mplayer-input" );

    system( "rm /tmp/mplayer-input" );

    video_playing = 0;
}

void video_pause( )
{
    system( "echo p > /tmp/mplayer-input" );
}

void video_fullscreen( )
{
    system( "echo vo_fullscreen > /tmp/mplayer-input" );
}

void video_seek_plus()
{
    system( "echo seek +600 0 > /tmp/mplayer-input" );
}

void video_seek_minus()
{
    system( "echo seek -600 0 > /tmp/mplayer-input" );
}

void video_play( char *filename )
{
    debug_start();

    char cmd[512];
    memset( cmd, 0, 512 );

    video_quit( );

    system( "mkfifo /tmp/mplayer-input" );

    sprintf( cmd, "mplayer -slave -input file=/tmp/mplayer-input -geometry 435:175 -monitoraspect 1.6666 -display :0 -zoom -xy 330 %s &", filename );

    system( cmd );

    video_playing = 1;

    while( 1 )
    {
        GLES2D_HandleEvents( 90 );

        if ( GLES2D_PadPressed ( B ) )
        {
            video_pause();
        }

        if ( GLES2D_PadPressed ( X ) )
        {
            video_fullscreen();
        }
        if ( GLES2D_PadPressed( PAD_LEFT ) )
        {
            video_seek_minus();
        }
        if ( GLES2D_PadPressed( PAD_RIGHT ) )
        {
            video_seek_plus();
        }
        if ( GLES2D_PadPressed ( MENU ) )
        {
            video_quit();
            break;
        }

        gui_draw();
        GLES2D_SwapBuffers();
    }
    debug_end();
}

void video_play_preview( char *filename )
{
    debug_start();

    char cmd[512];
    memset( cmd, 0, 512 );

    video_quit( );

    system( "mkfifo /tmp/mplayer-input" );

    sprintf( cmd, "mplayer -slave -input file=/tmp/mplayer-input -geometry 435:175 -monitoraspect 1.6666 -display :0 -zoom -xy 330 %s &", filename );

    system( cmd );

    video_playing = 1;

    debug_end();
}


