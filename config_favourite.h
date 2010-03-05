#ifndef __CONFIG_FAVOURITE_H__
#define __CONFIG_FAVOURITE_H__

#include "libconfig.h"

#define FAV_MAX 256

struct config_t cfg;

int cfg_fav_exist( char *id );
int cfg_fav_add( char *id );
int cfg_fav_del( char *id );

#endif
