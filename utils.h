#ifndef __UTILS_H__
#define __UTILS_H__

char *str_sub (const char *s, unsigned int start, unsigned int end);
void error(char *msg);
char* get_time_string(int mode24);
int copy(char *src, char *dst);
char *disk_space(char *path);
void set_cpu( int mhz );

#define TIME_24   1
#define TIME_12   0

#endif
