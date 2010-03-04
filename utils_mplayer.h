#ifndef __UTILS_MPLAYER_H__
#define __UTILS_MPLAYER_H__

void video_play( char *filename );
void video_play_preview( char *filename );
void video_quit( );
void video_pause( );
void video_fullscreen( );
void video_seek_plus();
void video_seek_minus();

int video_playing;

#endif
