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
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include <SDL.h>

#include "game.hpp"
#include "gameobjects.hpp"
#include "constants.hpp"


namespace grounded {

game game::s_game;


gamemap::gamemap() : mapnum_(0), w_(-1), h_(-1) {
    load_map(1);
}

gamemap::~gamemap() {
}

void gamemap::load_map(int n) {
    int x, y;
    
    mapnum_ = n;
    std::stringstream s;
    
    s << "../common/maps/" << n << ".txt";
    std::ifstream f;
    f.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
    f.open(s.str().c_str());
    
    h_ = 0;
    
    for (y=0; y<(WINDOW_HEIGHT/TILE_SIZE); ++y, ++h_) {
        char line[256];
        std::size_t line_len;
        
        f.getline(line, SDL_arraysize(line) - 1, '\n');
        line_len = std::strlen(line);
        
        m_.resize(m_.size() + 1);
        VC& fila = m_[m_.size() - 1];
        if (w_ == -1) {
            w_ = line_len;
        } else if (w_ != (int)line_len) {
            throw std::exception();
        }
        fila.insert(fila.begin(), line, line + line_len);
        for (x=0; x<w_; ++x) {
            
        }
    }
}

void gamemap::next_map() {
    load_map(mapnum_ + 1);
}

void gamemap::update() {
    
}

void gamemap::on_timer() {
    
}

bool gamemap::has_zombie_collision(int x, int y) {
    return false;
}

bool gamemap::has_exit_collision(int x, int y) {
    return false;
}

int gamemap::find_zombie_collision(int x, int y) {
    return -1;
}

bool gamemap::collect_items(int x, int y) {
    return false;
}



game::game() : run_(true) {
    
}

void game::poll_events() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            run_ = false;
            break;

        case SDL_KEYDOWN:
            if (ev.key.keysym.sym == SDLK_ESCAPE)
                run_ = false;
            break;
        }
    }
}

void game::update() {
    
}

}
