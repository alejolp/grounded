/*
 ============================================================================
 Author      : Alejandro Santos, ale@ralo.com.ar
 Version     : 1.0
 ============================================================================
 * 
 * Copyright (C) Alejandro Santos, 2014, ale@ralo.com.ar.
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <SDL/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>

#include "Util.h"

void Util_Init_Rect(SDL_Rect* r, int x, int y, int w, int h) {
	r->x = x;
	r->y = y;
	r->w = w;
	r->h = h;
}

int Util_Rect_Intersects(const SDL_Rect* r1, const SDL_Rect* r2) {
	if ((r1->x < r2->x + r2->w) && (r1->x + r1->w > r2->x)) {
		if ((r1->y < r2->y + r2->h) && (r1->y + r1->h > r2->y))
			return 1;
	}

	return 0;
}

void Test_Rect_Intersects(void) {
	SDL_Rect r1, r2;

	r1.x = r1.y = 0;
	r1.w = r1.h = 32;

	r2.x = r2.y = 10;
	r2.w = r2.h = 32;

	printf("%d %d\n", Util_Rect_Intersects(&r1, &r2), Util_Rect_Intersects(&r2, &r1));

	r2.x = r2.y = 40;
	r2.w = r2.h = 32;

	printf("%d %d\n", Util_Rect_Intersects(&r1, &r2), Util_Rect_Intersects(&r2, &r1));

	r2.x = 0;
	r2.y = 32;
	r2.w = r2.h = 32;

	printf("%d %d\n", Util_Rect_Intersects(&r1, &r2), Util_Rect_Intersects(&r2, &r1));

	r2.x = 32;
	r2.y = 0;
	r2.w = r2.h = 32;

	printf("%d %d\n", Util_Rect_Intersects(&r1, &r2), Util_Rect_Intersects(&r2, &r1));
}

SDL_Surface* Util_LoadBMP(const char* filename) {
	Uint32 ck;
	SDL_Surface *img;
	SDL_Surface *tmp;
	SDL_Surface *screen;

	screen = SDL_GetVideoSurface();

	ck = SDL_MapRGB(screen->format, 255, 0, 255);

	tmp = SDL_LoadBMP(filename);
	if (tmp == NULL)
		Grounded_SDL_Error();

	if (SDL_SetColorKey(tmp, SDL_SRCCOLORKEY | SDL_RLEACCEL, ck) != 0)
		Grounded_SDL_Error();

	img = SDL_DisplayFormat(tmp);
	if (img == NULL)
		Grounded_SDL_Error();

	SDL_FreeSurface(tmp);

	return img;
}

void Grounded_SDL_Error(void) {
	const char* e = SDL_GetError();
	fprintf(stderr, "SDL error: %s\n", e);
	exit(EXIT_FAILURE);
}

void Grounded_Std_Error(void) {
	fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
	exit(EXIT_FAILURE);
}

void Grounded_Error(const char* msg) {
	fprintf(stderr, "Error: %s\n", msg);
	exit(EXIT_FAILURE);
}

