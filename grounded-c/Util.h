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

#ifndef UTIL_H_
#define UTIL_H_

void Util_Init_Rect(SDL_Rect* r, int x, int y, int w, int h);
int Util_Rect_Intersects(const SDL_Rect* r1, const SDL_Rect* r2);

SDL_Surface* Util_LoadBMP(const char* filename);

void Grounded_SDL_Error(void);
void Grounded_Std_Error(void);
void Grounded_Error(const char* msg);

#endif /* UTIL_H_ */
