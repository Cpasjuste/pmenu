#ifndef __GET_APPS_H__
#define __GET_APPS_H__

#define MAXDEPTH 16
#define MAXDIRNUM 1024
#define MAXPATH 64

#define EMULATORS 0
#define GAMES 1
#define APPLICATIONS 2
#define FAVORITES 3

int applications_count[3], list_num[3], list_start[3], list_curpos[3], now_depth[3];
int category;

int pnd_app_get_list(void);

typedef struct
{
	char name[256][256];
	char path[256][256];
	char id[256][256];
	char category[256][256];
	char exec_path[256][256];
	char exec_name[256][256];
	char icon[256][256];
	char description[256][256];
} PND_APP;

PND_APP *applications[3];

#endif
