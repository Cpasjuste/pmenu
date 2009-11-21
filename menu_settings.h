#ifndef __MENU_SETTINGS_H__
#define __MENU_SETTINGS_H__

#define SETTINGS_COUNT 4
#define MENU_SKIN 0
#define MENU_CPU 1
#define MENU_BRIGHTNESS 2
#define MENU_EXIT 3

void settings_draw();
int get_skins_list ();
int load_skin_preview();

int setting_current;
int skin_previous;
int skin_current;
int skin_count;


typedef struct
{
	char cfg_path[512];
	char name[512];
	char path[512];

} SKIN_CONFIG;

SKIN_CONFIG *skin[256];

#endif


