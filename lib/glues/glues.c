/*
 * GLESonGL implementation
 * Version:  1.1
 * Copyright (c) 2005  Joachim Pouderoux
 * Copyright (C) 2003  David Blythe   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * DAVID BLYTHE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include "include/GLES/glues.h"

#pragma warning(disable : 4244)

#include <math.h>

#define __f2x(f)	((int)(f*65536))
#define __mulx(a,b)	((((unsigned long long int)(a))*(b))>>16)
#define __divx(a,b)	(((unsigned long long int)(a)<<16)/(b))
#define __PI		3.14159265358979323846f

const unsigned short __gl_sin_tab[] = {
#include "glues_sin.h"
};

#ifndef GLfloat 
#define GLfloat float
#endif

GLfixed
__cosx(GLfixed a) 
{
    GLfixed v;
    /* reduce to [0,1) */
    while (a < 0) a += 2*__f2x(__PI);
    a *= __f2x(1.0f/(2.f*__PI));
    a >>= 16;
    a += 0x4000;

    /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
    a >>= 4;

    v = (a & 0x400) ? __gl_sin_tab[0x3ff - (a & 0x3ff)] : __gl_sin_tab[a & 0x3ff];
    v = __mulx(v,__f2x(1));
    return (a & 0x800) ? -v : v;
}

GLfixed
__sinx(GLfixed a) 
{
    GLfixed v;

    /* reduce to [0,1) */
    while (a < 0) a += 2*__f2x(__PI);
    a *= __f2x(1.0f/(2.f*__PI));
    a >>= 16;

    /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
    a >>= 4;

    v = (a & 0x400) ? __gl_sin_tab[0x3ff - (a & 0x3ff)] : __gl_sin_tab[a & 0x3ff];
    v = __mulx(v,__f2x(1));
    return (a & 0x800) ? -v : v;
}

void
__identf(GLfloat m[]) 
{
    memset(m, 0, sizeof m[0]*16);
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

void
__identx(GLfixed m[]) 
{
    memset(m, 0, sizeof m[0]*16);
    m[0] = m[5] = m[10] = m[15] = __f2x(1.0f);
}

void 
gluPerspectivef(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f)
{
    GLfloat m[4][4];
    GLfloat s, cot, dz = f - n;
    GLfloat rad = fovy/2.f*__PI/180.f;

    s = sin(rad);
    if (dz == 0 || s == 0 || aspect == 0) 
    	return;

    cot = cos(rad)/s;

    __identf(&m[0][0]);
    m[0][0] = cot/aspect;
    m[1][1] = cot;
    m[2][2] = -(f + n)/dz;
    m[2][3] = -1.f;
    m[3][2] = -2.f*f*n/dz;
    m[3][3] = 0.f;
    glMultMatrixf(&m[0][0]);
}

void 
gluPerspectivex(GLfixed fovy, GLfixed aspect, GLfixed n, GLfixed f)
{
    GLfixed m[4][4];
    GLfixed s, cot, dz = f - n;
    GLfixed rad = __mulx(fovy,  __f2x(__PI/360.f));

    s = __sinx(rad);
    if (dz == 0 || s == 0 || aspect == 0) 
    	return;

    cot = __divx(__cosx(rad), s);

    __identx(&m[0][0]);
    m[0][0] = __divx(cot, aspect);
    m[1][1] = cot;
    m[2][2] = -__divx((f + n), dz);
    m[2][3] = -__f2x(1);
    m[3][2] = __divx(-2*__mulx(f,n), dz);
    m[3][3] = 0;
    glMultMatrixx(&m[0][0]);
}

void
normalizef(float v[3]) 
{
    float r;

    r = (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (r == 0.0) 
    	return;
    r = 1.f/r;

    v[0] *= r;
    v[1] *= r;
    v[2] *= r;
}

void
crossf(float v0[3], float v1[3], float r[3])
{
    r[0] = v0[1]*v1[2] - v0[2]*v1[1];
    r[1] = v0[2]*v1[0] - v0[0]*v1[2];
    r[2] = v0[0]*v1[1] - v0[1]*v1[0];
}

void 
gluLookAtf(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
      GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz) 
{
    GLfloat forward[3], side[3], up[3];
    GLfloat m[4][4];

    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;

    normalizef(forward);

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    crossf(forward, up, side);

    normalizef(side);
    crossf(side, forward, up);

    __identf(&m[0][0]);
    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = up[0];
    m[1][1] = up[1];
    m[2][1] = up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

    glMultMatrixf(&m[0][0]);
    glTranslatef(-eyex, -eyey, -eyez);
}

GLfixed
__sqrtx(GLfixed a) 
{
    GLfixed s;
    int i;
    s = (a + __f2x(1)) >> 1;
    /* 6 iterations to converge */
    for (i = 0; i < 6; i++)
	s = (s + __divx(a, s)) >> 1;
    return s;
}

void
normalizex(GLfixed v[3]) {
    float r;

    r = __sqrtx(__mulx(v[0],v[0]) + __mulx(v[1],v[1]) + __mulx(v[2],v[2]));
    if (r == 0.0f) return;
    r = __divx(1.f,r);

    v[0] *= r;
    v[1] *= r;
    v[2] *= r;
}

void
crossx(GLfixed v0[3], GLfixed v1[3], GLfixed r[3])
{
    r[0] = __mulx(v0[1], v1[2]) - __mulx(v0[2], v1[1]);
    r[1] = __mulx(v0[2], v1[0]) - __mulx(v0[0], v1[2]);
    r[2] = __mulx(v0[0], v1[1]) - __mulx(v0[1], v1[0]);
}

void 
gluLookAtx(GLfixed eyex, GLfixed eyey, GLfixed eyez, GLfixed centerx,
      GLfixed centery, GLfixed centerz, GLfixed upx, GLfixed upy, GLfixed upz) 
{
    GLfixed forward[3], side[3], up[3];
    GLfixed m[4][4];

    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;

    normalizex(forward);

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    crossx(forward, up, side);

    normalizex(side);
    crossx(side, forward, up);

    __identx(&m[0][0]);
    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = up[0];
    m[1][1] = up[1];
    m[2][1] = up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

    glMultMatrixx(&m[0][0]);
    glTranslatex(-eyex, -eyey, -eyez);
}



void 
gluLookAtf2x(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
      GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz)
{
	gluLookAtx(__f2x(eyex), __f2x(eyey), __f2x(eyez), __f2x(centerx),
		__f2x(centery), __f2x(centerz), __f2x(upx), __f2x(upy), __f2x(upz));
}

void 
gluPerspectivef2x(GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f)
{
	gluPerspectivex(__f2x(fovy), __f2x(aspect), __f2x(n), __f2x(f));
}
