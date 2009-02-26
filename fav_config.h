#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <libconfig.h>

struct config_t cfg;

#define FAV_MAX 1024

typedef struct
{
	int enabled[FAV_MAX];
	char name[FAV_MAX][256];
	char fullpath[FAV_MAX][256];
	char exec_name[FAV_MAX][256];
	char icon[FAV_MAX][256];
	char description[FAV_MAX][256];

} CONFIG;

CONFIG *fav;

int cfg_fav_read();
int cfg_fav_add(int fav_number, char *name, char *fullpath, char *exec_name, char *icon, char *description);
int cfg_fav_del(int fav_number);

#endif
