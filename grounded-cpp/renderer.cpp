
/*
 * Grounded. Pequeño juego de plataformas.
 *
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
 *
 */

#include <iostream>
#include <SDL.h>

#include "game.hpp"
#include "renderer.hpp"
#include "util.hpp"
#include "constants.hpp"


namespace grounded {

renderer renderer::s_renderer;

renderer::renderer() {}

renderer::~renderer() {}

void renderer::init() {
    init_sdl2();
}

void renderer::cleanup() {
    close_sdl2();
}

void renderer::init_sdl2() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
        SDL2_PrintErrorAndDie();
	}

	window_ = SDL_CreateWindow("Grounded",               //    window title
			SDL_WINDOWPOS_UNDEFINED,           //    initial x position
			SDL_WINDOWPOS_UNDEFINED,           //    initial y position
			WINDOW_WIDTH,                               //    width, in pixels
			WINDOW_HEIGHT,                               //    height, in pixels
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL //    flags - see below
			);
	if (window_ == NULL) {
        SDL2_PrintErrorAndDie();
	}

    int numdrivers = SDL_GetNumRenderDrivers ();
    std::cout << "Render driver count: " << numdrivers << std::endl;
    for (int i=0; i<numdrivers; i++) {
        SDL_RendererInfo drinfo;
        SDL_GetRenderDriverInfo (i, &drinfo);
        std::cout << "Driver name ("<<i<<"): " << drinfo.name << std::endl;
        if (drinfo.flags & SDL_RENDERER_SOFTWARE) std::cout << " the renderer is a software fallback" << std::endl;
        if (drinfo.flags & SDL_RENDERER_ACCELERATED) std::cout << " the renderer uses hardware acceleration" << std::endl;
        if (drinfo.flags & SDL_RENDERER_PRESENTVSYNC) std::cout << " present is synchronized with the refresh rate" << std::endl;
        if (drinfo.flags & SDL_RENDERER_TARGETTEXTURE) std::cout << " the renderer supports rendering to texture" << std::endl;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, 0);
    if (!renderer_) {
        SDL2_PrintErrorAndDie();
    }
    
    SDL_Surface* tmp;
    tmp = SDL_LoadBMP("../common/img/tiles.bmp");
    if (!tmp)
        SDL2_PrintErrorAndDie();
    
    if (0 != SDL_SetColorKey(tmp, 
        SDL_TRUE, 
        SDL_MapRGB(tmp->format, 255, 0, 255))) {
        SDL2_PrintErrorAndDie();
    }
        
    
    tiles_ = SDL_CreateTextureFromSurface(renderer_, tmp);
    if (!tiles_)
        SDL2_PrintErrorAndDie();
    
    SDL_FreeSurface(tmp);
}

void renderer::close_sdl2() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void renderer::fullscreen() {
    if (0 != SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP )) {
        SDL2_PrintErrorAndDie();
    }
}

void renderer::render() {
    game* g = game::instance();
    gamemap* m = g->map();
    
    int y, x, xpx, ypx;
    
    // SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    // SDL_RenderClear(renderer_);
    // draw_tile(0, 0, 0);
    
    for (y=0, ypx=0; y < m->height(); ++y, ypx += TILE_SIZE) {
        for (x=0, xpx=0; x < m->width(); ++x, xpx += TILE_SIZE) {
            draw_tile(m->get(x, y) == '.' ? ID_WALL : ID_BACKGROUND, xpx, ypx);
        }
    }
    
    // Items
    draw_objects(m->items());
    // Zombies
    draw_objects(m->zombies());
    // Exits
    draw_objects(m->exits());
    // Elevators
    draw_objects(m->elevators());
    // Specials
    draw_objects(m->specials());
    // Fireballs
    draw_objects(m->fireballs());
    // Player
    draw(m->getplayer());
    
    SDL_RenderPresent(renderer_);
}

void renderer::draw_objects(VG& v) {
    VG::iterator it;
    for (it = v.begin(); it != v.end(); ++it) {
        draw(*it);
    }
}

void renderer::draw_tile(int n, int x, int y) {
    SDL_Rect src, dst;
    
    src.w = src.h = dst.w = dst.h = TILE_SIZE;
    src.x = n * TILE_SIZE;
    src.y = 0;
    
    dst.x = x;
    dst.y = y;
    
    SDL_RenderCopy(renderer_, tiles_, &src, &dst);
}
}

