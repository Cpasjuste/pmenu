#ifndef __GET_APPS_H__
#define __GET_APPS_H__

#include <pnd_pndfiles.h>
#include <GLES2D/GLES2D.h>

#define MAXDIRNUM 1024
#define MAXPATH 64

#define CATEGORY_COUNT 6

#define EMULATORS 0
#define GAMES 1
#define MISC 2
#define FAVORITES 3
#define MEDIA 4
#define SETTINGS 5

#define IS_DIR 0
#define IS_FILE 1
#define IS_PND 2

int applications_count[CATEGORY_COUNT-1], list_num[CATEGORY_COUNT-1], \
	 list_start[CATEGORY_COUNT-1], list_curpos[CATEGORY_COUNT-1];

int category;

int category_icon_x[CATEGORY_COUNT], category_icon_y[CATEGORY_COUNT];

int pnd_app_get_list(void);

char *run_searchpath;
char *run_script;
char *pndrun;

typedef struct
{
    int fav_num[256];
	char name[256][256];
	char id[256][256];
	char category[256][256];
	char fullpath[256][256];
	char exec_name[256][256];
	char description[256][256];
	char preview_pic1[256][256];
	char preview_pic1_name[256][256];
	int clock[256];
	int noX[256];
	int scale[256];
	int type[256];

	GLES2D_Texture *icon[256];
	GLES2D_FontCache *name_cached[256];
	GLES2D_FontCache *description_cached[256];

} PND_APP;

PND_APP *applications[CATEGORY_COUNT-1];

#endif
