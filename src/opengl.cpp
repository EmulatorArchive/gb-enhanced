// GB Enhanced Copyright Daniel Baxter 2013
// Licensed under the GPLv2
// See LICENSE.txt for full license text

// File : opengl.cpp
// Date : March 16, 2014
// Description : Handles OpenGL functionality
//
// Sets up OpenGL for use in GBE
// Handles blit operations to the screen

#include "gpu.h"

/****** Initialize OpenGL through SDL ******/
void GPU::opengl_init()
{
	SDL_SetVideoMode((config::scaling_factor * 160), (config::scaling_factor * 144), 32, SDL_OPENGL | config::flags);
		
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0, 0, 0);

	glViewport(0, 0, (config::scaling_factor * 160), (config::scaling_factor * 144));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, (config::scaling_factor * 160), (config::scaling_factor * 144), 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gpu_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 256, 256, 32, 0, 0, 0, 0);
}

/****** Blit using OpenGL ******/
void GPU::opengl_blit()
{
	glTexImage2D(GL_TEXTURE_2D, 0, 4, gpu_screen->w, gpu_screen->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, gpu_screen->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width = (256 * config::scaling_factor)/2;
	int height = (256 * config::scaling_factor)/2;
	int x = 128 * config::scaling_factor;
	int y = 128 * config::scaling_factor;

	glTranslatef(x, y, 0);

	glBindTexture(GL_TEXTURE_2D, gpu_texture);
	glBegin(GL_QUADS);

	//Top Left
	glTexCoord2i(0, 1);
	glVertex2i(-width, height);

	//Top Right
	glTexCoord2i(1, 1);
	glVertex2i(width, height);

	//Bottom Right
	glTexCoord2i(1, 0);
	glVertex2i(width, -height);

	//Bottom Left
	glTexCoord2f(0, 0);
	glVertex2i(-width, -height);
	glEnd();

	glLoadIdentity();

	SDL_GL_SwapBuffers();
}