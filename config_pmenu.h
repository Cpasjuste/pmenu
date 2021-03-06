#ifndef __CONFIG_PMENU_H__
#define __CONFIG_PMENU_H__

#include "libconfig.h"
#include "get_apps.h"

struct config_t cfg;

typedef struct
{
	char skin_dir[512];
	char skin_dir_relative[512];
	int cpu_mhz;
	int brightness;
	int effect;

} PMENU_CONFIG;

PMENU_CONFIG *pmenu;

int cfg_pmenu_read();
int cfg_pmenu_update_skin_path( char *skin_path );
int cfg_pmenu_update_cpu_mhz( int mhz );
int cfg_pmenu_update_effect( int value );

#endif


