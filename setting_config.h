#ifndef __SETTING_CONFIG_H__
#define __SETTING_CONFIG_H__

#include <libconfig.h>

struct config_t cfg;

typedef struct
{
	int x11_mode;

} SETTING_CONFIG;

SETTING_CONFIG *setting;

int cfg_setting_read();

#endif


