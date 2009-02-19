#ifndef __GUI_CONFIG_H__
#define __GUI_CONFIG_H__

#include <libconfig.h>

struct config_t cfg;


typedef struct
{
	int applications_box_x;
	int applications_box_y;
	int arrow_left_x;
	int arrow_left_y;
	int arrow_right_x;
	int arrow_right_y;

} GUI_CONFIG;

GUI_CONFIG *gui;

int cfg_gui_read();

#endif


