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

#ifndef RENDERER_H
#define RENDERER_H

#include <boost/noncopyable.hpp>
#include <SDL.h>

namespace grounded {

class renderer : private boost::noncopyable {
public:
    static renderer* instance() { return &s_renderer; }

private:
    static renderer s_renderer;

public:
    renderer();
    virtual ~renderer();

    void init();
    void cleanup();
    void render();
    
private:
    void init_sdl2();
    void close_sdl2();
    
    void draw_tile(int n, int x, int y);
    
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_Texture* tiles_;
};

}

#endif

