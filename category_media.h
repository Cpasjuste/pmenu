#ifndef __CATEGORY_MEDIA_H__
#define __CATEGORY_MEDIA_H__

char now_path[1024];
int now_depth;
#define MAX_DEPTH 256
#define MAX_PATH 1024

int get_media_list ( char *directory );
void menu_media_draw();

#endif
