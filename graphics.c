#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "graphics.h"
#include "sprig.h"
#include "setting_config.h"

void gui_clean();

extern SDL_Color WHITE, BLUE, RED, GREEN, GRAY;

EGLint attrib_list[] =
{
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_BUFFER_SIZE, 0,
	EGL_DEPTH_SIZE, 16,
	EGL_NONE
};

static int power_of_two(int input)
{
	int value = 1;

	while ( value < input ) {
		value <<= 1;
	}
	return value;
}

void init_raw_screen()
{

	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;
	
	sglDisplay = eglGetDisplay( (NativeDisplayType)0 );
	if( sglDisplay == EGL_NO_DISPLAY )
	{
		printf( "GL No Display\n" );
	}

	if( !eglInitialize( sglDisplay, &majorVersion, &minorVersion ) ) 
	{
		printf( "GL Init\n" );
	}

	if( !eglChooseConfig( sglDisplay, attrib_list, &sglConfig, 1, &numConfigs ) )
	{
		printf( "GL Config\n" );
	}

	sglContext = eglCreateContext( sglDisplay, sglConfig, NULL, NULL );
	if( sglContext==0 )
	{
		printf( "GL Context\n" );
	}

	sglSurface = eglCreateWindowSurface( sglDisplay, sglConfig, (NativeWindowType)0, NULL );
	if( sglSurface==0 )
	{
		printf( "GL surface\n" );
	}
	
	eglMakeCurrent( sglDisplay, sglSurface, sglSurface, sglContext );   

}

void destroy_raw_screen()
{
	eglMakeCurrent(sglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface( sglDisplay, sglSurface ); 
	eglDestroyContext( sglDisplay, sglContext );
	eglTerminate(sglDisplay);
}

void init_2D_view()
{

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	/* This allows alpha blending of 2D textures with the scene */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glLoadIdentity();

	glOrthof(0.0, (GLfloat)SCREEN_WIDTH, (GLfloat)SCREEN_HEIGHT, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepthf(1.0f);

}    

void copy_buffer_to_texture(TEXTURE *texture)
{
	int w, h;

	/* Use the surface width and height expanded to powers of 2 */
	w = power_of_two(SCREEN_WIDTH);
	h = power_of_two(SCREEN_HEIGHT);
	texture->texMinX = 0.0f;			/* Min X */
	texture->texMinY = 0.0f;			/* Min Y */
	texture->texMaxX = (GLfloat)SCREEN_WIDTH / w;	/* Max X */
	texture->texMaxY = (GLfloat)SCREEN_HEIGHT / h;	/* Max Y */
	texture->w = SCREEN_WIDTH;
	texture->h = SCREEN_HEIGHT;

	glGenTextures(1, &texture->texture);
	glBindTexture(GL_TEXTURE_2D, texture->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, w, h, 0);
	
}

GLuint SDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord)
{
	GLuint texture;
	int w, h;
	SDL_Surface *image_tmp;
	SDL_Rect area;
	Uint32 saved_flags;
	Uint8  saved_alpha;

	/* Use the surface width and height expanded to powers of 2 */
	w = power_of_two(surface->w);
	h = power_of_two(surface->h);
	texcoord[0] = 0.0f;			/* Min X */
	texcoord[1] = 0.0f;			/* Min Y */
	texcoord[2] = (GLfloat)surface->w / w;	/* Max X */
	texcoord[3] = (GLfloat)surface->h / h;	/* Max Y */

	image_tmp = SDL_CreateRGBSurface(
			SDL_SWSURFACE,
			w, h,
			32,
			0x000000FF, 
			0x0000FF00, 
			0x00FF0000, 
			0xFF000000 );
	if ( image_tmp == NULL )
	{
		printf("Fail : image_tmp == NULL\n");
		return 0;
	}


	/* Save the alpha blending attributes */
	saved_flags = surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
	saved_alpha = surface->format->alpha;
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA )
	{
		SDL_SetAlpha(surface, 0, 0);
	}

	/* Copy the surface into the GL texture image */
	area.x = 0;
	area.y = 0;
	area.w = surface->w;
	area.h = surface->h;
	SDL_BlitSurface(surface, &area, image_tmp, &area);


	/* Restore the alpha blending attributes */
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
		SDL_SetAlpha(surface, saved_flags, saved_alpha);
	}

	/* Create an OpenGL texture for the image */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D,
		     0,
		     GL_RGBA,
		     w, h,
		     0,
		     GL_RGBA,
		     GL_UNSIGNED_BYTE,
		     image_tmp->pixels);

	SDL_FreeSurface(image_tmp); /* No longer needed */

	return texture;
}

int load_image_to_texture( TEXTURE *texture_struct, char *filename)
{
	printf("\tCreating GLES Texture Surface from : %s\n", filename);

	SDL_Surface *image = NULL;
	GLfloat texcoord[4];

	/* Load the image */
	image = IMG_Load( filename );
	if ( image == NULL )
	{
		printf("Fail : IMG_Load(%s);\n", filename);
		return 0;
	}
	texture_struct->w = image->w;
	texture_struct->h = image->h;

	/* Convert the image into an OpenGL texture */
	if ( ! (texture_struct->texture = SDL_GL_LoadTexture(image, texcoord) ) )
	{
		return 0;
	}

	/* We don't need the original image anymore */
	SDL_FreeSurface(image);

	/* Make texture coordinates easy to understand */
	texture_struct->texMinX = texcoord[0];
	texture_struct->texMinY = texcoord[1];
	texture_struct->texMaxX = texcoord[2];
	texture_struct->texMaxY = texcoord[3];

	/* Make sure that the texture conversion is okay */
	if ( ! texture_struct->texture )
	{
		printf("Fail : GLES Texture Surface NOK\n");
		return 0;
	}
	else
	{
		printf("Success : GLES Texture Surface OK\n");
	}

	return 1;
}

void DrawTexture(TEXTURE *texture_struct, int x, int y, int centered, GLfloat factor, GLfloat alpha)
{
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);		
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glColor4f(1.0f, 1.0f, 1.0f, alpha);

	GLfloat texcoords[4 * 2];
	GLfloat vertices[4 * 2];

	glBindTexture(GL_TEXTURE_2D, texture_struct->texture );

	if( centered )
	{
		if( factor )
		{
 			x -= (texture_struct->w / 2) * factor;
			y -= (texture_struct->h / 2) * factor;

			texcoords[0] = texture_struct->texMinX; texcoords[1] = texture_struct->texMinY;
			vertices[0] = x;   vertices[1] = y;

			texcoords[2] = texture_struct->texMaxX; texcoords[3] = texture_struct->texMinY;
			vertices[2] = x + texture_struct->w * factor; vertices[3] = y;

			texcoords[4] = texture_struct->texMinX; texcoords[5] = texture_struct->texMaxY;
			vertices[4] = x;   vertices[5] = y + texture_struct->h * factor;

			texcoords[6] = texture_struct->texMaxX; texcoords[7] = texture_struct->texMaxY;
			vertices[6] = x + texture_struct->w * factor; vertices[7] = y + texture_struct->h * factor;
		}
		else
		{
	 		x -= texture_struct->w / 2;
			y -= texture_struct->h / 2;

			texcoords[0] = texture_struct->texMinX; texcoords[1] = texture_struct->texMinY;
			vertices[0] = x;   vertices[1] = y;
			texcoords[2] = texture_struct->texMaxX; texcoords[3] = texture_struct->texMinY;
			vertices[2] = x+texture_struct->w; vertices[3] = y;
			texcoords[4] = texture_struct->texMinX; texcoords[5] = texture_struct->texMaxY;
			vertices[4] = x;   vertices[5] = y+texture_struct->h;
			texcoords[6] = texture_struct->texMaxX; texcoords[7] = texture_struct->texMaxY;
			vertices[6] = x+texture_struct->w; vertices[7] = y+texture_struct->h;
		}
	}
	else
	{
		if( factor )
		{
			texcoords[0] = texture_struct->texMinX; texcoords[1] = texture_struct->texMinY;
			vertices[0] = x;   vertices[1] = y;
			texcoords[2] = texture_struct->texMaxX; texcoords[3] = texture_struct->texMinY;
			vertices[2] = x + texture_struct->w * factor; vertices[3] = y;
			texcoords[4] = texture_struct->texMinX; texcoords[5] = texture_struct->texMaxY;
			vertices[4] = x;   vertices[5] = y + texture_struct->h * factor;
			texcoords[6] = texture_struct->texMaxX; texcoords[7] = texture_struct->texMaxY;
			vertices[6] = x + texture_struct->w * factor; vertices[7] = y + texture_struct->h * factor;
		}
		else
		{
			texcoords[0] = texture_struct->texMinX; texcoords[1] = texture_struct->texMinY;
			vertices[0] = x;   vertices[1] = y;
			texcoords[2] = texture_struct->texMaxX; texcoords[3] = texture_struct->texMinY;
			vertices[2] = x + texture_struct->w; vertices[3] = y;
			texcoords[4] = texture_struct->texMinX; texcoords[5] = texture_struct->texMaxY;
			vertices[4] = x;   vertices[5] = y + texture_struct->h;
			texcoords[6] = texture_struct->texMaxX; texcoords[7] = texture_struct->texMaxY;
			vertices[6] = x + texture_struct->w; vertices[7] = y + texture_struct->h;
		}
	}
	
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, vertices );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, texcoords );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}

void DrawBufferTexture(TEXTURE *texture_struct, int x, int y, int centered, GLfloat factor, GLfloat alpha)
{
	/* Rotate the screen 180° to have a proper texture Z axis */
	/* Since i'm crap and my copied buffer texture is inverted on Z axis */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0.0, (GLfloat)SCREEN_WIDTH , 0.0, (GLfloat)SCREEN_HEIGHT, 1.0, -1.0);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DrawTexture(texture_struct, x, y, centered, factor, alpha);

	/* Restore normal view */
	init_2D_view();
}

void draw_text(char *msg, int size, int type, SDL_Color COLOR, int x, int y)
{
/*
	TEXTURE *texture;
	texture = (TEXTURE *) malloc( sizeof(TEXTURE));

	SDL_Surface *s1;

	if(size == SMALL)
	{
		if(type == NORMAL) s1 = TTF_RenderUTF8_Solid( font, msg, COLOR );
			else if(type == BLEND) s1 = TTF_RenderUTF8_Blended( font, msg, COLOR );
	}
	else if(size == BIG)
	{
		if(type == NORMAL) s1 = TTF_RenderUTF8_Solid( font_big, msg, COLOR );
			else if(type == BLEND) s1 = TTF_RenderUTF8_Blended( font_big, msg, COLOR );
	}
	 
	if (s1)
	{
		int w = power_of_two(s1->w);
		int h = power_of_two(s1->h);

		texture->texMinX = 0.0f;
		texture->texMinY = 0.0f;
		texture->texMaxX = (GLfloat)s1->w / w;
		texture->texMaxY = (GLfloat)s1->h / h;
		texture->w = s1->w;
		texture->h = s1->h;

		SDL_Surface *s2 = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h,
                                   32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
 
        	if (s2)
		{
			SDL_SetAlpha(s1, 0, 0);
			SDL_BlitSurface(s1, 0, s2, 0);
 
			glGenTextures(1, &texture->texture);
			glBindTexture(GL_TEXTURE_2D, texture->texture);
 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s2->w, s2->h, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, s2->pixels);
            
			SDL_FreeSurface(s2);
		}
 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	DrawTexture(texture, x, y, NORMAL, 0, 1.0f);

	glDeleteTextures( 1, &texture->texture );

	free( texture );
*/
}

void draw_text_center(char *msg, int size, int type, SDL_Color COLOR, int x, int y)
{
/*
	SDL_Surface* text_img = NULL;

	if(size == SMALL)
	{
		if(type == NORMAL) text_img = TTF_RenderUTF8_Solid( font, msg, COLOR );
			else if(type == BLEND) text_img = TTF_RenderUTF8_Blended( font, msg, COLOR );
	}
	else if(size == BIG)
	{
		if(type == NORMAL) text_img = TTF_RenderUTF8_Solid( font_big, msg, COLOR );
			else if(type == BLEND) text_img = TTF_RenderUTF8_Blended( font_big, msg, COLOR );
	}

	SPG_DrawCenter( text_img, myscreen, x, y );

	SDL_FreeSurface( text_img );
*/
}

extern int scroll_count, reset_scroll_count;

void draw_text_scroll(char *msg, int size, int type, SDL_Color COLOR, int x, int y, int w)
{
	int i;

	if( reset_scroll_count ) 
	{
		if (text_img != NULL )
		{
			SDL_FreeSurface( text_img );
			text_img = NULL;
		}


		char *msg_clean = malloc(strlen(msg)+1);
		strncpy( msg_clean, msg, strlen(msg) );

		if(size == SMALL)
		{
			if(type == NORMAL) text_img = TTF_RenderUTF8_Solid( font, msg_clean, COLOR );
				else if(type == BLEND) text_img = TTF_RenderUTF8_Blended( font, msg_clean, COLOR );
		}
		else if(size == BIG)
		{
			if(type == NORMAL) text_img = TTF_RenderUTF8_Solid( font_big, msg_clean, COLOR );
				else if(type == BLEND) text_img = TTF_RenderUTF8_Blended( font_big, msg_clean, COLOR );
		}

		free(msg_clean);

		i = text_img->w ;

		scroll_count = x + w;
		reset_scroll_count = 0;
	}

	i = text_img->w ;

	SDL_Rect clipRect = { x, y, w, 40 };
	SPG_SetClip(myscreen, clipRect);

	SPG_Draw( text_img, myscreen, scroll_count, y );

	SPG_RestoreClip(myscreen);  // Reset it

	scroll_count -= 1;

	if(scroll_count == x - i) scroll_count = x + w;
}

void gui_swap_buffer()
{
	if( x11_mode ) SDL_GL_SwapBuffers();
		else eglSwapBuffers( sglDisplay, sglSurface );
}



