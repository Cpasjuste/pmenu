#ifndef __CONFIG_FAVOURITE_H__
#define __CONFIG_FAVOURITE_H__

#include "libconfig.h"

#define FAV_MAX 128

struct config_t cfg;

int cfg_fav_read();
int cfg_fav_add( char *name, char *id, char *category, char *cache_path, char *fullpath, char *exec_name, char *_icon, char *description, char *preview_pic1, char *preview_pic2 );
int cfg_fav_del( int fav );

char cfg_fav_path[10][512];
int cfg_fav_count;

#endif
