#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "graphics.h"
#include <GLES/glues.h>

float xrot = 0.0f;
float yrot = 0.0f;
float zrot = 0.0f;

float _randfloat()
{
	srand((unsigned)(time(0))); 
	return rand()/((float)(RAND_MAX)+1);
} 

float randfloat(float min, float max)
{
	if (min>max)
	{
		return _randfloat()*(min-max)+max;    
	}
	else
	{
		return _randfloat()*(max-min)+min;
	}    
} 

GLfloat box[] = {
	// FRONT
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	// BACK
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	// LEFT
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	// RIGHT
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	// TOP
	-0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 -0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	// BOTTOM
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
};

GLfloat texCoords[] = {
	// FRONT
	 0.0f, 0.0f,
	 1.0f, 0.0f,
	 0.0f, 1.0f,
	 1.0f, 1.0f,
	// BACK
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 0.0f,
	 0.0f, 1.0f,
	// LEFT
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 0.0f,
	 0.0f, 1.0f,
	// RIGHT
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 0.0f,
	 0.0f, 1.0f,
	// TOP
	 0.0f, 0.0f,
	 1.0f, 0.0f,
	 0.0f, 1.0f,
	 1.0f, 1.0f,
	// BOTTOM
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 0.0f,
	 0.0f, 1.0f
};

float lightAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };

float matAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };

int init_3D_view()
{
	glDisable(GL_BLEND);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	return 1;
}

void draw_3d_box(TEXTURE *tex, int x, int y, GLfloat z)
{
	init_3D_view();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glViewport( x - 400, 240 - y, 800, 480);
	gluPerspectivef(45.0f, 1.0f * 800 / 480, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, tex->texture );

	glVertexPointer(3, GL_FLOAT, 0, box);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	xrot += randfloat(0.0f, 3.0f);
	yrot += randfloat(0.0f, 3.0f);
	zrot += randfloat(0.0f, 3.0f);

 	gluLookAtf(
		z, 0.0f, 3.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 1.0f, 0.0f, 1.0f);

	// FRONT AND BACK
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

	// LEFT AND RIGHT
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

   	// TOP AND BOTTOM
   	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
   	glNormal3f(0.0f, 1.0f, 0.0f);
   	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
   	glNormal3f(0.0f, -1.0f, 0.0f);
   	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

	init_2D_view();
}


