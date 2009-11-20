#ifndef __PMENU_CONFIG_H__
#define __PMENU_CONFIG_H__

#include "libconfig.h"
#include "get_apps.h"

struct config_t cfg;

typedef struct
{
	char skin_dir[512];
	int cpu_mhz;

} PMENU_CONFIG;

PMENU_CONFIG *pmenu;

int cfg_pmenu_read();
int cfg_pmenu_update_skin_path( char *skin_path );
int cfg_pmenu_update_cpu_mhz( int mhz );

#endif

