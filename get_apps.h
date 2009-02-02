#ifndef __GET_APPS_H__
#define __GET_APPS_H__

typedef struct
{
	char name[256][256];
	char id[256][256];
	char category[256][256];
	char exec[256][256];
	char icon[256][256];
} PND_APP;

PND_APP *application;

int pnd_app_count;
int pnd_app_get_list(void);

#define MAXDEPTH 16
#define MAXDIRNUM 1024
#define MAXPATH 64

int app_list_num, app_list_start, app_list_curpos, app_cbuf_start[MAXDEPTH], app_cbuf_curpos[MAXDEPTH], app_now_depth;

#endif
