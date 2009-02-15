#ifndef __MOUSE_H__
#define __MOUSE_H__

int get_mouse_click(int button);
void get_mouse_loc();
void set_mouse_loc(int x, int y);
int mouse_is_over(int x1, int x2, int y1, int y2);
int mouse_is_over_surface_center(int x, int y, int w, int h);
int mouse_is_over_surface(int x, int y, int w, int h);

enum {MOUSE_LEFT = 1, MOUSE_RIGHT, MOUSE_MIDDLE};
int MOUSE_X, MOUSE_Y; 

#endif
