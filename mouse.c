#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_framerate.h>
#include <SDL_rotozoom.h>

#include "mouse.h"

int get_mouse_click(int button)
{
	SDL_PumpEvents();
	switch(button)
	{
		case MOUSE_LEFT:
			if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
			return 1;
		break;

		case MOUSE_MIDDLE:
			if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(2))
			return 1;
		break;

		case MOUSE_RIGHT:
			if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3))
			return 1;
		break;
	}
	return 0;
}

void get_mouse_loc()
{
	SDL_PumpEvents();
	SDL_GetMouseState(&MOUSE_X, &MOUSE_Y);
}

void set_mouse_loc(int x, int y)
{
	SDL_WarpMouse(x, y);
}

int mouse_is_over(int x1, int x2, int y1, int y2)
{
	if(MOUSE_X < x1) return 0;
	if(MOUSE_X > x2) return 0;
	if(MOUSE_Y < y1) return 0;
	if(MOUSE_Y > y2) return 0;
	return 1;
}

int mouse_is_over_surface_center(int x, int y, int w, int h)
{
	if(MOUSE_X > (x + (w / 2))) return 0;
	if(MOUSE_X < (x - (w / 2))) return 0;
	if(MOUSE_Y > (y + (h / 2))) return 0;
	if(MOUSE_Y < (y - (h / 2))) return 0;
	return 1;
}

int mouse_is_over_surface(int x, int y, int w, int h)
{
    if(MOUSE_X < x) return 0;
    if(MOUSE_X > (x + w)) return 0;
    if(MOUSE_Y < y) return 0;
    if(MOUSE_Y > (y + h)) return 0;
    return 1;
}
